#pragma once

#include "Window.hpp"

class CWindowGLFW : public IWindow
{
    CWindowGLFW(const std::string& title, VkExtent2D extent, CConfig* pConfig = g_pConfig.get())
};