#pragma once

#include "Export.h"

#include "Resource/ResourceManager.h"
#include "Graphics/Instance.h"
#include "Graphics/RenderWindow.h"
#include "Graphics/Device.h"
#include "Graphics/DescriptorSetLayoutCache.h"
#include "Graphics/PipelineLayoutCache.h"
#include "Graphics/Swapchain.h"
#include "Graphics/FrameCounter.h"
#include "Graphics/Renderer.h"

namespace bl
{

class Engine;

class GraphicsSystem : public NonCopyable, public ResourceBuilder
{
public:
    GraphicsSystem(Engine* engine); /** @brief Constructor. */
    ~GraphicsSystem(); /** @brief Destructor */

    Instance* GetInstance();
    PhysicalDevice* GetPhysicalDevice();
    Device* GetDevice();
    DescriptorSetLayoutCache* GetDescriptorCache();
    PipelineLayoutCache* GetPipelineLayoutCache();
    std::vector<PhysicalDevice*> GetPhysicalDevices();
    std::vector<Display*> GetDisplays();
    std::shared_ptr<RenderWindow> CreateWindow();

    virtual std::unique_ptr<Resource> BuildResource(const std::string& type, const std::filesystem::path& path);

private:
    Engine* _engine;
    std::unique_ptr<Instance> _instance;
    PhysicalDevice* _physicalDevice;
    std::unique_ptr<Device> _device;

    DescriptorSetLayoutCache _descriptorCache;
    PipelineLayoutCache _pipelineLayoutCache;

    std::vector<std::unique_ptr<Display>> _displays;
};

} // namespace bl
