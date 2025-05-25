#include "Core/Print.h"
#include "Engine/Engine.h"
#include "Graphics/VulkanDevice.h"
#include "Graphics/VulkanInstance.h"
#include "VulkanShader.h"
#include "Texture2D.h" 
#include "Model.h"
#include "GraphicsSystem.h"

namespace bl 
{

GraphicsSystem::GraphicsSystem(Engine* engine)
    : _engine(engine)
{
    _instance = {{}, "Maginvox", true};
    _physicalDevice = _instance.ChoosePhysicalDevice();
    _device = std::make_unique<VulkanDevice>(&_instance, _physicalDevice);
}

GraphicsSystem::~GraphicsSystem()
{
}

VulkanInstance* GraphicsSystem::GetInstance() 
{ 
    return &_instance;
}

VulkanPhysicalDevice* GraphicsSystem::GetPhysicalDevice() const
{ 
    return _physicalDevice; 
}

VulkanDevice* GraphicsSystem::GetDevice() 
{ 
    return _device.get(); 
}

std::unique_ptr<Window> GraphicsSystem::CreateWindow(const std::string& title, Rect2D videoMode, bool fullscreen)
{
    return std::make_unique<VulkanWindow>(_device.get(), title, videoMode, fullscreen);
}

std::unique_ptr<Renderer> GraphicsSystem::CreateRenderer(Window* window)
{
    auto vulkanWindow = dynamic_cast<VulkanWindow*>(window);
    assert(vulkanWindow != nullptr);

    return std::make_unique<Renderer>(_device.get(), vulkanWindow);
}

std::unique_ptr<Resource> GraphicsSystem::BuildResource(ResourceManager* manager, const std::string& type, const std::filesystem::path& path, const nlohmann::json& json)
{
    if (type == "Shader") 
    {
        return std::make_unique<VulkanShader>(manager, json, _device.get());
    } 
    else if (type == "Texture") 
    {
        return std::make_unique<Texture2D>(manager, json, _device.get());
    } 
    else if (type == "Model")
    {
        return std::make_unique<Model>(manager, json, _device.get());
    }
    else 
    {
        throw std::runtime_error("Could not create a resource that was not specified!");
    }
}

} // namespace bl
