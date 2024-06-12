#pragma once

#include "Math/Math.h"
#include "Device.h"
#include "Buffer.h"
#include "Pipeline.h"
#include "Image.h"
#include "Sampler.h"

namespace bl
{

class MaterialBase
{
public:
    MaterialBase(Device* device, uint32_t materialSet);
    virtual ~MaterialBase();

    void SetBoolean(const std::string& name, bool value);
    void SetInteger(const std::string& name, int value);
    void SetScaler(const std::string& name, float value);
    void SetVector2(const std::string& name, glm::vec2 value);
    void SetVector3(const std::string& name, glm::vec3 value);
    void SetVector4(const std::string& name, glm::vec4 value);
    void SetMatrix(const std::string& name, glm::mat4 value);
    void SetSampledImage2D(const std::string& name, Sampler* sampler, Image* image);
    void Bind(VkCommandBuffer cmd, uint32_t currentFrame); /** @brief Bind this material for rending using it and it's data. */
    void PushConstant(VkCommandBuffer cmd, uint32_t offset, uint32_t size, const void* value);
    void UpdateUniforms(); /** @brief Call before the render pass renders. */

protected:
    virtual const std::map<std::string, BlockVariable>& GetUniformMetadata() = 0;
    virtual const std::map<std::string, uint32_t>& GetSamplerMetadata() = 0;
    virtual Pipeline* GetPipeline() = 0;

    void BuildMaterialData(VkDescriptorSetLayout layout);
    void SetBindingDirty(uint32_t binding);
    template<typename T> 
    void SetGenericUniform(const std::string& name, T value);
    size_t CalculateDynamicAlignment(size_t uboSize);

    using SampledImage = std::pair<Sampler*, Image*>;
    // using BufferData = std::pair<Buffer, std::unique_ptr<void*, decltype(&bl::AlignedFree)>>;

    struct PerFrameData {
        VkDescriptorSet set;
        std::map<uint32_t, bool> dirty;
    };

    uint32_t _materialSet;
    uint32_t _currentFrame;
    std::map<uint32_t, std::variant<Buffer, SampledImage>> _data;
    std::array<PerFrameData, GraphicsConfig::numFramesInFlight> _perFrameData;

private:
    Device* _device;
};

/** @brief Shader uniforms merged together in an uncomfortably useful unison.
 * 
 * A brief explanation of descriptor sets in materials.
 *
 * Materials are limited to using descriptor set index one. Therefore
 * bindings and all material data are all inside of one set and limited.
 * Many material instances can be created from one material allowing
 * variation with material uniforms in a simple and effectively understandable
 * way.
 *
 * The renderer is supposed to handle descriptor sets zero and two for 
 * global and instance rendering data respectively. 
 * 
 */
class Material : public MaterialBase
{
public:
    Material(Device* device, RenderPass* pass, uint32_t subpass, const PipelineStateInfo& state, uint32_t materialSet = 1);
    ~Material();

    virtual Pipeline* GetPipeline();

protected:
    friend class MaterialInstance;
    virtual const std::map<std::string, BlockVariable>& GetUniformMetadata();
    virtual const std::map<std::string, uint32_t>& GetSamplerMetadata();
    
private:
    Device* _device;
    std::map<std::string, BlockVariable> _uniformMetadata; 
    std::map<std::string, uint32_t> _samplerMetadata; /** @brief Name -> Binding */
    VkDescriptorSetLayout _layout;
    std::unique_ptr<Pipeline> _pipeline;
};

class MaterialInstance : public MaterialBase {
public:
    MaterialInstance(Material* material);
    ~MaterialInstance();

protected:
    virtual const std::map<std::string, BlockVariable>& GetUniformMetadata();
    virtual const std::map<std::string, uint32_t>& GetSamplerMetadata();
    virtual Pipeline* GetPipeline();

private:
    Material* _material;
};

template<typename T>
void MaterialBase::SetGenericUniform(const std::string& name, T value)
{
    if (!GetUniformMetadata().contains(name))
    {
        blError("Could not set material uniform as it does not exist!");
        return;
    }

    auto meta = GetUniformMetadata().at(name);
    assert(sizeof(T) == meta.GetSize() && "Type must be the same as the uniform size!");

    auto& buffer = std::get<Buffer>(_data[meta.GetBinding()]);
    VkDeviceSize blockSize = buffer.GetSize() / GraphicsConfig::numFramesInFlight;
    // uint32_t previousFrame = (_currentFrame - 1) % GraphicsConfig::numFramesInFlight;

    auto offset = (blockSize * _currentFrame) + meta.GetOffset();

    char* uniform = nullptr;
    buffer.Map((void**)&uniform);
    uniform += offset;
    std::memcpy(uniform, &value, sizeof(T));
    buffer.Unmap();

    buffer.Flush(offset, sizeof(T));

    SetBindingDirty(meta.GetBinding()); /* The buffer data must be copied to the other parts of the dynamic uniform buffer. */
}

} // namespace bl