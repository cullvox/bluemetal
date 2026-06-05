#pragma once

#include <nlohmann/json.hpp>
#include <vulkan/vulkan_core.h>

#include "UniformData.h"
#include "VulkanBuffer.h"
#include "RenderData.h"
#include "Vertex.h"
#include "RenderPassType.h"

namespace bl {

class FrameCounter;
class VulkanWindow;
class VulkanDevice;
class VulkanSwapchain;
class VulkanMaterialInstance;
class VulkanImageView;
class VulkanImage;
class VulkanDescriptorSetAllocatorCache;
class Node;
class VulkanViewport;

using ObjectFunction = std::function<void(RenderData& rd)>;
using RenderFunction = std::function<void(RenderData& rd)>;

/// @brief What does a renderer not do
///
/// * Manage synchronization between frames of swapchains and their images.
/// * Determine what gets drawn.
/// * Store any image data.
/// * Present from a swapchain.
///
/// What does a renderer do:
/// * Do Render passes from render data.
/// * Interpret render data.
/// * Knows what types of passes/image formats are possible.
///

class Renderer {
public:
    Renderer(VulkanWindow* window, FrameCounter& fc); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    VulkanDevice* GetDevice() const;

    uint32_t GetSwapchainImageCount();
    uint32_t GetNextFrameIndex(); /** @brief Returns the circular frame index from zero to GraphicsConfig::maxFramesInFlight - 1. */
    void Render(RenderFunction func, RenderFunction guiPassFunc, ObjectFunction objectFunc);
    void Render(Node* root);

    void Render(VulkanViewport& viewport, RenderData& renderData);

    void SetProjection(const glm::mat4& projection);
    void SetView(const glm::mat4& view);
    void SetDebugMaterialInstance(VulkanMaterialInstance* pointMaterial, VulkanMaterialInstance* lineMaterial, VulkanMaterialInstance* triangleMaterial);
    void DrawPoint(const glm::vec3& point, float size = 1.0f, Color color = Color::Violet());
    void DrawLine(const glm::vec3& a, const glm::vec3& b, float thickness = 1.0f, Color color = Color::Violet());
    void DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float thickness = 1.0f, Color color = Color::Violet());

    void QueueSelectionBuffer();
    uint32_t GetSelectionValue(const glm::ivec2& position);

    std::vector<VkSampleCountFlagBits> GetMultisampleCounts();
    void SetMultisampleCount(VkSampleCountFlagBits samples);
    VkSampleCountFlagBits GetMultisampleCount();

    std::vector<VkFormat> GetColorAttachmentFormats(RenderPassType pass);
    std::vector<VkPipelineColorBlendAttachmentState> GetColorBlendAttachmentStates(RenderPassType pass);
    VkFormat GetDepthAttachmentFormat(RenderPassType pass);
    VkFormat GetStencilAttachmentFormat(RenderPassType pass);

    RenderData& GetRenderData();

    void PrepareRenderData(RenderData& rd);

    VulkanImageView* GetColorImageView();

    void AddMaterial(VulkanMaterialInstance* instance);
    void RemoveMaterial(VulkanMaterialInstance* instance);

    void AddViewport(VulkanViewport* viewport);
    void RemoveViewport(VulkanViewport* viewport);

private:
    VulkanDevice*       _device;
    FrameCounter&       _frameCounter;
    VulkanViewport*     _mainViewport;

    // Frame Synchronization
    struct SwapchainSync {
        std::array<VkSemaphore, VulkanConfig::maxFramesInFlight> imageAvailableSemaphore;
        std::vector<VkSemaphore> renderFinishedSemaphore;
        std::vector<VkFence> inFlightFences;
    };

    struct PerFrameData {
        VkSemaphore imageAvailableSemaphore;
        VkFence inFlightFence;
        VkCommandBuffer commandBuffer;
    };

    RenderData                                      _renderData;
    uint32_t                                        _currentFrame = 0;
    std::array<PerFrameData, VulkanConfig::maxFramesInFlight>  _perFrame;

    void CreatePerFrameSyncedData();
    void DestroyPerFrameSyncedData();

    // Viewport Data
    VulkanViewport* _windowViewport;
    VulkanViewport* _editorViewport;

    // Render Pass Data
    bool _changedSampleCount = false;
    VkSampleCountFlagBits               _sampleCount = VK_SAMPLE_COUNT_1_BIT;
    VkSampleCountFlagBits               _newSampleCount = VK_SAMPLE_COUNT_1_BIT;
    VkFormat                            _depthFormat, _positionFormat;
    std::unique_ptr<VulkanImage>        _colorImage;
    std::unique_ptr<VulkanImageView>    _colorImageView;
    std::unique_ptr<VulkanImage>        _colorImageResolved;
    std::unique_ptr<VulkanImageView>    _colorImageResolvedView;
    std::unique_ptr<VulkanImage>        _selectionImageSampled;
    std::unique_ptr<VulkanImageView>    _selectionImageSampledView;
    std::unique_ptr<VulkanImage>        _selectionImage;
    std::unique_ptr<VulkanImageView>    _selectionImageView;
    std::unique_ptr<VulkanBuffer>       _selectionBuffer;
    std::unique_ptr<VulkanImage>        _depthImage;
    std::unique_ptr<VulkanImageView>    _depthImageView;

    bool _queuedSelectionBuffer;

    bool                                                        recreateRequested = false;
    VkPresentModeKHR                                            recreatePresentMode = VK_PRESENT_MODE_FIFO_KHR;

    void DestroyImagesAndFramebuffers();
    void RecreateImages();
    void AcquireSampleCounts();
    void TransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageSubresourceRange range, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout);


    // Global Uniform Buffer
    struct ViewportData;

    std::unique_ptr<VulkanDescriptorSetAllocatorCache> _descriptorSetCache; 
    GlobalUBO               _uboData;
    VkDescriptorSetLayout   _globalDescriptorLayout;

    struct UniformData {
        GlobalUBO uboData;
        VulkanBufferFrameRing globalBuffer;
        std::array<VkDescriptorSet, VulkanConfig::maxFramesInFlight> globalDescriptorSets;
    };

    void CreateGlobalUniform(ViewportData& vp);
    void DestroyGlobalUniform(ViewportData& vp);
    void UpdateGlobalUniform(ViewportData& vp);

    // Viewports
    struct ViewportData {
        VulkanViewport* viewport;
        UniformData guboData; // Global Uniform Buffer Data
        SwapchainSync syncData; // Swapchain Sync Information
    };

    std::vector<ViewportData> _viewports;





    // Material Uniform Updates
    std::unordered_set<VulkanMaterialInstance*> _materials;

    void UpdateMaterialUniforms();

    VulkanMaterialInstance* _pointMaterial;
    VulkanMaterialInstance* _lineMaterial;
    VulkanMaterialInstance* _triangleMaterial;

    void RenderSceneToViewport(RenderData& rd, ViewportData& vp);
    void RenderUIToViewport(RenderFunction guiFunc, RenderData& rd, ViewportData& vp);

};

NLOHMANN_JSON_SERIALIZE_ENUM(RenderPassType, {
                                                 { RenderPassType::eGeometry, "geometry" },
                                             });

} // namespace bl
