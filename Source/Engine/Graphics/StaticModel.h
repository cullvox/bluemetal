#pragma once

#include "Graphics/MaterialInstance.h"
#include "Graphics/VulkanDevice.h"
#include "Graphics/VulkanRenderData.h"
#include "Resource/Resource.h"
#include "Mesh.h"

namespace bl
{

class StaticModel : public Resource
{
public:
    StaticModel(ResourceManager* manager, const nlohmann::json& data, VulkanDevice* device);
    ~StaticModel();

    virtual void Load() override;
    virtual void Unload() override;

    void Draw(VulkanRenderData& rd);

private:

    VulkanDevice* _device;
    std::vector<StaticMesh> _meshes;
    std::vector<ResourceRef<MaterialInstance>> _materials;
    std::vector<glm::mat4> _transforms;
    std::vector<int> _meshTransformIndicies;
};

}