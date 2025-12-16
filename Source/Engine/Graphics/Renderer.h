#pragma once

#include <nlohmann/json.hpp>

#include <unordered_set>

#include "UniformData.h"
#include "VulkanBuffer.h"

namespace bl {

class Material;
class Mesh;
class VulkanWindow;
class VulkanDevice;
class VulkanSwapchain;
class VulkanMaterialInstance;
class VulkanImageView;
class VulkanImage;
class VulkanDescriptorSetAllocatorCache;
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
    uint32_t GetNextFrameIndex(); /** @brief Returns the circular frame index from zero to GraphicsConfig::maxFramesInFlight - 1. */
    void Render(RenderFunction func);

    void SetProjection(const glm::mat4& projection);
    void SetView(const glm::mat4& view);
    void AddInstance(Mesh* mesh, const InstanceData& data);

    std::vector<VkPresentModeKHR> GetPresentModes();
    void SetPresentMode(VkPresentModeKHR mode);
    VkPresentModeKHR GetPresentMode() const;

    std::vector<VkSampleCountFlagBits> GetMultisampleCounts();
    void SetMultisampleCount(VkSampleCountFlagBits samples);
    VkSampleCountFlagBits GetMultisampleCount();

    std::vector<VkFormat> GetColorAttachmentFormats();
    VkFormat GetDepthAttachmentFormat();
    VkFormat GetStencilAttachmentFormat();

    void SetImageRecreateCallback(std::function<void()> onRecreate);

protected:
    friend class Material;
    friend class MaterialInstance;
    void AddMaterial(VulkanMaterialInstance* instance);
    void RemoveMaterial(VulkanMaterialInstance* instance);

private:
    void CreateSyncObjects();
    void DestroySyncObjects();
    void DestroyImagesAndFramebuffers();
    void CreateGlobalUniform();
    void DestroyGlobalUniform();
    void RecreateImages();
    void AcquireSampleCounts();
    void TransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageSubresourceRange range, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout);

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
    VkSampleCountFlagBits _sampleCount = VK_SAMPLE_COUNT_1_BIT;
    VkFormat _depthFormat, _positionFormat;
    std::unique_ptr<VulkanImage> _colorImage;
    std::unique_ptr<VulkanImageView> _colorImageView;
    std::unique_ptr<VulkanImage> _depthImage;
    std::unique_ptr<VulkanImageView> _depthImageView;
    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;
    bool recreateRequested = false;
    VkPresentModeKHR recreatePresentMode = VK_PRESENT_MODE_FIFO_KHR;

    std::unique_ptr<VulkanDescriptorSetAllocatorCache> _descriptorSetCache;

    // Uniform data
    GlobalUBO _uboData;
    VkDescriptorSetLayout _globalLayout;
    std::array<VulkanBuffer, VulkanConfig::maxFramesInFlight> _globalBuffer;
    std::array<VkDescriptorSet, VulkanConfig::maxFramesInFlight> _globalSet;
    std::array<void*, VulkanConfig::maxFramesInFlight> _globalBufferMap;
    float _prevTime;

    // Instance rendering
    std::unordered_map<Mesh*, std::tuple<int, std::vector<InstanceData>>> _instanceDraws;

    std::function<void()> _recreateCallback;

    std::unordered_set<VulkanMaterialInstance*> _materials;
};

NLOHMANN_JSON_SERIALIZE_ENUM(RenderPassType, {
                                                 { RenderPassType::eGeometry, "geometry" },
                                                 { RenderPassType::eUI, "ui" },
                                             });

} // namespace bl
