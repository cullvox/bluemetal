#include "Core/Print.h"
#include "Engine/Engine.h"
#include "Graphics/VulkanInstance.h"
#include "VulkanShader.h"
#include "Texture2D.h" 
#include "GraphicsSystem.h"

namespace bl 
{

GraphicsSystem::GraphicsSystem(Engine* engine)
    : _engine(engine)
{
    _instance = {{}, "Maginvox", true};
    _physicalDevice = _instance.ChoosePhysicalDevice();
    _device = {&_instance, _physicalDevice};
}

GraphicsSystem::~GraphicsSystem()
{
}

VulkanInstance* GraphicsSystem::GetInstance() 
{ 
    return &_instance;
}

const VulkanPhysicalDevice* GraphicsSystem::GetPhysicalDevice() const
{ 
    return _physicalDevice; 
}

VulkanDevice* GraphicsSystem::GetDevice() 
{ 
    return &_device; 
}

std::unique_ptr<Window> GraphicsSystem::CreateWindow(const std::string& title, Rect2D videoMode, bool fullscreen)
{
    return std::make_unique<VulkanWindow>(&_device, title, videoMode, fullscreen);
}

std::unique_ptr<Renderer> GraphicsSystem::CreateRenderer(Window* window)
{
    auto vulkanWindow = dynamic_cast<VulkanWindow*>(window);
    assert(vulkanWindow != nullptr);

    return std::make_unique<Renderer>(&_device, vulkanWindow);
}

std::unique_ptr<Resource> GraphicsSystem::BuildResource(const std::string& type, const std::filesystem::path& path, const nlohmann::json& json)
{
    if (type == "Shader") {
        return std::make_unique<VulkanShader>(json, &_device);
    } else if (type == "Texture") {
        return std::make_unique<Texture2D>(json, &_device);
    } else {
        throw std::runtime_error("Could not create a resource that was not specified!");
    }
}

} // namespace bl
