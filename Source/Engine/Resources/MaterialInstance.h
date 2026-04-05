#pragma once

#include "Graphics/VulkanMaterialInstance.h"
#include "Math/Math.h"
#include "Resource.h"
#include "Sampler.h"
#include "Texture.h"

namespace bl {

class ResourceSystem;
class GraphicsSystem;
class Renderer;
class RenderData;
class Material;
class VulkanMaterialSystem;

/// @class Material Instance
/// @brief An instance of a material containing it's own buffers.
///
/// A material instance holds its own uniform buffers and sampled images,
/// allowing multiple objects to use the same base material with different parameters.
class MaterialInstance : public Resource {
    Renderer* _renderer;
    std::unique_ptr<VulkanMaterialInstance> _materialInstance; // Set by either Material or is created.

protected:

public:
    virtual VulkanMaterialInstance* GetInstance() const;

    /// @brief Creates an empty material instance, used for constructing materials.
    /// This constructor is designated for the Material Resource.
    ///
    /// @param resourceSystem Pointer to the resource system.
    /// @param graphicsSystem Pointer to the graphics system.
    MaterialInstance(Engine& engine);

    MaterialInstance(Engine& engine, std::unique_ptr<VulkanMaterialInstance> instance);

    /// @brief Loads a material instance from file.
    /// This constructor is designated for the ResourceSystem.
    MaterialInstance(Engine& engine, const std::filesystem::path& path);

    /// @brief Destructor
    virtual ~MaterialInstance();

    /// @brief Sets a boolean uniform in the material instance.
    /// @param name Name of the uniform to set.
    /// @param value Value to set the uniform to.
    void SetBool(const std::string& name, bool value);

    /// @brief Sets an integer uniform in the material instance.
    /// @param name Name of the uniform to set.
    /// @param value Value to set the uniform to.
    void SetInteger(const std::string& name, int value);

    /// @brief Sets a scaler (float) uniform in the material instance.
    /// @param name Name of the uniform to set.
    /// @param value Value to set the uniform to.
    void SetScaler(const std::string& name, float value);
    void SetVector2(const std::string& name, glm::vec2 value);
    void SetVector3(const std::string& name, glm::vec3 value);
    void SetVector4(const std::string& name, glm::vec4 value);
    void SetMatrix(const std::string& name, glm::mat4 value);
    void SetSampledTexture2D(const std::string& name, Ref<Sampler> sampler, Ref<Texture> image);

    void Bind(RenderData& rd); /** @brief Bind this material for rending using it and it's data. */
    void PushConstant(RenderData& rd, uint32_t offset, uint32_t size, const void* value);
};

}