#pragma once

namespace bl {

class Viewport;
class RenderData;

class RenderPass {
public:
    RenderPass();
    virtual ~RenderPass() = 0;

    virtual void Render(Viewport& viewport, RenderData& renderData) = 0;
    virtual void RecreateImages(VkExtent2D extent) = 0;

};

}