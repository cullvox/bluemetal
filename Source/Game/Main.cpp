#include "Core/FrameCounter.h"
#include "Core/Print.h"
#include "Core/Time.h"
#include "Engine/Engine.h"
#include "Graphics/UniformData.h"
#include "Graphics/Vertex.h"
#include "Resources/Material.h"
#include "Resources/Model.h"
#include "Resources/Shader.h"
#include "Resources/Texture2D.h"
#include "Scene/AudioSource3D.h"
#include "Scene/PhysicsBody3D.h"
#include "Scene/MeshInstance3D.h"
#include "Window/Keyboard.h"
#include "Window/Mouse.h"
#include "Physics/ObjectLayers.h"

#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

static inline float randomValue()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

int main(int argc, const char** argv)
{
    (void)argc;
    (void)argv;

    try {
        bl::Engine engine{ argc, argv };
        engine.Initialize();

        auto resourceMgr = engine.GetResourceManager();
        auto audio = engine.GetAudio();
        auto graphics = engine.GetGraphics();
        auto imgui = engine.GetImGui();
        auto input = engine.GetInput();
        auto& keyboard = input->GetKeyboard();
        auto& mouse = input->GetMouse();
        auto& physics = engine.GetPhysics();
        auto renderer = engine.GetRenderer();
        auto window = engine.GetWindow();

        auto sound = resourceMgr->Load<bl::Sound>("Resources/Audio/Music/Taswell.flac");

        auto source = std::make_unique<bl::AudioSource3D>(engine);

        source->Play(sound, true);

        auto model = resourceMgr->Load<bl::Model>("Resources/Models/low_poly_fox.glb");

        auto cube = resourceMgr->Load<bl::Model>("Resources/Models/cube.glb");

        auto rootNode = std::make_unique<bl::Node3D>(engine);

        auto characterNode = model.lock()->GetTree()->Clone();
        characterNode->SetPosition({0.0f, -0.6f, 0.0f});

        JPH::Ref<JPH::CapsuleShape> shape = new JPH::CapsuleShape(0.8f, 0.3f);
        auto physicsBody = std::make_unique<bl::PhysicsBody3D>(engine);
        physicsBody->SetName("Character");
        physicsBody->SetPosition({ 0.0f, 0.0f, -5.0f });
        physicsBody->SetShape(shape.GetPtr());
        physicsBody->ResetBody();
        physicsBody->AddChild(characterNode);
        rootNode->AddChild(std::move(physicsBody));



        auto floorMaterial = resourceMgr->Load<bl::Material>("Resources/Materials/Default.mat");
        auto floorTexture = resourceMgr->Load<bl::Texture2D>("Resources/Textures/floor.jpg");
        auto defaultSampler = resourceMgr->Load<bl::Sampler>("Resources/Samplers/Default.json");

        floorMaterial.lock()->SetSampledTexture2D("inAlbedo", defaultSampler, floorTexture);
        floorMaterial.lock()->SetBool("material.useTriplanar", true);

        auto floorNode = cube.lock()->GetTree()->Clone();
        floorNode->SetName("Floor");
        floorNode->SetScale({ 100.0f, 1.0f, 100.0f });

        floorNode->GetChild("Cube")->As<bl::MeshInstance3D>()->SetMaterial(floorMaterial);

        JPH::Ref<JPH::Shape> floorShape = new JPH::BoxShape({50.0f, 0.5f, 50.0f});
        auto floorStaticBody = std::make_unique<bl::PhysicsBody3D>(engine);
        floorStaticBody->SetName("FloorBody");
        floorStaticBody->SetMotionType(JPH::EMotionType::Static);
        floorStaticBody->SetObjectLayer(bl::ObjectLayers::STATIC);
        floorStaticBody->SetShape(floorShape);
        floorStaticBody->SetPosition({0.0f, -5.0f, 0.0f});
        floorStaticBody->ResetBody();
        floorStaticBody->SetDOF(true, true, true, false, true, false); // Lock rotation around Z axis

        floorStaticBody->AddChild(std::move(floorNode));

        rootNode->AddChild(std::move(floorStaticBody));

        bl::FrameCounter frameCounter;
        float cameraAcceleration = 0.5f;
        float maxCameraSpeed = 8.f;
        bool enableCameraSmoothing = true;
        bool enableCameraMovementDamping = true;
        float cameraSmoothnessDampLambda = 5.0f;
        float cameraMovementDampLambda = 5.0f;
        glm::vec3 cameraVelocity = { 0.0f, 0.0f, 0.0f };
        glm::vec3 cameraPos = { 0.0f, 0.0f, 5.0f };
        glm::vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
        glm::vec3 cameraUp = { 0.0f, 1.0f, 0.0f };
        glm::mat4 view = glm::identity<glm::mat4>();
        float yaw = -90.0f, pitch = 0.0f;
        float yawVelocity = 0.0f, pitchVelocity = 0.0f;
        glm::vec3 direction;

        while (!window->GetCloseRequested()) {
            frameCounter.BeginFrame();

            physics.Update(frameCounter.GetDeltaTime());

            input->Poll([imgui](SDL_Event& event) {
                imgui->Process(event);
            });

            // Compute camera velocity
            bool cameraMoved = false;
            glm::vec3 acceleration = glm::zero<glm::vec3>();
            if (keyboard.GetKeyDown(bl::Scancode::W)) {
                acceleration += cameraFront;
                cameraMoved = true;
            }
            if (keyboard.GetKeyDown(bl::Scancode::S)) {
                acceleration -= cameraFront;
                cameraMoved = true;
            }
            if (keyboard.GetKeyDown(bl::Scancode::A)) {
                acceleration -= glm::normalize(glm::cross(cameraFront, cameraUp));
                cameraMoved = true;
            }
            if (keyboard.GetKeyDown(bl::Scancode::D)) {
                acceleration += glm::normalize(glm::cross(cameraFront, cameraUp));
                cameraMoved = true;
            }
            if (keyboard.GetKeyDown(bl::Scancode::Space)) {
                acceleration += cameraUp;
                cameraMoved = true;
            }
            if (keyboard.GetKeyDown(bl::Scancode::LeftShift)) {
                acceleration -= cameraUp;
                cameraMoved = true;
            }

            if (keyboard.GetKeyDown(bl::Scancode::Escape)) {
                mouse.SetCaptured(window, false);
                ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            }

            if (glm::length(acceleration) > 0.0f) {
                acceleration = glm::normalize(acceleration) * cameraAcceleration;
            }

            cameraVelocity += acceleration * frameCounter.GetDeltaTime();

            // Apply camera velocity
            if (enableCameraMovementDamping) {
                cameraPos += cameraVelocity * frameCounter.GetDeltaTime();

                float cameraSpeed = glm::length(cameraVelocity);
                if (cameraSpeed > maxCameraSpeed) {
                    cameraVelocity = glm::normalize(cameraVelocity) * maxCameraSpeed;
                }
            } else {
                // Treat acceleration as direct position change
                cameraPos += acceleration * frameCounter.GetDeltaTime();
            }

            // Apply friction to camera velocity
            if (enableCameraMovementDamping && !cameraMoved) {
                cameraVelocity = bl::DampExponential(cameraVelocity, glm::zero<glm::vec3>(), cameraMovementDampLambda, frameCounter.GetDeltaTime());
            } else if (!cameraMoved) {
                cameraVelocity = glm::zero<glm::vec3>();
            }

            auto mousePos = mouse.GetMousePosition();
            auto mouseDelta = mouse.GetMouseDelta();

            if (mouse.IsButtonDown(bl::MouseButton::Left) && window->GetFocused() && !ImGui::GetIO().WantCaptureMouse) {
                mouse.SetCaptured(window, true);
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
            }

            // Update camera direction based on mouse movement, use acceleration for smoother movement
            if (mouse.GetCaptured(window)) {
                float sensitivity = 0.1f;
                mouseDelta *= sensitivity;

                if (enableCameraSmoothing) {
                    yawVelocity = bl::DampExponential(yawVelocity, mouseDelta.x, cameraSmoothnessDampLambda, frameCounter.GetDeltaTime());
                    pitchVelocity = bl::DampExponential(pitchVelocity, mouseDelta.y, cameraSmoothnessDampLambda, frameCounter.GetDeltaTime());
                } else {
                    yawVelocity = mouseDelta.x;
                    pitchVelocity = mouseDelta.y;
                }

                yaw += yawVelocity;
                pitch -= pitchVelocity;

                if (pitch > 89.0f)
                    pitch = 89.0f;
                if (pitch < -89.0f)
                    pitch = -89.0f;

                if (yaw < 0.0f)
                    yaw += 360.0f;
                else if (yaw > 360.0f)
                    yaw -= 360.0f;

                direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                direction.y = sin(glm::radians(pitch));
                direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront = glm::normalize(direction);
            }

            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

            bl::Extent2D extent = window->GetExtent();
            glm::vec2 extentf = glm::vec2 { (float)extent.width, (float)extent.height };
            glm::mat4 projection = glm::perspective(glm::radians(70.0f), extentf.x / extentf.y, 0.1f, 1000.0f);
            projection[1][1] *= -1; // Invert the projection for Vulkan y (0, 1)

            renderer->SetView(view);
            renderer->SetProjection(projection);

            audio->Update();

            rootNode->Update(frameCounter.GetDeltaTime());

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

                rootNode->Draw(rd);

                imgui->BeginFrame();

                imgui->DrawDebug();

                ImGui::Begin("Settings");

                if (ImGui::TreeNode("Camera")) {
                    ImGui::Checkbox("Enable Mouse Smoothing", &enableCameraSmoothing);
                    ImGui::SetNextItemWidth(150.0f);
                    ImGui::SliderFloat("Mouse Smoothness", &cameraSmoothnessDampLambda, 1.0f, 10.0f);

                    ImGui::Dummy({0.0f, 10.0f});

                    ImGui::Checkbox("Enable Movement Damping", &enableCameraMovementDamping);
                    ImGui::SetNextItemWidth(150.0f);
                    ImGui::SliderFloat("Acceleration", &cameraAcceleration, 0.1f, 20.0f);
                    ImGui::SetNextItemWidth(150.0f);
                    ImGui::SliderFloat("Max Speed", &maxCameraSpeed, 1.0f, 20.0f);
                    ImGui::SetNextItemWidth(150.0f);
                    ImGui::SliderFloat("Movement Damping", &cameraMovementDampLambda, 1.0f, 10.0f);

                    ImGui::Separator();
                    ImGui::Text("Position: %.2f, %.2f, %.2f m", cameraPos.x, cameraPos.y, cameraPos.z);
                    ImGui::Text("Velocity: %.2f, %.2f, %.2f m/s", cameraVelocity.x, cameraVelocity.y, cameraVelocity.z);
                    ImGui::Text("Rotation: %.2f, %.2f deg", yaw, pitch);
                    ImGui::Text("Speed: %.2f m/s", glm::length(cameraVelocity));

                    ImGui::Separator();
                    ImGui::Text("Frame: %d", ImGui::GetFrameCount());
                    ImGui::Text("Current Frame: %d", rd.currentFrame);
                    ImGui::Text("Image Index: %d", rd.imageIndex);
                    ImGui::Text("Frame Time: %.5f ms", frameCounter.GetAverageMillisecondsPerFrame(144));
                    ImGui::Text("FPS: %d f/s", static_cast<int>(frameCounter.GetAverageFramesPerSecond(10)));

                    ImGui::TreePop();
                }

                ImGui::End();

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
