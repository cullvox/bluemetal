#include "Model.h"
#include "Graphics/Material.h"
#include "Graphics/Vertex.h"
#include "Graphics/VulkanDevice.h"
#include "Material/UniformData.h"
#include "Resource/Resource.h"
#include <assimp/config.h>
#include <assimp/postprocess.h>

namespace bl
{

static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
{
    glm::mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

Model::Model(const nlohmann::json& json, VulkanDevice* device)
    : Resource(json)
    , _device(device)
{
}

Model::~Model()
{
}

void Model::Load()
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(GetPath().c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        blError("ERROR::ASSIMP::{}", importer.GetErrorString());
        throw std::runtime_error("Could not load model!");
    }

    ProcessNodes(scene->mRootNode, scene);
}

void Model::Unload()
{
    _meshes.clear();
}

void Model::Draw(MaterialInstance* mat, VulkanRenderData& rd)
{
    for (int i = 0; i < (int)_meshes.size(); i++)
    {
        auto& mesh = _meshes[i];
        ObjectPC obj;
        obj.model = _transforms[_meshTransformIndicies[i]];

        mat->PushConstant(rd, 0, sizeof(ObjectPC), &obj);
        mesh.bind(rd.cmd);
        mesh.draw(rd.cmd);
    }
}

void Model::ProcessNodes(const aiNode* node, const aiScene* scene)
{
    _transforms.push_back(ConvertMatrixToGLMFormat(node->mTransformation));
    
    for (unsigned int j = 0; j < node->mNumMeshes; j++)
    {
        ProcessMesh(scene->mMeshes[node->mMeshes[j]], scene);
        _meshTransformIndicies.push_back(_transforms.size()-1);
    }


    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNodes(node->mChildren[i], scene);
    }
}

void Model::ProcessMesh(const aiMesh* mesh, const aiScene* scene)
{

    int numIndices = 0;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        numIndices += mesh->mFaces[i].mNumIndices;
    }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(mesh->mNumVertices);
    vertices.reserve(numIndices);
    
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex{};
    
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

    _meshes.emplace_back(_device, vertices, indices);
}

}