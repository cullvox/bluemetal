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

GraphicsSystem::GraphicsSystem(Engine& engine)
    : System(engine)
{
    auto rs = GetEngine().GetResourceSystem();

    rs->AddSystemType<Shader>(this);
    rs->AddSystemType<Sampler>(this);
    rs->AddSystemType<Material>(this);
    rs->AddSystemType<MaterialInstance>(this);
    rs->AddSystemType<Texture2D>(this);
    rs->AddSystemType<NoiseTexture2D>(this);
    rs->AddSystemType<Mesh>(this);
    rs->AddSystemType<Model>(this);

    _vulkanInstance = std::make_unique<VulkanInstance>(engineVersion, "bluemetal", true);
    _physicalDevice = _vulkanInstance->ChoosePhysicalDevice();
    _device = std::make_unique<VulkanDevice>(_vulkanInstance.get(), _physicalDevice);

    auto displays = Display::GetDisplays();

    _device->WaitForDevice();

    // Create the renderer.
    _renderer = std::make_unique<Renderer>(_device.get(), engine.GetFrameCounter());
    
    // Create the default window/surface's viewport.
    _windowViewport = std::make_unique<WindowViewport>(_renderer.get(), engine.GetWindow());
    
    // Enable ImGui rendering on the default viewport.
    _windowViewport->SetRenderFlags(_windowViewport->GetRenderFlags() | ViewportRenderFlags::eImGui);

    _renderer->AddViewport(_windowViewport.get());
}

GraphicsSystem::~GraphicsSystem()
{
}

std::shared_ptr<Resource> GraphicsSystem::ConstructResource(std::size_t typeHash, const std::filesystem::path& path)
{
    if (typeHash == typeid(Shader).hash_code()) {
        return std::make_shared<Shader>(GetEngine(), path);
    } else if (typeHash == typeid(Sampler).hash_code()) {
        return std::make_shared<Sampler>(GetEngine(), path);
    } else if (typeHash == typeid(Material).hash_code()) {
        return std::make_shared<Material>(GetEngine(), path);
    } else if (typeHash == typeid(MaterialInstance).hash_code()) {
        return std::make_shared<MaterialInstance>(GetEngine(), path);
    } else if (typeHash == typeid(Texture2D).hash_code()) {
        return std::make_shared<Texture2D>(GetEngine(), path);
    } else if (typeHash == typeid(NoiseTexture2D).hash_code()) {
        return std::make_shared<NoiseTexture2D>(GetEngine(), path);
    } else if (typeHash == typeid(Mesh).hash_code()) {
        return std::make_shared<Mesh>(GetEngine(), path);
    } else if (typeHash == typeid(Model).hash_code()) {
        return std::make_shared<Model>(GetEngine(), path);
    }

    return nullptr;
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
