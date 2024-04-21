#include "Material/UniformData.h"
#include "RenderActor.h"

namespace bl
{

RenderActor::RenderActor(
    const std::string&          name,
    uint64_t                    uid,
    std::shared_ptr<GfxDevice>  device)
    : Actor(name, uid)
{

    // Build the dynamic uniform buffer object.
    _objectDUBO = std::make_unique<GfxBuffer>(
        device, 
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        sizeof(ObjectUBO) * GfxLimits::maxFramesInFlight);

    // Allocate the object descriptor sets.


    // Update the descriptor sets.
    std::array<VkDescriptorBufferInfo, GfxLimits::maxFramesInFlight> bufferInfos{};
    std::array<VkWriteDescriptorSet, GfxLimits::maxFramesInFlight> descWrites{};

    for (uint32_t i = 0; i < GfxLimits::maxFramesInFlight; i++)
    {
        bufferInfos[i] = VkDescriptorBufferInfo{
            _objectDUBO->getBuffer(),
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

    vkUpdateDescriptorSets(device->get(), GfxLimits::maxFramesInFlight, descWrites.data(), 0, nullptr);
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

    _objectDUBO->upload(copy, &_objectData);
}

} // namespace bl