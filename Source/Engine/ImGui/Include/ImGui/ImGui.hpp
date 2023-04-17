#pragma once

#include "ImGui.h"

#include "Window/Window.hpp"
#include "Input/InputSystem.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/RenderPass.hpp"
#include "ImGui/Export.h"

class BLOODLUST_IMGUI_API blImGui
{
	static std::shared_ptr<const blWindow> _window;
	static std::shared_ptr<const blInputSystem> _inputSystem;
	static std::shared_ptr<const blRenderDevice> _renderDevice;
	static std::shared_ptr<blRenderPass> pCustomPass;
	static VkDescriptorPool _descriptorPool;

public:
	static void init(
		std::shared_ptr<blWindow> window, 
		std::shared_ptr<blRenderDevice> renderDevice) noexcept;
	static std::shared_ptr<blRenderPass> getRenderPass();
	static void shutdown() noexcept;
	static void beginFrame() noexcept;
	static void endFrame(VkCommandBuffer cmd) noexcept;

};