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

class Model : public Resource
{
    std::vector<Ref<Mesh>> _meshes;
    std::vector<Ref<MaterialInstance>> _materials;
    std::vector<Ref<Texture2D>> _textures;
    Ref<SceneTree> _tree;
public:
    Model(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::string& path);
    ~Model();

    virtual void Load() override;
    virtual void Unload() override;

    void Draw(VulkanRenderData& rd, VulkanMaterialInstance* material);
};

}