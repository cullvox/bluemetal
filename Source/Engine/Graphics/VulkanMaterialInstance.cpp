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

    std::vector<uint32_t> offsets;
    VkDescriptorSet currentSet = VK_NULL_HANDLE;
    if (_materialSet != -1) {
        PerFrameData& currentFrameData = _perFrameData[currentFrame];
        currentSet = currentFrameData.set;

        // assert(currentFrameData.dirty.none());

        // Compute the dynamic offsets for each uniform buffer.
        for (const auto& binding : _bindings) {
            if (binding.second.index() == 0) {
                auto& variant = _bindings[binding.first];
                UniformData& uniform = std::get<UniformData>(variant);
                uint32_t blockSize = static_cast<uint32_t>(uniform.buffer.GetSize()) / _material->_swapchainImageCount;
                offsets.push_back(blockSize * currentFrame);
            }
        }
    }

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->_pipeline->GetPipeline());

    std::array<VkDescriptorSet, 3> descriptorSets { globalSet, currentSet, instanceSet };

    uint32_t firstSet = 0;
    std::span<VkDescriptorSet> sets;
    auto support = _material->GetSupportFlags();

    if ((support & VulkanMaterialSupportFlags::eGlobalBuffer) != VulkanMaterialSupportFlags::eNone) {
        firstSet = 0;

        if (_materialSet == -1) {
            sets = std::span<VkDescriptorSet>{descriptorSets.begin(), 1};
        } else {
            sets = std::span<VkDescriptorSet>{descriptorSets.begin(), 2};
            if ((support & VulkanMaterialSupportFlags::eInstanceBuffer)  != VulkanMaterialSupportFlags::eNone) {
                sets = std::span<VkDescriptorSet>{descriptorSets.begin(), 3};
                offsets.push_back(rd.GetInstanceBufferDynamicOffset());
            }
        }
    } else {
        firstSet = 1;
        sets = std::span<VkDescriptorSet>{descriptorSets.begin() + 1, 1};
    }

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->_pipeline->GetPipelineLayout(), firstSet, static_cast<uint32_t>(sets.size()), sets.data(), static_cast<uint32_t>(offsets.size()), offsets.data());
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

void VulkanMaterialInstance::UpdateUniforms(uint32_t currentFrame)
{
    if (_materialSet == -1) {
        return;
    }

    uint32_t previousFrame = (static_cast<int>(currentFrame - 1) % static_cast<int>(_material->_swapchainImageCount) + static_cast<int>(_material->_swapchainImageCount)) % static_cast<int>(_material->_swapchainImageCount);

    PerFrameData& currentFrameData = _perFrameData[currentFrame];

    // If any previous frames changed their data this frame is dirty and must
    // preform a descriptor copy to this frame.

    for (int i = 0; i < currentFrameData.dirty.size(); i++) {
        // If second is marked as true, the binding is dirty and needs to be updated.
        if (!currentFrameData.dirty[i]) {
            continue;
        }

        auto& variant = _bindings[i];

        switch (variant.index()) {
        case 0: { // buffer type
            UniformData& uniform = std::get<UniformData>(variant);
            VkDeviceSize blockSize = uniform.buffer.GetSize() / _material->_swapchainImageCount;

            uintptr_t dstOffset = static_cast<uintptr_t>(blockSize) * currentFrame;

            // Update the uniform buffer
            char* mapped = nullptr;
            uniform.buffer.Map(reinterpret_cast<void**>(&mapped));

            std::memcpy(mapped + dstOffset, uniform.data.data(), blockSize);

            uniform.buffer.Unmap();
            uniform.buffer.Flush(dstOffset, blockSize);

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

            UniformData data;
            data.buffer = VulkanBuffer { _device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, bufferSize, nullptr };
            data.data.resize(binding.GetSize());

            auto& variant = (_bindings[binding.GetLocation()] = std::move(data) );

            bufferInfo.buffer = std::get<UniformData>(variant).buffer.Get();
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
            _bindings[binding.GetLocation()] = VulkanMaterialInstance::SampledImageData{};
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
        _perFrameData[i].dirty[binding] = true;
    }
}

}