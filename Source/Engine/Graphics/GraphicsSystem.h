#pragma once

#include "Engine/System.h"
#include "Core/ReferenceCounted.h"

namespace bl {

class Engine;
class VulkanInstance;
class VulkanPhysicalDevice;
class VulkanDevice;
class Renderer;
class ResourceSystem;
class ImGuiSystem;
class Material;
class VulkanWindow;

class GraphicsSystem : public System {
    std::unique_ptr<VulkanInstance> _vulkanInstance;
    std::unique_ptr<VulkanDevice> _device;
    VulkanPhysicalDevice* _physicalDevice;
    std::unique_ptr<VulkanWindow> _window;
    std::unique_ptr<Renderer> _renderer;
    std::unique_ptr<ImGuiSystem> _imgui;
    Ref<Material> _pointMaterial;
    Ref<Material> _lineMaterial;
    Ref<Material> _triangleMaterial;
    Ref<Material> _selectionMaterial;

public:
    GraphicsSystem(Engine& engine);
    ~GraphicsSystem();

    virtual std::shared_ptr<Resource> ConstructResource(std::size_t typeHash, const std::filesystem::path& path) override;

    VulkanInstance* GetInstance();
    VulkanPhysicalDevice* GetPhysicalDevice();
    VulkanDevice* GetDevice();
    VulkanWindow* GetWindow();
    Renderer* GetRenderer();
};

} // namespace bl
