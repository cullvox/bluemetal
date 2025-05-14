#pragma once

#include "Resource/ResourceManager.h"
#include "Window/Window.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanDescriptorSetLayoutCache.h"
#include "VulkanPipelineLayoutCache.h"
#include "Renderer.h"

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
    const VulkanPhysicalDevice* GetPhysicalDevice() const;
    VulkanDevice* GetDevice();

    std::unique_ptr<Window> CreateWindow(const std::string& title, Rect2D rect, bool fullscreen = true);
    std::unique_ptr<Renderer> CreateRenderer(Window* window);

    virtual std::unique_ptr<Resource> BuildResource(const std::string& type, const std::filesystem::path& path, const nlohmann::json& data);

private:
    Engine* _engine;
    VulkanInstance _instance;
    const VulkanPhysicalDevice* _physicalDevice;
    VulkanDevice _device;
};

} // namespace bl
