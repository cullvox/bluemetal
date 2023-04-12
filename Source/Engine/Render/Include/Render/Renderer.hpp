#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Image.hpp"
#include "Render/Swapchain.hpp"
#include "Render/RenderPass.hpp"
#include "Render/Pipeline.hpp"

struct Submission
{
    blPipeline* pipeline;
    blBuffer* indexBuffer;
    blBuffer* vertexBuffer;
    int instanceCount;
};

class BLOODLUST_RENDER_API blRenderer
{
public:
    blRenderer(const std::shared_ptr<blRenderDevice> renderDevice, 
        const std::vector<std::shared_ptr<blRenderPass>>& passes) noexcept;
    ~blRenderer();

    void resize(blExtent2D extent);
    void submit(const Submission& submission) noexcept;

private:
    static const inline uint32_t DEFAULT_FRAMES_IN_FLIGHT = 2;

    // TODO: Create a swap renderer class of some kind for the swapped obejects per frame.
    // This will probably have to include synchronized objects.
    
    const blRenderDevice* _pRenderDevice;
    std::vector<blRenderPass*> _passes;
};