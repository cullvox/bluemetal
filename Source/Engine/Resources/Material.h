#pragma once

#include "Graphics/VulkanMaterial.h"
#include "MaterialInstance.h"

namespace bl {

class Renderer;
class GraphicsSystem;

enum class EVertexType {
    eDefault,
    eSkinned,
    eDebug
};

/// @class Material
/// @brief A material resource defining how to render objects.
class Material : public MaterialInstance {
    OBJECT_BOILER(Material, MaterialInstance)

    Renderer* _renderer;
    std::unique_ptr<VulkanMaterial> _material;
    std::list<std::weak_ptr<MaterialInstance>> _instances;

protected:
    virtual VulkanMaterialInstance* GetInstance() const override;

public:
    Material();

    /// @brief Loads a material from file.
    /// @param path Filesystem path to the material JSON file.
    Material(const std::filesystem::path& path);

    Material(Material&&) = default;
    Material(const Material&);

    /** @brief Destructor */
    virtual ~Material();

    virtual void Load() override;

    virtual void Release() override;

    /// @brief Gets the Vulkan material used by this material.
    VulkanMaterial* GetVulkanMaterial();

    /// @brief Gets the Vulkan pipeline used by this material.
    const VulkanPipeline* GetVulkanPipeline();

    /// @brief Creates a material instance for this material.
    std::shared_ptr<MaterialInstance> CreateInstance();

    static void RegisterClass();
};

}