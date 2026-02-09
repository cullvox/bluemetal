#pragma once

#include <nlohmann/json.hpp>

#include <unordered_set>
#include <map>

#include "UniformData.h"
#include "VulkanBuffer.h"
#include "RenderData.h"
#include "Vertex.h"

namespace bl {

class FrameCounter;
class Material;
class Mesh;
class VulkanWindow;
class VulkanDevice;
class VulkanSwapchain;
class VulkanMaterialInstance;
class VulkanImageView;
class VulkanImage;
class VulkanDescriptorSetAllocatorCache;

using ObjectFunction = std::function<void(RenderData& rd)>;
using RenderFunction = std::function<void(RenderData& rd)>;

enum class RenderPassType : uint32_t {
    eGeometry = 0,
    eUI = 2,
};

class Renderer {
public:
    Renderer(VulkanWindow* window, FrameCounter& fc); /** @brief Constructor */
    ~Renderer(); /** @brief Destructor */

    VulkanDevice* GetDevice() const;

    uint32_t GetSwapchainImageCount();
    uint32_t GetNextFrameIndex(); /** @brief Returns the circular frame index from zero to GraphicsConfig::maxFramesInFlight - 1. */
    void Render(RenderFunction func, ObjectFunction objectFunc);

    void SetProjection(const glm::mat4& projection);
    void SetView(const glm::mat4& view);
    void SetImageRecreateCallback(std::function<void()> onRecreate);
    void AddInstance(Mesh* mesh, const InstanceData& data);
    void SetDebugMaterialInstance(VulkanMaterialInstance* pointMaterial, VulkanMaterialInstance* lineMaterial, VulkanMaterialInstance* triangleMaterial);
    void DrawPoint(const glm::vec3& point, float size = 1.0f, Color color = Color::Violet());
    void DrawLine(const glm::vec3& a, const glm::vec3& b, float thickness = 1.0f, Color color = Color::Violet());
    void DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float thickness = 1.0f, Color color = Color::Violet());

    std::vector<VkPresentModeKHR> GetPresentModes();
    void SetPresentMode(VkPresentModeKHR mode);
    VkPresentModeKHR GetPresentMode() const;

    std::vector<VkSampleCountFlagBits> GetMultisampleCounts();
    void SetMultisampleCount(VkSampleCountFlagBits samples);
    VkSampleCountFlagBits GetMultisampleCount();

    std::vector<VkFormat> GetColorAttachmentFormats();
    VkFormat GetDepthAttachmentFormat();
    VkFormat GetStencilAttachmentFormat();


protected:
    friend class Material;
    friend class MaterialInstance;
    void AddMaterial(VulkanMaterialInstance* instance);
    void RemoveMaterial(VulkanMaterialInstance* instance);

private:
    void CreateCommandBuffers();
    void DestroyCommandBuffers();
    void DestroyImagesAndFramebuffers();
    void CreateGlobalUniform();
    void DestroyGlobalUniform();
    void RecreateImages();
    void AcquireSampleCounts();
    void TransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageSubresourceRange range, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout);

    VulkanDevice* _device;
    VulkanWindow* _window;
    VulkanSwapchain* _swapchain;
    FrameCounter& _frameCounter;

    // Frame Synchronization
    RenderData _renderData;
    std::array<VkCommandBuffer, VulkanConfig::maxFramesInFlight> _commandBuffers;

    // Render Pass Data
    VkSampleCountFlagBits _sampleCount = VK_SAMPLE_COUNT_1_BIT;
    VkSampleCountFlagBits _newSampleCount = VK_SAMPLE_COUNT_1_BIT;
    VkFormat _depthFormat, _positionFormat;
    std::unique_ptr<VulkanImage> _colorImage;
    std::unique_ptr<VulkanImageView> _colorImageView;
    std::unique_ptr<VulkanImage> _depthImage;
    std::unique_ptr<VulkanImageView> _depthImageView;
    std::array<VkImage, VulkanConfig::maxFramesInFlight> _swapchainImages;
    std::array<VkImageView, VulkanConfig::maxFramesInFlight> _swapchainImageViews;
    bool recreateRequested = false;
    VkPresentModeKHR recreatePresentMode = VK_PRESENT_MODE_FIFO_KHR;

    std::unique_ptr<VulkanDescriptorSetAllocatorCache> _descriptorSetCache;

    // Uniform data
    void UpdateGlobalUniform(uint32_t currentFrame);

    GlobalUBO _uboData;
    VkDescriptorSetLayout _globalLayout;
    std::array<VulkanBuffer, VulkanConfig::maxFramesInFlight> _globalBuffer;
    std::array<VkDescriptorSet, VulkanConfig::maxFramesInFlight> _globalSet;
    std::array<void*, VulkanConfig::maxFramesInFlight> _globalBufferMap;

    // Instance rendering
    struct DrawCall {
        int count;
        std::vector<InstanceData> instances;
    };

    using DrawKey = std::pair<MaterialInstance*, Mesh*>;
    std::map<DrawKey, DrawCall> _calls;

    std::function<void()> _recreateCallback;

    // Material Uniform Updates
    void UpdateMaterialUniforms(uint32_t currentFrame);

    std::unordered_set<VulkanMaterialInstance*> _materials;

    // Debug Rendering
    void CreateDebugBuffer();
    void UpdateDebugBuffers(uint32_t currentFrame);
    void DrawDebugBuffers(RenderData& rd);

    VulkanMaterialInstance* _debugMaterial = nullptr;
    std::vector<VertexDebug> _points;
    std::vector<VertexDebug> _lines;
    std::vector<VertexDebug> _triangles;
    std::vector<VertexDebug> _debugVertices;
    VulkanBufferFrameRing _debugBuffer;
};

NLOHMANN_JSON_SERIALIZE_ENUM(RenderPassType, {
                                                 { RenderPassType::eGeometry, "geometry" },
                                                 { RenderPassType::eUI, "ui" },
                                             });

} // namespace bl
