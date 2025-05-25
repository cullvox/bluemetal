#include <assimp/config.h>
#include <assimp/postprocess.h>

#include "Core/FileByte.h"
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
    std::ifstream modelFile(GetPath());
    auto  bl::ReadT<uint32_t>(modelFile);
}

void StaticModel::Unload()
{
    _meshes.clear();
}

void StaticModel::Draw(MaterialInstance* mat, VulkanRenderData& rd)
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