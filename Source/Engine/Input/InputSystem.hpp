#pragma once

#include "Core/CallbackList.hpp"
#include "Core/Precompiled.hpp"
#include "Input/Export.h"
#include "Input/InputEvents.hpp"
#include "Math/Vector2.hpp"
#include "Math/Vector3.hpp"

#include <SDL.h>

#define BL_INPUT_SYSTEM_MAX_FILTER_EVENTS 16

struct blGenericInputEventFilter {
    blEventTypeFlags types;
    std::array<blGenericEvent, BL_INPUT_SYSTEM_MAX_FILTER_EVENTS> events;
};

using blInputHookCallbackFunction = blCallbackList<void(const SDL_Event*)>::Callback;
using blInputHookCallback = blCallbackList<void(const SDL_Event*)>::Handle;

class BLUEMETAL_INPUT_API blInputSystem {
public:
    static std::shared_ptr<blInputSystem> getInstance() noexcept;

    blInputSystem();
    ~blInputSystem();

    // Updates the input values for the next get functions.
    void poll() noexcept;

    blButton getKey(blKey key) const noexcept;
    blButton getMouseButton(blMouseButtonEvent button) const noexcept;
    blVector2f getMouse(blMouseEvent mouse) const noexcept;
    bool shouldClose() const noexcept;

    blInputHookCallback addHook(const blInputHookCallbackFunction& function);
    void removeHook(blInputHookCallback handle);

private:
    void processKeyboard() const noexcept;
    void processGamepad() const noexcept;

    bool _shouldClose = false;
    blCallbackList<void(const SDL_Event*)> _hookCallbacks;

    friend class std::shared_ptr<blInputSystem>;
};