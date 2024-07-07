#pragma once

#include "Math/Math.h"

namespace bl {

class Window;

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
    Raw, ///<! Hides the cursor and an Uses raw mouse input directly from the device, no accleration is applied.
};

class Mouse {
public:
    virtual ~Mouse() = default;

    /// @brief Changes the input mode of the mouse on a window basis.
    ///
    /// By default the mouse uses MouseInputMode::Default taking what the 
    /// system gives us. For gaming applications users may want to turn on
    /// raw mouse input mode to get the most accurate data without 
    /// accelration applied at the system level.
    ///
    /// @param[in] window Window to change mouse mode on.
    /// @param[in] mode The input mode to set the mouse to.
    ///
    void SetInputMode(Window* window, MouseInputMode mode = MouseInputMode::Normal);

    /// @brief Returns true if the selected button was down at poll. 
    ///
    /// @param[in] button The mouse button to poll input for.
    ///
    /// @returns True if the button was down.
    virtual bool IsButtonDown(MouseButton button) = 0;

    /// @brief Returns the position of the mouse relative to the window.
    ///
    /// Use this function for most mouse usage since window relative mouse
    /// positioning makes sense for in window applications.
    ///
    /// @returns The mouse position within the window.
    virtual glm::vec2 GetMousePosition() = 0;

    /// @brief Returns the position of the mouse in global system coordinates.
    ///
    /// Typically you'll want to use \ref GetMousePosition but in some 
    /// circumstances if you need global position it can be found here.
    ///
    /// @returns The mouse position from the system.
    virtual glm::vec2 GetMousePositionGlobal() = 0;

    /// @brief Returns the delta between the previous poll and the latest one.
    ///
    /// 
    ///
    virtual glm::vec2 GetMouseDelta() = 0;

    /// @brief Returns the delta of scroll movement.
    ///
    /// Many platforms provide two dimensional scrolling especially laptops 
    /// and computers with trackpads or trackballs. Whereas the typical mouse
    /// only proves vertical scrolling.
    ///
    /// @returns The delta of the mouse scroll.
    virtual glm::vec2 GetMouseScrollDelta() = 0;
};

} // namespace bl