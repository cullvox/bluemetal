#pragma once

#include "Core/Precompiled.hpp"
#include "Render/RenderPass.hpp"
#include "Render/Swapchain.hpp"

class blBasicPresentRenderPass : public blRenderPass
{
public:
	blBasicPresentRenderPass(
		std::shared_ptr<const blRenderDevice> renderDevice,
		std::shared_ptr<const blSwapchain> swapchain,
		const blRenderPassFunction& func);
	~blBasicPresentRenderPass();


	virtual void record(VkCommandBuffer cmd, uint32_t index) override;
	virtual void resize(blExtent2D extent) override;

private:
	void createFramebuffers();
	void destroyFramebuffers();

	std::shared_ptr<const blRenderDevice> _renderDevice;
	std::shared_ptr<const blSwapchain> _swapchain;
	
	std::vector<VkFramebuffer> _swapFramebuffers;
};