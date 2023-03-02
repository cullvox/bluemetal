#pragma once

#include "Render/RenderDevice.hpp"

namespace bl {

struct PipelineData
{
};

class Pipeline{
public:
    Pipeline();
    Pipeline(RenderDevice& renderDevice, const std::vector<uint32_t>& code, PipelineData& data);
    ~Pipeline();
};

}; // namespace bl