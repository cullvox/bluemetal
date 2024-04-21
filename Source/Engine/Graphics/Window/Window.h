#pragma once

#include "Precompiled.h"
#include "Export.h"
#include "Engine/SDLSubsystem.h"
#include "Math/Rect.h"
#include "Display.h"
#include "VideoMode.h"

namespace bl 
{

class Window : public NonCopyable
{
public:
    static void UseTemporaryWindow(const std::function<void(SDL_Window*)>& func);

public:
    Window(const VideoMode& video, const std::string& title, std::shared_ptr<Display> display = nullptr);
    ~Window();

    SDL_Window* Get() const; /** @brief Returns the underlying SDL window object. */
    VideoMode GetVideoMode() const; /** @brief Returns the current video mode. */
    void SetTitle(const std::string& title); /** @brief Changes the title displayed on the top of a windowed window. */
    void SetVideoMode(const VideoMode& mode); 

private:
    void CreateWindow(const VideoMode& video, const std::string& title, std::shared_ptr<Display>& display);
    
    static void OnResize(); // Callback for window resizing from the input system.

    SDL_Window* _window;
};

} // namespace bl
