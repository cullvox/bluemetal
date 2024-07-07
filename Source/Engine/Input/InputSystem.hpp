#pragma once

#include "Core/CallbackList.hpp"
#include "Core/Precompiled.hpp"
#include "Input/Export.h"
#include "Input/InputEvents.hpp"


class InputAction {
    using Func = std::function<void(Modifier modifier)>

public:
    InputAction(Modifier mod, Key )

private:
    MulticastDelegate<void(Modifier mod, )>
};

class InputSystemComponent {
public:

    InputSystemComponent();
    ~InputSystemComponent();

    // Updates the input values for the next get functions.
    void Update() noexcept;


private:
    void processKeyboard() const noexcept;
    void processGamepad() const noexcept;

    bool _shouldClose = false;
    blCallbackList<void(const SDL_Event*)> _hookCallbacks;
};