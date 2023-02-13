#pragma once

#include <memory>

#include "config/Config.hpp"


class CRenderDevice;
class IWindow : public IConfigurable
{
public:
    virtual VkSurfaceKHR    CreateSurface(const CRenderDevice *const) = 0;
    virtual VkExtent2D      GetExtent() const noexcept = 0;
};

IWindow* CreateWindow();

inline std::unique_ptr<IWindow> g_pWindow;