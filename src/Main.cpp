#include <iostream>

#include "config/Config.hpp"
#include "Render/RenderDevice.hpp"

int main(int argc, const char** argv)
{
    bl::Window window{bl::Screen::getScreen(0).getDesktopMode()};
    
    window.getExtent();

}