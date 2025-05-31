#pragma once

#include "ImGui/ImGuiSystem.h"
#include "Resource/ResourceManager.h"
#include "Window/Window.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanDescriptorSetLayoutCache.h"
#include "VulkanPipelineLayoutCache.h"
#include "Renderer.h"
#include <memory>

namespace bl 
{

class Engine;

class GraphicsSystem : 
    public NonCopyable,
    public ResourceBuilder 
{
public:

    /// @brief Initializes the graphics system.
    GraphicsSystem(Engine* engine); /** @brief Constructor. */
    virtual ~GraphicsSystem(); /** @brief Destructor */

    VulkanInstance* GetInstance();
    VulkanPhysicalDevice* GetPhysicalDevice() const;
    VulkanDevice* GetDevice();
    Window* GetWindow() { return _window.get(); }
    Renderer* GetRenderer() { return _renderer.get(); }

    std::unique_ptr<Window> CreateWindow(const std::string& title, Rect2D rect, bool fullscreen = true);
    std::unique_ptr<Renderer> CreateRenderer(Window* window);

    virtual std::unique_ptr<Resource> BuildResource(ResourceManager* manager, const std::string& type, const std::filesystem::path& path, const nlohmann::json& data);

private:
    Engine* _engine;
    VulkanInstance _instance;
    VulkanPhysicalDevice* _physicalDevice;
    std::unique_ptr<Window> _window;
    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<VulkanDevice> _device;
    std::unique_ptr<ImGuiSystem> _imgui;
};

} // namespace bl
