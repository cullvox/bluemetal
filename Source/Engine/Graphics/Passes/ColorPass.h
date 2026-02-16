#pragma once

#include "RenderPass.h"

class ColorPass : public RenderPass {
    VkImageView _colorView;
    VkImageView _depthView;

protected:
    void BeginColorPass();
    void EndColorPass();

public:
    ColorPass();
    ~ColorPass();

    virtual void Record(RenderData&) = 0;
    virtual void Resize();

}