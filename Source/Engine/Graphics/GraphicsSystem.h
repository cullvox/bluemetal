#pragma once

#include "Export.h"

#include "Resource/ResourceManager.h"
#include "Graphics/RenderDoc.h"
#include "Graphics/Instance.h"
#include "Graphics/RenderWindow.h"
#include "Graphics/Device.h"
#include "Graphics/DescriptorSetLayoutCache.h"
#include "Graphics/PipelineLayoutCache.h"
#include "Graphics/Swapchain.h"
#include "Graphics/FrameCounter.h"
#include "Graphics/Renderer.h"

namespace bl
{

class Engine;

class GraphicsSystem : public NonCopyable, public ResourceBuilder
{
public:
    GraphicsSystem(Engine& engine); /** @brief Constructor. */
    ~GraphicsSystem(); /** @brief Destructor */

    Window& GetWindow();
    Instance& GetInstance();
    PhysicalDevice& GetPhysicalDevice();
    Device& GetDevice();
    DescriptorSetLayoutCache& GetDescriptorCache();
    PipelineLayoutCache& GetPipelineLayoutCache();
    Renderer& GetRenderer();

private:
    Engine& _engine;
    Instance _instance;
    PhysicalDevice& _physicalDevice;
    Device _device;
    Window _window;
    DescriptorSetLayoutCache _descriptorCache;
    PipelineLayoutCache _pipelineLayoutCache;
    Swapchain _swapchain;
    Renderer _renderer;
};

} // namespace bl
