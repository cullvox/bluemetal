#pragma once

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

    std::unique_ptr<Window> CreateWindow(const std::string& title, Rect2D rect, bool fullscreen = true);
    std::unique_ptr<Renderer> CreateRenderer(Window* window);

    virtual std::unique_ptr<Resource> BuildResource(ResourceManager* manager, const std::string& type, const std::filesystem::path& path, const nlohmann::json& data);

private:
    Engine* _engine;
    VulkanInstance _instance;
    VulkanPhysicalDevice* _physicalDevice;
    std::unique_ptr<VulkanDevice> _device;
};

} // namespace bl
