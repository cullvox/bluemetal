
// The asset processing pipeline happens automagically before the game(s) are compiled.
//
// We convert the many asset types possibly used in game to file types that are faster
// and more accessible to the engine.
//
// Here's a basic table of how assets are converted.
//
// Texture -> QOI (Quite Ok Image)
// Static Model -> A custom binary of packed vertices and images directly after
// Sound -> FLAC
// Shader (GLSL) -> SPIR-V using glslc


#include <assimp/material.h>
#include <filesystem>
#include <fstream>
#include <regex>

#include <argparse/argparse.hpp>

#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/json.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/material.h>

#include "Graphics/ModelFormat.h"
#include "qoixx.hpp"

#include "Core/Print.h"
#include "Core/FileByte.h"
#include "Graphics/stb_image.h"
#include "Graphics/Vertex.h"


struct ResourceFile
{
    std::string type;
    std::string relativePath;
    std::filesystem::path absolutePath;
    std::filesystem::path bakedPath; // Empty if no baking took place.
    nlohmann::json properties;
};

struct ProcessorState
{
    std::vector<ResourceFile> resources;
    std::unordered_set<std::string> resourceChecker;
    std::filesystem::path manifestPath;
    std::filesystem::path outputPath;
    std::filesystem::path materialOutputPath;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_ONLY_SERIALIZE(ResourceFile, type, relativePath, bakedPath)

std::filesystem::path GetBakedParentPath(ProcessorState& state, ResourceFile& resource);
bool ProcessShader(ProcessorState& state, ResourceFile& resource);
bool ProcessTexture(ProcessorState& state, ResourceFile& resource);
bool ProcessAudio(ProcessorState& state, ResourceFile& resource);
bool ProcessModel(ProcessorState& state, ResourceFile& resource);
bool ProcessGeneric(ProcessorState& state, ResourceFile& resource);

int main(int argc, const char** argv)
{
    argparse::ArgumentParser parser{"AssetProcessor", "0.1"};
    parser
        .add_argument("-m", "--manifest")
        .help("The resource manifest containing all project resources.")
        .required();
    parser
        .add_argument("-o", "--bakedPath")
        .help("The path where all baked resources end up when processed.")
        .required();
    parser
        .add_argument("-mo", "--materialOutputPath")
        .help("Exported model materials can be exported to a separate material folder.")
        .default_value("");
    parser
        .add_argument("-mt", "--materialTextureOutputPath")
        .help("Exports a models textures into a specific folder.");
    parser
        .add_argument("-v", "--verbose")
        .help("Turns on verbose logging mode.")
        .default_value(false);

    try
    {
        parser.parse_args(argc, argv);
    }
    catch (const std::exception& e)
    {
        bl::Log::Error("{}, {}", e.what(), parser.help().str());
        std::exit(EXIT_FAILURE);
    }

    ProcessorState state;

    // Process the manifest file.
    state.manifestPath = parser.get<std::string>("manifest");
    state.outputPath = parser.get<std::string>("bakedPath");
    state.materialOutputPath = parser.get<std::string>("materialOutputPath");

    if (parser.get<bool>("verbose"))
    {
        bl::Log::EnableVerboseLogging(true);
        bl::Log::Verbose("Enabling verbose logging.");
    }

    std::filesystem::path manifestRoot = state.manifestPath.parent_path();

    try
    {
        std::ifstream manifestFile(state.manifestPath);
        auto manifestJson = nlohmann::json::parse(manifestFile);
        manifestFile.close();

        auto objectArray = manifestJson["resources"];

        for (auto object : objectArray)
        {
            ResourceFile resource;
            resource.type = object["type"].get<std::string>();
            resource.relativePath = object["relativePath"].get<std::string>();
            resource.absolutePath = manifestRoot / resource.relativePath;
            resource.bakedPath.clear();

            // Ensure that the path doesn't exist yet.
            if (state.resourceChecker.find(resource.relativePath) != state.resourceChecker.end())
            {
                bl::Log::Error("{}: Already exists, two resources cannot have the same path! Skipping...", resource.relativePath);
                continue;
            }

            state.resources.push_back(resource);
            state.resourceChecker.emplace(resource.relativePath);
        }

        
    } 
    catch(...)
    {
        bl::Log::Error("Could not parse the manifest file!");
        exit(EXIT_FAILURE);
    }

    for (auto& resource : state.resources)
    {
        // Ensure that the resource actually exists.
        if (!std::filesystem::exists(resource.absolutePath) ||
            !std::filesystem::is_regular_file(resource.absolutePath))
        {
            bl::Log::Error("Resource does not exist or is not a file: {}", resource.absolutePath);
            continue;
        }

        bl::Log::Verbose("Beginning processing of: {}", resource.relativePath);
        bool status = false;

        if (resource.type == "Shader")
        {
            status = ProcessShader(state, resource);
        }
        else if (resource.type == "Texture")
        {
            status = ProcessTexture(state, resource);
        } 
        else if (resource.type == "Audio")
        {
            status = ProcessGeneric(state, resource);
        }
        else if (resource.type == "Model")
        {
            status = ProcessModel(state, resource);
        }
        else if (resource.type == "Material")
        {
            status = ProcessGeneric(state, resource);
        }

        if (status)
            bl::Log::Info("{}: Processed successfully.", resource.relativePath);
        else
            bl::Log::Error("{}: Could not be processed.", resource.relativePath);
    }

    nlohmann::json manifestJson;
    for (const auto& resource : state.resources)
    {
        nlohmann::json resourceJson;
        resourceJson = resource;
        resourceJson["properties"] = resource.properties;
        
        manifestJson["resources"].push_back(resourceJson);
        
        //if (resource.bakedPath.empty())
        //    manifestJson["resources"].erase("bakedPath");
    }

    std::ofstream manifestFile(state.manifestPath);
    manifestFile << std::setw(4) << manifestJson << std::endl;
    manifestFile.close();

    return EXIT_SUCCESS;
}

std::filesystem::path GetBakedParentPath(ProcessorState& state, ResourceFile& resource)
{
    auto exportedPath = state.outputPath / resource.relativePath;
    std::filesystem::create_directories(exportedPath.parent_path());

    auto filename = exportedPath.filename();
    auto relativeExportedPath = std::filesystem::path(resource.relativePath).parent_path();
    relativeExportedPath /= filename.string();

    auto relativePath = std::filesystem::relative(state.outputPath.parent_path(), state.manifestPath.parent_path().parent_path());
    auto newPath = relativePath / relativeExportedPath;

    return newPath;
}

bool ProcessShader(ProcessorState& state, ResourceFile& resource)
{
    // Build the final absolutePath, with proper 'spv' extension.
    auto bakedPath = GetBakedParentPath(state, resource);

    // Maybe increase this checks complexity down the line!
    std::string shaderType = bakedPath.extension() == ".vert" ? "vertex" : "fragment";

    bakedPath.replace_extension(bakedPath.extension().string() + ".spv");

    // Run the glslc shader compilation command.
    std::string cmd = fmt::format("glslc {} -o {}", resource.absolutePath.string(), bakedPath.string());
    if (std::system(cmd.c_str()) != EXIT_SUCCESS)
    {
        bl::Log::Error("{}: Could not compile shader resource.", resource.relativePath);
        bl::Log::Warn("This asset will not be added to the engine manifest.");
        bl::Log::Warn("Please ensure that you have the Vulkan SDK Installed.");
        return false;
    }

    // Export the final resource into the engine manifest.
    resource.bakedPath = bakedPath;
    resource.properties["shaderType"] = shaderType;
    return true;
}

bool ProcessTexture(ProcessorState& state, ResourceFile& resource)
{
    // Build the final absolutePath, with proper qoi extension.
    auto bakedPath = GetBakedParentPath(state, resource);
    bakedPath.replace_extension(".qoi");

    if (resource.absolutePath.extension() == ".qoi")
    {
        bl::Log::Info("{}: Already in QOI format, not baking.", bakedPath.string());
        auto relativePath = std::filesystem::relative(resource.absolutePath.parent_path(), state.manifestPath.parent_path().parent_path());
        resource.bakedPath = relativePath / resource.absolutePath.filename();
        return true;
    }
    
    if (resource.absolutePath.extension() != ".png" &&
        resource.absolutePath.extension() != ".jpg" &&
        resource.absolutePath.extension() != ".jpeg")
    {
        bl::Log::Error("{}: Invalid texture file type, please convert it manually.", resource.relativePath);
        return false;
    }

    int x = 0, y = 0, channels = 0;
    auto data = stbi_load(resource.absolutePath.string().c_str(), &x, &y, &channels, 4);

    if (data == nullptr)
    {
        bl::Log::Error("{}: Could not load this texture.", resource.absolutePath.string());
        return false;
    }

    qoixx::qoi::desc desc;
    desc.width = x;
    desc.height = y;
    desc.channels = 4;
    desc.colorspace = qoixx::qoi::colorspace::linear; // We assume every texture is linear, it's easier to do math on!

    auto out = qoixx::qoi::encode<std::vector<char>, stbi_uc>(data, x * y * 4, desc);

    std::ofstream outFile(bakedPath, std::ios::out | std::ios::binary);
    outFile.write(out.data(), out.size());
    outFile.close();

    resource.bakedPath = bakedPath;
    return true;
}

// Models have a custom binary format that's just fast for the engine to load without much processing.
// Any materials are going to be broken out of the 
//
// Model (header)
//  magic       (uint32_t) 'B' 'M' 'M' 'F'
//  modelVersion (uint32_t) (0)
//  numStaticMeshes (uint32_t)
//  numTextures     (uint32_t)
//  numMaterials    (uint32_t)
//
// Static Meshes (array)
//  numVertices (uint32_t)
//  numIndices  (uint32_t)
//  numTextures (uint32_t)
//  Vertices    (sizeof(bl::Vertex) * numVertices)
//  Indices     (sizeof(uint32_t) * numIndices)
//  Transform   (sizeof(glm::mat4))
//
// Materials (header)
//  numMaterialResourcePaths (uint32_t)
//  numBytesInResourcePaths (uint64_t)
//
//    Resource Paths (array)
//      materialResourcePath (nul-terminated string)
//


void ProcessNodes(const aiNode* node, const aiScene* scene, std::ofstream& stream);
void ProcessMesh(const aiScene* scene, const aiMesh* mesh, std::ofstream& stream);
void ProcessMaterials(const aiMesh* mesh, const aiScene* scene, std::ofstream& stream);

void ProcessNodes(const aiNode* node, const aiScene* scene, std::ofstream& stream)
{

    for (unsigned int j = 0; j < node->mNumMeshes; j++)
    {
        ProcessMesh(scene, scene->mMeshes[node->mMeshes[j]], stream);

        auto& m = node->mTransformation;
        float transformationMatrix[16] = {
            (float)m.a1, (float)m.a2, (float)m.a3, (float)m.a4,
            (float)m.b1, (float)m.b2, (float)m.b3, (float)m.b4,
            (float)m.c1, (float)m.c2, (float)m.c3, (float)m.c4,
            (float)m.d1, (float)m.d2, (float)m.d3, (float)m.d4
        };

        stream.write(reinterpret_cast<char*>(transformationMatrix), sizeof(transformationMatrix));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNodes(node->mChildren[i], scene, stream);
    }
}

//void ProcessModelMaterials(const aiScene* scene, std::ofstream& stream)
//{
//
//    std::regex regexExpress("^\\*\\d+");
//    std::cmatch regexMatches;
//
//    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
//    {
//        auto material = scene->mMaterials[i];
//
//
//        aiString texturePath;
//        if(material->GetTexture(aiTextureType_LIGHTMAP, 0, &texturePath) == aiReturn_SUCCESS)
//        {
//                    // Check if it's an embedded or external  texture.
//            if(std::regex_search(texturePath.C_Str(), regexMatches, regexExpress))
//            {
//                // Get the index str.
//                std::string indexStr = *(regexMatches.begin());
//
//                // Drop the "*" character.
//                indexStr = indexStr.erase(0,1);
//
//                // Convert the string to an integer. (This is the index in the
//                // Scene::mTextures[] array.
//                int index = std::stoi(indexStr);
//
//
//            }
//            else
//            {
//                // Print the texture file path.
//                std::cout << "File Path: " << texturePath.C_Str() << std::endl;
//            }
//        }
//    }
//
//}

void ProcessMesh(const aiScene* scene, const aiMesh* mesh, std::ofstream& stream)
{
    int numIndices = 0;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        numIndices += mesh->mFaces[i].mNumIndices;
    }

    bl::WriteT<uint32_t>(stream, mesh->mNumVertices);
    bl::WriteT<uint32_t>(stream, numIndices);

    std::vector<bl::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<bl::TextureReference> textureReferences;

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(numIndices);
    // textureReferences.reserve(mesh->mMaterialIndex)

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        bl::Vertex vertex{};

        auto& p = mesh->mVertices[i];

        vertex.position.x = p.x;
        vertex.position.y = p.y;
        vertex.position.z = p.z;

        if (mesh->HasNormals())
        {
            auto& n = mesh->mNormals[i];
            vertex.normal.x = n.x;
            vertex.normal.y = n.y;
            vertex.normal.z = n.z;
        }

        if (mesh->HasTextureCoords(0))
        {
            auto& t = mesh->mTextureCoords[0][i];
            vertex.texCoords.x = t.x;
            vertex.texCoords.y = t.y;
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        auto& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    bl::WriteVecT(stream, vertices);
    bl::WriteVecT(stream, indices);

   //auto material = scene->mMaterials[mesh->mMaterialIndex];
   //for (material->GetTexture(aiTextureType type, unsigned int index, aiString *path)))

   //bl::WriteVecT(std::ofstream &out, const std::vector<T> &data)
}

void ProcessModelTextures(const aiScene* scene, std::ofstream& stream)
{
    for (unsigned int i = 0; i < scene->mNumTextures; i++)
    {
        auto texture = scene->mTextures[i];

        if (texture->mHeight == 0)
        {
            int x = 0, y = 0, channels = 0;
            auto data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(texture->pcData), texture->mWidth, &x, &y, &channels, 4);

            if (data == nullptr)
            {
                bl::Log::Error("Could not load model texture.");
                continue;
            }

            qoixx::qoi::desc desc;
            desc.width = x;
            desc.height = y;
            desc.channels = 4;
            desc.colorspace = qoixx::qoi::colorspace::linear; // We assume every texture is linear, it's easier to do math on!

            auto imageBuffer = qoixx::qoi::encode<std::vector<char>, stbi_uc>(data, x * y * 4, desc);

            bl::TextureHeader header {
                .type = bl::TextureType::eAlbdeo,
                .numBytes = (uint32_t)imageBuffer.size()
            };

            bl::WriteT(stream, header);
            bl::WriteVecT(stream, imageBuffer);
        }
    }
}

bool ProcessModel(ProcessorState& state, ResourceFile& resource)
{
    auto bakedPath = GetBakedParentPath(state, resource);
    auto exportedPath = bakedPath.replace_extension(".bmm");

    std::filesystem::create_directories(exportedPath.parent_path());

    // Load the model file using ASSIMP.
    std::ofstream out(exportedPath, std::ios::out | std::ios::binary);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(resource.absolutePath.string(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        bl::Log::Error("ERROR::ASSIMP::{}", importer.GetErrorString());
        return false;
    }

    if (scene->hasSkeletons() || scene->HasAnimations())
    {
        bl::Log::Warn("{}: Cannot process model with animations or skeletons yet.", resource.relativePath);
        // return false;
    }

    // Write out the file header.
    bl::WriteT(out, bl::ModelHeader::ModelMagic);
    bl::WriteT<uint32_t>(out, scene->mNumMeshes);
    // bl::WriteT<uint32_t>(out, scene->mNumTextures);

    // Write out the meshes.
    ProcessNodes(scene->mRootNode, scene, out);
    //ProcessModelTextures(scene, out);

    out.flush();
    out.close();

    resource.bakedPath = bakedPath;
    return true;
}

bool ProcessGeneric(ProcessorState& state, ResourceFile& resource)
{
    auto relativePath = std::filesystem::relative(resource.absolutePath.parent_path(), state.manifestPath.parent_path().parent_path());
    resource.bakedPath = relativePath / resource.absolutePath.filename();
    return true;
}
