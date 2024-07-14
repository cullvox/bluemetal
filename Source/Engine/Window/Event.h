#pragma once

#include "Precompiled.h"
#include "Core/Flags.h"

namespace bl {

class Keyboard;
class Mouse;
class Gamepad;

/// @brief Input object for interfacing with all input sources.
class Input {
public:

    /// @brief Destructor
    virtual ~Input() = default;

    /// @brief Polls the system for events.
    ///
    /// Updates the internal state for all input objects. This includes but is
    /// not limited to, Keyboard, Mouse, and Gamepads. Later as more features 
    /// are added it will include more advanced input like VR.
    /// 
    virtual void Poll() = 0;

    /// @brief Gets the system keyboard input object.
    ///
    /// Since systems can only technically have one keyboard in most 
    /// circumstances there is only one keyboard object. Use the returned 
    /// keyboard object to update input bindings and other actions that should
    /// take place because of keyboard input.
    ///
    /// @returns The keyboard object.
    ///
    virtual Keyboard& GetKeyboard() = 0;
    
    /// @brief Gets the mouse input object.
    ///
    /// We only support one mouse and one keyboard so we return a singular 
    /// mouse object. Use the returned mouse input object to gather 
    /// information about the state of the cursor and the mouse's raw data.
    ///
    /// @returns The mouse object.
    ///
    virtual Mouse& GetMouse() = 0;

    
    /// @brief Gets all connected gamepad objects.
    ///
    /// Returns an array of the maximum amount of gamepads supported by the 
    /// window platform. The gamepads are thoughtfully 
    ///
    /// @return 
    virtual const std::vector<Gamepad*>& GetGamepads() = 0;

    /// @brief 
    /// @return 
    virtual std::vector<Gamepad*> GetConnectedGamepads() = 0;
};

} // namespace bl