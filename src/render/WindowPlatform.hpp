#pragma once

//===========================//
#include <memory>
#include <vector>
//===========================//
#include "core/Version.hpp"
#include "config/Config.hpp"
#include "Monitor.hpp"
#include "Window.hpp"
//===========================//

class IWindowPlatform
{
public:
    IWindowPlatform() = default;
    virtual ~IWindowPlatform() = default;
    virtual std::shared_ptr<IWindow> CreateWindow(const std::string& title, CConfig* const pConfig = g_pConfig.get()) = 0;
    virtual const std::vector<std::shared_ptr<IMonitor>>& GetMonitors() = 0;
};

enum class EWindowPlatformAPI
{
    eGLFW = 0
};

std::shared_ptr<IWindowPlatform> CreateWindowPlatform(EWindowPlatformAPI);

inline std::shared_ptr<IWindowPlatform> g_pWindowPlatform{  };
