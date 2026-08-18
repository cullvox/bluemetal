#include "Core/Print.h"

#include "Core/Version.h"
#include "Graphics/Viewport.h"
#include "Graphics/WindowViewport.h"
#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanShader.h"
#include "Renderer.h"
#include "WindowViewport.h"

#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/MaterialInstance.h"
#include "Resources/Model.h"
#include "Resources/ResourceSystem.h"
#include "Resources/Sampler.h"
#include "Resources/Shader.h"
#include "Resources/Texture2D.h"
#include "Resources/NoiseTexture2D.h"

#include "Engine/Engine.h"
#include "ImGui/ImGuiSystem.h"
#include "GraphicsSystem.h"

namespace bl {

GraphicsSystem::GraphicsSystem()
    : System()
{
    _vulkanInstance = std::make_unique<VulkanInstance>(engineVersion, "bluemetal", true);
    _physicalDevice = _vulkanInstance->ChoosePhysicalDevice();
    _device = std::make_unique<VulkanDevice>(_vulkanInstance.get(), _physicalDevice);

    auto displays = Display::GetDisplays();

    _device->WaitForDevice();

    // Create the renderer.
    _renderer = std::make_unique<Renderer>(_device.get());

    // Create the default window/surface's viewport.
    _windowViewport = std::make_unique<WindowViewport>(_renderer.get(), GetEngine()->GetWindow());

    // Enable ImGui rendering on the default viewport.
    _windowViewport->SetRenderFlags(_windowViewport->GetRenderFlags() | ViewportRenderFlags::eImGui);

    _renderer->AddViewport(_windowViewport.get());
}

GraphicsSystem::~GraphicsSystem()
{
}

GraphicsSystem* GraphicsSystem::Get()
{
    static GraphicsSystem system;
    return &system;
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

Renderer* GraphicsSystem::GetRenderer() 
{
    return _renderer.get(); 
}

WindowViewport* GraphicsSystem::GetViewport()
{
    return _windowViewport.get();
}

} // namespace bl
