#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Graphics/Window.h"
#define IMGUI_IMPL_VULKAN_NO_PROTOTYPES
#include "imgui.h"

#include "Engine/SDLInitializer.h"
#include "Graphics/Renderer.h"
#include "Graphics/Vulkan.h"

namespace bl
{

class Engine;

class ImGuiSystem
{
public:
    ImGuiSystem(Engine* engine, VulkanWindow* window, Renderer* renderer);
    ~ImGuiSystem();

    void Process(const SDL_Event& event);
    void BeginFrame();
    void EndFrame(VkCommandBuffer cmd);

private:
    static void ApplyStyle();
    void Init();
    void Unload();

    Engine* _engine;
    VulkanWindow* _window;
    Renderer* _renderer;
    VkDescriptorPool _descriptorPool;
};

} // namespace bl