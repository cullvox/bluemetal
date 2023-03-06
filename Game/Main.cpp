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
        inputController.bindAxis<&CharacterController::Walk>("Walk", this);
        inputController.bindAxis<&CharacterController::Strafe>("Strafe", this);
    }

    ~CharacterController()
    {
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
    bl::Display display{};
    
    bl::Window window{bl::Display::getScreens()[0].getDesktopMode(), std::nullopt};
    bl::RenderDevice renderDevice{window};
    bl::Swapchain swapchain{window, renderDevice};
    bl::Renderer renderer{renderDevice, swapchain};
    bl::FrameCounter frameCounter{};
    bl::InputSystem inputSystem{window};
    bl::InputController windowInput{};
    CharacterController characterController{};
    bool closeWindow = false;
    auto closeWindowFunctor = [](const void* pData){
        bool* pClose = (bool*)pData;
        *pClose = true;
    };

    windowInput.bindAction("Exit", bl::InputEvent::Pressed, closeWindowFunctor, &closeWindow);

    inputSystem.registerAction("Exit", {bl::Key::WindowClose});
    inputSystem.registerAxis("Walk", {{bl::Key::W, 1.0f}, {bl::Key::S, -1.0f}});
    inputSystem.registerAxis("Strafe", {{bl::Key::D, 1.0f}, {bl::Key::A, -1.0f}});

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
}