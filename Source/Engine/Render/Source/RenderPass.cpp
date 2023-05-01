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

void blRenderPass::record(vk::CommandBuffer cmd, vk::Framebuffer framebuffer, vk::Rect2D renderArea)
{
    const vk::RenderPassBeginInfo beginInfo
    {
        getRenderPass(),
        framebuffer,
        renderArea,
        
    }

}

vk::RenderPass blRenderPass::getRenderPass() const noexcept
{
    return _pass.get();
}