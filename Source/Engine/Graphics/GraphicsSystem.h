#pragma once

#include "Resource/ResourceManager.h"
#include "Instance.h"
#include "Device.h"
#include "Window.h"
#include "Swapchain.h"
#include "DescriptorSetLayoutCache.h"
#include "PipelineLayoutCache.h"
#include "Renderer.h"

namespace bl
{

class Engine;

class GraphicsSystem : public NonCopyable, public ResourceBuilder
{
public:
    GraphicsSystem(Engine* engine); /** @brief Constructor. */
    virtual ~GraphicsSystem(); /** @brief Destructor */

    Instance* GetInstance();
    PhysicalDevice* GetPhysicalDevice();
    Device* GetDevice();
    DescriptorSetLayoutCache* GetDescriptorCache();
    PipelineLayoutCache* GetPipelineLayoutCache();
    std::vector<PhysicalDevice*> GetPhysicalDevices();
    std::vector<Display*> GetDisplays();
    
    std::unique_ptr<Window> CreateWindow(const std::string& title, std::optional<VideoMode> videoMode = std::nullopt, bool fullscreen = true);
    std::unique_ptr<Renderer> CreateRenderer(Window* window);

    virtual std::unique_ptr<Resource> BuildResource(const std::string& type, const std::filesystem::path& path, const nlohmann::json& data);

private:
    Engine* _engine;
    std::unique_ptr<Instance> _instance;
    PhysicalDevice* _physicalDevice;
    std::unique_ptr<Device> _device;

    std::unique_ptr<DescriptorSetLayoutCache> _descriptorCache;
    std::unique_ptr<PipelineLayoutCache> _pipelineLayoutCache;

    std::vector<std::unique_ptr<Display>> _displays;
};

} // namespace bl