#pragma once

#include "Core/Precompiled.hpp"
#include "Render/RenderPass.hpp"

class blBasicPresentRenderPass : public blRenderPass
{
public:
	blBasicPresentRenderPass(
		const std::shared_ptr<blRenderDevice> renderDevice,
		const std::shared_ptr<blSwapchain> swapchain);
	~blBasicPresentRenderPass();


	void record(VkCommandBuffer cmd) override;

private:
	void createSync();
	void createImageViews();
	void createFramebuffers();
	
	const std::shared_ptr<blRenderDevice> _renderDevice;
	const std::shared_ptr<blSwapchain> _swapchain;

	uint32_t _currentFrame;
	std::vector<VkImageView> _swapImageViews;
	std::vector<VkFramebuffer> _swapFramebuffers;
	std::vector<VkSemaphore> _imageAvailableSemaphores;
	std::vector<VkSemaphore> _renderFinishedSemaphores;
	std::vector<VkFence> _inFlightFences;
};