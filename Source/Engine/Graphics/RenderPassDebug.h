#pragma once

#include "RenderPass.h"

namespace bl {

class DebugRenderPass : public RenderPass {
public:


    virtual void Record(RenderData& rd);

};

} // namespace bl

