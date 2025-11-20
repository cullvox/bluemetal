#pragma once

#include "Graphics/Renderer.h"
#include "Graphics/VulkanMaterial.h"
#include "MaterialInstance.h"

namespace bl 
{

/// @class Material
/// @brief A material resource defining how to render objects.
class Material : public MaterialInstance
{
    ResourceSystem* _resourceSystem;
    GraphicsSystem* _graphicsSystem;
    Renderer* _renderer;
    std::unique_ptr<VulkanMaterial> _material;

protected:
    virtual VulkanMaterialInstance* GetInstance() const;

public:

    /// @brief Loads a material from file.
    /// @param resourceSystem Pointer to the resource system.
    /// @param graphicsSystem Pointer to the graphics system.
    /// @param path Filesystem path to the material JSON file.
    Material(ResourceSystem* resourceSystem, GraphicsSystem* graphicsSystem, const std::filesystem::path& path);

    /// @brief Destructor
    virtual ~Material();

    /// @brief Gets the Vulkan material used by this material.
    VulkanMaterial* GetVulkanMaterial();

    /// @brief Gets the Vulkan pipeline used by this material.
    const VulkanPipeline* GetVulkanPipeline();

    /// @brief Creates a material instance for this material.
    Ref<MaterialInstance> CreateInstance();
};

}