#include <iostream>
#include <optional>

#define SDL_MAIN_HANDLED
#include "Core/Log.hpp"
#include "Window/Display.hpp"
#include "Config/Config.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Swapchain.hpp"
#include "Render/Renderer.hpp"
#include "Render/FrameCounter.hpp"
#include "Input/InputSystem.hpp"

#include <spdlog/spdlog.h>

class CharacterController
{

    bl::InputController inputController{};

public:

    CharacterController()
    {
        inputController.bindAction<CharacterController>("Fire", bl::InputEvent::Pressed, *this, &CharacterController::Fire);
        inputController.bindAxis<CharacterController>("Walk", *this, &CharacterController::Walk);
        inputController.bindAxis<CharacterController>("Strafe", *this, &CharacterController::Strafe);
    }

    ~CharacterController()
    {
    }

    CharacterController(const CharacterController&) = delete;

    void Fire()
    {
        spdlog::info("FIRE!");
    }

    void Walk(float axis)
    {
        spdlog::info("Walking at: {}", axis);
    }

    void Strafe(float axis)
    {
        spdlog::info("Strafing at: {}", axis);
    }

    bl::InputController& getInputController() { return inputController; };

};

bool onClose = false;
void closeWindow()
{
    onClose = true;
}

int main(int argc, const char** argv)
{
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        bl::Logger::Log("Could not init SDL: {}", SDL_GetError());
    }

    std::vector<bl::Display> displays = bl::Display::getDisplays();
    bl::Window window{displays[0].getDesktopMode()};
    bl::RenderDevice renderDevice{window};
    bl::Swapchain swapchain{window, renderDevice};
    bl::Renderer renderer{renderDevice, swapchain};
    bl::FrameCounter frameCounter{};
    bl::InputSystem inputSystem{};
    bl::InputController windowInput{};
    CharacterController characterController{};
    
    windowInput.bindAction("Exit", bl::InputEvent::Pressed, closeWindow);

    inputSystem.registerAction("Exit", {bl::Key::WindowClose});
    inputSystem.registerAction("Fire", {bl::Key::MouseButtonLeft});
    inputSystem.registerAxis("Walk", {{bl::Key::KeyW, 1.0f}, {bl::Key::KeyS, -1.0f}});
    inputSystem.registerAxis("Strafe", {{bl::Key::KeyD, 1.0f}, {bl::Key::KeyA, -1.0f}});
    
    inputSystem.registerController(characterController.getInputController());
    inputSystem.registerController(windowInput);

    while (!onClose) // window.shouldNotClose()
    {
        frameCounter.beginFrame();
        inputSystem.poll();
        renderer.beginFrame();


        
        renderer.endFrame();
        if (frameCounter.endFrame())
        {
            spdlog::info("Rendering at: {} f/s and {} ms/f\n\tavg f/s: {} and avg ms/f: {}", frameCounter.getFramesPerSecond(), frameCounter.getMillisecondsPerFrame(), frameCounter.getAverageFramesPerSecond(10), frameCounter.getAverageMillisecondsPerFrame(10));
        }
    }

    SDL_Quit();
    return 0;
}