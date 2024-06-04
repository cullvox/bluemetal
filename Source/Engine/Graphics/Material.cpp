#include "Core/Print.h"
#include "Material.h"

namespace bl
{

Material::Material(Device* device, Renderer* renderer, RenderPass* pass, uint32_t subpass, const PipelineStateInfo& state, uint32_t materialSet)
    : _device(device)
{
    
    // Create the material pipeline.
    _pipeline = std::make_unique<Pipeline>(_device, pass, subpass, state);

    // Get the descriptor set dedicated to this material.
    const auto& sets = _pipeline->GetDescriptorSetMetadata();

    if (!sets.contains(materialSet))
        throw std::runtime_error("Material does not contain the used set!");

    const auto& meta = sets.at(materialSet);

    // Build the uniform ranges for each uniform buffer 
    auto metaBindings = meta.GetMetaBindings();

    for (const auto& binding : metaBindings) {
        switch (binding.GetType()) {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
            auto blocks = binding.GetMembers(); 
            for (const auto& variable : blocks) {
                _uniforms[variable.GetName()] = variable;
            }
            break;
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
            
            break;
        }
    }

}


void Material::SetBoolean(const std::string& name, bool value)
{
    if (!_uniformMetadata.contains(name))
    {
        blError("Could not set material uniform as it does not exist!");
        return;
    }

    auto meta = _uniformMetadata[name];

    VkBufferCopy copy = {};
    copy.srcOffset = 
    copy.dstOffset

    SetBindingDirty(meta.GetBinding()); /* The buffer data must be copied to the other parts of the dynamic uniform buffer. */
}

void Material::SetInteger(const std::string& name, int value)
{

}

void Material::SetScaler(const std::string& name, float value)
{

}

void Material::SetVector2(const std::string& name, glm::vec2 value)
{

}

void Material::SetVector3(const std::string& name, glm::vec3 value)
{

}

void Material::SetVector4(const std::string& name, glm::vec4 value)
{

}

void Material::SetMatrix(const std::string& name, glm::mat4 value)
{

}

void Material::Bind(uint32_t currentFrame) {
    uint32_t previousFrame = currentFrame = (currentFrame - 1) % GraphicsConfig::numFramesInFlight;
    
    PerFrameData& data = _perFrameData[currentFrame];
    PerFrameData& prevData = _perFrameData[previousFrame];

    // If any previous frames changed their data this frame is dirty and must 
    // preform a descriptor copy to this frame.
    if (data.isDirty) {
        VkCopyDescriptorSet copy = {};
        copy.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
        copy.pNext = nullptr;
        copy.srcSet = prevData.set;
        copy.dstSet = data.set;
        copy.
    }
}

void Material::SetImage2D(const std::string& name, Sampler* sampler, Image* image)
{
    if (!_samplerMetadata.contains(name)) {
        blError("Material does not contain {} sampler!", name);
        return;
    }

    uint32_t binding = _samplerMetadata[name];

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.sampler = sampler->Get();
    imageInfo.imageView = image->GetView();
    imageInfo.imageLayout = image->GetLayout();
    _imageInfos.push_back(imageInfo);

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstSet = VK_NULL_HANDLE; /* Since this is getting added to the queue, every descriptor set will be updated later. */
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &_imageInfos.back();
    write.pBufferInfo = nullptr;
    write.pTexelBufferView = nullptr;
    _writes.push_back(write);
}

void Material::BuildPerFrameData()
{
    
}

void Material::SetBindingDirty(uint32_t binding)
{
    for (PerFrameData& data : _perFrameData) {
        if (data.buffers.contains(binding)) {
            data.buffers[binding].isDirty = true;
        } else if (data.buffers.contains(binding)) {
            data.samplers[binding].isDirty = true;
        }
    }
}

} // namespace bl
