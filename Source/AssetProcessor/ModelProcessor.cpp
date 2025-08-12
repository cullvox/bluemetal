#include "ModelProcessor.h"

bool ModelProcessor::Process(ProcessorState& state, ResourceFile& resource)
{
    nextNodeIndex = 0;
    nodeParents.clear();
    nodes.clear();

    auto bakedPath = GetBakedParentPath(state, resource);
    auto exportedPath = bakedPath.replace_extension(".bmm");

    std::filesystem::create_directories(exportedPath.parent_path());

    // Load the model file using ASSIMP.
    out.open(exportedPath, std::ios::out | std::ios::binary);
    if (!out.is_open()) {
        bl::Log::Error("Could not open output file: {}", exportedPath.string());
        return false;
    }

    Assimp::Importer importer;
    scene = importer.ReadFile(resource.absolutePath.string(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        bl::Log::Error("ERROR::ASSIMP::{}", importer.GetErrorString());
        return false;
    }

    if (scene->hasSkeletons() || scene->HasAnimations()) {
        bl::Log::Warn("{}: Cannot process model with animations or skeletons yet.", resource.relativePath);
        // return false;
    }

    // Write out the file header.
    bl::BMMFHeader header = {
        .magic = bl::BMMF_MAGIC,
        .numNodes = GetNodeCount(scene->mRootNode),
        .numMeshes = scene->mNumMeshes,
        .numTextures = GetMaterialTextureCount(),
        .numMaterials = scene->mNumMaterials,
    };

    bl::WriteT(out, header);

    strings.reserve(header.numNodes + header.numMeshes + header.numMaterials);

    ProcessNodes(-1, scene->mRootNode);
    ProcessMeshes();
    // ProcessMaterials();

    out.flush();
    out.close();

    resource.bakedPath = bakedPath;
    return true;
}

uint32_t ModelProcessor::GetNodeCount(const aiNode* node)
{
    uint32_t count = 1; // Count this node.
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        count += GetNodeCount(node->mChildren[i]);
    }
    return count;
}

void ModelProcessor::ProcessNodes(uint32_t parent, const aiNode* node)
{
    uint32_t nodeIndex = nextNodeIndex++;
    nodeParents[nodeIndex] = parent;

    // Write the node to the stream.
    {
        // Add the node name to the string table.
        strings.emplace_back(node->mName.C_Str());

        // Write the node to the stream.
        auto& mat = node->mTransformation;
        bl::BMMFNode bmmfNode = {
            .transformation = {
                mat.a1, mat.a2, mat.a3, mat.a4,
                mat.b1, mat.b2, mat.b3, mat.b4,
                mat.c1, mat.c2, mat.c3, mat.c4,
                mat.d1, mat.d2, mat.d3, mat.d4 },
            .nodeIndex = nodeIndex,
            .meshesCount = node->mNumMeshes,
            .nameIndex = strings.size() - 1,
        };

        bl::WriteT(stream, bmmfNode);

        // Write the mesh indices for this node.
        for (int i = 0; i < node->mNumMeshes; i++) {
            // Write the mesh index for this node.
            // This is the index of the mesh in the scene's mMeshes array.
            // We write it as a uint32_t.
            bl::WriteT(stream, node->mMeshes[i]);
        }
    }

    // Write all child nodes.
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNodes(state, nodeIndex, node->mChildren[i], scene, stream);
    }

    // If this is the root node, write the parent indices.
    if (parent == -1) {
        bl::WriteVecT(stream, nodeParents);
    }
}

void ProcessTexture(const aiString& texturePath, aiTextureType type)
{
    std::regex regexExpress("^\\*\\d+");
    std::cmatch regexMatches;

    stbi_uc* data = nullptr;
    int x = 0, y = 0, channels = 0;

    // Check if it's an embedded or external texture.
    if (std::regex_search(texturePath.C_Str(), regexMatches, regexExpress)) {
        // Get the index str.
        std::string indexStr = *(regexMatches.begin());

        // Drop the "*" character.
        indexStr = indexStr.erase(0, 1);

        // Convert the string to an integer. (This is the index in the Scene::mTextures[] array.
        int index = std::stoi(indexStr);

        auto texture = scene->mTextures[index];

        data = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(texture->pcData), texture->mWidth, &x, &y, &channels, 4);
    } else {
        data = stbi_load(texturePath.C_Str(), &x, &y, &channels, 4);
    }

    if (data == nullptr) {
        bl::Log::Error("Could not load model texture.");
        return;
    }

    // Convert the image to QOI format.
    qoixx::qoi::desc desc;
    desc.width = x;
    desc.height = y;
    desc.channels = 4;
    desc.colorspace = qoixx::qoi::colorspace::linear; // We assume every texture is linear, it's easier to do math on!

    // Encode the image to QOI format.
    auto imageBuffer = qoixx::qoi::encode<std::vector<char>, stbi_uc>(data, x * y * 4, desc);

    // Free the image data.
    stbi_image_free(data);

    if (imageBuffer.empty()) {
        bl::Log::Error("Failed to encode model texture to QOI format.");
        return;
    }

    bl::BMMFTextureType textureType;
    switch (type) {
    case aiTextureType_DIFFUSE:
        textureType = bl::BMMFTextureType::eDiffuse;
        break;
    case aiTextureType_NORMALS:
        textureType = bl::BMMFTextureType::eNormal;
        break;
    case aiTextureType_SPECULAR:
        textureType = bl::BMMFTextureType::eSpecular;
        break;
    case aiTextureType_EMISSIVE:
        textureType = bl::BMMFTextureType::eEmissive;
        break;
    case aiTextureType_DIFFUSE_ROUGHNESS:
        textureType = bl::BMMFTextureType::eRoughness;
        break;
    case aiTextureType_METALNESS:
        textureType = bl::BMMFTextureType::eMetallic;
        break;
    case aiTextureType_AMBIENT:
        textureType = bl::BMMFTextureType::eAO;
        break;
    default:
        bl::Log::Error("Unknown texture type: {}", type);
        return;
    }

    // Write the texture header and data to the stream.
    bl::BMMFTextureHeader header = {
        .type = textureType,
        .numBytes = (uint32_t)imageBuffer.size(),
    };

    bl::WriteT(stream, header);
    bl::WriteVecT(stream, imageBuffer);
}

uint32_t ModelProcessor::GetMaterialTextureCount()
{
    std::unordered_set<aiString> textures;
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        auto material = scene->mMaterials[i];

        aiString texturePath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS) {
            textures.insert(texturePath);
        }

        if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == aiReturn_SUCCESS) {
            textures.insert(texturePath);
        }
    }
    return textures.size();
}

void ModelProcessor::ProcessMaterials()
{
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        auto material = scene->mMaterials[i];

        aiString texturePath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS) {
            // Check if it's an embedded or external texture.
            ProcessModelTexture(scene, stream, texturePath, aiTextureType_DIFFUSE);
        } else {
            bl::Log::Warn("Material {} does not have a diffuse texture.", i);
        }

        if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == aiReturn_SUCCESS) {
            // Check if it's an embedded or external texture.
            ProcessModelTexture(scene, stream, texturePath, aiTextureType_NORMALS);
        } else {
            bl::Log::Warn("Material {} does not have a normal texture.", i);
        }

        strings.emplace_back(material->GetName().C_Str());

        bl::BMMFMaterialHeader material = {
            
        }
    }
}

void ModelProcessor::ProcessMeshes()
{
    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        auto mesh = scene->mMeshes[i];

        // Get the number of indices in the mesh.
        int numIndices = 0;
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            numIndices += mesh->mFaces[i].mNumIndices;
        }

        // Write the mesh header.
        bl::BMMFMeshHeader header = {
            .numVertices = mesh->mNumVertices,
            .numIndices = (uint32_t)numIndices,
            .materialIndex = mesh->mMaterialIndex,
        };

        bl::WriteT(stream, header);

        // Build out the vertex and index buffers.
        {
            std::vector<bl::Vertex> vertices;
            vertices.resize(mesh->mNumVertices);

            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
                auto& p = mesh->mVertices[i];
                bl::Vertex& vertex = vertices[i];

                vertex.position.x = p.x;
                vertex.position.y = p.y;
                vertex.position.z = p.z;

                if (mesh->HasNormals()) {
                    auto& n = mesh->mNormals[i];
                    vertex.normal.x = n.x;
                    vertex.normal.y = n.y;
                    vertex.normal.z = n.z;
                }

                if (mesh->HasTextureCoords(0)) {
                    auto& t = mesh->mTextureCoords[0][i];
                    vertex.texCoords.x = t.x;
                    vertex.texCoords.y = t.y;
                }
            }

            bl::WriteVecT(stream, vertices);
        }

        {
            std::vector<uint32_t> indices;
            indices.resize(numIndices);

            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                auto& face = mesh->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices[i] = face.mIndices[j];
                }
            }

            bl::WriteVecT(stream, indices);
        }
    }
}
