#pragma once

#include "MaterialInstance.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanMaterial.h"

namespace bl {

class Material : public MaterialInstance {
public:
    Material(const std::filesystem::path& path);
    virtual ~Material();

    VulkanMaterial* GetMaterial() { return _material.get(); }
    const VulkanPipeline* GetPipeline();

    void SetBool(const std::string& name, bool value);
    void SetInteger(const std::string& name, int value);
    void SetScaler(const std::string& name, float value);
    void SetVector2(const std::string& name, glm::vec2 value);
    void SetVector3(const std::string& name, glm::vec3 value);
    void SetVector4(const std::string& name, glm::vec4 value);
    void SetMatrix(const std::string& name, glm::mat4 value);
    void SetSampledImage2D(const std::string& name, Ref<Sampler> sampler, Ref<Texture> image);
    void SetPushConstant(VulkanRenderData& rd, const std::string& name, const void* value);
private:
    Renderer* _renderer;
    VulkanDevice* _device;
    std::unique_ptr<VulkanMaterial> _material;
};

}