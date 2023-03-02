#include <iostream>
#include <optional>

#include "Window/Screen.hpp"
#include "config/Config.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Swapchain.hpp"
#include "Render/Renderer.hpp"

#include <spdlog/spdlog.h>

#include <chrono>

int main(int argc, const char** argv)
{
    bl::Window window{bl::Screen::getScreen(0).getDesktopMode(), std::nullopt};
    bl::RenderDevice renderDevice{window};
    bl::Swapchain swapchain{window, renderDevice};
    bl::Renderer renderer{renderDevice, swapchain};

    int frames;
    auto previous = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    while (true) // window.shouldNotClose()
    {
        now = std::chrono::system_clock::now();
        if (now - previous > std::chrono::seconds(1))
        {
            previous = now;
            spdlog::info("FPS: {}", frames);
            frames = 0;
        }

        renderer.beginFrame();

        

        renderer.endFrame();
        frames++;
    }
}