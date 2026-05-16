#include "VulkanMaterialInstance.h"
#include "VulkanMaterial.h"
#include "UniformData.h"
#include "RenderData.h"
#include "Renderer.h"

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

void VulkanMaterialInstance::Bind(RenderData& rd) const
{
    VkCommandBuffer cmd = rd.GetCommandBuffer();
    VkDescriptorSet globalSet = rd.GetGlobalDescriptorSet();
    VkDescriptorSet instanceSet = rd.GetInstanceDescriptorSet();
    VkPipeline pipeline = _material->_pipeline->GetPipeline(rd.GetSampleCount());
    VkPipelineLayout pipelineLayout = _material->_pipeline->GetPipelineLayout();

    auto& frame = _perFrameData[_currentFrame];
    auto materialSet = frame.set;

    std::array<VkDescriptorSet, 4> descriptorSets { globalSet, instanceSet, materialSet };

    auto support = _material->GetSupportFlags();
    std::span<VkDescriptorSet> sets = descriptorSets;

    // If any of the buffers aren't used, bind an empty set to ensure that the pipeline doesn't read from a random set.
    if ((support & VulkanMaterialSupportFlags::eGlobalBuffer) == VulkanMaterialSupportFlags::eNone) {
        descriptorSets[0] = _material->_emptySet;
    }

    if ((support & VulkanMaterialSupportFlags::eInstanceBuffer) == VulkanMaterialSupportFlags::eNone) {
        descriptorSets[1] = _material->_emptySet;
    }

    if ((support & VulkanMaterialSupportFlags::eMaterialBuffer) != VulkanMaterialSupportFlags::eMaterialBuffer) {
        descriptorSets[2] = _material->_emptySet;
    }

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, _material->_setCount, sets.data(), 0, nullptr);
}

void VulkanMaterialInstance::SetSampledImage2D(const std::string& name, VulkanSampler* sampler, VulkanImage* image)
{
    const auto& samplers = _material->GetSamplers();
    if (!samplers.contains(name)) {
        Print::Error("VulkanMaterial does not contain sampler \"{}\"!", name);
        return;
    }

    uint32_t binding = samplers.at(name);

    auto& uniform = std::get<SampledImageData>(_bindings[binding]);
    uniform.image = image;
    uniform.sampler = sampler;

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

void VulkanMaterialInstance::PushConstant(RenderData& rd, uint32_t offset, uint32_t size, const void* data) const
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

void VulkanMaterialInstance::UpdateUniforms(uint32_t currentFrame)
{
    if (_materialSet == -1) {
        return;
    }

    PerFrameData& currentFrameData = _perFrameData[currentFrame];

    // If any previous frames changed their data this frame is dirty and must
    // preform a descriptor copy to this frame.

    for (std::size_t i = 0; i < currentFrameData.dirty.size(); i++) {
        // If second is marked as true, the binding is dirty and needs to be updated.
        if (!currentFrameData.dirty[i]) {
            continue;
        }

        auto& variant = _bindings[i];

        switch (variant.index()) {
        case 0: { // buffer type
            UniformData& uniform = std::get<UniformData>(variant);

            // Update the uniform buffer
            uniform.buffer.Upload(uniform.data, currentFrame);
            break;
        }
        case 1: { // sampler type
            auto& uniform = std::get<SampledImageData>(variant);

            VkDescriptorImageInfo imageInfo = {};
            imageInfo.sampler = uniform.sampler->Get();
            imageInfo.imageView = uniform.image->GetDefaultView();
            imageInfo.imageLayout = uniform.image->GetLayout();

            VkWriteDescriptorSet write = {};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.pNext = nullptr;
            write.dstSet = _perFrameData[currentFrame].set; /* Since this is getting added to the queue, every descriptor set will be updated later. */
            write.dstBinding = i;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.pImageInfo = &imageInfo;
            write.pBufferInfo = nullptr;
            write.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(_device->Get(), 1, &write, 0, nullptr);
            break;
        }
        }

        currentFrameData.dirty[i] = false; // No longer dirty.
    }

    _currentFrame = currentFrame;
}

VulkanMaterial* VulkanMaterialInstance::GetBaseMaterial()
{
    return _material;
}

void VulkanMaterialInstance::BuildPerFrameBindings(VkDescriptorSetLayout layout)
{
    if (_materialSet == -1) {
        return;
    }

    // Allocate the per frame descriptor sets.
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++) {
        _perFrameData[i].set = _material->_descriptorSetCache.Allocate(layout);
    }

    const auto& reflection = _material->_pipeline->GetReflection();
    const auto& sets = reflection.descriptorSetMetadata;
    const auto& set = sets.at(_materialSet);

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;

    auto bindings = set.GetMetaBindings();

    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkWriteDescriptorSet> writes;
    bufferInfos.reserve(bindings.size() * VulkanConfig::maxFramesInFlight);
    writes.reserve(bindings.size() * VulkanConfig::maxFramesInFlight);

    for (const auto& binding : bindings) {
        switch (binding.GetType()) {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
            const auto bufferSize = binding.GetSize();

            UniformData data;
            data.buffer = VulkanBufferFrameRing { _device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, bufferSize };
            data.data.resize(binding.GetSize());

            auto& variant = (_bindings[binding.GetLocation()] = std::move(data) );

            write.dstBinding = binding.GetLocation();
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++)
            {
                bufferInfos.push_back(std::get<UniformData>(variant).buffer.GetDescriptorInfo(i));

                write.pBufferInfo = &bufferInfos.back();
                write.dstSet = _perFrameData[i].set;
                writes.push_back(write);
            }
            break;
        }
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            _bindings[binding.GetLocation()] = VulkanMaterialInstance::SampledImageData{};
            break; // An image/sampler must be written to before being rendering.
        default:
            break;
        }
    }

    vkUpdateDescriptorSets(_device->Get(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}

void VulkanMaterialInstance::SetBindingDirty(uint32_t binding)
{
    assert(_bindings.contains(binding) && "Binding must exist to set it dirty!");

    _perFrameData[_currentFrame].dirty[binding] = false; /* This current frame is now the current data and is no longer dirty. */
    for (uint32_t i = 0; i < VulkanConfig::maxFramesInFlight; i++) {
        _perFrameData[i].dirty[binding] = true;
    }
}

}