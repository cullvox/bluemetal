#include <iostream>
#include <optional>

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
        inputController.bindAction<&CharacterController::Fire>("Fire", bl::InputEvent::Pressed, this);
        inputController.bindAxis<&CharacterController::Walk>("Walk", this);
        inputController.bindAxis<&CharacterController::Strafe>("Strafe", this);
    }

    ~CharacterController()
    {
    }

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

int main(int argc, const char** argv)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);

    std::vector<bl::Display> displays = bl::Display::getDisplays();
    bl::Window window{displays[0].getDesktopMode()};
    bl::RenderDevice renderDevice{window};
    bl::Swapchain swapchain{window, renderDevice};
    bl::Renderer renderer{renderDevice, swapchain};
    bl::FrameCounter frameCounter{};
    bl::InputSystem inputSystem{};
    bl::InputController windowInput{};
    CharacterController characterController{};
    bool closeWindow = false;
    auto closeWindowFunctor = [](const void* pData){
        bool* pClose = (bool*)pData;
        *pClose = true;
    };

    windowInput.bindAction("Exit", bl::InputEvent::Pressed, closeWindowFunctor, &closeWindow);

    inputSystem.registerAction("Exit", {bl::Key::WindowClose});
    inputSystem.registerAction("Fire", {bl::Key::MouseButtonLeft});
    inputSystem.registerAxis("Walk", {{bl::Key::KeyW, 1.0f}, {bl::Key::KeyS, -1.0f}});
    inputSystem.registerAxis("Strafe", {{bl::Key::KeyD, 1.0f}, {bl::Key::KeyA, -1.0f}});
    
    inputSystem.registerController(characterController.getInputController());
    inputSystem.registerController(windowInput);

    while (!closeWindow) // window.shouldNotClose()
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
}