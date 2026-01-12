#pragma once

#include "Math/Extent.h"

namespace bl {

class RenderData;

class RenderPass {
public:
    RenderPass();
    virtual ~RenderPass() = default;

    virtual void Record(RenderData& renderData) = 0;
    virtual void Resize(Extent2D extent) = 0;
};

}