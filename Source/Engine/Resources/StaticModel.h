#pragma once

#include "Graphics/MaterialInstance.h"
#include "Graphics/ModelFormat.h"
#include "Graphics/VulkanDevice.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "Graphics/VulkanRenderData.h"
#include "Graphics/VulkanSampler.h"
#include "Resource/Resource.h"
#include "StaticMesh.h"

namespace bl
{

class StaticModel : public Resource
{
public:
    StaticModel(ResourceSystem* manager, const nlohmann::json& data, VulkanDevice* device);
    ~StaticModel();

    virtual void Load() override;
    virtual void Unload() override;

    void Draw(VulkanRenderData& rd, VulkanMaterialInstance* material);

private:

    VulkanDevice* _device;
    std::vector<StaticMesh> _meshes;
    std::vector<ResourceRef<MaterialInstance>> _materials;
    std::vector<glm::mat4> _transforms;
    std::vector<int> _meshTransformIndicies;
    std::unique_ptr<VulkanSampler> _sampler;
    std::vector<VulkanImage> _images;
    std::vector<ResourceRef<Texture>> _textures;
};

}