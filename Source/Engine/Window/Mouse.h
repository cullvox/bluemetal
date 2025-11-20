#pragma once

#include "Math/Math.h"
#include "Window.h"

namespace bl {

enum class MouseButton {
    Left = 0, ///!< Left mouse button.
    Right, ///!< Right mouse button.
    Middle, ///!< Middle scroll wheel mouse button.
    SideFront, ///!< Front side button.
    SideBack, ///!< Back side button.
};

enum class MouseInputMode {
    Normal, ///<! Uses whatever mouse data the system gives us.
    Hidden, ///<! Hides the cursor over our applications windows.
    HiddenLocked, ///<! Hidden + locks the cursor to the center of the window.
    Raw, ///<! Hides the cursor and and uses raw mouse input directly from the device, no acceleration is applied. This input mode acts a lot like HiddenLocked.
};

class Mouse {
    SDL_MouseButtonFlags _buttonFlags { 0 };
    glm::vec2 _lastLocation {};
    glm::vec2 _relative {};
    glm::vec2 _location {};
    glm::vec2 _wheel {};

    MouseButton MouseButtonFromSDL(int buttonFlag);
    int MouseButtonToSDL(MouseButton button);

protected:
    friend class InputSystem;

    void SetRelativeMotion(glm::vec2 motion);
    void SetLocation(glm::vec2 location);
    void SetMouseButtonFlag(int flags, bool value);
    void SetWheel(glm::vec2 wheel);

public:
    Mouse() = default;
    ~Mouse() = default;

    void Poll();

    /// @brief Changes the input mode of the mouse on a window basis.
    ///
    /// By default the mouse uses MouseInputMode::Default taking what the
    /// system gives us. For gaming applications users may want to turn on
    /// raw mouse input mode to get the most accurate data without
    /// acceleration applied at the system level.
    ///
    /// @param[in] window Window to change mouse mode on.
    /// @param[in] mode The input mode to set the mouse to.
    ///
    void SetCaptured(Window* window, bool captured);

    bool GetCaptured(Window* window);

    /// @brief Returns true if the selected button was down at poll.
    ///
    /// @param[in] button The mouse button to poll input for.
    ///
    /// @returns True if the button was down.
    bool IsButtonDown(MouseButton button);

    /// @brief Returns the position of the mouse relative to the window.
    ///
    /// Use this function for most mouse usage since window relative mouse
    /// positioning makes sense for in window applications.
    ///
    /// @returns The mouse position within the window.
    glm::vec2 GetMousePosition();

    /// @brief Returns the delta between the previous poll and the latest one.
    ///
    ///
    ///
    glm::vec2 GetMouseDelta();

    /// @brief Returns the delta of scroll movement.
    ///
    /// Many platforms provide two dimensional scrolling especially laptops
    /// and computers with track-pads or track-balls. Whereas the typical mouse
    /// only proves vertical scrolling.
    ///
    /// @returns The delta of the mouse scroll.
    glm::vec2 GetMouseScrollDelta();
};

} // namespace bl