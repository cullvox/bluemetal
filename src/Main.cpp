#include <iostream>

#include "config/Config.hpp"
#include "render/WindowPlatform.hpp"

int main(int argc, const char** argv)
{
    std::cout << "My Test" << std::endl;
    CConfig newConfig{};

    auto windowPlatform = CreateWindowPlatform(EWindowPlatformAPI::eGLFW);
    windowPlatform->CreateWindow("Dark Red");

}