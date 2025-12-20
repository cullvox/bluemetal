#pragma once

#include <vector>
#include <map>
#include <set>
#include <functional>

#include "Vulkan.h"
#include "UniformData.h"

namespace bl
{

class MaterialInstance;
class Mesh;
class Renderer;

class RenderData
{
    VkCommandBuffer cmd;
    uint32_t currentFrame;
    uint32_t imageIndex;
    VkDescriptorSet globalSet;

    struct DrawCall {
        int count;
        std::vector<InstanceData> instances;
    };

    using DrawKey = std::pair<MaterialInstance*, Mesh*>;
    std::map<DrawKey, DrawCall> _calls;

public:
    RenderData(Renderer* renderer);

    void SetCommandBuffer(VkCommandBuffer cmd);
    void IncrementCurrentFrame();
    void SetImageIndex(uint32_t index);
    void SetGlobalDescriptorSet(VkDescriptorSet set);

    VkCommandBuffer GetCommandBuffer();
    uint32_t GetCurrentFrame();
    uint32_t GetImageIndex();
    VkDescriptorSet GetGlobalDescriptorSet();

    void DrawInstance(MaterialInstance* material, Mesh* mesh, const InstanceData& instance);
    void DrawCustom(std::function<void (RenderData& rd)> renderData);

    /**
     * @brief Sorts calls and executes them to the command buffer.
     */
    void WriteDrawCommands();
};

}