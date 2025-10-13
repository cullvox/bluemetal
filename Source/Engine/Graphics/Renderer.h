#pragma once

#include <nlohmann/json.hpp>

#include "VulkanDevice.h"
#include "VulkanImage.h"
#include "VulkanDescriptorSetAllocatorCache.h"
#include "VulkanWindow.h"
#include "VulkanMaterial.h"
#include "VulkanRenderData.h"

namespace bl 
{

using RenderFunction = std::function<void(VulkanRenderData& rd)>;

class Material;

enum class RenderPassType : uint32_t
{
    eGeometry = 0,
    eUI = 2,
};

class Renderer 
{
public:
    Renderer(VulkanWindow* window); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    uint32_t GetSwapchainImageCount() { return _imageCount; }
    uint32_t GetNextFrameIndex(); /** @brief Returns the circular frame index from zero to GraphicsConfig::numFramesInFlight - 1. */
    void Render(RenderFunction func);
    std::tuple<VkRenderPass, uint32_t> GetRenderPass(RenderPassType pass) const;

protected:
    friend class VulkanMaterial;
    void AddMaterial(VulkanMaterial* material);
    void RemoveMaterial(VulkanMaterial* material);

private:
    void CreateSyncObjects();
    void DestroySyncObjects();
    void CreateRenderPasses();
    void DestroyRenderPasses();
    void DestroyImagesAndFramebuffers();
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

    std::unordered_set<VulkanMaterial*> _materials;
};

NLOHMANN_JSON_SERIALIZE_ENUM(RenderPassType, {
    {RenderPassType::eGeometry, "geometry"},
    {RenderPassType::eLighting, "lighting"},
    {RenderPassType::eUI, "ui"},
});

} // namespace bl
