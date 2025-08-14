#include "Core/Print.h"
#include "Engine/Engine.h"
#include "Graphics/VulkanDevice.h"
#include "Graphics/VulkanInstance.h"
#include "VulkanShader.h"

#include "GraphicsSystem.h"

#include "ImGui/ImGuiSystem.h"

namespace bl 
{

GraphicsSystem::GraphicsSystem(Engine* engine)
    : _engine(engine)
{
    _instance = {{}, "Maginvox", true};
    _physicalDevice = _instance.ChoosePhysicalDevice();
    _device = std::make_unique<VulkanDevice>(&_instance, _physicalDevice);

    
    auto displays = Display::GetDisplays();

    _device->WaitForDevice();
    _window = CreateWindow("Maginvox", Rect2D{{}, displays[0].GetDesktopMode().extent}, false);

    _renderer = CreateRenderer(_window.get());



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

} // namespace bl
