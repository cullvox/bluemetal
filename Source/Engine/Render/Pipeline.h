#pragma once

#include "Device.h"
#include "Shader.h"
#include "RenderPass.h"

enum class blUniformType
{
    eUniformBuffer,
    eSampler,
    eSamplerArray,
};

enum class blUniformMemberType
{
    eScalar,
    eVector, // glsl all vectors are internally vec4
    eMatrix3,
    eMatrix4,
};

struct blUniformMemberReflection
{
    size_t              size;
    blUniformMemberType type;
    std::string         name;
};

struct blUniformReflection
{
    size_t                                  size;
    blUniformType                           type;
    std::string                             name;
    std::vector<blUniformMemberReflection>  members;
};

class BLUEMETAL_API blPipeline
{
public:
    explicit blPipeline(std::shared_ptr<blDevice> device, const std::vector<std::shared_ptr<blShader>>& shaders, std::shared_ptr<blRenderPass> renderPass, uint32_t subpass = 0);
    ~blPipeline() noexcept;

    VkPipelineLayout getPipelineLayout();
    VkPipeline getPipeline();
    std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts();
    std::vector<blUniformReflection> getUniformReflection(); // materials will create buffers to specifications

private:
    void createDescriptorSetLayouts(const std::vector<std::shared_ptr<blShader>>& shaders);
    void createLayout();
    void createPipeline(const std::vector<std::shared_ptr<blShader>>& shaders);

    std::shared_ptr<blDevice>           _device;
    std::shared_ptr<blRenderPass>       _renderPass;
    uint32_t                            _subpass;
    std::vector<VkDescriptorSetLayout>  _setLayouts;
    VkPipelineLayout                    _pipelineLayout;
    VkPipeline                          _pipeline;
};


/* Getting real with descriptor sets.

    Descriptor Slot: 0
        Reserved for global data of the frame.
        
        This Includes
            * Current Time
            * Camera MVP
            * Resolution
            * Texture Atlas
            * ... (TBD)
    
    Descriptor Slot: 1
        Reserved for material data. Will change multiple times per frame. Each
        material instance's data will be uploaded here.

        Material data is custom to the shader and pipelines will agree to what
        ever the shader says because of reflection. The material system will 
        also take from the pipelines reflection and build a list of values that
        can be changed at runtime using this descriptor index.

        Pipelines are essentially here on the order.

    Descriptor Slot: 2
        Reserved for object data. This will change many times during the runtime
        of a singe frame. 

        This Includes:
            * Object/Mesh Transform
            * ... (TBD)

    Descriptor Slot: 3
        To be determined, might be for instanced data and/or textures?
        Instanced data might need to go on slot 2.

*/