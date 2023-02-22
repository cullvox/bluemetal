#include <iostream>

#include "config/Config.hpp"
#include "render/WindowPlatform.hpp"
#include "render/RenderDevice.hpp"

int main(int argc, const char** argv)
{
    std::cout << "My Test" << std::endl;
    CConfig newConfig{};

    auto windowPlatform = CreateWindowPlatform(EWindowPlatformAPI::eGLFW);
    std::shared_ptr<IWindow> window = windowPlatform->CreateWindow("Dark Red", &newConfig);
    std::shared_ptr<CRenderDevice> renderDevice = std::make_shared<CRenderDevice>(windowPlatform.get(), window.get(), &newConfig);

    while(true)
    {
        windowPlatform->PollEvents();
    }

}