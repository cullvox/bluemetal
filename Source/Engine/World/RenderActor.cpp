#include "RenderActor.h"

namespace bl
{

RenderActor::RenderActor(
    const std::string& name,
    uint64_t uid,
    std::shared_ptr<GfxDevice> device)
{

    // Build the dynamic uniform buffer object.
    _objectDUBO = std::make_unique<GfxBuffer>()


    VkDescriptorBufferInfo bufferInfo {
        globalBuffer->getBuffer(),
        0,
        globalBuffer->getSize()
    };

    VkWriteDescriptorSet descWrite {
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        nullptr,
        VK_NULL_HANDLE,
        0,
        0,
        1,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        nullptr,
        &bufferInfo,
        nullptr
    };

}

} // namespace bl