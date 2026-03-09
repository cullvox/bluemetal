#include "RenderData.h"
#include "Renderer.h"
#include "Graphics/VulkanMaterial.h"
#include "Graphics/VulkanMesh.h"
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

    _instanceBuffer = VulkanBufferFrameRing{renderer->GetDevice(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, MAX_INSTANCE_BUFFER_SIZE * sizeof(InstanceData)};

    std::array<VkDescriptorSetLayoutBinding, 1> instanceBindings = {
        {0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr}
    };

    _instanceSetLayout = renderer->GetDevice()->AcquireDescriptorSetLayout(instanceBindings);

    std::array<VkDescriptorBufferInfo, VulkanConfig::maxFramesInFlight> descriptorBufferInfos;
    std::array<VkWriteDescriptorSet, VulkanConfig::maxFramesInFlight> descriptorWrites;

    for (int i = 0; i < VulkanConfig::maxFramesInFlight; i++)
    {
        _instanceSets[i] = _descriptorCache.Allocate(_instanceSetLayout);

        descriptorBufferInfos[i] = _instanceBuffer.GetDescriptorInfo(i);
        descriptorWrites[i] = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = _instanceSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pImageInfo = nullptr,
            .pBufferInfo = &descriptorBufferInfos[i],
            .pTexelBufferView = nullptr,
        };
    }

    vkUpdateDescriptorSets(renderer->GetDevice()->Get(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

    CreateDebugBuffer(renderer->GetDevice());
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
    return _instanceSets[_currentFrame];
}

static Profiler profiler;

void RenderData::DrawInstance(Node* node, const VulkanMaterialInstance* material, const VulkanMesh* mesh, const glm::mat4& instance)
{
    DrawCall newCall(material, mesh);
    newCall.offset = static_cast<uint32_t>(_instances.size());
    newCall.count = 1;
    _calls.push_back(newCall);

    _instances.push_back(instance);
}

void RenderData::DrawMultiInstance(Node* node, const VulkanMaterialInstance* material, const VulkanMesh* mesh, const std::span<glm::mat4> instances)
{
    DrawCall call{material, mesh};
    call.offset = static_cast<uint32_t>(_instances.size());
    call.count = static_cast<uint32_t>(instances.size());
    _calls.push_back(call);

    std::copy_n(instances.begin(), std::min(_instances.capacity(), instances.size()), std::back_inserter(_instances));
}

void RenderData::WriteInstanceBuffer()
{
    // Upload instances to the staging buffer.
    _instanceBuffer.Upload(std::as_bytes(std::span(_instances)), _currentFrame);
}

void RenderData::WriteDrawCommands()
{
    // Perform draw commands.
    const VulkanMaterialInstance* material = nullptr;
    const VulkanMesh* mesh = nullptr;
    for (int i = 0; i < _calls.size(); i++) {
        DrawCall& call = _calls[i];

        if (material != call.material) {
            material = call.material;
            material->Bind(*this);
        }

        if (mesh != call.mesh) {
            mesh = call.mesh;
            mesh->Bind(_cmd);
        }

        bool shouldInstance = call.count > 1;

        ObjectPC objectPC;
        if (shouldInstance) {
            objectPC.useInstanceBuffer.x = 1;
        } else {
            objectPC.data.model = _instances[call.offset];
            objectPC.useInstanceBuffer.x = 0;
        }

        objectPC.objectID = i;

        material->PushConstant(*this, 0, sizeof(ObjectPC), &objectPC);

        vkCmdDrawIndexed(_cmd, mesh->GetIndicesCount(), call.count, 0, 0, call.offset);
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

    _points.clear();
    _lines.clear();
    _triangles.clear();
}

void RenderData::DrawPoint(const glm::vec3& point, float size, Color color)
{
    _points.emplace_back(point, color.ToVector3(), 0.0f);
}

void RenderData::DrawLine(const glm::vec3& a, const glm::vec3& b, float thickness, Color color)
{
    _lines.emplace_back(a, color.ToVector3(), 0.0f);
    _lines.emplace_back(b, color.ToVector3(), 0.0f);
}

void RenderData::DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float thickness, Color color)
{
    _triangles.emplace_back(a, color.ToVector3(), 0.0f);
    _triangles.emplace_back(b, color.ToVector3(), 0.0f);
    _triangles.emplace_back(c, color.ToVector3(), 0.0f);
}

#define MAX_DEBUG_VERTICES 8196

void RenderData::CreateDebugBuffer(VulkanDevice* device)
{
    _lines.reserve(MAX_DEBUG_VERTICES / 3);
    _points.reserve(MAX_DEBUG_VERTICES / 3);
    _triangles.reserve(MAX_DEBUG_VERTICES / 3);
    _debugVertices.reserve(MAX_DEBUG_VERTICES);
    _debugBuffer = VulkanBufferFrameRing(device, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, MAX_DEBUG_VERTICES * sizeof(VertexDebug));
}

void RenderData::UpdateDebugBuffers()
{
    _debugVertices.clear();

    std::copy_n(_points.begin(), std::min(_debugVertices.capacity(), _points.size()), std::back_inserter(_debugVertices));
    std::copy_n(_lines.begin(), std::min(_debugVertices.capacity(), _lines.size()), std::back_inserter(_debugVertices));
    std::copy_n(_triangles.begin(), std::min(_debugVertices.capacity(), _triangles.size()), std::back_inserter(_debugVertices));

    // Update this current frames buffer.
    _debugBuffer.Upload(std::as_bytes(std::span { _debugVertices }), _currentFrame);
}

void RenderData::DrawDebugBuffers(RenderData& rd)
{
    // Draw the points list
    if (_points.empty() && _lines.empty() && _triangles.empty())
        return;

    auto cmd = rd.GetCommandBuffer();
    VkBuffer buffer = _debugBuffer.GetBuffer(rd.GetCurrentFrame());
    VkDeviceSize vertexOffset = 0;

    if (_pointMaterial != nullptr && _points.size() > 0) {
        _pointMaterial->Bind(rd);
        vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &vertexOffset);
        vkCmdDraw(cmd, static_cast<uint32_t>(_points.size()), 1, 0, 0);
    }

    // Draw the lines list
    uint32_t firstVertex = static_cast<uint32_t>(_points.size());
    if (_lineMaterial != nullptr && _lines.size() > 0) {
        _lineMaterial->Bind(rd);
        vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &vertexOffset);
        vkCmdSetLineWidth(cmd, 3.0f);
        vkCmdDraw(cmd, static_cast<uint32_t>(_lines.size()), 1, firstVertex, 0);
    }

    // Draw the trangles list
    firstVertex += static_cast<uint32_t>(_lines.size());
    if (_triangleMaterial != nullptr && _triangles.size() > 0) {
        _triangleMaterial->Bind(rd);
        vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &vertexOffset);
        vkCmdDraw(cmd, static_cast<uint32_t>(_triangles.size()), 1, firstVertex, 0);
    }

    _points.clear();
    _lines.clear();
    _triangles.clear();
}


} // namespace bl