#pragma once

#include <memory>

#include <vulkan/vulkan.h>

#include "config/Configurable.hpp"

class IWindow : public IConfigurable
{
public:
    virtual VkSurfaceKHR    CreateSurface(VkInstance instance) = 0;
    virtual VkExtent2D      GetExtent() const noexcept = 0;
};

IWindow* CreateWindow();

inline std::unique_ptr<IWindow> g_pWindow;