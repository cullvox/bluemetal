#include "Material/UniformData.h"
#include "RenderActor.h"

namespace bl
{

RenderActor::RenderActor(const std::string& name, uint64_t uid, Device* device)
    : Actor(name, uid)
{

    // Build the dynamic uniform buffer object.
    _objectDUBO = std::make_unique<Buffer>(
        device, 
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        sizeof(ObjectUBO) * GraphicsConfig::numFramesInFlight);

    // Allocate the object descriptor sets.


    // Update the descriptor sets.
    std::array<VkDescriptorBufferInfo, GraphicsConfig::numFramesInFlight> bufferInfos{};
    std::array<VkWriteDescriptorSet, GraphicsConfig::numFramesInFlight> descWrites{};

    for (uint32_t i = 0; i < GraphicsConfig::numFramesInFlight; i++)
    {
        bufferInfos[i] = VkDescriptorBufferInfo{
            _objectDUBO->Get(),
            sizeof(ObjectUBO) * i,
            sizeof(ObjectUBO)
        };

        descWrites[i] = VkWriteDescriptorSet{
            VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            nullptr,
            VK_NULL_HANDLE,
            0,
            0,
            1,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            nullptr,
            &bufferInfos[i],
            nullptr
        };
    }

    vkUpdateDescriptorSets(device->Get(), GraphicsConfig::numFramesInFlight, descWrites.data(), 0, nullptr);
}

void RenderActor::UpdateObjectUBO(int currentFrame)
{
    // Update the UBO data.
    _objectData.model = transform.GetMatrix();

    // Copy the UBO data to the GPU.
    VkBufferCopy copy{
        0,
        currentFrame * sizeof(ObjectUBO),
        sizeof(ObjectUBO)
    };

    _objectDUBO->Upload(copy, &_objectData);
}

} // namespace bl