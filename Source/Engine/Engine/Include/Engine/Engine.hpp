#pragma once

#define SDL_MAIN_HANDLED

#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Swapchain.hpp"
#include "Render/Renderer.hpp"
#include "Render/FrameCounter.hpp"
#include "Input/InputSystem.hpp"
#include "Engine/Export.h"


class BLOODLUST_ENGINE_API blEngine
{
public:
    blEngine(const std::string& applicationName);
    ~blEngine();

    bool run();

private:
    std::shared_ptr<blWindow> _window;
    std::shared_ptr<blRenderDevice> _renderDevice;
    std::shared_ptr<blSwapchain> _swapchain;
    std::shared_ptr<blRenderPass> _presentPass;
    std::shared_ptr<blRenderer> _renderer;
    std::shared_ptr<blInputSystem> _inputSystem;
    blFrameCounter _frameCounter;
    bool _close;
};