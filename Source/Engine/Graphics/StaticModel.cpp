#include <cstddef>
#include <cstdint>
#include <glm/fwd.hpp>
#include <string>
#include <qoixx.hpp>

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
    auto header = bl::ReadT<BMMFHeader>(modelFile);

    if (header.magic != bl::BMMF_MAGIC)
        throw std::runtime_error("Model magic is incorrect!");

    _meshes.reserve(header.numMeshes);
    _transforms.reserve(header.numMeshes);

    _sampler = std::make_unique<VulkanSampler>(_device);

    for (uint32_t i = 0; i < header.numMeshes; i++)
    {
        auto meshHeader = bl::ReadT<BMMFMeshHeader>(modelFile);
        auto vertices = bl::ReadVecT<Vertex>(modelFile, meshHeader.numVertices);
        auto indices = bl::ReadVecT<uint32_t>(modelFile, meshHeader.numIndices);
        // auto textureReferences = bl::ReadVecT<TextureReference>(modelFile, meshHeader.numTextureReferences);

        _meshes.emplace_back(_device, vertices, indices);
        _transforms.push_back(bl::ReadT<glm::mat4>(modelFile));
        _meshTransformIndicies.push_back(i);
        //_textures.push_back(textureReferences);
    }

    //for (uint32_t i = 0; i < header.numTextures; i++)
    //{
    //    auto textureHeader = bl::ReadT<TextureHeader>(modelFile);
    //    auto textureBuffer = bl::ReadVecT<std::byte>(modelFile, textureHeader.numBytes);
    //
    //    const auto [actual, desc] = qoixx::qoi::decode<std::vector<std::byte>>(textureBuffer, 4);
    //
    //    _images.emplace_back(_device, VK_IMAGE_TYPE_2D, VkExtent3D{desc.width,desc.height, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    //}
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