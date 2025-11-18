#pragma once

#include "Math/Math.h"
#include "Resource.h"
#include "Sampler.h"
#include "Texture.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "Graphics/VulkanRenderData.h"

namespace bl
{

class ResourceSystem;
class GraphicsSystem;
class Material; // Material Resource

/// @class Material Instance
/// @brief An instance of a material containing it's own buffers.
///
/// Material instances are best described by JSON.
/// baseMaterial: string
/// properties: map
///
class MaterialInstance : public Resource 
{
    Ref<Material> _baseMaterial;
    std::unique_ptr<VulkanMaterialInstance> _materialInstance;

public:

    /// @brief Loads a material instance from file.
    /// This constructor is designated for the ResourceSystem.
    MaterialInstance(ResourceSystem* resourceSystem, GraphicsSystem* graphicsSystem, const std::filesystem::path& path);
    MaterialInstance(ResourceSystem* resourceSystem, GraphicsSystem* graphicsSystem, std::unique_ptr<VulkanMaterialInstance> instance);
    ~MaterialInstance();

    void SetBool(const std::string& name, bool value);
    void SetInteger(const std::string& name, int value);
    void SetScaler(const std::string& name, float value);
    void SetVector2(const std::string& name, glm::vec2 value);
    void SetVector3(const std::string& name, glm::vec3 value);
    void SetVector4(const std::string& name, glm::vec4 value);
    void SetMatrix(const std::string& name, glm::mat4 value);
    void SetSampledTexture(const std::string& name, Ref<Sampler> sampler, Ref<Texture> image);
    void UpdateUniforms(); /** @brief This function must be called before the renderer starts rendering the frame. */

    void Bind(VulkanRenderData& rd); /** @brief Bind this material for rending using it and it's data. */
    void PushConstant(VulkanRenderData& rd, uint32_t offset, uint32_t size, const void* value);
};

}