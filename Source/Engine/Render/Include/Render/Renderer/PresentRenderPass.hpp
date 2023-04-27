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
	~blPresentRenderPass();


	virtual void record(VkCommandBuffer cmd, uint32_t index) override;
	virtual void resize(blExtent2D extent) override;

private:
	void createFramebuffers();
	void destroyFramebuffers();

	std::vector<vk::UniqueFramebuffer> _swapFramebuffers;
	std::shared_ptr<const blSwapchain> _swapchain;
};