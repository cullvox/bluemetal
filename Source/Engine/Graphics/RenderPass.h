#pragma once

namespace bl {

class Viewport;
class RenderData;

class RenderPass {

public:
    virtual void Render(Viewport& viewport, RenderData& renderData) = 0;
    virtual void RecreateImages(VkExtent2D extent) = 0;

};

}