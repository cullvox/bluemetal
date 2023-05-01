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
	static std::shared_ptr<blInputSystem> _inputSystem;
	static std::shared_ptr<const blRenderDevice> _renderDevice;
	static std::shared_ptr<const blRenderPass> _renderPass;
	static std::shared_ptr<blRenderPass> pCustomPass;
	static vk::DescriptorPool _descriptorPool;
	static blInputHookCallback _hookCallback;

public:
	static void init(
		std::shared_ptr<const blWindow> window, 
		std::shared_ptr<const blRenderDevice> renderDevice,
		std::shared_ptr<const blRenderPass> renderPass);
	static std::shared_ptr<blRenderPass> getRenderPass();
	static void shutdown() noexcept;
	static void beginFrame() noexcept;
	static void endFrame(VkCommandBuffer cmd) noexcept;
};