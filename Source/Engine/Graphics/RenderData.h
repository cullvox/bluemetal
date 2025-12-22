#pragma once

#include <vector>
#include <map>
#include <set>
#include <functional>
#include <deque>

#include "Vulkan.h"
#include "VulkanBuffer.h"
#include "UniformData.h"
#include "VulkanDescriptorSetAllocatorCache.h"

namespace bl
{

class MaterialInstance;
class Mesh;
class Renderer;

class RenderData
{
    VkCommandBuffer _cmd;
    uint32_t _currentFrame;
    uint32_t _imageIndex;
    VkDescriptorSet _globalSet;

    std::vector<glm::mat4> _tempInstances;
    std::vector<uint32_t> _instanceToCallMap;
    std::vector<glm::mat4> _instances;
    VulkanBuffer _stagingBuffer; // Dynamic buffer with per frame offsets.
    void* _stagingBufferMap;
    VulkanBuffer _instanceBuffer; // Dynamic buffer with per frame offsets.
    VulkanDescriptorSetAllocatorCache _descriptorCache;
    VkDescriptorSetLayout _instanceSetLayout;
    VkDescriptorSet _instanceSet;


    struct DrawCall {
        DrawCall(MaterialInstance* material, Mesh* mesh)
            : material(material)
            , mesh(mesh)
        {
        }

        MaterialInstance* material;
        Mesh* mesh;

        uint32_t instanceOffset = 0;
        uint32_t instanceCount = 0;
    };

    std::vector<DrawCall> _calls;

public:
    RenderData(Renderer* renderer);

    void SetCommandBuffer(VkCommandBuffer cmd);
    void SetCurrentFrame(uint32_t currentFrame);
    void SetImageIndex(uint32_t index);
    void SetGlobalDescriptorSet(VkDescriptorSet set);

    VkCommandBuffer GetCommandBuffer();
    uint32_t GetCurrentFrame();
    uint32_t GetImageIndex();
    VkDescriptorSet GetGlobalDescriptorSet();
    VkDescriptorSet GetInstanceDescriptorSet();
    uint32_t GetInstanceBufferDynamicOffset();

    void DrawInstance(MaterialInstance* material, Mesh* mesh, const InstanceData& instance);
    void DrawCustom(std::function<void (RenderData& rd)> renderData);

    void WriteInstanceBuffer();

    /**
     * @brief Sorts calls and executes them to the command buffer.
     */
    void WriteDrawCommands();
};

}