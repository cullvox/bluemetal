#pragma once

#include <nlohmann/json.hpp>

#include "UniformData.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanImage.h"

namespace bl {

class Material;
class VulkanWindow;
class VulkanDevice;
class VulkanSwapchain;
class VulkanMaterialInstance;
struct VulkanRenderData;

using RenderFunction = std::function<void(VulkanRenderData& rd)>;

enum class RenderPassType : uint32_t {
    eGeometry = 0,
    eUI = 2,
};

class Renderer {
public:
    Renderer(VulkanWindow* window); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    uint32_t GetSwapchainImageCount() { return _imageCount; }
    uint32_t GetNextFrameIndex(); /** @brief Returns the circular frame index from zero to GraphicsConfig::numFramesInFlight - 1. */
    void Render(RenderFunction func);
    std::tuple<VkRenderPass, uint32_t> GetRenderPass(RenderPassType pass) const;

    void SetProjection(const glm::mat4& projection);
    void SetView(const glm::mat4& view);

protected:
    friend class Material;
    friend class MaterialInstance;
    void AddMaterial(VulkanMaterialInstance* instance);
    void RemoveMaterial(VulkanMaterialInstance* instance);

private:
    void CreateSyncObjects();
    void DestroySyncObjects();
    void CreateRenderPasses();
    void DestroyRenderPasses();
    void DestroyImagesAndFramebuffers();
    void CreateGlobalUniform();
    void DestroyGlobalUniform();
    void RecreateImages();

    VulkanDevice* _device;
    VulkanWindow* _window;
    VulkanSwapchain* _swapchain;

    // Frame Synchronization
    uint32_t _imageCount;
    uint32_t _imageIndex;
    uint32_t _currentFrame;
    std::vector<VkCommandBuffer> _commandBuffers;
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;

    // Render Pass Data
    VkFormat _depthFormat, _positionFormat;
    VkRenderPass _pass;
    std::vector<VulkanImage> _positionImages;
    std::vector<VulkanImage> _depthImages;
    std::vector<VkFramebuffer> _framebuffers;

    VulkanDescriptorSetAllocatorCache _descriptorSetCache;

    // Uniform data
    GlobalUBO _uboData;
    VkDescriptorSetLayout _globalLayout;
    std::array<VulkanBuffer, VulkanConfig::numFramesInFlight> _globalBuffer;
    std::array<VkDescriptorSet, VulkanConfig::numFramesInFlight> _globalSet;
    std::array<void*, VulkanConfig::numFramesInFlight> _globalBufferMap;
    float _prevTime;

    std::unordered_set<VulkanMaterialInstance*> _materials;
};

NLOHMANN_JSON_SERIALIZE_ENUM(RenderPassType, {
                                                 { RenderPassType::eGeometry, "geometry" },
                                                 { RenderPassType::eUI, "ui" },
                                             });

} // namespace bl
