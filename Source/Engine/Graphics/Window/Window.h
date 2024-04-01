#pragma once

#include "Display.h"
#include "Export.h"
#include "Graphics/Instance.h"
#include "Math/Vector2.h"
#include "Precompiled.h"
#include "VideoMode.h"

namespace bl {

class BLUEMETAL_API Window 
{
public:

    struct CreateInfo 
    {
        std::shared_ptr<GfxInstance> instance;
        VideoMode mode;
        std::string title = "Window";
        std::shared_ptr<Display> display = nullptr;
    };

    Window(const CreateInfo &createInfo);

    ~Window();

public:
    /** @brief Gets the current window extent in pixels. */
    Extent2D getExtent() const;

    /** @brief Returns the Vulkan surface created with this window. */
    VkSurfaceKHR getSurface() const;

    /** @brief Returns the underlying SDL window object. */
    SDL_Window* get() const;

    /** @brief Changes the title displayed on the top of a windowed window. */
    void setTitle(const std::string& title);

private:
    void createWindow(const CreateInfo& createInfo);
    void createSurface();

    std::shared_ptr<GfxInstance> _instance;
    SDL_Window* _pWindow;
    VkSurfaceKHR _surface;
};

} // namespace bl
