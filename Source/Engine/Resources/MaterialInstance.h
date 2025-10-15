#pragma once

#include "Math/Math.h"
#include "Resource.h"
#include "Sampler.h"
#include "Texture.h"
#include "Graphics/VulkanRenderData.h"

namespace bl 
{

class Material; // Material Resource

class MaterialInstance : public Resource 
{
    Ref<Material> material;

public:
    MaterialInstance();
    MaterialInstance(const std::filesystem::path& path);
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