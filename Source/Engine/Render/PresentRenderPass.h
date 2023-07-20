#pragma once

#include "RenderPass.h"
#include "Swapchain.h"

class BLUEMETAL_API blPresentRenderPass : public blRenderPass
{
public:
	blPresentRenderPass(std::shared_ptr<blDevice> device, std::shared_ptr<blSwapchain> swapchain, const blRenderPassFunction& func);
	virtual ~blPresentRenderPass();

	virtual VkRenderPass getRenderPass() override;
	virtual void record(VkCommandBuffer cmd, VkRect2D renderArea, uint32_t imageIndex) override;
	virtual void resize(VkExtent2D extent) override;

private:
	void createFramebuffers();
	void destroyFramebuffers();


	std::shared_ptr<blDevice> 		_renderDevice;
	std::shared_ptr<blSwapchain> 	_swapchain;	
	std::vector<VkFramebuffer> 		_swapFramebuffers;
	VkRenderPass 					_pass;
	blRenderPassFunction 			_func;
};