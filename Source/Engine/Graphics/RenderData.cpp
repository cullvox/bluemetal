#include "RenderData.h"
#include "Renderer.h"
#include "Resources/MaterialInstance.h"
#include "Resources/Mesh.h"

namespace bl {

#define MAX_INSTANCE_BUFFER_SIZE 1024

RenderData::RenderData(Renderer* renderer)
    : _cmd(VK_NULL_HANDLE)
    , _currentFrame(0)
    , _imageIndex(0)
    , _globalSet(VK_NULL_HANDLE)
    , _descriptorCache(renderer->GetDevice(), 10, VulkanDescriptorRatio::Default())
{
    _tempInstances.reserve(MAX_INSTANCE_BUFFER_SIZE);
    _instanceToCallMap.reserve(MAX_INSTANCE_BUFFER_SIZE);
    _instances.reserve(MAX_INSTANCE_BUFFER_SIZE);

    _instanceBuffer = VulkanBufferFrameRing{renderer->GetDevice(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, renderer->GetSwapchainImageCount(), MAX_INSTANCE_BUFFER_SIZE * sizeof(glm::mat4), false};

    std::array<VkDescriptorSetLayoutBinding, 1> instanceBindings = {
        {0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr}
    };

    _instanceSetLayout = renderer->GetDevice()->AcquireDescriptorSetLayout(instanceBindings);
    _instanceSet = _descriptorCache.Allocate(_instanceSetLayout);

    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = _instanceBuffer.GetBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range = MAX_INSTANCE_BUFFER_SIZE * sizeof(glm::mat4);

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstSet = _instanceSet;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    write.pImageInfo = nullptr;
    write.pBufferInfo = &bufferInfo;
    write.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(renderer->GetDevice()->Get(), 1, &write, 0, nullptr);
}

void RenderData::SetCommandBuffer(VkCommandBuffer cmd)
{
    _cmd = cmd;
}

void RenderData::SetCurrentFrame(uint32_t currentFrame)
{
    _currentFrame = currentFrame;
}

void RenderData::SetImageIndex(uint32_t index)
{
    _imageIndex = index;
}

void RenderData::SetGlobalDescriptorSet(VkDescriptorSet set)
{
    _globalSet = set;
}


VkCommandBuffer RenderData::GetCommandBuffer()
{
    return _cmd;
}

uint32_t RenderData::GetCurrentFrame()
{
    return _currentFrame;
}

uint32_t RenderData::GetImageIndex()
{
    return _imageIndex;
}

VkDescriptorSet RenderData::GetGlobalDescriptorSet()
{
    return _globalSet;
}

VkDescriptorSet RenderData::GetInstanceDescriptorSet()
{
    return _instanceSet;
}

void RenderData::DrawInstance(MaterialInstance* material, Mesh* mesh, const InstanceData& instance)
{
    for (int i = 0; i < _calls.size(); i++) {
        auto& call = _calls[i];
        if (call.material == call.material && call.mesh == mesh) {
            _tempInstances.push_back(instance);
            _instanceToCallMap.push_back(i);
            return;
        }
    }

    _calls.emplace_back(material, mesh);
    uint32_t lastCall = _calls.size() - 1;
    _tempInstances.push_back(instance);
    _instanceToCallMap.push_back(lastCall);
}

void RenderData::DrawCustom(std::function<void (RenderData& rd)> renderData)
{
}

void RenderData::WriteInstanceBuffer()
{
    // Sort instances into the proper buffer areas.
    _instances.clear();

    if (_tempInstances.empty())
        return;

    for (int i = 0; i < _tempInstances.size(); i++) {
        InstanceData& instance = _tempInstances[i];
        uint32_t callIndex = _instanceToCallMap[i];
        _instances.push_back(instance);

        DrawCall& call = _calls[callIndex];
        if (call.instanceCount == 0) {
            call.instanceOffset = i;
        }

        call.instanceCount++;
    }

    _tempInstances.clear();

    // Upload instances to the staging buffer.
    _instanceBuffer.Upload(_cmd, std::as_bytes(std::span(_instances)), _currentFrame);
}

uint32_t RenderData::GetInstanceBufferDynamicOffset()
{
    return _instanceBuffer.GetDynamicOffset(_currentFrame);
}

void RenderData::WriteDrawCommands()
{
    // Perform draw commands.
    MaterialInstance* prevMaterial = nullptr;
    Mesh* prevMesh = nullptr;
    for (int i = 0; i < _calls.size(); i++) {
        DrawCall& call = _calls[i];

        if (prevMaterial != call.material)
            call.material->Bind(*this);

        if (prevMesh != call.mesh)
            call.mesh->Bind(_cmd);

        vkCmdDrawIndexed(_cmd, call.mesh->GetIndicesCount(), call.instanceCount, 0, 0, call.instanceOffset);
    }

}

} // namespace bl