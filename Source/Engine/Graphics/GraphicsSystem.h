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

namespace bl {

class Engine;

class GraphicsSystem 
{
    Engine* _engine;
    std::unique_ptr<VulkanInstance> _vulkanInstance;
    VulkanPhysicalDevice* _physicalDevice;
    std::unique_ptr<VulkanWindow> _window;
    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<VulkanDevice> _device;
    std::unique_ptr<ImGuiSystem> _imgui;

    GraphicsSystem(); /** @brief Constructor. */
    ~GraphicsSystem(); /** @brief Destructor */

public:
    static GraphicsSystem* GetInstance();

    VulkanInstance* GetVulkanInstance();
    VulkanPhysicalDevice* GetPhysicalDevice();
    VulkanDevice* GetDevice();
    VulkanWindow* GetWindow() { return _window.get(); }
    Renderer* GetRenderer() { return _renderer.get(); }
};

} // namespace bl
