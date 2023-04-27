#include "Render/RenderPass.hpp"

blRenderPass::blRenderPass(
    std::shared_ptr<const blRenderDevice> renderDevice,
    const blRenderPassFunction& func)
    : _renderDevice(renderDevice)
    , _func(func)
{
}

blRenderPass::~blRenderPass()
{
}

void blRenderPass::resize(blExtent2D extent)
{
    _extent = extent;
}

void blRenderPass::record(VkCommandBuffer cmd, uint32_t index)
{
}

vk::RenderPass blRenderPass::getRenderPass() const noexcept
{
    return _pass.get();
}