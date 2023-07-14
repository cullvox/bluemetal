#pragma once

#include "Render/Precompiled.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Shader.hpp"
#include "Render/RenderPass.hpp"

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


class BLUEMETAL_RENDER_API blPipeline
{
public:
    blPipeline(blRenderDevice& renderDevice, const std::vector<std::shared_ptr<blShader>>& shaders, blRenderPass& renderPass, uint32_t subpass = 0);
    ~blPipeline() noexcept;

    vk::PipelineLayout getPipelineLayout() const noexcept;
    vk::Pipeline getPipeline() const noexcept;
    void getDescriptorSetLayouts(std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts) const noexcept;
    void getUniformReflection(std::vector<blUniformReflection>& uniformReflection) const noexcept; // materials will create buffers to specifications

private:
    void createDescriptorSetLayouts(const std::vector<std::shared_ptr<blShader>>& shaders);
    void createLayout();
    void createPipeline(const std::vector<std::shared_ptr<blShader>>& shaders);

    blRenderDevice&                             _renderDevice;
    blRenderPass&                               _renderPass;
    uint32_t                                    _subpass;
    std::vector<vk::UniqueDescriptorSetLayout>  _setLayouts;
    vk::UniquePipelineLayout                    _pipelineLayout;
    vk::UniquePipeline                          _pipeline;
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