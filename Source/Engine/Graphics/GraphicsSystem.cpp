#include "Core/Print.h"
#include "Engine/Engine.h"
#include "Graphics/VulkanDevice.h"
#include "Graphics/VulkanInstance.h"
#include "VulkanShader.h"

#include "GraphicsSystem.h"

#include "ImGui/ImGuiSystem.h"

namespace bl 
{

GraphicsSystem::GraphicsSystem()
{
    _vulkanInstance = std::make_unique<VulkanInstance>(bl::Version{bl::VersionRelease::eAlpha, 0, 1, 7}, "bluemetal", true);
    _physicalDevice = _vulkanInstance->ChoosePhysicalDevice();
    _device = std::make_unique<VulkanDevice>(_vulkanInstance.get(), _physicalDevice);

    auto displays = Display::GetDisplays();

    _device->WaitForDevice();
    _window = std::make_unique<VulkanWindow>(_device.get(), "Maginvox", Rect2D{{}, displays[0].GetDesktopMode().extent}, false);
    _renderer = std::make_unique<Renderer>(_window.get());
}

GraphicsSystem::~GraphicsSystem()
{
}

VulkanInstance* GraphicsSystem::GetInstance()
{ 
    return _vulkanInstance.get();
}

VulkanPhysicalDevice* GraphicsSystem::GetPhysicalDevice()
{ 
    return _physicalDevice; 
}

VulkanDevice* GraphicsSystem::GetDevice() 
{ 
    return _device.get(); 
}

} // namespace bl
