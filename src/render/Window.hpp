#pragma once

#include <memory>

#include "config/Config.hpp"
#include "vulkan/vulkan_core.h"


class IWindow : public IConfigurable
{
public:
    IWindow(uint32_t windowIndex = 0, CConfig* pConfig = g_pConfig.get());
    ~IWindow();

    virtual VkSurfaceKHR    GetSurface() const noexcept = 0;
    virtual VkExtent2D      GetExtent() const noexcept = 0;
};

inline std::unique_ptr<IWindow> g_pWindow;