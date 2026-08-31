#pragma once

#include "Engine/System.h"
#include "Core/Reference.h"

namespace bl {

class Engine;
class VulkanInstance;
class VulkanPhysicalDevice;
class VulkanDevice;
class Renderer;
class ResourceSystem;
class ImGuiSystem;
class Material;
class WindowViewport;

class GraphicsSystem : public System {
    std::unique_ptr<VulkanInstance> _vulkanInstance;
    std::unique_ptr<VulkanDevice> _device;
    VulkanPhysicalDevice* _physicalDevice;
    std::unique_ptr<WindowViewport> _windowViewport;
    std::unique_ptr<Renderer> _renderer;
    Ref<Material> _pointMaterial;
    Ref<Material> _lineMaterial;
    Ref<Material> _triangleMaterial;
    Ref<Material> _selectionMaterial;

    GraphicsSystem();
    ~GraphicsSystem();

public:
    static GraphicsSystem* Get();

    VulkanInstance* GetInstance();
    VulkanPhysicalDevice* GetPhysicalDevice();
    VulkanDevice* GetDevice();
    WindowViewport* GetViewport();
    Renderer* GetRenderer();
};

} // namespace bl
