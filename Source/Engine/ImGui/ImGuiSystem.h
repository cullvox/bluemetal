#pragma once

#include "Engine/System.h"
#include "Engine/SDL.h"
#include "Graphics/Renderer.h"
#include "Graphics/Vulkan.h"
#include "Window/Window.h"

#define IMGUI_IMPL_VULKAN_NO_PROTOTYPES
#include "imgui.h"

namespace bl {

class Engine;
class GraphicsSystem;

class ImGuiSystem : public System {
    VulkanWindow* _window;
    Renderer* _renderer;
    VkDescriptorPool _descriptorPool;

    void ApplyStyle();
    void Init();
    void Unload();

public:
    ImGuiSystem(Engine& engine, VulkanWindow* window, Renderer* renderer);
    ~ImGuiSystem();

    virtual std::shared_ptr<Resource> ConstructResource(std::size_t typeHash, const std::filesystem::path& path) override;
    void Process(const SDL_Event& event);
    void BeginFrame();
    void EndFrame(VkCommandBuffer cmd);
    void DrawDebug();
};

} // namespace bl

namespace ImGui {
// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see
// docs/FONTS.md)
extern void HelpMarker(const char* desc);
}