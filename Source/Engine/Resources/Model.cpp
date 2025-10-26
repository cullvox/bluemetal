#include <cstddef>
#include <cstdint>
#include <glm/fwd.hpp>
#include <string>
#include <qoixx.hpp>

#include <tiny_gltf.h>

#include "Core/FileByte.h"
#include "Graphics/ModelFormat.h"
#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "Graphics/VulkanSampler.h"
#include "Resources/Resource.h"
#include "Resources/ResourceManager.h"
#include "Resources/Texture2D.h"
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

StaticModel::StaticModel(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::path& path)
    : Resource(resourceSystem, system, path)
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, path.string());

    if (!warn.empty())
        Log::Warn("GLTF Load: {}", warn);

    if (!err.empty())
        Log::Error("GLTF Load: {}", err);

    if (!res)
        throw std::runtime_error("Could not load a model file!");
}

StaticModel::~StaticModel()
{
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
        // instance->SetSampledImage2D("inAlbedo", _sampler.get(), &_images[0]);
        mesh.Bind(rd.cmd);
        mesh.Draw(rd.cmd);
    }
}



}