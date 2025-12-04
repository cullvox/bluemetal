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

        auto rootNode = std::make_unique<bl::Node3D>(engine);

        auto characterNode = model.lock()->GetTree()->Clone();
        characterNode->SetPosition({0.0f, -1.0f, 0.0f});

        JPH::Ref<JPH::CapsuleShape> shape = new JPH::CapsuleShape(0.8f, 0.3f);
        auto physicsBody = std::make_unique<bl::PhysicsBody3D>(engine);
        physicsBody->SetName("Character");
        physicsBody->SetPosition({ 0.0f, 0.0f, -5.0f });
        physicsBody->SetShape(shape.GetPtr());
        physicsBody->ResetBody();
        physicsBody->AddChild(characterNode);
        rootNode->AddChild(std::move(physicsBody));

        std::array<bl::Vertex, 8 * 3> cubeVertices = {{
            {{ -0.5f, -0.5f, -0.5f}, {}, {}, { 0.0f, 0.0f}},  // A 0
            {{ 0.5f, -0.5f, -0.5f}, {}, {}, { 1.0f, 0.0f}},  // B 1
            {{ 0.5f,  0.5f, -0.5f}, {}, {}, { 1.0f, 1.0f}},  // C 2
            {{ -0.5f,  0.5f, -0.5f}, {}, {}, { 0.0f, 1.0f}},  // D 3
            {{ -0.5f, -0.5f,  0.5f}, {}, {}, { 0.0f, 0.0f}},  // E 4
            {{ 0.5f, -0.5f,  0.5f}, {}, {}, { 1.0f, 0.0f}},   // F 5
            {{ 0.5f,  0.5f,  0.5f}, {}, {}, { 1.0f, 1.0f}},   // G 6
            {{ -0.5f,  0.5f,  0.5f}, {}, {}, { 0.0f, 1.0f}},   // H 7
    
            {{-0.5f,  0.5f, -0.5f}, {}, {},  {0.0f, 0.0f}},  // D 8
            {{-0.5f, -0.5f, -0.5f}, {}, {},  {1.0f, 0.0f}},  // A 9
            {{-0.5f, -0.5f,  0.5f}, {}, {},  {1.0f, 1.0f}},  // E 10
            {{-0.5f,  0.5f,  0.5f}, {}, {},  {0.0f, 1.0f}},  // H 11
            {{0.5f, -0.5f, -0.5f}, {}, {},  {0.0f, 0.0f}},   // B 12
            {{0.5f,  0.5f, -0.5f}, {}, {},  {1.0f, 0.0f}},   // C 13
            {{0.5f,  0.5f,  0.5f}, {}, {},  {1.0f, 1.0f}},   // G 14
            {{0.5f, -0.5f,  0.5f}, {}, {},  {0.0f, 1.0f}},   // F 15
    
            {{-0.5f, -0.5f, -0.5f}, {}, {}, { 0.0f, 0.0f }},  // A 16
            {{0.5f, -0.5f, -0.5f}, {}, {}, { 1.0f, 0.0f }},   // B 17
            {{0.5f, -0.5f,  0.5f}, {}, {}, { 1.0f, 1.0f }},   // F 18
            {{-0.5f, -0.5f,  0.5f}, {}, {}, { 0.0f, 1.0f }},  // E 19
            {{0.5f,  0.5f, -0.5f}, {}, {}, {  0.0f, 0.0f }},  // C 20
            {{-0.5f,  0.5f, -0.5f}, {}, {}, { 1.0f, 0.0f }},  // D 21
            {{-0.5f,  0.5f,  0.5f}, {}, {}, { 1.0f, 1.0f }},  // H 22
            {{0.5f,  0.5f,  0.5f}, {}, {}, {  0.0f, 1.0f }},  // G 23

        }};

        std::array<uint32_t, 36> cubeIndices = {{
            // front and back
            0, 3, 2,
            2, 1, 0,
            4, 5, 6,
            6, 7 ,4,
            // left and right
            11, 8, 9,
            9, 10, 11,
            12, 13, 14,
            14, 15, 12,
            // bottom and top
            16, 17, 18,
            18, 19, 16,
            20, 21, 22,
            22, 23, 20
        }};

        auto cubeMesh = resourceMgr->Add<bl::Mesh>("", std::make_shared<bl::Mesh>(graphics));
        cubeMesh.lock()->UploadVertices<bl::Vertex>(cubeVertices);
        cubeMesh.lock()->UploadIndices(cubeIndices);

        auto floorMaterial = resourceMgr->Load<bl::Material>("Resources/Materials/Default.mat");
        auto floorTexture = resourceMgr->Load<bl::Texture2D>("Resources/Textures/floor.jpg");
        auto defaultSampler = resourceMgr->Load<bl::Sampler>("Resources/Samplers/Default.json");

        floorMaterial.lock()->SetSampledTexture2D("inAlbedo", defaultSampler, floorTexture);

        auto floorNode = std::make_unique<bl::MeshInstance3D>(engine);
        floorNode->SetName("Floor");
        floorNode->SetMesh(cubeMesh);
        floorNode->SetMaterial(floorMaterial);
        floorNode->SetScale({ 100.0f, 1.0f, 100.0f });
        floorNode->SetPosition({ 0.0f, 0, 0.0f });

        JPH::Ref<JPH::Shape> floorShape = new JPH::BoxShape({50.0f, 0.5f, 50.0f});
        auto floorStaticBody = std::make_unique<bl::PhysicsBody3D>(engine);
        floorStaticBody->SetName("FloorBody");
        floorStaticBody->SetMotionType(JPH::EMotionType::Static);
        floorStaticBody->SetObjectLayer(bl::ObjectLayers::STATIC);
        floorStaticBody->SetShape(floorShape);
        floorStaticBody->SetPosition({0.0f, -5.0f, 0.0f});
        floorStaticBody->ResetBody();

        floorStaticBody->AddChild(std::move(floorNode));

        rootNode->AddChild(std::move(floorStaticBody));

        //tree->SetPosition({ 0.0f, 0.0f, -30.0f });
        //tree->SetScale({ 0.01f, 0.01f, 0.01f });

        bl::FrameCounter frameCounter;
        float cameraAcceleration = 00.5f;
        float maxCameraSpeed = 8.f;
        bool useCameraAcceleration = true;
        glm::vec3 cameraVelocity = { 0.0f, 0.0f, 0.0f };
        glm::vec3 cameraPos = { 0.0f, 0.0f, 5.0f };
        glm::vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
        glm::vec3 cameraUp = { 0.0f, 1.0f, 0.0f };
        glm::mat4 view = glm::identity<glm::mat4>();
        float yaw = -90.0f, pitch = 0.0f;
        float yawVelocity = 0.0f, pitchVelocity = 0.0f;
        float walkingSpeed = 9.0f;
        glm::vec3 direction;

        while (!window->GetCloseRequested()) {
            frameCounter.BeginFrame();

            physics.Update(frameCounter.GetDeltaTime());

            input->Poll([imgui](SDL_Event& event) {
                imgui->Process(event);
            });

            // Compute camera velocity
            if (keyboard.GetKeyDown(bl::Scancode::W))
                cameraVelocity += cameraFront * cameraAcceleration * frameCounter.GetDeltaTime();
            if (keyboard.GetKeyDown(bl::Scancode::S))
                cameraVelocity -= cameraFront * cameraAcceleration * frameCounter.GetDeltaTime();
            if (keyboard.GetKeyDown(bl::Scancode::A))
                cameraVelocity -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraAcceleration * frameCounter.GetDeltaTime();
            if (keyboard.GetKeyDown(bl::Scancode::D))
                cameraVelocity += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraAcceleration * frameCounter.GetDeltaTime();
            if (keyboard.GetKeyDown(bl::Scancode::Space))
                cameraVelocity += cameraUp * cameraAcceleration * frameCounter.GetDeltaTime();
            if (keyboard.GetKeyDown(bl::Scancode::LeftShift))
                cameraVelocity -= cameraUp * cameraAcceleration * frameCounter.GetDeltaTime();
                

            if (glm::length(cameraVelocity) > maxCameraSpeed) {
                cameraVelocity = glm::normalize(cameraVelocity) * maxCameraSpeed;
            }
            
            // Apply camera velocity
            cameraPos += cameraVelocity;

            // Apply friction to camera velocity
            cameraVelocity = bl::DampExponential(cameraVelocity, glm::vec3 { 0.0f, 0.0f, 0.0f }, 5.0f, frameCounter.GetDeltaTime());

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

            // Update camera direction based on mouse movement, use acceleration for smoother movement
            if (mouse.GetCaptured(window)) {
                float sensitivity = 0.1f;
                mouseDelta *= sensitivity;

                if (useCameraAcceleration) {
                    yawVelocity = bl::DampExponential(yawVelocity, mouseDelta.x, 5.0f, frameCounter.GetDeltaTime());
                    pitchVelocity = bl::DampExponential(pitchVelocity, mouseDelta.y, 5.0f, frameCounter.GetDeltaTime());
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

                direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                direction.y = sin(glm::radians(pitch));
                direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront = glm::normalize(direction);

                //bl::Print::Info("Camera direction: {}, {}, {}", direction.x, direction.y, direction.z);
            }

            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            //bl::Print::Info("Mouse: {}, {}", mousePos.x, mousePos.y);
            //bl::Print::Info("Camera Pos: {}, {}, {}", cameraPos.x, cameraPos.y, cameraPos.z);

            bl::Extent2D extent = window->GetExtent();
            glm::vec2 extentf = glm::vec2 { (float)extent.width, (float)extent.height };
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
