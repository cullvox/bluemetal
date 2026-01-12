#pragma once

#include <vector>

#include "Graphics/RenderPass.h"
#include "Math/Math.h"
#include "Core/Color.h"

namespace bl {


struct DebugVertex {
    glm::vec3 position;
    glm::vec3 color;
    float width;
};

class VulkanDevice;
class VulkanImage;
class VulkanBuffer;
class VulkanPipeline;
class Renderer;

/// Controls rendering of the debug pass.
///
/// Can be rendered concurrently to the whole frame.
///
class DebugRenderPass : public RenderPass {
    std::vector<DebugVertex> _points;
    std::vector<DebugVertex> _lines;
    std::vector<DebugVertex> _triangles;
    std::vector<DebugVertex> _vertices;
    VkImageView _colorView;
    VkImageView _depthView;
    std::unique_ptr<VulkanBuffer> _buffer;
    std::unique_ptr<VulkanPipeline> _debugPipeline;
    Extent2D _extent;

public:
    DebugRenderPass(Extent2D extent, VulkanDevice* device, Renderer* renderer, VkImageView colorView, VkImageView depthView);
    virtual ~DebugRenderPass();

    virtual void Record(RenderData& rd);
    virtual void Resize(Extent2D extent);

    void DrawPoint(const glm::vec3& point, float size = 1.0f, Color color = Color::Violet());
    void DrawLine(const glm::vec3& a, const glm::vec3& b, float thickness = 1.0f, Color color = Color::Violet());
    void DrawTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float thickness = 1.0f, Color color = Color::Violet());
};

} // namespace bl

