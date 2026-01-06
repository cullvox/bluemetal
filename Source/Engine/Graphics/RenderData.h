#pragma once

#include <vector>
#include <functional>

#include "Vulkan.h"
#include "VulkanBufferFrameRing.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "UniformData.h"

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