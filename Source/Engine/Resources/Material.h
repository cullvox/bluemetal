#pragma once

#include "MaterialInstance.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanMaterial.h"

namespace bl {

class Material : public MaterialInstance {
    ResourceSystem* _resourceSystem;
    GraphicsSystem* _graphicsSystem;
    Renderer* _renderer;
    std::unique_ptr<VulkanMaterial> _material;
public:
    Material(ResourceSystem* resourceSystem, GraphicsSystem* graphicsSystem, const std::filesystem::path& path);
    virtual ~Material();

    VulkanMaterial* GetVulkanMaterial() { return _material.get(); }
    const VulkanPipeline* GetVulkanPipeline();

    Ref<MaterialInstance> CreateInstance();

    void SetBool(const std::string& name, bool value);
    void SetInteger(const std::string& name, int value);
    void SetScaler(const std::string& name, float value);
    void SetVector2(const std::string& name, glm::vec2 value);
    void SetVector3(const std::string& name, glm::vec3 value);
    void SetVector4(const std::string& name, glm::vec4 value);
    void SetMatrix(const std::string& name, glm::mat4 value);
    void SetSampledImage2D(const std::string& name, Ref<Sampler> sampler, Ref<Texture> image);
    void SetPushConstant(VulkanRenderData& rd, const std::string& name, const void* value);

    void Bind(VulkanRenderData& rd); /** @brief Bind this material for rending using it and it's data. */
    void PushConstant(VulkanRenderData& rd, uint32_t offset, uint32_t size, const void* value);
};

}