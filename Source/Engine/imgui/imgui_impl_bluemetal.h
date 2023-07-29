#pragma once

#include "imgui.h"

#include "Window/Window.h"
#include "Render/Instance.h"
#include "Render/PhysicalDevice.h"
#include "Render/Device.h"
#include "Render/RenderPass.h"

struct ImGui_ImplBluemetal_InitInfo
{
    std::shared_ptr<bl::Instance>           instance;
    std::shared_ptr<bl::PhysicalDevice>     physicalDevice;
    std::shared_ptr<bl::Device>             device;
    std::shared_ptr<bl::RenderPass>         renderPass;
    std::shared_ptr<bl::Window>             window;
};

IMGUI_IMPL_API bool         ImGui_ImplBluemetal_Init(ImGui_ImplBluemetal_InitInfo* info);
IMGUI_IMPL_API void         ImGui_ImplBluemetal_Shutdown();
IMGUI_IMPL_API void         ImGui_ImplBluemetal_BeginFrame();
IMGUI_IMPL_API void         ImGui_ImplBluemetal_EndFrame(VkCommandBuffer cmd);
 