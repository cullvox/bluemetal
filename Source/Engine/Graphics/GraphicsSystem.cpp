#include "Core/Print.h"
#include "Engine/Engine.h"
#include "VulkanShader.h"
#include "GraphicsSystem.h"

namespace bl 
{

GraphicsSystem::GraphicsSystem(Engine* engine)
    : _engine(engine)
{
    _instance = std::make_unique<VulkanInstance>(Version{}, "Maginvox", true);
    _physicalDevice = _instance->ChoosePhysicalDevice();
    _device = std::make_unique<VulkanDevice>(_instance.get(), _physicalDevice);
    // _descriptorCache = std::make_unique<VulkanDescriptorSetLayoutCache>(_device.get());
    // _pipelineLayoutCache = std::make_unique<VulkanPipelineLayoutCache>(_device.get());
}

GraphicsSystem::~GraphicsSystem()
{
}

VulkanInstance* GraphicsSystem::GetInstance() 
{ 
    return _instance.get(); 
}

const VulkanPhysicalDevice* GraphicsSystem::GetPhysicalDevice() 
{ 
    return _physicalDevice; 
}

VulkanDevice* GraphicsSystem::GetDevice() 
{ 
    return _device.get(); 
}

std::vector<const VulkanPhysicalDevice*> GraphicsSystem::GetPhysicalDevices()
{
    return _instance->GetPhysicalDevices();
}

std::vector<Display*> GraphicsSystem::GetDisplays()
{
    int displayCount = SDL_GetNumVideoDisplays();

    _displays.clear();
    _displays.reserve(displayCount);

    std::vector<Display*> temp{};
    temp.reserve(displayCount);

    for (int i = 0; i < displayCount; i++)
    {
        _displays.push_back(std::make_unique<Display>(i));
        temp.push_back(_displays[i].get());
    }

    return temp;
}

std::unique_ptr<VulkanWindow> GraphicsSystem::CreateWindow(const std::string& title, std::optional<VideoMode> videoMode, bool fullscreen)
{
    return std::make_unique<VulkanWindow>(_device.get(), title, videoMode, fullscreen);
}

std::unique_ptr<Renderer> GraphicsSystem::CreateRenderer(VulkanWindow* window)
{
    return std::make_unique<Renderer>(_device.get(), window);
}

std::unique_ptr<Resource> GraphicsSystem::BuildResource(const std::string& type, const std::filesystem::path& path, const nlohmann::json& json)
{
    if (type == "Shader")
    {
        return std::make_unique<VulkanShader>(json, _device.get());
    }
    else
    {
        throw std::runtime_error("Could not create a resource that was not specified!");
    }
}

} // namespace bl
