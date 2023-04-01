#pragma once

#include "ImGui.h"

#include "Window/Window.hpp"
#include "Input/InputSystem.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Renderer.hpp"
#include "Generated/Export_ImGui.h"

namespace bl
{

	class BLOODLUST_IMGUI_API ImmediateModeGUI
	{
		static Window* _pWindow;
		static InputSystem* _pInputSystem;
		static RenderDevice* _pRenderDevice;
		static Renderer* _pRenderer;
		static VkDescriptorPool _descriptorPool;

	public:
		
		static bool init(Window& window, InputSystem& inputSystem, RenderDevice& renderDevice, Renderer& renderer) noexcept;
		static void shutdown() noexcept;
		static void beginFrame() noexcept;
		static void endFrame(VkCommandBuffer cmd) noexcept;
	};

}