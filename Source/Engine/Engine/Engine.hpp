#pragma once


#include "Export.h"
#include "Window/Window.h"
#include "Render/Instance.h"
#include "Render/Device.h"
#include "Render/Swapchain.h"
#include "Render/Renderer.h"
#include "Render/FrameCounter.h"

namespace bl
{

class BLUEMETAL_API Engine
{
public:
    blEngine(const std::string& appName);
    ~blEngine();

    bool run();

private:
    bool _close;
    std::shared_ptr<Window> _window;
    std::shared_ptr<Device> _renderDevice;
    std::shared_ptr<Swapchain> _swapchain;
    std::shared_ptr<RenderPass> _presentPass;
    std::shared_ptr<Renderer> _renderer;
    blFrameCounter _frameCounter;
    std::queue<std::function<void()>> _postRenderCommands;
};

} // namespace bl