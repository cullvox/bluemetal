#pragma once

#include "Resource/Resource.h"
#include "Renderer.h"
#include "VulkanMaterial.h"

namespace bl
{

class Material : public Resource
{
public:
    Material(ResourceManager* manager, const nlohmann::json& data, VulkanDevice* device, Renderer* renderer);
    ~Material();

    virtual void Load() override;
    virtual void Unload() override;

    void SetBool(const std::string& name, bool value);
    void SetInteger(const std::string& name, int value);
    void SetScaler(const std::string& name, float value);
    void SetVector2(const std::string& name, glm::vec2 value);
    void SetVector3(const std::string& name, glm::vec3 value);
    void SetVector4(const std::string& name, glm::vec4 value);
    void SetMatrix(const std::string& name, glm::mat4 value);
    void SetSampledImage2D(const std::string& name, VulkanSampler* sampler, VulkanImage* image);
    void UpdateUniforms(); /** @brief This function must be called before the renderer starts rendering the frame. */

    void Bind(VulkanRenderData& rd); /** @brief Bind this material for rending using it and it's data. */
    void PushConstant(VulkanRenderData& rd, uint32_t offset, uint32_t size, const void* value);

    VulkanMaterial* GetMaterial() { return _material.get(); }
    VulkanPipeline* GetPipeline();

private:
    Renderer* _renderer;
    VulkanDevice* _device;
    std::unique_ptr<VulkanMaterial> _material;
};

}