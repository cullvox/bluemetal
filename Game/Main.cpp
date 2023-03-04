#include <iostream>
#include <optional>

#include "Window/Screen.hpp"
#include "Config/Config.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Swapchain.hpp"
#include "Render/Renderer.hpp"
#include "Render/FrameCounter.hpp"
#include "Input/InputSystem.hpp"

#include <spdlog/spdlog.h>

int main(int argc, const char** argv)
{
    bl::Window window{bl::Screen::getScreen(0).getDesktopMode(), std::nullopt};
    bl::RenderDevice renderDevice{window};
    bl::Swapchain swapchain{window, renderDevice};
    bl::Renderer renderer{renderDevice, swapchain};
    bl::FrameCounter frameCounter{};
    bl::InputSystem inputSystem{window};

    while (true) // window.shouldNotClose()
    {
        frameCounter.beginFrame();

        inputSystem.poll();

        renderer.beginFrame();
        
        renderer.endFrame();

        if (frameCounter.endFrame())
        {
            spdlog::info("Rendering at: {} f/s and {} ms/f\n\tavg f/s: {} and avg ms/f: {}", frameCounter.getFramesPerSecond(), frameCounter.getMillisecondsPerFrame(), frameCounter.getAverageFramesPerSecond(10), frameCounter.getAverageMillisecondsPerFrame(10));
        }
        
    }
}