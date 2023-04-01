#pragma once

#include "Core/Precompiled.hpp"
#include "Core/CallbackList.hpp"
#include "Input/InputEvents.hpp"
#include "Generated/Export_Input.h"

#include <SDL2/SDL.h>

namespace bl
{

/** \brief An input system manages all input for the engine.
* 
* 
* 
*/
class BLOODLUST_INPUT_API InputSystem
{
    using _HookCallbackList = CallbackList<void(const SDL_Event*)>;
    using _ActionCallbackList = CallbackList<void()>;
    using _AxisCallbackList = CallbackList<void(float)>;
    using _HookHandle = _HookCallbackList::Handle;
    using _ActionHandle = _ActionCallbackList::Handle;
    using _AxisHandle = _AxisCallbackList::Handle;
    using _KeyWithScale = std::pair<Key, float>;

    struct _ActionBinding
    {
        std::vector<Key> triggerKeys;
        _ActionCallbackList callbacks;
    };

    struct _AxisBinding
    {
        std::vector<_KeyWithScale> triggerAxes;
        _AxisCallbackList callbacks;
    };

    _HookHandle _windowCbHandle;
    _HookCallbackList _hookCallbacks;
    std::unordered_map<std::string, _ActionBinding> _actions;
    std::unordered_map<std::string, _AxisBinding> _axes;

public:

    /** \brief An axis key with a directionality scale. */
    using KeyWithScale = _KeyWithScale;
 
    /** \brief Hook event callback, unfiltered SDL events from the poll. */
    using HookCallback = _HookCallbackList::Callback;

    /** \brief A callback function type for action events. */
    using ActionCallback = _ActionCallbackList::Callback;

    /** \brief A callback function type for axes events. */ 
    using AxisCallback = _AxisCallbackList::Callback;

    using HookHandle = _HookHandle;
    using ActionHandle = _ActionHandle;
    using AxisHandle = _AxisHandle;


    /** \brief Default constructor
    * 
    * Creates an empty input system ready to poll input events.
    * 
    * \since BloodLust 1.0.0
    * 
    */
    InputSystem() noexcept;
    
    /** \brief Default destructor
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
    bool registerAxis(const std::string& name, const std::vector<KeyWithScale>& onKeys) noexcept;

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
    auto addHookCallback(const HookCallback& callback) noexcept -> HookHandle;

    template<typename TObject>
    auto addHookCallback(TObject* pObject, void (TObject::* pCallback)(const SDL_Event*)) -> HookHandle
    {
        return addHookCallback(HookCallback(std::bind_front(pCallback, pObject)));
    }

    auto removeHookCallback(HookHandle handle) noexcept -> void;


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
    auto addActionCallback(const std::string& name, const ActionCallback& callback) noexcept -> std::optional<ActionHandle>;
    
    template<typename TObject>
    ActionHandle addActionCallback(const std::string& name, TObject* pObject, void (TObject::* pCallback)())
    {
        return addActionCallback(name, ActionCallback(std::bind_front(pCallback, pObject)));
    }

    auto removeActionCallback(ActionHandle handle) noexcept -> void;

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
    auto addAxisCallback(const std::string& name, const AxisCallback& callback) noexcept -> std::optional<AxisHandle>;

    template<typename TObject>
    auto addAxisCallback(const std::string& name, TObject* pObject, void (TObject::* pCallback)(float)) -> AxisHandle
    {
        return addAxisCallback(name, AxisCallback(std::bind_front(pCallback, pObject)));
    }

    auto removeAxisCallback(AxisHandle handle) noexcept -> void;

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
    void windowEventHook(const SDL_Event*);
    

};

} // namespace bl