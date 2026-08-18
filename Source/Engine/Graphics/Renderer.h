#pragma once

#include "Core/Object.h"
#include "Graphics/RenderPass.h"
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
class VulkanResource;
class VulkanDescriptorSetAllocatorCache;
class Node;
class Viewport;

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


class Renderer : public Object {
public:
    Renderer(VulkanDevice* device); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    VulkanDevice* GetDevice() const;

    uint32_t GetSwapchainImageCount();
    uint32_t GetNextFrameIndex(); /** @brief Returns the circular frame index from zero to GraphicsConfig::maxFramesInFlight - 1. */

    void SetDebugMaterialInstance(VulkanMaterialInstance* pointMaterial, VulkanMaterialInstance* lineMaterial, VulkanMaterialInstance* triangleMaterial);
    void DrawPoint(const glm::vec3& point, float size = 1.0f, Color color = Color::Violet());
    void DrawLine(const glm::vec3& a, const glm::vec3& b, float thickness = 1.0f, Color color = Color::Violet());
    void DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float thickness = 1.0f, Color color = Color::Violet());

    void SetGUIFunction(RenderFunction func);
    void SetObjectFunction(ObjectFunction func);

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
    VkFormat GetViewportDepthFormat();
    VkFormat GetViewportSelectionFormat();

    void PrepareRenderData(RenderData& rd);

    void AddMaterial(VulkanMaterialInstance* instance);
    void RemoveMaterial(VulkanMaterialInstance* instance);

    void AddToDeletionQueue(std::unique_ptr<VulkanResource> resource); // Adds a resource to the deletion queue for the current frame. It will be deleted after the frame is finished rendering.

    void AddViewport(Viewport* viewport);
    void RemoveViewport(Viewport* viewport);

    void RenderFrame();

private:
    VulkanDevice*                                                   _device;
    Viewport*                                                       _mainViewport;
    RenderData                                                      _renderData;
    uint32_t                                                        _currentFrame = 0;
    std::array<VkCommandBuffer, VulkanConfig::maxFramesInFlight>    _commandBuffers;
    VkFormat                                                        _colorFormat, _colorFormatHDR, _depthFormat, _selectionFormat;
    std::vector<VkFence>                                            _inFlightFences;
    std::unique_ptr<VulkanDescriptorSetAllocatorCache>              _descriptorSetCache;
    std::vector<VkSurfaceFormatKHR>                                 _surfaceFormats;

    std::vector<RenderPass> _renderPasses;

    void CreateCommandBuffers();
    void DestroyCommandBuffers();

    std::vector<Viewport*> _viewports;
    std::vector<VkSemaphoreSubmitInfo> _submitWaitInfos;
    std::vector<VkSemaphoreSubmitInfo> _submitSignalInfos;

    // Material Uniform Updates
    std::unordered_set<VulkanMaterialInstance*> _materials;

    void UpdateMaterialUniforms();

    VulkanMaterialInstance* _pointMaterial;
    VulkanMaterialInstance* _lineMaterial;
    VulkanMaterialInstance* _triangleMaterial;

    std::array<std::vector<std::unique_ptr<VulkanResource>>, VulkanConfig::maxFramesInFlight> _deletionQueues;

    RenderFunction _guiFunc;
    ObjectFunction _objectFunc;

    void RenderSceneToViewport(RenderData& rd, Viewport& vp);

};

NLOHMANN_JSON_SERIALIZE_ENUM(RenderPassType, {
                                                 { RenderPassType::eGeometry, "geometry" },
                                             });

} // namespace bl
