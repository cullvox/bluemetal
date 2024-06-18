#pragma once

#include "Resource/ResourceManager.h"
#include "Window.h"
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

class GraphicsSystem : public NonCopyable, public ResourceBuilder
{
public:
    GraphicsSystem(Engine* engine); /** @brief Constructor. */
    virtual ~GraphicsSystem(); /** @brief Destructor */

    VulkanInstance* GetInstance();
    const VulkanPhysicalDevice* GetPhysicalDevice();
    VulkanDevice* GetDevice();
    VulkanDescriptorSetLayoutCache* GetDescriptorCache();
    VulkanPipelineLayoutCache* GetPipelineLayoutCache();
    std::vector<const VulkanPhysicalDevice*> GetPhysicalDevices();
    std::vector<Display*> GetDisplays();
    
    std::unique_ptr<VulkanWindow> CreateWindow(const std::string& title, std::optional<VideoMode> videoMode = std::nullopt, bool fullscreen = true);
    std::unique_ptr<Renderer> CreateRenderer(VulkanWindow* window);

    virtual std::unique_ptr<Resource> BuildResource(const std::string& type, const std::filesystem::path& path, const nlohmann::json& data);

private:
    Engine* _engine;
    std::unique_ptr<VulkanInstance> _instance;
    const VulkanPhysicalDevice* _physicalDevice;
    std::unique_ptr<VulkanDevice> _device;

    std::vector<std::unique_ptr<Display>> _displays;
};

} // namespace bl
