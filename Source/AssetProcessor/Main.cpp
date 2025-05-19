
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


#include <fstream>

#include <argparse/argparse.hpp>

#include <nlohmann/json.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "qoixx.hpp"

#include "Core/Print.h"
#include "Graphics/stb_image.h"
#include "Graphics/Vertex.h"

struct ResourceFile
{
    std::filesystem::path relativePath;
    std::filesystem::path path;
    std::string type;

    std::filesystem::path exportedRelativePath;
};

struct ProcessorState
{
    std::vector<ResourceFile> resources;
    std::filesystem::path manifestPath;
    std::filesystem::path outputPath;
    std::filesystem::path materialOutputPath;
    nlohmann::json engineManifest;
};

bool ProcessShader(ProcessorState& state, ResourceFile& resource);
bool ProcessTexture(ProcessorState& state, ResourceFile& resource);
bool ProcessAudio(ProcessorState& state, ResourceFile& resource);
bool ProcessModel(ProcessorState& state, ResourceFile& resource);

int main(int argc, const char** argv)
{
    argparse::ArgumentParser parser{"AssetProcessor", "0.1"};
    parser
        .add_argument("-m", "--manifest")
        .help("The resource manifest containing all project resources.")
        .required();
    parser
        .add_argument("-o", "--outputPath")
        .help("The path where all resources end up when processed.")
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
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    ProcessorState state;

    // Process the manifest file.
    state.manifestPath = parser.get<std::string>("manifest");
    state.outputPath = parser.get<std::string>("outputPath");
    state.materialOutputPath = parser.get<std::string>("materialOutputPath");

    if (parser.get<bool>("verbose"))
    {
        bl::logging::enableVerboseLogging(true);
        blVerbose("Enabling verbose logging.");
    }

    std::filesystem::path manifestRoot = state.manifestPath.parent_path();

    try
    {
        std::ifstream manifestFile(state.manifestPath);
        auto manifestJson = nlohmann::json::parse(manifestFile);
        auto objectArray = manifestJson["Resources"];

        for (auto object : objectArray)
        {
            ResourceFile res;
            res.relativePath = object["Path"].get<std::string>();
            res.path = manifestRoot / res.relativePath;
            res.type = object["Type"].get<std::string>();
            state.resources.push_back(res);
        }
    } 
    catch(...)
    {
        std::cerr << "Could not parse the manifest file!" << std::endl;
        exit(EXIT_FAILURE);
    }


    for (auto& resource : state.resources)
    {
        // Ensure that the resource actually exists.
        if (!std::filesystem::exists(resource.path) && std::filesystem::is_regular_file(resource.path))
        {
            std::cerr << "Resource does not exist or is not a file: " << resource.path << std::endl;
            continue;
        }

        blVerbose("Beginning processing of: {}", resource.relativePath.string());
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
            status = ProcessAudio(state, resource);
        }
        else if (resource.type == "Model")
        {
            status = ProcessModel(state, resource);
        }

        if (status)
            blInfo("{}: Processed successfully.", resource.exportedRelativePath.string());
        else
            blError("{}: Could not be processed.", resource.exportedRelativePath.string());
    }

    return EXIT_SUCCESS;
}

bool ProcessShader(ProcessorState& state, ResourceFile& resource)
{
    // Build the final path, with proper spir-v extensions.
    auto exportedPath = state.outputPath / resource.relativePath;
    std::filesystem::create_directories(exportedPath.parent_path());

    exportedPath.replace_extension(exportedPath.extension().string() + ".spv");
    auto exportedFilename = exportedPath.filename();
    auto relativeExportedPath = resource.relativePath.parent_path();
    relativeExportedPath.concat("/" + exportedFilename.string());

    // Run the glslc shader compilation command.
    std::string cmd = fmt::format("glslc {} -o {}", resource.path.string(), exportedPath.string());
    if (std::system(cmd.c_str()) != EXIT_SUCCESS)
    {
        blError("{}: Could not compile shader resource.", resource.relativePath.string());
        blWarning("This asset will not be added to the engine manifest.");
        blWarning("Please ensure that you have the Vulkan SDK Installed.");
        return false;
    }

    // Export the final resource into the engine manifest.
    resource.exportedRelativePath = relativeExportedPath;

    nlohmann::json obj;
    obj["Path"] = relativeExportedPath;
    obj["Type"] = "Shader";

    state.engineManifest["Resources"].push_back(obj);
    return true;
}

bool ProcessTexture(ProcessorState& state, ResourceFile& resource)
{
    // Build the final path, with proper qoi extension.
    auto exportedPath = (state.outputPath / resource.relativePath).replace_extension(".qoi");
    auto exportedFilename = exportedPath.filename();
    auto relativeExportedPath = resource.relativePath.parent_path() / exportedFilename;

    std::filesystem::create_directories(exportedPath.parent_path());

    if (resource.path.extension() == ".qoi")
    {
        std::filesystem::copy_file(resource.path, exportedPath, std::filesystem::copy_options::overwrite_existing);
        blInfo("{}: Already in qoi format, copying instead of converting.", relativeExportedPath.string());
        resource.exportedRelativePath = relativeExportedPath;
        return true;
    }
    
    if (resource.path.extension() != ".png" &&
        resource.path.extension() != ".jpg" &&
        resource.path.extension() != ".jpeg")
    {
        blError("{}: Invalid texture file type, please convert it manually.", resource.relativePath.string());
        return false;
    }

    int x = 0, y = 0, channels = 0;
    auto data = stbi_load(resource.path.string().c_str(), &x, &y, &channels, 4);

    if (data == nullptr)
    {
        blError("{}: Could not load this texture.", resource.path.string());
        return false;
    }

    qoixx::qoi::desc desc;
    desc.width = x;
    desc.height = y;
    desc.channels = 4;
    desc.colorspace = qoixx::qoi::colorspace::linear; // We assume every texture is linear, it's easier to do math on!

    auto out = qoixx::qoi::encode<std::vector<char>, stbi_uc>(data, x * y * 4, desc);

    std::ofstream outFile(exportedPath, std::ios::out | std::ios::binary);
    outFile.write(out.data(), out.size());
    outFile.close();

    resource.exportedRelativePath = relativeExportedPath;

    nlohmann::json obj;
    obj["Path"] = relativeExportedPath;
    obj["Type"] = "Texture";

    state.engineManifest["Resources"].push_back(obj);
    return true;
}

bool ProcessAudio(ProcessorState& state, ResourceFile& resource)
{
    // The engine by default using FMOD supports a lot of audio extensions, we just copy audio files to baked folder.
    // Build the final path.
    auto exportedPath = state.outputPath / resource.relativePath;
    auto relativeExportedPath = resource.relativePath;

    std::filesystem::create_directories(exportedPath.parent_path());

    std::filesystem::copy_file(resource.path, exportedPath, std::filesystem::copy_options::overwrite_existing);

    resource.exportedRelativePath = relativeExportedPath;

    nlohmann::json obj;
    obj["Path"] = relativeExportedPath;
    obj["Type"] = "Audio";

    state.engineManifest["Resources"].push_back(obj);
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
namespace ap
{
    void write_uint32(std::ofstream& out, uint32_t data)
    {
        out.write(reinterpret_cast<char*>(&data), sizeof(uint32_t));
    }
}

void ProcessNodes(const aiNode* node, const aiScene* scene, std::ofstream& stream);
void ProcessMesh(const aiMesh* mesh, std::ofstream& stream);
void ProcessMaterials(const aiMesh* mesh, const aiScene* scene, std::ofstream& stream);

void ProcessNodes(const aiNode* node, const aiScene* scene, std::ofstream& stream)
{

    for (unsigned int j = 0; j < node->mNumMeshes; j++)
    {
        ProcessMesh(scene->mMeshes[node->mMeshes[j]], stream);

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

void ProcessMesh(const aiMesh* mesh, std::ofstream& stream)
{
    int numIndices = 0;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        numIndices += mesh->mFaces[i].mNumIndices;
    }

    ap::write_uint32(stream, mesh->mNumVertices);
    ap::write_uint32(stream, numIndices);

    std::vector<bl::Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(mesh->mNumVertices);
    vertices.reserve(numIndices);

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

    stream.write(reinterpret_cast<char*>(vertices.data()), vertices.size());
    stream.write(reinterpret_cast<char*>(indices.data()), indices.size());
}

void ProcessMaterials(const aiMesh* mesh, const aiScene* scene, std::ofstream& stream)
{
}

bool ProcessModel(ProcessorState& state, ResourceFile& resource)
{
    auto exportedPath = (state.outputPath / resource.relativePath).replace_extension(".bmm");
    auto exportedFilename = exportedPath.filename();
    auto relativeExportedPath = resource.relativePath.parent_path() / exportedFilename;

    std::filesystem::create_directories(exportedPath.parent_path());

    std::ofstream out(exportedPath, std::ios::out | std::ios::binary);

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(resource.path.string(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        blError("ERROR::ASSIMP::{}", importer.GetErrorString());
        throw std::runtime_error("Could not load model!");
    }

    // Write out the file header.
    out.write("BMMF", 4);
    ap::write_uint32(out, 0);
    ap::write_uint32(out, scene->mNumMeshes);

    // Write out the meshes.
    ProcessNodes(scene->mRootNode, scene, out);

    out.flush();
    out.close();

    resource.exportedRelativePath = relativeExportedPath;

    nlohmann::json obj;
    obj["Path"] = relativeExportedPath;
    obj["Type"] = "Audio";

    state.engineManifest["Resources"].push_back(obj);

    return true;
}
