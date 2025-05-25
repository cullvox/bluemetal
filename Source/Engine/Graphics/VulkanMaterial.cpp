#include "Core/Print.h"
#include "Core/Memory.h"
#include "VulkanBuffer.h"
#include "VulkanMaterial.h"

namespace bl 
{


VulkanMaterial::VulkanMaterial(VulkanDevice* device, VkRenderPass pass, uint32_t subpass, const VulkanPipelineStateInfo& state, uint32_t imageCount, uint32_t materialSet)
    : VulkanMaterialInstance(device, this)
    , _imageCount(imageCount)
    , _descriptorSetCache(device, 1024, VulkanDescriptorRatio::Default()) 
{
    _materialSet = materialSet;

    // Preform reflection on the pipeline shaders to retrieve detailed descriptor set info.
    VulkanReflectedPipeline reflection = {state.stages};

    auto& sets = reflection.GetReflectedDescriptorSets();
    if (!sets.contains(materialSet))
        throw std::runtime_error("VulkanMaterial does not contain the used set!");

    auto& meta = sets.at(materialSet);

    // Ensure that all uniform buffers are actually dynamic uniform buffers.
    // This allows for us to use one buffer and swap between areas of it.
    auto& bindings = meta.GetBindings();

    for (auto& pair : bindings) 
    {
        auto binding = pair.second.GetBinding();

        if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) 
        {
            pair.second.SetType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
        }
    }

    // Construct the pipeline.
    _pipeline = std::make_unique<VulkanPipeline>(device, state, pass, subpass, &reflection);

    const auto& pipelineDescriptorSetLayouts = _pipeline->GetDescriptorSetLayouts();
    _layout = pipelineDescriptorSetLayouts.at(materialSet);

    BuildPerFrameBindings(_layout);

    // Create buffers/sampler uniform data.
    auto metaBindings = meta.GetMetaBindings();
    for (const auto& binding : metaBindings) 
    {
        switch (binding.GetType()) 
        {
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: 
            {
                auto blocks = binding.GetMembers();

                // Get each uniform members block for its offsets.
                for (const auto& variable : blocks) 
                {
                    _uniforms[variable.GetName()] = variable;
                }
                break;
            }
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                _samplers[binding.GetName()] = binding.GetLocation();
                break;
            default:
                break;
        }
    }
}

VulkanMaterial::~VulkanMaterial()
{
}

} // namespace bl
