#include "VulkanMaterialInstance.h"
#include "VulkanMaterial.h"
#include "UniformData.h"
#include "RenderData.h"

namespace bl {

VulkanMaterialInstance::VulkanMaterialInstance(VulkanDevice* device)
    : _device(device)
    , _currentFrame(0)
{
}

VulkanMaterialInstance::VulkanMaterialInstance(VulkanDevice* device, VulkanMaterial* material)
    : _device(device)
    , _material(material)
    , _materialSet(material->_materialSet)
    , _currentFrame(0)
{
    BuildPerFrameBindings(material->_layout);
}

VulkanMaterialInstance::~VulkanMaterialInstance()
{
}

void VulkanMaterialInstance::SetBool(const std::string& name, bool value)
{
    SetGenericUniform<uint32_t>(name, static_cast<uint32_t>(value));
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

void VulkanMaterialInstance::Bind(RenderData& rd)
{
    auto cmd = rd.GetCommandBuffer();
    auto currentFrame = rd.GetCurrentFrame();
    auto globalSet = rd.GetGlobalDescriptorSet();
    auto instanceSet = rd.GetInstanceDescriptorSet();
    PerFrameData& currentFrameData = _perFrameData[currentFrame];

    std::vector<uint32_t> offsets;

    // Compute the dynamic offsets for each uniform buffer.
    for (const auto& binding : _bindings) {
        if (binding.second.index() == 0) {
            auto& variant = _bindings[binding.first];
            VulkanBuffer& buffer = std::get<VulkanBuffer>(variant);
            uint32_t blockSize = static_cast<uint32_t>(buffer.GetSize()) / _material->_swapchainImageCount;
            offsets.push_back(blockSize * currentFrame);
        }
    }

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->_pipeline->GetPipeline());

    std::array<VkDescriptorSet, 3> descriptorSets { globalSet, currentFrameData.set, instanceSet };

    uint32_t firstSet = 0;
    std::span<VkDescriptorSet> sets;
    auto support = _material->GetSupportFlags();

    if ((support & VulkanMaterialSupportFlags::eGlobalBuffer) != VulkanMaterialSupportFlags::eNone) {
        firstSet = 0;
        sets = std::span<VkDescriptorSet>{descriptorSets.begin(), 2};

        if ((support & VulkanMaterialSupportFlags::eInstanceBuffer)  != VulkanMaterialSupportFlags::eNone) {
            sets = std::span<VkDescriptorSet>{descriptorSets.begin(), 3};
            offsets.push_back(rd.GetInstanceBufferDynamicOffset());
        }
    } else {
        firstSet = 1;
        sets = std::span<VkDescriptorSet>{descriptorSets.begin() + 1, 1};
    }

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->_pipeline->GetPipelineLayout(), firstSet, sets.size(), sets.data(), static_cast<uint32_t>(offsets.size()), offsets.data());

    // Save the next frame number for when updating what bindings are dirty/not updated.
    if (_currentFrame == currentFrame)
        _currentFrame = (currentFrame + 1) % _material->_swapchainImageCount;
}

void VulkanMaterialInstance::SetSampledImage2D(const std::string& name, VulkanSampler* sampler, VulkanImage* image)
{
    const auto& samplers = _material->GetSamplers();
    if (!samplers.contains(name)) {
        Print::Error("VulkanMaterial does not contain sampler \"{}\"!", name);
        return;
    }

    uint32_t binding = samplers.at(name);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.sampler = sampler->Get();
    imageInfo.imageView = image->GetDefaultView();
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

void VulkanMaterialInstance::PushConstant(RenderData& rd, uint32_t offset, uint32_t size, const void* data)
{

    // Find the shader stage that uses the offset and size.
    const auto& pushConstantReflections = _material->_pipeline->GetReflection().pushConstantMetadata;

    auto it = std::find_if(pushConstantReflections.begin(), pushConstantReflections.end(),
        [offset, size](const auto& pcr) {
            return pcr.Compare(offset, size);
        });

    if (it == pushConstantReflections.end()) {
        Print::Error("Push constant offset or size does not align to a push constant range in any stage!");
        return;
    }

    auto pcr = (*it);

    vkCmdPushConstants(rd.GetCommandBuffer(), _material->_pipeline->GetPipelineLayout(), pcr.GetStages(), offset, size, data);
}

void VulkanMaterialInstance::UpdateUniforms(VkCommandBuffer cmd)
{
    uint32_t previousFrame = (_currentFrame - 1) % _material->_swapchainImageCount;

    PerFrameData& currentFrameData = _perFrameData[_currentFrame];
    PerFrameData& previousFrameData = _perFrameData[previousFrame];

    // If any previous frames changed their data this frame is dirty and must
    // preform a descriptor copy to this frame.

    std::vector<VkCopyDescriptorSet> descriptorCopies;
    descriptorCopies.reserve(currentFrameData.dirty.size());

    for (auto& binding : currentFrameData.dirty) {
        // If second is marked as true, the binding is dirty and needs to be updated.
        if (binding.second) {
            assert(_bindings.contains(binding.first) && "Binding not found, something in the constructor went wrong!");
            auto& variant = _bindings[binding.first];

            switch (variant.index()) {
            case 0: // buffer type
            {
                VulkanBuffer& buffer = std::get<VulkanBuffer>(variant);
                VkDeviceSize blockSize = buffer.GetSize() / _material->_swapchainImageCount;

                VkBufferCopy copyRegion = {};
                copyRegion.srcOffset = blockSize * previousFrame;
                copyRegion.dstOffset = blockSize * _currentFrame;
                copyRegion.size = blockSize;

                vkCmdCopyBuffer(cmd, buffer.Get(), buffer.Get(), 1, &copyRegion);

                break;
            }
            case 1: // sampler type
            {
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

    if (!descriptorCopies.empty()) {
        vkUpdateDescriptorSets(_device->Get(), 0, nullptr, (uint32_t)descriptorCopies.size(), descriptorCopies.data());
    }
}

void VulkanMaterialInstance::BuildPerFrameBindings(VkDescriptorSetLayout layout)
{
    // Allocate the per frame descriptor sets.
    _perFrameData.resize(_material->_swapchainImageCount);
    for (uint32_t i = 0; i < _material->_swapchainImageCount; i++) {
        _perFrameData[i].set = _material->_descriptorSetCache.Allocate(layout);
    }

    const auto& reflection = _material->_pipeline->GetReflection();
    const auto& sets = reflection.descriptorSetMetadata;
    const auto& set = sets.at(_materialSet);

    VkDescriptorBufferInfo bufferInfo = {};
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;

    auto bindings = set.GetMetaBindings();

    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkWriteDescriptorSet> writes;
    bufferInfos.reserve(bindings.size());
    writes.reserve(bindings.size());

    for (const auto& binding : bindings) {
        switch (binding.GetType()) {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: {
            auto dynamicAlignment = _device->GetDynamicAlignment(binding.GetSize());
            auto bufferSize = dynamicAlignment * _material->_swapchainImageCount;
            auto& variant = (_bindings[binding.GetLocation()] = VulkanBuffer { _device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, bufferSize, nullptr });

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
            _bindings[binding.GetLocation()] = VulkanMaterialInstance::SampledImage {};
            break; // An image/sampler must be written to before being rendering.
        default:
            break;
        }
    }

    for (uint32_t i = 0; i < _material->_swapchainImageCount; i++) {
        for (auto& write_ : writes)
            write_.dstSet = _perFrameData[i].set;

        vkUpdateDescriptorSets(_device->Get(), (uint32_t)writes.size(), writes.data(), 0, nullptr);
    }
}

void VulkanMaterialInstance::SetBindingDirty(uint32_t binding)
{
    assert(_bindings.contains(binding) && "Binding must exist to set it dirty!");

    _perFrameData[_currentFrame].dirty[binding] = false; /* This current frame is now the current data and is no longer dirty. */
    for (uint32_t i = 0; i < _material->_swapchainImageCount; i++) {
        if (i != _currentFrame)
            _perFrameData[i].dirty[binding] = true;
    }
}

}