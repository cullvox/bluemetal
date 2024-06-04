#pragma once

#include "Device.h"
#include "Buffer.h"
#include "Pipeline.h"
#include "Image.h"
#include "Sampler.h"

namespace bl
{

class MaterialInstance;

static inline const uint32_t MaxPushConstantSize = 128;

class MaterialBase
{
public:
    MaterialBase();
    ~MaterialBase();



private:
    VkDescriptorSet _set;
    std::unordered_map<uint32_t, Buffer> _buffers; /** @brief Key is the binding index. */
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
    Material(Device* device, Renderer* renderer, RenderPass* pass, uint32_t subpass, const PipelineStateInfo& state, uint32_t materialSet = 1);
    ~Material();

    void SetBoolean(const std::string& name, bool value);
    void SetInteger(const std::string& name, int value);
    void SetScaler(const std::string& name, float value);
    void SetVector2(const std::string& name, glm::vec2 value);
    void SetVector3(const std::string& name, glm::vec3 value);
    void SetVector4(const std::string& name, glm::vec4 value);
    void SetMatrix(const std::string& name, glm::mat4 value);
    void SetImage2D(const std::string& name, Image* image, Sampler* sampler = nullptr);

    void Bind(uint32_t currentFrame);
    Pipeline* GetPipeline() const;

private:
    void BuildPerFrameData();
    void SetBindingDirty(uint32_t binding);

    struct BufferData
    {
        bool isDirty; /** @brief If any data was changed from frame to frame all descriptor data is copied.  */
        Buffer buffer;
    };

    struct ImageData
    {
        bool isDirty; /** @brief If any data was changed from frame to frame all descriptor data is copied.  */
        Sampler* sampler;
        Image* image;
    };

    struct PerFrameData
    {
        VkDescriptorSet set;
        std::unordered_map<uint32_t, BufferData> buffers;
        std::unordered_map<uint32_t, ImageData> samplers;
    };

    Device* _device;
    std::unique_ptr<Pipeline> _pipeline;
    std::unordered_map<std::string, BlockVariable> _uniformMetadata; 
    std::unordered_map<std::string, uint32_t> _samplerMetadata; /** @brief Name -> Binding */

    std::vector<VkDescriptorImageInfo> _imageInfos;
    std::vector<VkWriteDescriptorSet> _writes;
    
    std::vector<std::function<void(void)>> _preBindUpdates;
    std::array<PerFrameData, GraphicsConfig::numFramesInFlight> _perFrameData;
};

} // namespace bl