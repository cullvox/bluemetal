#pragma once

#include "Resource/ResourceManager.h"
#include "Window/Window.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanDescriptorSetLayoutCache.h"
#include "VulkanPipelineLayoutCache.h"
#include "Renderer.h".

namespace bl {

class Engine;

class GraphicsSystem : 
    public NonCopyable,
    public ResourceBuilder {
public:

    /// @brief Initializes the graphics system.
    GraphicsSystem(
        Engine* engine, 
        bool quickInit = true); /** @brief Constructor. */
    virtual ~GraphicsSystem(); /** @brief Destructor */

    const VulkanInstance* GetInstance() const;
    const VulkanPhysicalDevice* GetPhysicalDevice() const;
    const VulkanDevice* GetDevice() const;
    VulkanDescriptorSetLayoutCache* GetDescriptorCache();
    VulkanPipelineLayoutCache* GetPipelineLayoutCache();
    std::vector<const VulkanPhysicalDevice*> GetPhysicalDevices();
    VulkanWindow CreateRenderWindow(const std::string& title, Rect2D rect, bool fullscreen = true);

    virtual std::unique_ptr<Resource> BuildResource(const std::string& type, const std::filesystem::path& path, const nlohmann::json& data);

private:
    Engine* _engine;
    VulkanInstance _instance;
    const VulkanPhysicalDevice* _physicalDevice;
    VulkanDevice _device;
    VulkanDescriptorSetLayoutCache _descriptorSetLayoutCache;
    VulkanPipelineLayoutCache _pipelineLayoutCache;

};

} // namespace bl
