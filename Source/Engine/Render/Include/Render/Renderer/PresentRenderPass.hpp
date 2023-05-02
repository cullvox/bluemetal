#pragma once

#include "Core/Precompiled.hpp"
#include "Render/RenderPass.hpp"
#include "Render/Swapchain.hpp"

class BLOODLUST_RENDER_API blPresentRenderPass : public blRenderPass
{
public:
	blPresentRenderPass(
		std::shared_ptr<const blRenderDevice> renderDevice,
		std::shared_ptr<const blSwapchain> swapchain,
		const blRenderPassFunction& func);
	virtual ~blPresentRenderPass();

	virtual vk::RenderPass getRenderPass() const noexcept override;
	virtual void record(vk::CommandBuffer cmd, vk::Rect2D renderArea, 
					uint32_t imageIndex) override;
	virtual void resize(vk::Extent2D extent) override;

private:
	void createFramebuffers();
	void destroyFramebuffers();

	std::shared_ptr<const blRenderDevice> _renderDevice;
	std::shared_ptr<const blSwapchain> _swapchain;
	blRenderPassFunction _func;
	std::vector<vk::UniqueFramebuffer> _swapFramebuffers;
	vk::UniqueRenderPass _pass;

};