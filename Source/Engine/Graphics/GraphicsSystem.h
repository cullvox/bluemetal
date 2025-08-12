#pragma once

#include "ImGui/ImGuiSystem.h"
#include "Renderer.h"
#include "Resources/ResourceManager.h"
#include "VulkanDescriptorSetLayoutCache.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanPipelineLayoutCache.h"
#include "VulkanSwapchain.h"
#include "Window/Window.h"
#include <memory>

namespace bl {

class Engine;

class GraphicsSystem : public NonCopyable,
                       public ResourceBuilder {
    Engine* _engine;
    VulkanInstance _instance;
    VulkanPhysicalDevice* _physicalDevice;
    std::unique_ptr<Window> _window;
    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<VulkanDevice> _device;
    std::unique_ptr<ImGuiSystem> _imgui;

protected:
    virtual std::unique_ptr<Resource> BuildResource(ResourceManager* manager, const std::string& type) override;
    virtual void AddDefaultResources(ResourceManager* manager) override;

public:
    GraphicsSystem(Engine* engine); /** @brief Constructor. */
    virtual ~GraphicsSystem(); /** @brief Destructor */

    VulkanInstance* GetInstance();
    VulkanPhysicalDevice* GetPhysicalDevice() const;
    VulkanDevice* GetDevice();
    Window* GetWindow() { return _window.get(); }
    Renderer* GetRenderer() { return _renderer.get(); }

    std::unique_ptr<Window> CreateWindow(const std::string& title, Rect2D rect, bool fullscreen = true);
    std::unique_ptr<Renderer> CreateRenderer(Window* window);
};

} // namespace bl
