#include "RenderData.h"
#include "Renderer.h"
#include "Resources/MaterialInstance.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "Graphics/VulkanMaterial.h"
#include "Resources/Mesh.h"
#include "Core/Profiler.h"

namespace bl {

#define MAX_INSTANCE_BUFFER_SIZE 8192

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

    _instanceBuffer = VulkanBufferFrameRing{renderer->GetDevice(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, VulkanConfig::maxFramesInFlight, MAX_INSTANCE_BUFFER_SIZE * sizeof(InstanceData), false};

    std::array<VkDescriptorSetLayoutBinding, 1> instanceBindings = {
        {0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr}
    };

    _instanceSetLayout = renderer->GetDevice()->AcquireDescriptorSetLayout(instanceBindings);
    _instanceSet = _descriptorCache.Allocate(_instanceSetLayout);

    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = _instanceBuffer.GetBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range = MAX_INSTANCE_BUFFER_SIZE * sizeof(InstanceData);

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

static Profiler profiler;

uint32_t HashDrawCall(MaterialInstance* material, Mesh* mesh)
{
    size_t h1 = std::hash<MaterialInstance*>{}(material);
    size_t h2 = std::hash<Mesh*>{}(mesh);

    return static_cast<uint32_t>(h1 ^ (h2 << 1));
}

void RenderData::DrawInstance(Node* node, MaterialInstance* material, Mesh* mesh, const glm::mat4& instance)
{
    DrawCall newCall(material, mesh);
    newCall.offset = static_cast<uint32_t>(_instances.size());
    newCall.count = 1;
    _calls.push_back(newCall);

    _instances.push_back(instance);
}

void RenderData::DrawMultiInstance(Node* node, MaterialInstance* material, Mesh* mesh, const std::span<glm::mat4> instances)
{
    DrawCall call{material, mesh};
    call.offset = static_cast<uint32_t>(_instances.size());
    call.count = static_cast<uint32_t>(instances.size());
    _calls.push_back(call);

    std::copy_n(instances.begin(), std::min(_instances.capacity(), instances.size()), std::back_inserter(_instances));
}

void RenderData::WriteInstanceBuffer()
{
    // Sort instances into the proper buffer areas.
    profiler.StartProfile("Upload Instance Data");
    // Upload instances to the staging buffer.
    _instanceBuffer.Upload(_cmd, std::as_bytes(std::span(_instances)), _currentFrame);
    profiler.EndProfile("Upload Instance Data");
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

        bool shouldInstance = call.count > 1;
        bool materialSupportsInstancing = (call.material->GetInstance()->GetBaseMaterial()->GetSupportFlags() & VulkanMaterialSupportFlags::eInstanceBuffer) != VulkanMaterialSupportFlags::eNone;

        ObjectPC objectPC;
        if (shouldInstance && materialSupportsInstancing) {
            objectPC.useInstanceBuffer.x = 1;
        } else {
            objectPC.data.model = _instances[call.offset];
            objectPC.useInstanceBuffer.x = 0;
        }

        objectPC.objectID = i;

        call.material->PushConstant(*this, 0, sizeof(ObjectPC), &objectPC);
        vkCmdDrawIndexed(_cmd, call.mesh->GetIndicesCount(), call.count, 0, 0, call.offset);
    }
}


void RenderData::WriteDrawCommands(VulkanMaterialInstance* instance)
{
    // Perform draw commands.
    instance->Bind(*this);

    Mesh* prevMesh = nullptr;
    for (int i = 0; i < _calls.size(); i++) {
        DrawCall& call = _calls[i];

        if (prevMesh != call.mesh)
            call.mesh->Bind(_cmd);
    
        ObjectPC objectPC;
        objectPC.useInstanceBuffer.x = 1;
        objectPC.objectID = i;

        instance->PushConstant(*this, 0, sizeof(ObjectPC), &objectPC);
        vkCmdDrawIndexed(_cmd, call.mesh->GetIndicesCount(), call.count, 0, 0, call.offset);
    }
}

void RenderData::SetSampleCount(VkSampleCountFlagBits sampleCount)
{
    _sampleCount = sampleCount;
}

VkSampleCountFlagBits RenderData::GetSampleCount()
{
    return _sampleCount;
}

void RenderData::Reset()
{
    _calls.clear();
    _instanceToCallMap.clear();
    _instances.clear();
}

} // namespace bl