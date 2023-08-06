#pragma once

#include "imgui.h"

#include "Graphics/Window/Window.h"
#include "Graphics/Instance.h"
#include "Graphics/PhysicalDevice.h"
#include "Graphics/Device.h"
#include "Graphics/RenderPass.h"

struct ImGui_ImplBluemetal_InitInfo
{
    bl::GraphicsInstance*           pInstance;
    bl::GraphicsPhysicalDevice*     pPhysicalDevice;
    bl::GraphicsDevice*             pDevice;
    bl::RenderPass*                 pRenderPass;
    bl::Window*                     pWindow;
};

IMGUI_IMPL_API bool         ImGui_ImplBluemetal_Init(ImGui_ImplBluemetal_InitInfo* info);
IMGUI_IMPL_API void         ImGui_ImplBluemetal_Shutdown();
IMGUI_IMPL_API void         ImGui_ImplBluemetal_BeginFrame();
IMGUI_IMPL_API void         ImGui_ImplBluemetal_EndFrame(VkCommandBuffer cmd);
 