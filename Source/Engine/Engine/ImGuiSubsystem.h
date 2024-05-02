#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#define IMGUI_IMPL_VULKAN_NO_PROTOTYPES
#include "imgui/imgui.h"

#include "Export.h"
#include "Engine/SDLInitializer.h"
#include "Graphics/Vulkan.h"

namespace bl
{

class Engine;

class ImGuiSystem
{
public:
    ImGuiSystem(Engine* engine);
    ~ImGuiSystem();

    void Process(const SDL_Event& event);
    void BeginFrame();
    void EndFrame(VkCommandBuffer cmd);

private:
    void Init();
    void Unload();

    Engine* _engine;
    VkDescriptorPool _descriptorPool;
};

} // namespace bl