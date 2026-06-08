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
    Renderer(VulkanDevice* device, VulkanViewport* window, FrameCounter& frameCounter); /** @brief Constructor */
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

    //void QueueSelectionBuffer(ViewportData& vp);
    //uint32_t GetSelectionValue(const glm::ivec2& position);

    std::vector<VkSampleCountFlagBits> GetMultisampleCounts();
    void SetMultisampleCount(VkSampleCountFlagBits samples);
    VkSampleCountFlagBits GetMultisampleCount();

    std::vector<VkFormat> GetColorAttachmentFormats(RenderPassType pass);
    std::vector<VkPipelineColorBlendAttachmentState> GetColorBlendAttachmentStates(RenderPassType pass);
    VkFormat GetDepthAttachmentFormat(RenderPassType pass);
    VkFormat GetStencilAttachmentFormat(RenderPassType pass);

    VulkanDescriptorSetAllocatorCache* GetDescriptorSetAllocatorCache();

    RenderData& GetRenderData();

    float GetCurrentFrameTime(); // In seconds, the current time of this rendered frame.
    float GetCurrentFrameDeltaTime(); // In seconds, the time between frames.

    // Depending on if the renderer is running on an HDR format.
    VkFormat GetViewportColorFormat();

    void PrepareRenderData(RenderData& rd);

    VulkanImageView* GetColorImageView();

    void AddMaterial(VulkanMaterialInstance* instance);
    void RemoveMaterial(VulkanMaterialInstance* instance);

    void AddViewport(VulkanViewport* viewport);
    void RemoveViewport(VulkanViewport* viewport);

private:
    VulkanDevice*                                                   _device;
    FrameCounter&                                                   _frameCounter;
    VulkanViewport*                                                 _mainViewport;
    RenderData                                                      _renderData;
    uint32_t                                                        _currentFrame = 0;
    std::array<VkCommandBuffer, VulkanConfig::maxFramesInFlight>    _commandBuffers;
    VkFormat                                                        _colorFormat, _depthFormat, _positionFormat;
    std::vector<VkFence>                                            _inFlightFences;

    std::vector<VkFence> queuedSemaphores;

    void CreateCommandBuffers();
    void DestroyCommandBuffers();

    void CreatePerFrameSyncData(ViewportData& vp);
    void DestroyPerFrameSyncData(ViewportData& vp);

    // Render Pass Data
    struct RenderPassData {
        bool                                changedSampleCount = false;
        VkSampleCountFlagBits               sampleCount = VK_SAMPLE_COUNT_1_BIT;
        VkSampleCountFlagBits               newSampleCount = VK_SAMPLE_COUNT_1_BIT;
        std::unique_ptr<VulkanImage>        colorImage;
        std::unique_ptr<VulkanImageView>    colorImageView;
        std::unique_ptr<VulkanImage>        colorImageResolved;
        std::unique_ptr<VulkanImageView>    colorImageResolvedView;    
        std::unique_ptr<VulkanImage>        selectionImageSampled;
        std::unique_ptr<VulkanImageView>    selectionImageSampledView;
        std::unique_ptr<VulkanImage>        selectionImage;
        std::unique_ptr<VulkanImageView>    selectionImageView;
        std::unique_ptr<VulkanBuffer>       selectionBuffer;
        std::unique_ptr<VulkanImage>        depthImage;
        std::unique_ptr<VulkanImageView>    depthImageView;
        bool                                queuedSelectionBuffer;
    };

    void RecreateImages(ViewportData& vp);
    void DestroyImages(ViewportData& vp);

    // Global Uniform Buffer
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
        RenderPassData renderData; // Images For Rendering Passes

        bool RequiresRecreation();
    };

    std::vector<ViewportData> _viewports;
    std::vector<VkSemaphoreSubmitInfo> _submitWaitInfos;
    std::vector<VkSemaphoreSubmitInfo> _submitSignalInfos;


    // Material Uniform Updates
    std::unordered_set<VulkanMaterialInstance*> _materials;

    void UpdateMaterialUniforms();

    VulkanMaterialInstance* _pointMaterial;
    VulkanMaterialInstance* _lineMaterial;
    VulkanMaterialInstance* _triangleMaterial;

    void RenderSceneToViewport(RenderData& rd, ViewportData& vp);
    void RenderUIToViewport(RenderFunction guiFunc, RenderData& rd, ViewportData& vp);

    class RenderNode {
        RenderNode* next;
        ViewportData* renderPort;

        std::vector<ViewportData*> viewportDependencies;
    };

};

NLOHMANN_JSON_SERIALIZE_ENUM(RenderPassType, {
                                                 { RenderPassType::eGeometry, "geometry" },
                                             });

} // namespace bl
