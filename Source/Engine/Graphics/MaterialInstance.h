#pragma once

#include "Precompiled.h"
#include "Material.h"
#include "Buffer.h"

namespace bl
{

/** @brief */
class MaterialInstance : public Resource
{
public:
    constexpr static inline uint32_t MAX_PUSH_CONSTANT_SIZE = 128;

    MaterialInstance(Device* _device, Material* material);
    ~MaterialInstance();

    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVector2b(const std::string& name, glm::bvec2 value);
    void SetVector2i(const std::string& name, glm::ivec2 value);
    void SetVector2(const std::string& name, glm::vec2 value);
    void SetVector3b(const std::string& name, glm::bvec3 value);
    void SetVector3i(const std::string& name, glm::ivec3 value);
    void SetVector3(const std::string& name, glm::vec3 value);
    void SetVector4b(const std::string& name, glm::bvec4 value);
    void SetVector4i(const std::string& name, glm::ivec4 value);
    void SetVector4(const std::string& name, glm::vec4 value);
    void SetMatrix2(const std::string& name, glm::mat2 value);
    void SetMatrix3(const std::string& name, glm::mat3 value);
    void SetMatrix4(const std::string& name, glm::mat4 value);

private:
    void CreateBuffers();
    void UpdateDescriptorBindings();

    Device* _device;
    Material* _material;
    VkDescriptorSet _set;
    std::unordered_map<std::string, Buffer> _uniformBuffers;
};

} // namespace bl