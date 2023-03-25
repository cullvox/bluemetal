#pragma once

#include "Core/Export.h"
#include "Core/Precompiled.hpp"
#include "Core/Delegate.hpp"
#include "Input/InputEvents.hpp"

#include <SDL2/SDL.h>

namespace bl
{

/** \brief An input system manages all input for the engine.
* 
* 
* 
*/
class BLOODLUST_API InputSystem
{

public:
 
    /** \brief Delegate of the action function. */
    using ActionDelegate = Delegate<void(void)>;

    Delegate()

    /** \brief Delegate of the axis function. */
    using AxisDelegate = Delegate<void(float)>;

    /** @brief Hook delegate get all SDL Events directly from the poll. */
    using HookDelegate = Delegate<void(const SDL_Event*)>;

    /** @brief An axis is just a key with a directionality scale between 1 to 0. */
    using Axis = std::pair<Key, float>; // (key, scale)

    /** @brief Default constructor
    * 
    * Creates an empty input system ready to poll input events.
    * 
    * \since BloodLust 1.0.0
    * 
    */
    InputSystem() noexcept;
    
    /** @brief Default destructor
    * 
    * Collapses the input system.
    * 
    * \since BloodLust 1.0.0
    * 
    */
    ~InputSystem() noexcept;

    /** \brief Move Operator
    *
    * Does a complete move of the data from rhs to this. Before any data is moved
    * this->collapse() is called removing any data within this. All values of rhs
    * are then also collapsed after the move is completed.
    * 
    * \param rhs InputSystem to move from.
    * 
    * \return *this
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa InputSystem()
    * 
    */
    InputSystem& operator=(InputSystem&& rhs) noexcept;

    /** \brief Registers a bindable action with the input system.
    * 
    * This function registers the name of an action and the keys that currently 
    * are bound to it. The keys can be rebound at any point by calling the 
    * function again with the same name. When \ref poll() is called each of 
    * these binds are checked for an input if the input is an action type.
    * 
    * \param name The name of the binding.
    * \param onKeys Any bound function will only be activated when these key(s) are pressed.
    * 
    * \return true on successful binding.
    * \return false on failure to bind.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa delegateAction()
    * 
    */
    bool registerAction(const std::string& name, const std::vector<Key>& onKeys) noexcept;
    
    /** \brief Registers a bindable axis with the input system.
    * 
    * This bind an axis key to a name and ensures that they are checked on poll input.
    * Upon any axis key is activated the callback all callbacks will be ran. 
    *
    * You can bind an axis using \ref delegateAxis() the callback will be ran when activated.
    * 
    * \param name The name of this binding.
    * \param onAxes The axes required to activate this binding and send out callbacks.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa delegateAxis()
    * 
    */
    bool registerAxis(const std::string& name, const std::vector<Axis>& onAxes) noexcept;
    
    /** \brief Retrieves the hook delegate to add SDL2 hooked events
    *
    * Some parts of the engine require almost all of SDL2s specific events so this hook was
    * created allowing full info into all events. ImGui when using SDL2 specifically triggered
    * this addendum. Hooks are called before all other inputs are checked.
    * 
    * Hooks can be useful when dealing with other libraries that use SDL2.
    * 
    * \param hook The function called when an event is polled.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa getActionDelegate()
    * \sa getAxisDelegate()
    * 
    */
    HookDelegate& getHookDelegate(void) noexcept;

    /** \brief Gets a delegate so users can add callbacks to the action.
    *
    * Returns a delegate so users can freely add and remove callbacks when
    * a specific action is activated.
    * 
    * \param name The name of the binding delegate to get.
    *
    * \since BloodLust 1.0.0
    * 
    * \sa registerAction()
    * 
    * \return The delegate created with the binding for the user to add callbacks to.
    * 
    */
    ActionDelegate* getActionDelegate(const std::string& name) noexcept;
    
    /** \brief Delegates an axis function to a specific binding.
    * 
    * Provides a delegate that callbacks can be added to when this specific axis is triggered.
    * The axis must be registered before this function is called with \ref registerAxis.
    * If the axis is not registered with the system it will return nullptr.
    * 
    * \param name The name of the registered axis.
    * 
    * \return Pointer to a valid delegate upon success.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa registerAxis()
    * 
    */
    AxisDelegate* getAxisDelegate(const std::string& name) noexcept;

    /** \brief Polls the event queue for all events and runs callbacks.
    *
    * Pools events from SDL2 and receives all events about keyboard, window and more.
    * When an event is received it will be checked with the current actions and axes registered
    * running callbacks when one is found to match.
    * 
    * \since BloodLust 1.0.0
    * 
    * \sa registerAction()
    * \sa registerAxis()
    * \sa delegateAction()
    * \sa delegateAxis()
    * 
    */
    void poll() noexcept;

private:
    
    /** \brief Destroys and cleans the entire input system. */
    void collapse() noexcept;

    /** \brief Processes the pumped keyboard events from SDL_PumpEvents(). */
    void processKeyboardPump() const noexcept;

    /** \brief An event hook for window specific events and running their callbacks. */
    void windowEventHook(const SDL_Event*) const noexcept;
    
    struct ActionBinding
    {
        std::vector<Key> onKeys;
        ActionDelegate delegate;
    };

    struct AxisBinding
    {
        std::vector<Axis> onAxes;
        AxisDelegate delegate;
    };

    HookDelegate m_hookDelegate;
    std::unordered_map<std::string, ActionBinding> m_actions;
    std::unordered_map<std::string, AxisBinding> m_axes;
};

} // namespace bl