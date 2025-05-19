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
    // Assimp::Importer importer;
    // const aiScene* scene = importer.ReadFile(GetPath().string(), aiProcess_Triangulate);
    // if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    //     blError("ERROR::ASSIMP::{}", importer.GetErrorString());
    //     throw std::runtime_error("Could not load model!");
    // }

    // ProcessNodes(scene->mRootNode, scene);
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



}