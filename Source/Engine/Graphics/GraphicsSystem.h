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

class GraphicsSystem : public NonCopyable {
    static Engine* _engine;
    static VulkanInstance _instance;
    static VulkanPhysicalDevice* _physicalDevice;
    static std::unique_ptr<Window> _window;
    static std::unique_ptr<Renderer> _renderer;
    static std::unique_ptr<VulkanDevice> _device;
    static std::unique_ptr<ImGuiSystem> _imgui;

    GraphicsSystem(Engine* engine); /** @brief Constructor. */
    ~GraphicsSystem(); /** @brief Destructor */

public:
    static GraphicsSystem* GetInstance();

    static VulkanInstance* GetVulkanInstance();
    static VulkanPhysicalDevice* GetPhysicalDevice();
    static VulkanDevice* GetDevice();
    static Window* GetWindow() { return _window.get(); }
    static Renderer* GetRenderer() { return _renderer.get(); }
};

} // namespace bl
