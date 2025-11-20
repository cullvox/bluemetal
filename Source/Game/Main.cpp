#include "Core/FrameCounter.h"
#include "Core/Print.h"
#include "Core/Time.h"
#include "Engine/Engine.h"
#include "Graphics/UniformData.h"
#include "Graphics/Vertex.h"
#include "Graphics/VulkanConversions.h"
#include "Graphics/VulkanPhysicalDevice.h"
#include "Graphics/VulkanPipeline.h"
#include "Graphics/VulkanShader.h"
#include "Graphics/VulkanWindow.h"
#include "Math/Transform.h"
#include "Resources/Material.h"
#include "Resources/Model.h"
#include "Resources/Shader.h"
#include "Resources/Texture2D.h"
#include "Scene/AudioSource3D.h"
#include "Window/Keyboard.h"
#include "Window/Mouse.h"

static inline float randomValue()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

int main(int argc, const char** argv)
{
    (void)argc;
    (void)argv;

    try {
        auto engine = bl::Engine {};
        engine.Initialize();
        auto resourceMgr = engine.GetResourceManager();

        auto audio = engine.GetAudio();
        auto sound = resourceMgr->Load<bl::Sound>("Resources/Audio/Music/Taswell.flac");

        auto source = std::make_unique<bl::AudioSource3D>(&engine);

        source->Play(sound, true);

        auto graphics = engine.GetGraphics();
        auto imgui = engine.GetImGui();

        auto input = engine.GetInput();
        auto& keyboard = input->GetKeyboard();
        auto& mouse = input->GetMouse();

        auto vert = resourceMgr->Load<bl::Shader>("Resources/Shaders/Default.vert.spv");
        auto frag = resourceMgr->Load<bl::Shader>("Resources/Shaders/Default.frag.spv");
        auto model = resourceMgr->Load<bl::Model>("Resources/Models/low_poly_fox.glb");

        auto renderer = engine.GetRenderer();

        auto window = engine.GetWindow();
        auto vulkanWindow = dynamic_cast<bl::VulkanWindow*>(window);

        auto presentModes = graphics->GetPhysicalDevice()->GetPresentModes(vulkanWindow);

        bl::FrameCounter frameCounter;

        glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::identity<glm::mat4>();
        float yaw = -90.0f, pitch = 0.0f;
        float walkingSpeed = 9.0f;
        glm::vec3 direction;

        auto extent = window->GetExtent();
        auto extenti = glm::ivec2 { (int)extent.width, (int)extent.height };
        auto extentf = glm::vec2 { (float)extent.width, (float)extent.height };

        while (!window->GetCloseRequested()) {
            frameCounter.BeginFrame();

            input->Poll([imgui, &mouse, window](SDL_Event& event) {
                imgui->Process(event);
            });

            if (keyboard.GetKeyDown(bl::Scancode::W))
                cameraPos += walkingSpeed * cameraFront * frameCounter.GetDeltaTime();
            if (keyboard.GetKeyDown(bl::Scancode::S))
                cameraPos -= walkingSpeed * cameraFront * frameCounter.GetDeltaTime();
            if (keyboard.GetKeyDown(bl::Scancode::A))
                cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * walkingSpeed * frameCounter.GetDeltaTime();
            if (keyboard.GetKeyDown(bl::Scancode::D))
                cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * walkingSpeed * frameCounter.GetDeltaTime();
            if (keyboard.GetKeyDown(bl::Scancode::Space))
                cameraPos += walkingSpeed * cameraUp * frameCounter.GetDeltaTime();
            if (keyboard.GetKeyDown(bl::Scancode::LeftShift))
                cameraPos -= walkingSpeed * cameraUp * frameCounter.GetDeltaTime();
            if (keyboard.GetKeyDown(bl::Scancode::Escape)) {
                mouse.SetCaptured(window, false);
                ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            }

            auto mousePos = mouse.GetMousePosition();
            auto mouseDelta = mouse.GetMouseDelta();

            bl::Print::Debug("Mouse down: {}", mouse.IsButtonDown(bl::MouseButton::Left));

            if (mouse.IsButtonDown(bl::MouseButton::Left) && window->GetFocused() && !ImGui::GetIO().WantCaptureMouse) {
                mouse.SetCaptured(window, true);
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
            }

            if (mouse.GetCaptured(window)) {
                float sensitivity = 0.1f;
                mouseDelta *= sensitivity;

                yaw += mouseDelta.x;
                pitch -= mouseDelta.y;

                if (pitch > 89.0f)
                    pitch = 89.0f;
                if (pitch < -89.0f)
                    pitch = -89.0f;

                direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                direction.y = sin(glm::radians(pitch));
                direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront = glm::normalize(direction);

                bl::Print::Info("Camera direction: {}, {}, {}", direction.x, direction.y, direction.z);
            }

            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            bl::Print::Info("Mouse: {}, {}", mousePos.x, mousePos.y);
            bl::Print::Info("Camera Pos: {}, {}, {}", cameraPos.x, cameraPos.y, cameraPos.z);

            extent = window->GetExtent();
            extentf = glm::vec2 { (float)extent.width, (float)extent.height };
            glm::mat4 projection = glm::perspective(glm::radians(70.0f), extentf.x / extentf.y, 0.1f, 1000.0f);
            projection[1][1] *= -1; // Invert the projection for Vulkan y (0, 1)

            renderer->SetView(view);
            renderer->SetProjection(projection);

            glm::vec3 position { sinf(bl::Time::Current() / 1000.f) * 10.f, 0.0f, 10.0f };
            glm::vec3 velocity { cosf(bl::Time::Current() / 1000.f) * 1 / 100.f, 0.0f, 0.0f };

            glm::vec4 color = { 1.f, 0.5f, 0.f, 1.0f };
            glm::vec4 val {};

            std::memcpy(&val, &color, sizeof(glm::vec4));

            // glm::vec4 color = { 1.0f, 0.0f, 0.0f, 1.0f};
            // material->SetVector4("material.color", color);

            audio->Update();

            renderer->Render([&](bl::VulkanRenderData& rd) {
                auto extent = window->GetExtent();

                VkViewport viewport;
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = (float)extent.width;
                viewport.height = (float)extent.height;
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                vkCmdSetViewport(rd.cmd, 0, 1, &viewport);

                VkRect2D scissor;
                scissor.offset = { 0, 0 };
                scissor.extent = { extent.width, extent.height };
                vkCmdSetScissor(rd.cmd, 0, 1, &scissor);

                model->GetTree()->Draw(rd);

                // dragonModel.Get()->Draw(rd, material->GetMaterial());

                imgui->BeginFrame();

                imgui->DrawDebug();

                imgui->EndFrame(rd.cmd);
            });

            frameCounter.EndFrame();
        }

        graphics->GetDevice()->WaitForDevice();

        resourceMgr->UnloadAll();

    } catch (std::exception& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception Error", e.what(), nullptr);
        return EXIT_FAILURE;
    }

    return 0;
}
