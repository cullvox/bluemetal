#include "Core/Print.h"

#include "VulkanDevice.h"
#include "VulkanInstance.h"
#include "VulkanShader.h"
#include "VulkanWindow.h"

#include "Resources/Material.h"
#include "Resources/MaterialInstance.h"
#include "Resources/Model.h"
#include "Resources/ResourceSystem.h"
#include "Resources/Sampler.h"
#include "Resources/Shader.h"
#include "Resources/Texture2D.h"
#include "Resources/NoiseTexture2D.h"

#include "Engine/Engine.h"
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

    _vulkanInstance = std::make_unique<VulkanInstance>(bl::Version { bl::VersionRelease::eAlpha, 0, 1, 7 }, "bluemetal", true);
    _physicalDevice = _vulkanInstance->ChoosePhysicalDevice();
    _device = std::make_unique<VulkanDevice>(_vulkanInstance.get(), _physicalDevice);

    auto displays = Display::GetDisplays();

    _device->WaitForDevice();
    _window = std::make_unique<VulkanWindow>(_device.get(), "Maginvox", Rect2D { {}, displays[0].GetDesktopMode().extent }, false);
    _renderer = std::make_unique<Renderer>(_window.get(), engine.GetFrameCounter());

    _pointMaterial = rs->Load<Material>("Resources/Materials/DebugPoint.mat");
    _lineMaterial = rs->Load<Material>("Resources/Materials/DebugLine.mat");
    _triangleMaterial = rs->Load<Material>("Resources/Materials/DebugTriangle.mat");
    _renderer->SetDebugMaterialInstance(_pointMaterial.lock()->GetVulkanMaterial(), _lineMaterial.lock()->GetVulkanMaterial(), _triangleMaterial.lock()->GetVulkanMaterial());
}

GraphicsSystem::~GraphicsSystem()
{
}

std::shared_ptr<Resource> GraphicsSystem::ConstructResource(ResourceSystem& resourceSystem, std::size_t typeHash, const std::filesystem::path& path)
{
    if (typeHash == typeid(Shader).hash_code()) {
        return std::make_shared<Shader>(resourceSystem, this, path);
    } else if (typeHash == typeid(Sampler).hash_code()) {
        return std::make_shared<Sampler>(resourceSystem, this, path);
    } else if (typeHash == typeid(Material).hash_code()) {
        return std::make_shared<Material>(resourceSystem, this, path);
    } else if (typeHash == typeid(MaterialInstance).hash_code()) {
        return std::make_shared<MaterialInstance>(resourceSystem, this, path);
    } else if (typeHash == typeid(Texture2D).hash_code()) {
        return std::make_shared<Texture2D>(resourceSystem, this, path);
    } else if (typeHash == typeid(NoiseTexture2D).hash_code()) {
        return std::make_shared<NoiseTexture2D>(resourceSystem, this, path);
    } else if (typeHash == typeid(Mesh).hash_code()) {
        return std::make_shared<Mesh>(resourceSystem, this, path);
    } else if (typeHash == typeid(Model).hash_code()) {
        return std::make_shared<Model>(resourceSystem, this, path);
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

} // namespace bl
