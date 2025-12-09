#pragma once

#include "Engine/System.h"
#include "ImGui/ImGuiSystem.h"
#include "Renderer.h"
#include "VulkanDescriptorSetLayoutCache.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanPipelineLayoutCache.h"

namespace bl {

class Engine;
class ResourceSystem;

class GraphicsSystem : public System {
    std::unique_ptr<VulkanInstance> _vulkanInstance;
    std::unique_ptr<VulkanDevice> _device;
    VulkanPhysicalDevice* _physicalDevice;
    std::unique_ptr<VulkanWindow> _window;
    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<ImGuiSystem> _imgui;

public:
    GraphicsSystem(Engine& engine);
    ~GraphicsSystem();

    virtual std::shared_ptr<Resource> ConstructResource(ResourceSystem& resourceSystem, std::size_t typeHash, const std::filesystem::path& path) override;

    VulkanInstance* GetInstance();
    VulkanPhysicalDevice* GetPhysicalDevice();
    VulkanDevice* GetDevice();
    VulkanWindow* GetWindow() { return _window.get(); }
    Renderer* GetRenderer() { return _renderer.get(); }
};

} // namespace bl
