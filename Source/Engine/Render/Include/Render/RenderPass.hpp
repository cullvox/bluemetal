#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Export.h"

using blRenderPassFunction = std::function<void(VkCommandBuffer)>;

struct blRenderPassAttachmentInfo
{
	VkFormat format;
};

class BLOODLUST_RENDER_API blRenderPass
{
public:
	blRenderPass(const blRenderDevice* renderDevice, 
		const blRenderPassAttachmentInfo& info, 
		const blRenderPassFunction& func);
	~blRenderPass();

	virtual void resize(blExtent2D extent);
	virtual void record(VkCommandBuffer cmd);
	
private:
	const blRenderDevice* _renderDevice;
	blRenderPassFunction _func;
	VkRenderPass _pass;
};