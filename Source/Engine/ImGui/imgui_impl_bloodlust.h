#pragma once

#include "Window/Window.hpp"
#include "Input/InputSystem.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Renderer.hpp"

#include "imgui.h"
#include "imgui_impl_vulkan.h"

bool ImGui_ImplBloodLust_Init(bl::Window& window, bl::InputSystem& inputSystem, bl::RenderDevice& renderDevice, bl::Renderer& renderer);
void ImGui_ImplBloodLust_Shutdown();
void ImGui_ImplBloodLust_NewFrame();