#pragma once

#include "Math/Math.h"
#include "Precompiled.h"

namespace bl {

class Gamepad {
public:

    /// @brief Any action buton on the gamepad.
    ///
    /// A button on the gamepad that isn't variable input.
    /// These gamepad buttons are mapped by typical Xbox controller controls.
    /// Other gamepads for consoles like the Playstation use similar enough 
    /// controls that they can easily be mapped onto this.
    ///
    /// Steam input is much better for button mapping anymore, we shouldn't 
    /// have to worry much about action mapping for controller schemes, 
    /// it wouldn't be a bad idea to have some basic mappings for ease of use.
    /// Other platforms may be a problem for this however. 
    ///
    enum class Button : uint32_t {
        A,
        B,
        X,
        Y,
        PadUp,
        PadDown,
        PadLeft,
        PadRight,
        LeftShoulder,
        RightShoulder, 
        LeftStick, ///!< Under each stick many controllers support button inputs.
        RightStick, ///!< Under each stick many controllers support button inputs.
        LeftGrip, /// !< Some controllers support a grip button where the lower fingers hold the gamepad.
        RightGrip, /// !< Some controllers support a grip button where the lower fingers hold the gamepad.
    };

    /// @brief Any two dimensional axis analog controller input.
    ///
    /// Joysticks are typically controlled by the thumbs on most modern 
    /// controllers.
    ///
    enum class Joystick : uint32_t {
        LeftJoystick,
        RightJoystick,
    };

    /// @brief Any single axis analog controller input.
    ///
    /// Typically a trigger is on the back of the controller pressed by
    /// the index finger or middle fingers. 
    ///
    enum class Trigger : uint32_t {
        LeftTrigger,
        RightTrigger,
    };

    /// @brief Most gamepads are modeled after the Xbox controller.
    ///
    /// Some players may have different types of gamepads being used. We can
    /// differentiate what kinds of controls to show the players on a per 
    /// gamepad basis rather than a per console basis. This is particularly 
    /// useful for sit down/live streamed gaming sessions with friends who all
    /// may use different types of gamepads. 
    ///
    enum class Type {
        Generic, ///!< Generic control display 
        Xbox, ///!< Show Xbox style controls 
        Playstation, ///!< Show Playstation shape controls
        Steam, ///!< Show Steam controls
    };

    struct Info {
        std::string vendor;
        std::string name;
        Type type;
    };

public:
    /// @brief Destructor
    virtual ~Gamepad() = default;

    /// @brief 
    /// @return 
    virtual bool IsConnected() = 0;

    /// @brief Returns the state of a button
    virtual bool GetButtonDown(Button button) = 0;

    /// @brief Returns an analog joystick input from a gamepad.
    ///
    /// Return a normalized joystick value from -1.0 to 1.0 in both directions.
    /// If the joystick is near the center it will not be considered down and 
    /// will return a zero vector value respresenting no input.
    ///
    /// @param[in] analog The joystick input to read from.
    ///
    /// @returns The analog value of the joystick's positioning.
    ///
    virtual glm::vec2 GetJoystickValue(Joystick stick) = 0;

    /// @brief Returns an analog value of a gampad trigger.
    ///
    /// Returns a normalized floating value from 0.0 to 1.0 with 0.0 being not 
    /// triggered and a full one being pressed all the way down. If the trigger 
    /// is near zero this function will return a zero value.
    ///
    /// @param trigger The trigger to read from.
    ///
    /// @returns The analog value from the trigger.
    ///
    virtual float GetTriggerValue(Trigger trigger) = 0;

    /// @brief Checks for rumble support.
    ///
    /// Not all gamepad vendors have rumble motors built into their devices.
    /// Here is a simple way to determine if it's supported. Calling this 
    /// function isn't a requirement to call @ref Gamepad::Rumble() because
    /// if a controller doesn't have rumble support calling 
    /// @ref Gamepad::Rumble() will do no operation. 
    ///
    /// @return True if the gamepad has rumble support.
    ///
    virtual bool HasRumble() = 0; 

    /// @brief Actives a rumble effect on the motors of a gamepad.
    ///
    /// As stated in the description of @ref Gamepad::HasRumble, not all 
    /// gamepads have rumble support. If this gamepad does not support rumble
    /// or the platform doesn't support it this function will do no operation.
    ///
    /// @param[in] leftRumble The amount of rumble on the left handed motor. Must be a normalized value from 0.0 to 1.0, will be clamped for safety.
    /// @param[in] rightRumble The amount of rumble on the right handed motor. Must be a normalized value from 0.0 to 1.0, will be clamped for safety.
    /// @param[in] ms The amount of time to rumble the motors for.
    ///
    virtual void Rumble(float leftRumble, float rightRumble, uint64_t ms);
};


} // namespace bl