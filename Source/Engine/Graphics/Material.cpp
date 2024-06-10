#include "Core/Print.h"
#include "Material.h"

namespace bl {

// ========== MaterialBase ========== //

MaterialBase::MaterialBase(Device* device, uint32_t materialSet)
    : _materialSet(materialSet)
    , _currentFrame(0)
    , _device(device)  {}

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
            assert(_data.contains(binding.first) && "Binding not found, something in the constructor went wrong!");
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

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline()->Get());
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

void MaterialBase::PushConstant(VkCommandBuffer cmd, uint32_t offset, uint32_t size, const void* data) {

    // Find the shader stage that uses the offset and size.
    auto pushConstantReflections = GetPipeline()->GetReflection().GetPushConstantReflections();
    
    auto it = std::find_if(pushConstantReflections.begin(), pushConstantReflections.end(), [offset, size](const auto& pcr){
                    return pcr.Compare(offset, size);
                });

    if (it == pushConstantReflections.end()) {
        blError("Push constant offset or size does not align to a push constant range in any stage!");
        return;
    }

    auto pcr = (*it);

    vkCmdPushConstants(cmd, GetPipeline()->GetLayout(), pcr.GetStages(), offset, size, data);
}

void MaterialBase::BuildMaterialData(VkDescriptorSetLayout layout) {

    // Allocate the per frame descriptor sets.
    for (uint32_t i = 0; i < GraphicsConfig::numFramesInFlight; i++) {
        _perFrameData[i].set = _device->AllocateDescriptorSet(layout);
    }

    const auto& reflection = GetPipeline()->GetReflection();
    const auto& sets = reflection.GetDescriptorSetReflections();
    const auto& set = sets.at(_materialSet);

    VkDescriptorBufferInfo bufferInfo = {};
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;

    auto bindings = set.GetMetaBindings();

    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkWriteDescriptorSet> writes;

    for (const auto& binding : bindings) {
        switch(binding.GetType()) {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: {
            auto dynamicAlignment = CalculateDynamicAlignment(binding.GetSize());
            auto& variant = (_data[binding.GetLocation()] = Buffer{_device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, dynamicAlignment * GraphicsConfig::numFramesInFlight, nullptr});

            bufferInfo.buffer = std::get<Buffer>(variant).Get();
            bufferInfo.offset = 0;
            bufferInfo.range = dynamicAlignment;
            bufferInfos.push_back(bufferInfo);

            write.dstBinding = binding.GetLocation();
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            write.pBufferInfo = &bufferInfos.back();
            writes.push_back(write);
        } break;
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            _data[binding.GetLocation()] = SampledImage{};
            // An image/sampler must be written before the first material bind!
            break;
        default: 
            break;
        }
    }
    
    for (uint32_t i = 0; i < GraphicsConfig::numFramesInFlight; i++) {        
        for (auto& write : writes)
            write.dstSet = _perFrameData[i].set;

        vkUpdateDescriptorSets(_device->Get(), (uint32_t)writes.size(), writes.data(), 0, nullptr);
    }

    
}

void MaterialBase::SetBindingDirty(uint32_t binding) {
    assert(_data.contains(binding) && "Binding must exist to set it dirty!");

    for (uint32_t i = (_currentFrame + 1) % GraphicsConfig::numFramesInFlight; i != _currentFrame; i = (i + 1) % GraphicsConfig::numFramesInFlight) {
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
    : MaterialBase(device, materialSet)
    , _device(device) {

    // Get the descriptor set metadata and dedicated set for this material.
    
    PipelineReflection reflection{state};

    auto& sets = reflection.GetDescriptorSetReflections();
    if (!sets.contains(materialSet))
        throw std::runtime_error("Material does not contain the used set!");

    auto& meta = sets.at(materialSet);

    // Ensure that all uniform buffers are actually dynamic uniform buffers.
    auto& bindings = meta.GetBindings();
    for (auto& pair : bindings) {
        auto binding = pair.second.GetBinding();
        if (binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            pair.second.SetType(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
        }
    }

    // Construct the pipeline.
    _pipeline = std::make_unique<Pipeline>(_device, pass, subpass, state, &reflection);
    
    _layout = _pipeline->GetDescriptorSetLayouts().at(materialSet);

    BuildMaterialData(_layout);

    // Create buffers/sampler uniform data.
    auto metaBindings = meta.GetMetaBindings();
    for (const auto& binding : metaBindings) {
        switch (binding.GetType()) {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: {
            auto blocks = binding.GetMembers();

            // Get each uniform members block for its offsets.
            for (const auto& variable : blocks) {
                _uniformMetadata[variable.GetName()] = variable;
            }
            break;
        }
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            _samplerMetadata[binding.GetName()] = binding.GetLocation();
            break;
        default:
            break;
        }
    }
}

Material::~Material() { }

const std::map<std::string, BlockVariable>& Material::GetUniformMetadata() {
    return _uniformMetadata;
}

const std::map<std::string, uint32_t>& Material::GetSamplerMetadata() {
    return _samplerMetadata;
}

Pipeline* Material::GetPipeline() {
    return _pipeline.get();
}

// ========== MaterialInstance ========== //

MaterialInstance::MaterialInstance(Material* material)
    : MaterialBase(material->_device, material->_materialSet) {
    BuildMaterialData(material->_layout);
}

MaterialInstance::~MaterialInstance() {}

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
