#pragma once

#include "Precompiled.h"
#include "Core/Singleton.h"
#include "Display.h"
#include "Window.h"

namespace bl {

/// @brief A window platform api.
/// There may be more platforms and direct native support later.
enum class WindowPlatformApi {
    Glfw, ///!< gh:glfw/glfw | The popular platform agnostic windowing library.
    Sdl, ///!< gh:libsdl-org/sdl | The popular platform agnostic multimedia library.
};

/// @brief Options for changing window styles and movement options. 
enum class FullscreenMode {
    Windowed = 0, ///!< Disables all fullscreen settings and keeps on window borders and styles.
    Borderless = 1, ///!< Fake fullscreen, disables all borders while remaining in a windowed mode.
    Fullscreen = 2, ///!< Disables all borders and enables a true fullscreen video mode.
};

/// @brief Generic control for window systems.
class WindowPlatform : public Singleton<WindowPlatform> {
public:

    /// @brief Returns the recommended available window platform api on this device. 
    static WindowPlatformApi GetRecommendedApi();

    /// @brief Initializes a window platform.
    ///
    /// A window platform can only be initialized once therefore a program must
    /// be terminated and reopened to change the platform. If this function is
    /// called more than once it will return the previously created singleton,
    /// ignoring the argument.
    ///
    /// The WindowPlatform object returned is owned as a singleton.
    ///
    /// @param[in] api The api to initalize, safe recommendation from @ref GetRecommendedApi().
    ///
    /// @returns A pointer to the window platform.
    static WindowPlatform* Initialize(WindowPlatformApi api = GetRecommendedApi());

public:
    virtual ~WindowPlatform() = 0;

    /// @brief Returns the input object to interact with this platform's input devices.
    virtual Input& GetInput() = 0;

    /// @brief Returns the displays currently connected to the device.
    ///
    /// Displays may change in real time after a poll event any display may
    /// change or become unavailable. To reasonably correct for this, use a
    /// delegate to callback to when displays change in the input object.
    ///
    /// The display objects returned are owned by the window platform object.
    ///
    virtual std::vector<Display*> GetDisplays() = 0;

    /// @brief Creates a window.
    ///
    /// The Window object returned is owned by the caller.
    ///
    /// @param[in] name The name on the title bar of the window.
    /// @param[in] rect The location of the window in virtual screen space pixel coordinates.
    /// @param[in] fullscreen Enables a fullscreen mode.
    ///
    virtual std::unique_ptr<Window> CreateWindow(
        std::string_view name = "Bluemetal",
        Rect2D           rect = {{0, 0}, {1080, 720}},
        FullscreenMode   mode = FullscreenMode::Windowed) = 0;

    

    /// @brief Returns the required vulkan instance extensions for this platform.
    virtual std::vector<const char*> GetVulkanInstanceExtensions() = 0;
};

} // namespace bl