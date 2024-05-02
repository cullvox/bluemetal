#include "Core/Print.h"
#include "Engine/Engine.h"
#include "GraphicsSystem.h"

namespace bl 
{

GraphicsSystem::GraphicsSystem(Engine& engine)
    : _engine(engine)
    , _instance({}, {}, true)
    , _physicalDevice(_instance.ChoosePhysicalDevice())
    , _device(_instance, _physicalDevice)
    , _window()
{

    auto displays = Display::getDisplays();
    _window = std::make_shared<RenderWindow>(_instance, displays[0]->getDesktopMode(), "Maginvox");

    _descriptorCache = std::make_shared<DescriptorSetLayoutCache>(_device);
    _pipelineLayoutCache = std::make_shared<PipelineLayoutCache>(_device);
    _swapchain = std::make_shared<Swapchain>(_device, _window);
    _renderer = std::make_shared<Renderer>(_device, _swapchain);
}

GraphicsSystem::~GraphicsSystem() 
{
}

Instance& GraphicsSystem::GetInstance() 
{ 
    return _instance; 
}

PhysicalDevice& GraphicsSystem::GetPhysicalDevice() 
{ 
    return _physicalDevice; 
}

RenderWindow& GraphicsSystem::GetWindow() 
{ 
    return _window; 
}

Device& GraphicsSystem::GetDevice() 
{ 
    return _device; 
}

Renderer& GraphicsSystem::GetRenderer() 
{ 
    return _renderer; 
}

DescriptorSetLayoutCache& GraphicsSystem::GetDescriptorCache() 
{ 
    return _descriptorCache; 
}

PipelineLayoutCache& GraphicsSystem::GetPipelineLayoutCache() 
{ 
    return _pipelineLayoutCache; 
}

std::vector<Display*> GrahphicsSystem::GetDisplays()
{
    int displayCount = SDL_GetNumVideoDisplays();

    static std::vector<Display*> displays = [displayCount](){
        std::vector<Display*> temp;

        for (int i = 0; i < displayCount; i++)
            temp.push_back(new Display(i));

        return temp;
    }();

    return displays;
}
}

std::unique_ptr<Resource> BuildResource(const std::string& type, const std::filesystem::path& path)
{
    if (type == "shader")
    {

        path.get_extension

    }
}

} // namespace bl
