#pragma once


#include "Export.h"
#include "Window/Window.h"
#include "Render/Instance.h"
#include "Render/Device.h"
#include "Render/Swapchain.h"
#include "Render/Renderer.h"
#include "Render/FrameCounter.h"
#include "Input/InputSystem.h"

class BLUEMETAL_API blEngine
{
public:
    blEngine(const std::string& appName);
    ~blEngine();

    bool run();

private:
    bool _close;
    std::shared_ptr<blWindow> _window;
    std::shared_ptr<blDevice> _renderDevice;
    std::shared_ptr<blSwapchain> _swapchain;
    std::shared_ptr<blRenderPass> _presentPass;
    std::shared_ptr<blRenderer> _renderer;
    std::shared_ptr<blInputSystem> _inputSystem;
    blFrameCounter _frameCounter;
    std::queue<std::function<void()>> _postRenderCommands;
};