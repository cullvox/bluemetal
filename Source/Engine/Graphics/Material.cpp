#include "Core/Print.h"
#include "Core/Memory.h"
#include "VulkanBuffer.h"
#include "Material.h"

namespace bl {

MaterialPipeline::MaterialPipeline(
    VulkanDevice* device, 
    VkRenderPass pass, 
    uint32_t subpass, 
    const VulkanPipelineStateInfo& state, 
    uint32_t materialSet = 1)
    : VulkanPipeline(device, )
{

}

~MaterialPipeline();

// ========== MaterialBase ========== //

MaterialInstance::MaterialInstance(
    VulkanDevice* device,
    Material* material)
    : _device(device)
    , _material(material)
    , _materialSet(material->_materialSet)
    , _currentFrame(material->_currentFrame)
{
}

MaterialInstance::~MaterialInstance() {}

void MaterialInstance::SetBool(const std::string& name, bool value) 
{
    SetGenericUniform(name, value);
}

void MaterialInstance::SetInteger(const std::string& name, int value) 
{
    SetGenericUniform(name, value);
}

void MaterialInstance::SetScaler(const std::string& name, float value) 
{
    SetGenericUniform(name, value);
}

void MaterialInstance::SetVector2(const std::string& name, glm::vec2 value) 
{
    SetGenericUniform(name, value);
}

void MaterialInstance::SetVector3(const std::string& name, glm::vec3 value) 
{
    SetGenericUniform(name, value);
}

void MaterialInstance::SetVector4(const std::string& name, glm::vec4 value) 
{
    SetGenericUniform(name, value);
}

void MaterialInstance::SetMatrix(const std::string& name, glm::mat4 value) 
{
    SetGenericUniform(name, value);
}

void MaterialInstance::Bind(VulkanRenderData& rd) 
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
            uint32_t blockSize = (uint32_t)buffer.GetSize() / VulkanConfig::numFramesInFlight;
            offsets.push_back(blockSize * rd.currentFrame);
        }
    }

    vkCmdBindPipeline(rd.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->GetPipeline());
    vkCmdBindDescriptorSets(rd.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _material->GetPipelineLayout(), 1, 1, &currentFrameData.set, (uint32_t)offsets.size(), offsets.data());

    // Save the next frame number for when updating what bindings are dirty/not updated.
    _currentFrame = (rd.currentFrame + 1) % VulkanConfig::numFramesInFlight;
}

void MaterialInstance::SetSampledImage2D(const std::string& name, VulkanSampler* sampler, VulkanImage* image) 
{
    
    const auto& samplers = _material->GetSamplers();
    if (!samplers.contains(name)) 
    {
        blError("Material does not contain sampler \"{}\"!", name);
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

void MaterialInstance::PushConstant(VulkanRenderData& rd, uint32_t offset, uint32_t size, const void* data) 
{

    // Find the shader stage that uses the offset and size.
    const auto& pushConstantReflections = _material->GetReflection().GetReflectedPushConstants();
    
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

    vkCmdPushConstants(rd.cmd, _material->GetPipelineLayout(), pcr.GetStages(), offset, size, data);
}

void MaterialInstance::UpdateUniforms() 
{
    uint32_t previousFrame = (_currentFrame - 1) % VulkanConfig::numFramesInFlight;

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
                    VkDeviceSize blockSize = buffer.GetSize() / VulkanConfig::numFramesInFlight;

                    void* mapped = nullptr;
                    buffer.Map(&mapped);

                    void* dstData = static_cast<intptr_t*>(mapped) + (blockSize * _currentFrame);
                    void* newData = static_cast<intptr_t*>(mapped) + (blockSize * previousFrame);

                    std::memcpy(dstData, newData, blockSize);

                    buffer.Unmap();
                    buffer.Flush(blockSize * _currentFrame, blockSize);
                } break;
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

void MaterialInstance::BuildPerFrameBindings(VkDescriptorSetLayout layout) 
{

    // Allocate the per frame descriptor sets.
    for (uint32_t i = 0; i < VulkanConfig::numFramesInFlight; i++) 
    {
        _perFrameData[i].set = _material->GetDescriptorSetCache()->Allocate(layout);
    }

    const auto& reflection = _material->GetReflection();
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
                auto bufferSize = dynamicAlignment * VulkanConfig::numFramesInFlight;
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
            } break;

            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: 
            {
                _bindings[binding.GetLocation()] = MaterialInstance::SampledImage{};
            } break; // An image/sampler must be written to before being rendering.

            default:
                break;
        }
    }

    for (uint32_t i = 0; i < VulkanConfig::numFramesInFlight; i++) 
    {
        for (auto& write_ : writes)
            write_.dstSet = _perFrameData[i].set;

        vkUpdateDescriptorSets(_device->Get(), (uint32_t)writes.size(), writes.data(), 0, nullptr);
    }
}

void MaterialInstance::SetBindingDirty(uint32_t binding) 
{
    assert(_bindings.contains(binding) && "Binding must exist to set it dirty!");

    _perFrameData[_currentFrame].dirty[binding] = false; /* This current frame is now the current data and is no longer dirty. */
    for (uint32_t i = (_currentFrame + 1) % VulkanConfig::numFramesInFlight; i != _currentFrame; i = (i + 1) % VulkanConfig::numFramesInFlight) 
    {
        _perFrameData[i].dirty[binding] = true;
    }
}

size_t MaterialInstance::CalculateDynamicAlignment(size_t uboSize) 
{
    size_t minUboAlignment = _device->GetPhysicalDevice()->GetProperties().limits.minUniformBufferOffsetAlignment;
    size_t dynamicAlignment = uboSize;

    if (minUboAlignment > 0)
        dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);

    return dynamicAlignment;
}

// ========== Material ========== //

Material::Material(VulkanDevice* device, VkRenderPass pass, uint32_t subpass, const VulkanPipelineStateInfo& state, uint32_t materialSet)
    : MaterialPipeline(device, pass, subpass, state, materialSet)
    , MaterialInstance(device, this)
    , _descriptorSetCache(device, 1024, VulkanDescriptorRatio::Default()) 
{

    // Get the descriptor set metadata and dedicated set for this material.
    VulkanReflectedPipeline reflection = {state.stages};

    auto& sets = reflection.GetReflectedDescriptorSets();
    if (!sets.contains(materialSet))
        throw std::runtime_error("Material does not contain the used set!");

    auto& meta = sets.at(materialSet);

    // Ensure that all uniform buffers are actually dynamic uniform buffers.
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
    BuildPerFrame(GetDescriptorSetLayout());

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

const std::map<std::string, VulkanBlockVariable>& Material::GetUniformMetadata() {
    return _uniformMetadata;
}

const std::map<std::string, uint32_t>& Material::GetSamplerMetadata() {
    return _samplerMetadata;
}

VulkanPipeline* Material::GetPipeline() {
    return _pipeline.get();
}

// ========== MaterialInstance ========== //

MaterialInstance::MaterialInstance(Material* material)
    : Material(material) {
    
    BuildMaterialData(material->_layout);
}

MaterialInstance::~MaterialInstance() {}

const std::map<std::string, VulkanBlockVariable>& MaterialInstance::GetUniformMetadata() {
    return _material->GetUniformMetadata();
}

const std::map<std::string, uint32_t>& MaterialInstance::GetSamplerMetadata() {
    return _material->GetSamplerMetadata();
}

VulkanPipeline* MaterialInstance::GetPipeline() {
    return _material->GetPipeline();
}

} // namespace bl
