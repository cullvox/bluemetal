#include "Core/Print.h"
#include "Material.h"

namespace bl {

// ========== MaterialBase ========== //

MaterialBase::MaterialBase(Device* device)
    : _device(device) {}

MaterialBase::~MaterialBase() {}

void MaterialBase::SetBoolean(const std::string& name, bool value) {
    SetGenericUniform(name, value);
}

void MaterialBase::SetInteger(const std::string& name, int value) {
    SetGenericUniform(name, value);
}

void MaterialBase::SetScaler(const std::string& name, float value) {
    SetGenericUniform(name, value);
}

void MaterialBase::SetVector2(const std::string& name, glm::vec2 value) {
    SetGenericUniform(name, value);
}

void MaterialBase::SetVector3(const std::string& name, glm::vec3 value) {
    SetGenericUniform(name, value);
}

void MaterialBase::SetVector4(const std::string& name, glm::vec4 value) {
    SetGenericUniform(name, value);
}

void MaterialBase::SetMatrix(const std::string& name, glm::mat4 value) {
    SetGenericUniform(name, value);
}

void MaterialBase::Bind(VkCommandBuffer cmd, uint32_t currentFrame) {
    uint32_t previousFrame = (currentFrame - 1) % GraphicsConfig::numFramesInFlight;
    
    PerFrameData& currentFrameData = _perFrameData[currentFrame];
    PerFrameData& previousFrameData = _perFrameData[previousFrame];

    // If any previous frames changed their data this frame is dirty and must 
    // preform a descriptor copy to this frame.

    std::vector<VkCopyDescriptorSet> descriptorCopies;
    for (auto& binding : currentFrameData.dirty) {
        // If second is marked as true, the binding is dirty and needs to be updated.
        if (binding.second) {
            assert(_data.contains(binding.first) && "Binding not found, creation went wrong!");
            auto& variant = _data[binding.first];

            switch(variant.index()) {
            case 0: { // buffer type
                Buffer& buffer = std::get<Buffer>(variant);
                VkDeviceSize blockSize = buffer.GetSize() / GraphicsConfig::numFramesInFlight;

                VkBufferCopy bufferCopy = {};
                bufferCopy.srcOffset = blockSize * currentFrame;
                bufferCopy.dstOffset = blockSize * previousFrame;
                bufferCopy.size = blockSize;

                vkCmdCopyBuffer(cmd, buffer.Get(), buffer.Get(), 1, &bufferCopy);
            } break;
            case 1: { // sampler type
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
            } break;
            }

            binding.second = false; // No longer dirty.
        }
    }

    if (!descriptorCopies.empty()) {
        vkUpdateDescriptorSets(_device->Get(), 0, nullptr, (uint32_t)descriptorCopies.size(), descriptorCopies.data());
    }

    // Compute the dynamic offsets for each uniform buffer.
    std::vector<uint32_t> offsets;
    for (const auto& binding : _data) {
        if (binding.second.index() == 0) {
            auto& variant = _data[binding.first];
            Buffer& buffer = std::get<Buffer>(variant);
            uint32_t blockSize = (uint32_t)buffer.GetSize() / GraphicsConfig::numFramesInFlight;
            offsets.push_back(blockSize * currentFrame);
        }
    }

    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline()->GetLayout(), 1, 1, &currentFrameData.set, (uint32_t)offsets.size(), offsets.data());

    // Save the next frame number for when updating what bindings are dirty/not updated.
    _currentFrame = (currentFrame + 1) % GraphicsConfig::numFramesInFlight;
}

void MaterialBase::SetSampledImage2D(const std::string& name, Sampler* sampler, Image* image) {
    if (!GetSamplerMetadata().contains(name)) {
        blError("Material does not contain sampler \"{}\"!", name);
        return;
    }

    uint32_t binding = GetSamplerMetadata().at(name);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.sampler = sampler->Get();
    imageInfo.imageView = image->GetView();
    imageInfo.imageLayout = image->GetLayout();

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstSet = VK_NULL_HANDLE; /* Since this is getting added to the queue, every descriptor set will be updated later. */
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

void MaterialBase::SetBindingDirty(uint32_t binding) {
    assert(_data.contains(binding) && "Binding must exist to set it dirty!");
    for (uint32_t i = _currentFrame; i != _currentFrame; i = (i + 1) % GraphicsConfig::numFramesInFlight) {
        _perFrameData[i].dirty[binding] = true;
    }
}

size_t MaterialBase::CalculateDynamicAlignment(size_t uboSize) {
    size_t minUboAlignment = _device->GetPhysicalDevice()->GetProperties().limits.minUniformBufferOffsetAlignment;
    size_t dynamicAlignment = uboSize;

    if (minUboAlignment > 0)
        dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);

    return dynamicAlignment;
}

// ========== Material ========== //

Material::Material(Device* device, RenderPass* pass, uint32_t subpass, const PipelineStateInfo& state, uint32_t materialSet)
    : Pipeline(device, pass, subpass, state)
    , MaterialBase(device) {
    // Get the descriptor set dedicated to this material.
    const auto& sets = GetDescriptorSetMetadata();

    if (!sets.contains(materialSet))
        throw std::runtime_error("Material does not contain the used set!");

    const auto& meta = sets.at(materialSet);

    auto metaBindings = meta.GetMetaBindings();

    for (const auto& binding : metaBindings) {
        switch (binding.GetType()) {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: {
            auto dynamicAlignment = CalculateDynamicAlignment(binding.GetSize());
            auto blocks = binding.GetMembers();

            // Create a buffer for this binding.
            _data[binding.GetLocation()] = Buffer{device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, dynamicAlignment * GraphicsConfig::numFramesInFlight, nullptr};

            // Get each uniform members block for its offsets.
            for (const auto& variable : blocks) {
                _uniformMetadata[variable.GetName()] = variable;
            }
            break;
        }
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            _samplerMetadata[binding.GetName()] = binding.GetLocation();
            _data[binding.GetLocation()] = SampledImage{};
            break;
        default:
            break;
        }
    }

}

Material::~Material() {}

const std::map<std::string, BlockVariable>& Material::GetUniformMetadata() {
    return _uniformMetadata;
}

const std::map<std::string, uint32_t>& Material::GetSamplerMetadata() {
    return _samplerMetadata;
}

Pipeline* Material::GetPipeline() {
    return static_cast<Pipeline*>(this);
}

// ========== MaterialInstance ========== //

const std::map<std::string, BlockVariable>& MaterialInstance::GetUniformMetadata() {
    return _material->GetUniformMetadata();
}

const std::map<std::string, uint32_t>& MaterialInstance::GetSamplerMetadata() {
    return _material->GetSamplerMetadata();
}

Pipeline* MaterialInstance::GetPipeline() {
    return _material->GetPipeline();
}

} // namespace bl
