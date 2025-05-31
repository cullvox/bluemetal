#include <assimp/config.h>
#include <assimp/postprocess.h>
#include <cstdint>
#include <glm/fwd.hpp>

#include "Core/FileByte.h"
#include "Graphics/ModelFormat.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "Resource/Resource.h"
#include "Material.h"
#include "Vertex.h"
#include "VulkanDevice.h"
#include "UniformData.h"
#include "StaticModel.h"

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

StaticModel::StaticModel(ResourceManager* manager, const nlohmann::json& json, VulkanDevice* device)
    : Resource(manager, json)
    , _device(device)
{
}

StaticModel::~StaticModel()
{
}

void StaticModel::Load()
{
    std::ifstream modelFile(GetFilePath(), std::ios::in | std::ios::binary);
    auto header = bl::ReadT<ModelHeader>(modelFile);

    if (header.magic != ModelMagic)
        throw std::runtime_error("Model magic is incorrect!");

    _meshes.reserve(header.numMeshes);
    _transforms.reserve(header.numMeshes);
    
    for (uint32_t i = 0; i < header.numMeshes; i++)
    {
        auto meshHeader = bl::ReadT<MeshHeader>(modelFile);
        auto vertices = bl::ReadVecT<Vertex>(modelFile, meshHeader.numVertices);
        auto indices = bl::ReadVecT<uint32_t>(modelFile, meshHeader.numIndices);

        _meshes.emplace_back(_device, vertices, indices);
        _transforms.push_back(bl::ReadT<glm::mat4>(modelFile));
        _meshTransformIndicies.push_back(i);
    }
}

void StaticModel::Unload()
{
    _meshes.clear();
}

void StaticModel::Draw(VulkanRenderData& rd, VulkanMaterialInstance* instance)
{
    for (int i = 0; i < (int)_meshes.size(); i++)
    {
        auto& mesh = _meshes[i];
        ObjectPC obj;
        obj.model = _transforms[_meshTransformIndicies[i]];

        instance->PushConstant(rd, 0, sizeof(ObjectPC), &obj);
        mesh.Bind(rd.cmd);
        mesh.Draw(rd.cmd);
    }
}



}