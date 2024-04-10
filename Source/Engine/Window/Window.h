#pragma once

#include "Precompiled.h"
#include "Export.h"
#include "Engine/SDLSubsystem.h"
#include "Math/Rect.h"
#include "Display.h"
#include "VideoMode.h"

namespace bl {

class BLUEMETAL_API Window 
{
public:
    static void useTemporaryWindow(const std::function<void(SDL_Window*)>& func);

public:
    Window(
        const VideoMode&                video,
        const std::string&              title,
        std::shared_ptr<Display>        display = nullptr);
    ~Window();

public:
    /** @brief Gets the current window extent in pixels. */
    glm::ivec2 getExtent() const;

    /** @brief Returns the underlying SDL window object. */
    SDL_Window* get() const;

    /** @brief Changes the title displayed on the top of a windowed window. */
    void setTitle(const std::string& title);


private:
    void createWindow(
        const VideoMode&            video,
        const std::string&          title,
        std::shared_ptr<Display>&   display);

    SDL_Window* _window;
    VkSurfaceKHR _surface;
};

} // namespace bl
