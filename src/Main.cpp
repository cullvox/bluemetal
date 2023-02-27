#include <iostream>

#include "Window/Screen.hpp"
#include "config/Config.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Swapchain.hpp"
#include "Render/Renderer.hpp"

int main(int argc, const char** argv)
{
    bl::Window window{bl::Screen::getScreen(0).getDesktopMode(), bl::Screen::getScreen(0)};
    
    bl::Extent2D extent = window.getExtent();

    bl::RenderDevice renderDevice{window};
    bl::Swapchain swapchain{window, renderDevice};
    bl::Renderer renderer{renderDevice, swapchain};

}