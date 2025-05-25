#include "VulkanMaterial.h"
#include "VulkanMaterialInstance.h"

namespace bl
{

VulkanMaterialInstance::VulkanMaterialInstance(VulkanDevice* device, VulkanMaterial* material)
    : _device(device)
    , _material(material)
    , _materialSet(material->_materialSet)
    , _currentFrame(0)
{
}

VulkanMaterialInstance::~VulkanMaterialInstance() 
{
}

void VulkanMaterialInstance::SetBool(const std::string& name, bool value) 
{
    SetGenericUniform(name, value);
}

void VulkanMaterialInstance::SetInteger(const std::string& name, int value) 
{
    SetGenericUniform(name, value);
}

void VulkanMaterialInstance::SetScaler(const std::string& name, float value) 
{
    SetGenericUniform(name, value);
}

void VulkanMaterialInstance::SetVector2(const std::string& name, glm::vec2 value) 
{
    SetGenericUniform(name, value);
}

void VulkanMaterialInstance::SetVector3(const std::string& name, glm::vec3 value) 
{
    SetGenericUniform(name, value);
}

void VulkanMaterialInstance::SetVector4(const std::string& name, glm::vec4 value) 
{
    SetGenericUniform(name, value);
}

void VulkanMaterialInstance::SetMatrix(const std::string& name, glm::mat4 value) 
{
    SetGenericUniform(name, value);
}

void VulkanMaterialInstance::Bind(VulkanRenderData& rd) 
{
    PerFrameData& currentFrameData = _perFrameData[rd.currentFrame];

    // Compute the dynamic offsets for each uniform buffer.
    std::vector<uint32_t> offsets;
    for (const auto& binding : _bindings) 
    {
        if (binding.second.index() == 0) 
        {
            auto& variant = _bindings[binding.first];
            VulkanBuffer& buffer = std::get<VulkanBuffer>(variant);
            uint32_t blockSize = (uint32_t)buffer.GetSize() / _material->_imageCount;
            offsets.push_back(blockSize * rd.currentFrame);
        }
    }

    vkCmdBindPipeline(rd.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->_pipeline->GetPipeline());

    std::array<VkDescriptorSet, 2> descriptorSets{ rd.globalSet, currentFrameData.set };
    auto firstSet = rd.globalSet ? 0 : 1;
    auto descriptorSetCount = rd.globalSet ? 2 : 1;
    auto sets = rd.globalSet ? descriptorSets.data() : &descriptorSets[1];
    vkCmdBindDescriptorSets(rd.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->_pipeline->GetPipelineLayout(), firstSet, descriptorSetCount, sets, (uint32_t)offsets.size(), offsets.data());

    // Save the next frame number for when updating what bindings are dirty/not updated.
    _currentFrame = (rd.currentFrame + 1) % _material->_imageCount;
}

void VulkanMaterialInstance::SetSampledImage2D(const std::string& name, VulkanSampler* sampler, VulkanImage* image) 
{
    const auto& samplers = _material->GetSamplers();
    if (!samplers.contains(name)) 
    {
        blError("VulkanMaterial does not contain sampler \"{}\"!", name);
        return;
    }

    uint32_t binding = samplers.at(name);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.sampler = sampler->Get();
    imageInfo.imageView = image->GetView();
    imageInfo.imageLayout = image->GetLayout();

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstSet = _perFrameData[_currentFrame].set; /* Since this is getting added to the queue, every descriptor set will be updated later. */
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &imageInfo;
    write.pBufferInfo = nullptr;
    write.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(_device->Get(), 1, &write, 0, nullptr);
    SetBindingDirty(binding);
}

void VulkanMaterialInstance::PushConstant(VulkanRenderData& rd, uint32_t offset, uint32_t size, const void* data) 
{

    // Find the shader stage that uses the offset and size.
    const auto& pushConstantReflections = _material->_pipeline->GetReflection().GetReflectedPushConstants();

    auto it = std::find_if(pushConstantReflections.begin(), pushConstantReflections.end(), 
        [offset, size](const auto& pcr)
        {
            return pcr.Compare(offset, size);
        });

    if (it == pushConstantReflections.end()) 
    {
        blError("Push constant offset or size does not align to a push constant range in any stage!");
        return;
    }

    auto pcr = (*it);

    vkCmdPushConstants(rd.cmd, _material->_pipeline->GetPipelineLayout(), pcr.GetStages(), offset, size, data);
}

void VulkanMaterialInstance::UpdateUniforms() 
{
    uint32_t previousFrame = (_currentFrame - 1) % _material->_imageCount;

    PerFrameData& currentFrameData = _perFrameData[_currentFrame];
    PerFrameData& previousFrameData = _perFrameData[previousFrame];

    // If any previous frames changed their data this frame is dirty and must 
    // preform a descriptor copy to this frame.

    std::vector<VkCopyDescriptorSet> descriptorCopies;
    for (auto& binding : currentFrameData.dirty) 
    {
        // If second is marked as true, the binding is dirty and needs to be updated.
        if (binding.second) 
        {
            assert(_bindings.contains(binding.first) && "Binding not found, something in the constructor went wrong!");
            auto& variant = _bindings[binding.first];

            switch(variant.index()) 
            {
                case 0: 
                { // buffer type
                    VulkanBuffer& buffer = std::get<VulkanBuffer>(variant);
                    VkDeviceSize blockSize = buffer.GetSize() / _material->_imageCount;

                    void* mapped = nullptr;
                    buffer.Map(&mapped);

                    void* dstData = static_cast<intptr_t*>(mapped) + (blockSize * _currentFrame);
                    void* newData = static_cast<intptr_t*>(mapped) + (blockSize * previousFrame);

                    std::memcpy(dstData, newData, blockSize);

                    buffer.Unmap();
                    buffer.Flush(blockSize * _currentFrame, blockSize);
                    break;
                }
                case 1: 
                { // sampler type
                    VkCopyDescriptorSet descriptorCopy = {};
                    descriptorCopy.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
                    descriptorCopy.pNext = nullptr;
                    descriptorCopy.srcSet = previousFrameData.set;
                    descriptorCopy.srcBinding = binding.first;
                    descriptorCopy.srcArrayElement = 0;
                    descriptorCopy.dstSet = currentFrameData.set;
                    descriptorCopy.dstBinding = binding.first;
                    descriptorCopy.dstArrayElement = 0;
                    descriptorCopy.descriptorCount = 1;
                    descriptorCopies.push_back(descriptorCopy);
                    break;
                }
            }

            binding.second = false; // No longer dirty.
        }
    }

    if (!descriptorCopies.empty()) 
    {
        vkUpdateDescriptorSets(_device->Get(), 0, nullptr, (uint32_t)descriptorCopies.size(), descriptorCopies.data());
    }
}

void VulkanMaterialInstance::BuildPerFrameBindings(VkDescriptorSetLayout layout) 
{

    // Allocate the per frame descriptor sets.
    _perFrameData.resize(_material->_imageCount);
    for (uint32_t i = 0; i < _material->_imageCount; i++) 
    {
        _perFrameData[i].set = _material->_descriptorSetCache.Allocate(layout);
    }

    const auto& reflection = _material->_pipeline->GetReflection();
    const auto& sets = reflection.GetReflectedDescriptorSets();
    const auto& set = sets.at(_materialSet);

    VkDescriptorBufferInfo bufferInfo = {};
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;

    auto bindings = set.GetMetaBindings();

    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkWriteDescriptorSet> writes;

    for (const auto& binding : bindings) 
    {
        switch (binding.GetType()) 
        {
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: 
            {
                auto dynamicAlignment = CalculateDynamicAlignment(binding.GetSize());
                auto bufferSize = dynamicAlignment * _material->_imageCount;
                auto& variant = (_bindings[binding.GetLocation()] = VulkanBuffer{_device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, bufferSize, nullptr});

                bufferInfo.buffer = std::get<VulkanBuffer>(variant).Get();
                bufferInfo.offset = 0;
                bufferInfo.range = dynamicAlignment;
                bufferInfos.push_back(bufferInfo);

                write.dstBinding = binding.GetLocation();
                write.dstArrayElement = 0;
                write.descriptorCount = 1;
                write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                write.pBufferInfo = &bufferInfos.back();
                writes.push_back(write);
                break;
            }
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                _bindings[binding.GetLocation()] = VulkanMaterialInstance::SampledImage{};
                break; // An image/sampler must be written to before being rendering.
            default:
                break;
        }
    }

    for (uint32_t i = 0; i < _material->_imageCount; i++) 
    {
        for (auto& write_ : writes)
            write_.dstSet = _perFrameData[i].set;

        vkUpdateDescriptorSets(_device->Get(), (uint32_t)writes.size(), writes.data(), 0, nullptr);
    }
}

void VulkanMaterialInstance::SetBindingDirty(uint32_t binding) 
{
    assert(_bindings.contains(binding) && "Binding must exist to set it dirty!");

    _perFrameData[_currentFrame].dirty[binding] = false; /* This current frame is now the current data and is no longer dirty. */
    for (uint32_t i = (_currentFrame + 1) % _material->_imageCount; i != _currentFrame; i = (i + 1) % _material->_imageCount) 
    {
        _perFrameData[i].dirty[binding] = true;
    }
}

size_t VulkanMaterialInstance::CalculateDynamicAlignment(size_t uboSize) 
{
    size_t minUboAlignment = _device->GetPhysicalDevice()->GetProperties().limits.minUniformBufferOffsetAlignment;
    size_t dynamicAlignment = uboSize;

    if (minUboAlignment > 0)
        dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);

    return dynamicAlignment;
}

}