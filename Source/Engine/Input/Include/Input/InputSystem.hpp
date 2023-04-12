#pragma once

#include "Core/Precompiled.hpp"
#include "Math/Vector2.hpp"
#include "Math/Vector3.hpp"
#include "Input/InputEvents.hpp"
#include "Input/Export.h"

#include <SDL2/SDL.h>

#define BL_INPUT_SYSTEM_MAX_FILTER_EVENTS 16

struct blGenericInputEventFilter
{
    blEventTypeFlags                        types;
    std::array<blGenericEvent, 
        BL_INPUT_SYSTEM_MAX_FILTER_EVENTS>  events;
};

class blInputObserver
{
public:
    virtual void onEvent(blEventTypeFlagBits type, blGenericEvent event,  
        blEventValueType valueType,  blEventValue value) = 0;
};

class BLOODLUST_INPUT_API blInputSystem
{
public:
    static std::shared_ptr<blInputSystem> getInstance() noexcept;

    // Updates the input values for the next get functions.
    void poll() noexcept;

    blButton getKey(blKey key) const noexcept;
    blButton getMouseButton(blMouseButtonEvent button) const noexcept;
    blVector2f getMouse(blMouseEvent mouse) const noexcept;


    // Attaches an observer to get a direct channel to input values.
    void attach(const blInputObserver* pObserver, 
        const blGenericInputEventFilter& filter);
    void remove(const blInputObserver* pObserver);

private:
    static std::shared_ptr<blInputSystem> _instance;

    blInputSystem();
    ~blInputSystem();

    void processKeyboard() const noexcept;
    void processGamepad() const noexcept;

    std::vector<blInputObserver*> _observers;
    
};