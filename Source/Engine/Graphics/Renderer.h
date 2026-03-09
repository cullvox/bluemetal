#pragma once

#include <nlohmann/json.hpp>

#include <unordered_set>
#include <map>

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

using ObjectFunction = std::function<void(RenderData& rd)>;
using RenderFunction = std::function<void(RenderData& rd)>;

class Renderer {
public:
    Renderer(VulkanWindow* window, FrameCounter& fc); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    VulkanDevice* GetDevice() const;

    uint32_t GetSwapchainImageCount();
    uint32_t GetNextFrameIndex(); /** @brief Returns the circular frame index from zero to GraphicsConfig::maxFramesInFlight - 1. */
    void Render(RenderFunction func, RenderFunction guiPassFunc, ObjectFunction objectFunc);
    void Render(Node* root);

    void SetProjection(const glm::mat4& projection);
    void SetView(const glm::mat4& view);
    void SetDebugMaterialInstance(VulkanMaterialInstance* pointMaterial, VulkanMaterialInstance* lineMaterial, VulkanMaterialInstance* triangleMaterial);
    void DrawPoint(const glm::vec3& point, float size = 1.0f, Color color = Color::Violet());
    void DrawLine(const glm::vec3& a, const glm::vec3& b, float thickness = 1.0f, Color color = Color::Violet());
    void DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float thickness = 1.0f, Color color = Color::Violet());

    void QueueSelectionBuffer();
    uint32_t GetSelectionValue(const glm::ivec2& position);

    std::vector<VkPresentModeKHR> GetPresentModes();
    void SetPresentMode(VkPresentModeKHR mode);
    VkPresentModeKHR GetPresentMode() const;

    std::vector<VkSampleCountFlagBits> GetMultisampleCounts();
    void SetMultisampleCount(VkSampleCountFlagBits samples);
    VkSampleCountFlagBits GetMultisampleCount();

    std::vector<VkFormat> GetColorAttachmentFormats(RenderPassType pass);
    std::vector<VkPipelineColorBlendAttachmentState> GetColorBlendAttachmentStates(RenderPassType pass);
    VkFormat GetDepthAttachmentFormat(RenderPassType pass);
    VkFormat GetStencilAttachmentFormat(RenderPassType pass);

    RenderData& GetRenderData();

    void PrepareRenderData(RenderData& rd);


protected:
    friend class Material;
    friend class MaterialInstance;
    void AddMaterial(VulkanMaterialInstance* instance);
    void RemoveMaterial(VulkanMaterialInstance* instance);

private:
    VulkanDevice*       _device;
    VulkanWindow*       _window;
    VulkanSwapchain*    _swapchain;
    FrameCounter&       _frameCounter;


    // Frame Synchronization
    struct PerFrameData {
        VkSemaphore imageAvailableSemaphore;
        VkFence inFlightFence;
        VkCommandBuffer commandBuffer;
    };

    static constexpr uint32_t                       MAX_FRAMES_IN_FLIGHT = 2;
    RenderData                                      _renderData;
    uint32_t                                        _currentFrame = 0;
    std::array<PerFrameData, MAX_FRAMES_IN_FLIGHT>  _perFrame;
    std::vector<VkSemaphore>                        _renderFinishedSemaphores;

    void CreatePerFrameSyncedData();
    void DestroyPerFrameSyncedData();


    // Render Pass Data
    std::vector<VkImage>                _swapchainImages;
    std::vector<VkImageView>            _swapchainImageViews;
    bool _changedSampleCount = false;
    VkSampleCountFlagBits               _sampleCount = VK_SAMPLE_COUNT_1_BIT;
    VkSampleCountFlagBits               _newSampleCount = VK_SAMPLE_COUNT_1_BIT;
    VkFormat                            _depthFormat, _positionFormat;
    std::unique_ptr<VulkanImage>        _colorImage;
    std::unique_ptr<VulkanImageView>    _colorImageView;
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
    std::unique_ptr<VulkanDescriptorSetAllocatorCache> _descriptorSetCache;
    GlobalUBO               _uboData;
    VkDescriptorSetLayout   _globalDescriptorLayout;
    VulkanBufferFrameRing   _globalBuffer;
    std::array<VkDescriptorSet, VulkanConfig::maxFramesInFlight> _globalDescriptorSets;

    void CreateGlobalUniform();
    void DestroyGlobalUniform();
    void UpdateGlobalUniform();


    // Material Uniform Updates
    std::unordered_set<VulkanMaterialInstance*> _materials;

    void UpdateMaterialUniforms();



};

NLOHMANN_JSON_SERIALIZE_ENUM(RenderPassType, {
                                                 { RenderPassType::eGeometry, "geometry" },
                                                 { RenderPassType::eUI, "ui" },
                                             });

} // namespace bl
