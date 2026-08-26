#pragma once

#include "Graphics/VulkanBuffer.h"
#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanImageView.h"
#include "Graphics/VulkanConfig.h"
#include "VulkanBufferFrameRing.h"
#include "Core/Delegates.h"
#include "UniformData.h"

namespace bl {

class Renderer;
class VulkanDevice;
class VulkanImage;
class VulkanImageView;
class VulkanRenderImage;
class RenderData;
class RenderPass;
class VulkanSwapchain;
class RendererViewportData;

enum class ViewportRenderFlags : uint32_t {
    eNone = 0x0000, // No render?
    eWireframe = 0x0001, // Overrides the RenderData material with a wireframe pipeline.
    eScene = 0x0002, // Fully rendered scene with all the bells and whistles.
    eUI = 0x0004, // Renders the UI Layer onto this.
    eSampled = 0x0008, // Transition the viewport image layout after rendering is completed and use it as a sampled image.
    eImGui = 0x0010, // Uses the ImGui context to draw onto the viewport.
};

static inline ViewportRenderFlags operator&(ViewportRenderFlags a, ViewportRenderFlags b) 
{ 
    return static_cast<ViewportRenderFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

static inline ViewportRenderFlags operator|(ViewportRenderFlags a, ViewportRenderFlags b) 
{ 
    return static_cast<ViewportRenderFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

static inline bool HasFlag(ViewportRenderFlags value, ViewportRenderFlags flag)
{
    return (value & flag) == flag; 
}

// Viewports manage their own frames and render images. They are responsible for presenting to the swapchain, but not for managing synchronization or determining what gets drawn.
class Viewport {
protected:
    VulkanDevice* _device;
    Renderer* _renderer;

    ViewportRenderFlags _renderFlags;
    VkSampleCountFlagBits _sampleCount;
    VkExtent2D _extent;
    VkRect2D _scissor;
    float _scissorTop = 0.0f, _scissorBottom = 0.0f, _scissorLeft = 0.0f, _scissorRight = 0.0f;

    std::unique_ptr<VulkanImage> _colorImage, _colorImageResolved, 
        _selectionImage, _selectionImageResolved, _depthImage;
    std::unique_ptr<VulkanImageView> _colorImageView, _colorImageResolvedView, 
        _selectionImageView, _selectionImageResolvedView, _depthImageView;
    std::unique_ptr<VulkanBuffer> _selectionBuffer;

    ViewportUBO _uboData = {};
    VulkanBufferFrameRing _globalBuffer;
    VkDescriptorSetLayout _globalDescriptorSetLayout;
    std::array<std::unique_ptr<VulkanDescriptorSet>, VulkanConfig::maxFramesInFlight> _globalDescriptorSets;

    VkBool32 _imagesDirty = VK_TRUE; // Determines if images need to be recreated at the beginning of a frame.

    std::vector<VkRenderingAttachmentInfo> _colorAttachments;
    VkRenderingAttachmentInfo _depthAttachment;
    RendererViewportData* data;

    int32_t _priority = 0;

    virtual void RecreateImages();

public:
    Viewport(Renderer* renderer); // Does not create images until Recreate is called.
    Viewport(Renderer* renderer, VkExtent2D extent); // Creates the viewport and its images.
    Viewport(const Viewport&);
    Viewport(Viewport&&);
    ~Viewport();

    Viewport& operator=(const Viewport&);
    Viewport& operator=(Viewport&&);

    void SetSize(VkExtent2D extent); // Size of the images rendered.
    void SetRenderFlags(ViewportRenderFlags renderFlags); // Render flags determine what passes get drawn onto the viewport.
    void SetSampleCount(VkSampleCountFlagBits sampleCount); // How many samples are rendered, then averaged.
    void SetScissor(float top = 0.0f, float bottom = 0.0f, float left = 0.0f, float right = 0.0f); // Sets the scissor as normalized coordinates starting from point to the opposite in a line across the frame.
    void SetProjection(const glm::mat4& projection);
    void SetView(const glm::mat4& view);
    void SetRendererData(RendererViewportData* data);
    void SetRenderingPriority(int32_t priority) { _priority = priority; }
    
    VkExtent2D GetExtent() const;
    VkImageView GetColorImageView();
    VkImageView GetColorResolveImageView();
    VkImageView GetSelectionImageView();
    VkImageView GetSelectionResolveImageView();
    VkImageView GetDepthImageView();
    VkSampleCountFlagBits GetSampleCount();
    VkPresentModeKHR GetPresentMode();
    ViewportRenderFlags GetRenderFlags() const;
    int32_t GetRenderingPriority() const { return _priority; }
    
    virtual void GetColorRenderingAttachments(std::vector<VkRenderingAttachmentInfo>& attachments);
    virtual void GetDepthRenderingAttachment(VkRenderingAttachmentInfo& attachment);

    virtual void FillColorRenderingAttachmentsForUI(std::vector<VkRenderingAttachmentInfo>& attachments);

    virtual void UpdateUniform(RenderData& rd);
    virtual void PrepareForFrame(RenderData& rd);
    virtual void PrepareEndFrame();

    virtual bool Ready(); // Returns false if an image needs to be recreated.
    virtual bool Bind(RenderData& rd); // Returns false if the viewport isn't ready.
    virtual void TransitionPreRender(RenderData& rd);
    virtual void TransitionPostRender(RenderData& rd);
    virtual void TransitionPrePresent(RenderData& rd);
    virtual void QueuePresent(RenderData& rd);

    MulticastDelegate<Viewport*> onPreViewportResized;
    MulticastDelegate<Viewport*> onPostViewportResized;

    virtual VkImageView GetRenderedImageView();
};

} // namespace bl
