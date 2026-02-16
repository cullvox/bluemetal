#pragma once

#include <vector>
#include <functional>

#include "Core/Color.h"
#include "Vulkan.h"
#include "VulkanBufferFrameRing.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "UniformData.h"

namespace bl
{

class MaterialInstance;
class VulkanMaterialInstance;
class Mesh;
class Renderer;
class Node;

class RenderData
{
    VkCommandBuffer _cmd;
    uint32_t _currentFrame;
    uint32_t _imageIndex;
    VkDescriptorSet _globalSet;
    VkSampleCountFlagBits _sampleCount;
    VkImageView _swapchainImageView;
    uint32_t _nodeID;

    std::vector<InstanceData> _tempInstances;
    std::vector<uint32_t> _instanceToCallMap;
    std::vector<InstanceData> _instances;
    VulkanBufferFrameRing _instanceBuffer;
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
        uint32_t hash;
        uint32_t nodeID;
        uint32_t offset = 0;
        uint32_t count = 0;
    };

    std::vector<DrawCall> _calls;

public:
    RenderData(Renderer* renderer);

    void SetCommandBuffer(VkCommandBuffer cmd);
    void SetCurrentFrame(uint32_t currentFrame);
    void SetImageIndex(uint32_t index);
    void SetGlobalDescriptorSet(VkDescriptorSet set);
    void SetSampleCount(VkSampleCountFlagBits sampleCount);
    void SetSwapchainImageView(VkImageView swapchainImageView);

    VkCommandBuffer GetCommandBuffer();
    uint32_t GetCurrentFrame();
    uint32_t GetImageIndex();
    VkSampleCountFlagBits GetSampleCount();
    VkImageView GetSwapchainImageView();
    VkDescriptorSet GetGlobalDescriptorSet();
    VkDescriptorSet GetInstanceDescriptorSet();
    uint32_t GetInstanceBufferDynamicOffset();

    virtual void DrawInstance(Node* node, MaterialInstance* material, Mesh* mesh, const InstanceData& instance);

    void WriteInstanceBuffer();

    /**
     * @brief Sorts calls and executes them to the command buffer.
     */
    virtual void WriteDrawCommands();

    void WriteDrawCommands(VulkanMaterialInstance* material);
    void Reset();
};

}