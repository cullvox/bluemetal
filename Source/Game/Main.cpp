#include "Core/FrameCounter.h"
#include "Core/Print.h"
#include "Core/Time.h"
#include "Editor/Editor.h"
#include "Engine/Engine.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/UniformData.h"
#include "Graphics/Vertex.h"
#include "Physics/ObjectLayers.h"
#include "Physics/PhysicsSystem.h"
#include "Resources/Material.h"
#include "Resources/Model.h"
#include "Resources/ResourceSystem.h"
#include "Resources/Shader.h"
#include "Resources/Texture2D.h"
#include "Resources/NoiseTexture2D.h"
#include "Scene/AudioSource3D.h"
#include "Scene/MeshInstance3D.h"
#include "Scene/PhysicsBody3D.h"
#include "Window/Input.h"
#include "Window/Keyboard.h"
#include "Window/Mouse.h"
#include <Scene/FlyCamera3D.h>

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

int main(int argc, const char** argv)
{
    (void)argc;
    (void)argv;

    try {
        bl::Engine engine { argc, argv };

        auto resourceMgr = engine.GetResourceSystem();
        auto audio = engine.GetAudio();
        auto& graphics = engine.GetGraphics();
        auto imgui = engine.GetImGui();
        auto input = engine.GetInput();
        auto& keyboard = input->GetKeyboard();
        auto& mouse = input->GetMouse();
        auto& physics = engine.GetPhysics();
        auto renderer = engine.GetRenderer();
        auto window = engine.GetWindow();
        auto& editor = engine.GetEditor();

        auto sound = resourceMgr->Load<bl::Sound>("Resources/Audio/Music/Taswell.flac");
        auto source = std::make_unique<bl::AudioSource3D>(engine);

        source->Play(sound, true);

        auto model = resourceMgr->Load<bl::Model>("Resources/Models/low_poly_fox.glb");
        auto cube = resourceMgr->Load<bl::Model>("Resources/Models/cube.glb");
        auto grass = resourceMgr->Load<bl::Model>("Resources/Models/Grass.glb");

        auto rootNode = std::make_unique<bl::Node3D>(engine);

        auto characterNode = model.lock()->GetTree()->Clone();
        characterNode->SetName("Character");
        characterNode->SetPosition({ 0.0f, -0.6f, 0.0f });

        JPH::Ref<JPH::CapsuleShape> shape = new JPH::CapsuleShape(0.8f, 0.3f);
        auto physicsBody = std::make_unique<bl::PhysicsBody3D>(engine);
        physicsBody->SetName("CharacterBody");
        physicsBody->SetPosition({ 0.0f, 0.0f, -5.0f });
        physicsBody->SetShape(shape.GetPtr());
        physicsBody->ResetBody();
        physicsBody->AddChild(characterNode);
        rootNode->AddChild(std::move(physicsBody));

        auto floorMaterial = resourceMgr->Load<bl::Material>("Resources/Materials/Default.mat");
        auto floorTexture = resourceMgr->Load<bl::Texture2D>("Resources/Textures/floor.jpg");
        auto defaultTexture = resourceMgr->Load<bl::Texture2D>("Resources/Textures/Default.png");
        auto defaultSampler = resourceMgr->Load<bl::Sampler>("Resources/Samplers/Default.json");
        auto nearestSampler = resourceMgr->Load<bl::Sampler>("Resources/Samplers/Nearest.json");
        auto noiseTexture = resourceMgr->Load<bl::NoiseTexture2D>("Resources/Textures/Noise.json");
        auto grssMaterial = resourceMgr->Load<bl::Material>("Resources/Materials/Grass.mat");

        auto grassMaterial = grssMaterial.lock()->CreateInstance();
        grassMaterial->SetSampledTexture2D("noiseSampler", defaultSampler, noiseTexture);
        grassMaterial->SetSampledTexture2D("windNoiseTexture", defaultSampler, noiseTexture);
        grassMaterial->SetVector4("material.factors", { 1.0f, 0.4f, 0.0f, 0.0f });
        grassMaterial->SetVector4("material.backLightColor", { 1.0f, 1.0f, 1.0f, 1.0f});
        grassMaterial->SetVector4("material.clumping", { 1.0f, 0.4f, 1.0f, 0.0f });
        grassMaterial->SetVector4("material.colorSmall", {0.5f, 0.7f, 0.9f, 1.0f});
        grassMaterial->SetVector4("material.colorLarge", {0.5f, 0.8f, 0.3f, 1.0f});
        grassMaterial->SetVector4("material.windParams", {0.008f, 1.1f, 0.01f, 1.0f});
        grassMaterial->SetVector4("material.windDirection", {0.3f, 0.3f, 0.0f, 0.0f});
        grassMaterial->SetVector4("material.playerParams", {10.0f, -4.0f, 10.0f, 0.6f});

        // grassMaterial->SetScaler("material.grassScale", 1.0f);
        // grassMaterial->SetScaler("material.patchScale", 1.0f);
        // grassMaterial->SetScaler("material.miniumGrassScale", 0.4f);
        // grassMaterial->SetScaler("material.maxGrassScale", 1.0f);
        // grassMaterial->SetScaler("material.windSpeed", 0.008f);
        // grassMaterial->SetScaler("material.windSway", 1.1f);
        // grassMaterial->SetScaler("material.windScale", 0.01f);
        // grassMaterial->SetVector2("material.windDirectionVector", {0.3f, 0.3f});
        // grassMaterial->SetScaler("material.bladeBendFactor", 3.f);
        // grassMaterial->SetVector3("material.colorSmall", );
        // grassMaterial->SetVector3("material.colorLarge", {0.5, 0.8, 0.3});
        // grassMaterial->SetScaler("material.playerRadius", 0.6f);

        auto grasses = std::make_unique<bl::Node3D>(engine);
        grasses->SetName("Grass");

        for (int i = 0; i < 512; i++) {
            float x = static_cast<float>(rand()) / ( static_cast<float>(RAND_MAX/(20.0f)));
            float z = static_cast<float>(rand()) / ( static_cast<float>(RAND_MAX/(20.0f)));
            float rot_x = static_cast<float>(rand()) / ( static_cast<float>(RAND_MAX/(360.0f)));

            auto grassNode = grass.lock()->GetTree()->Clone();
            grassNode->SetName("Grass_" + std::to_string(i));
            grassNode->SetPosition({ x, -3.5f, z });
            //grassNode->SetRotation({0.0f, rot_x, 0.0f});
            grassNode->GetChild("Plane")->As<bl::MeshInstance3D>()->SetMaterial(grassMaterial);
            grasses->AddChild(grassNode);
        }
        rootNode->AddChild(std::move(grasses));

        floorMaterial.lock()->SetSampledTexture2D("inAlbedo", defaultSampler, noiseTexture);
        floorMaterial.lock()->SetBool("material.useTriplanar", true);

        auto floorNode = cube.lock()->GetTree()->Clone();
        floorNode->SetName("Floor");
        floorNode->SetScale({ 100.0f, 1.0f, 100.0f });

        floorNode->GetChild("Cube")->As<bl::MeshInstance3D>()->SetMaterial(floorMaterial);

        JPH::Ref<JPH::Shape> floorShape = new JPH::BoxShape({ 50.0f, 0.5f, 50.0f });
        auto floorStaticBody = std::make_unique<bl::PhysicsBody3D>(engine);
        floorStaticBody->SetName("FloorBody");
        floorStaticBody->SetMotionType(JPH::EMotionType::Static);
        floorStaticBody->SetObjectLayer(bl::ObjectLayers::STATIC);
        floorStaticBody->SetShape(floorShape);
        floorStaticBody->SetPosition({ 0.0f, -5.0f, 0.0f });
        floorStaticBody->ResetBody();
        floorStaticBody->SetDOF(true, true, true, false, true, false); // Lock rotation around Z axis

        floorStaticBody->AddChild(std::move(floorNode));

        rootNode->AddChild(std::move(floorStaticBody));

        auto flycam = std::make_unique<bl::FlyCamera3D>(engine);
        flycam->SetName("FlyCam");
        flycam->SetPosition({ 0.0f, 0.0f, 5.0f });
        rootNode->AddChild(std::move(flycam));

        auto flyCamNode = rootNode->GetChild("FlyCam")->As<bl::FlyCamera3D>();

        bl::FrameCounter& frameCounter = engine.GetFrameCounter();
        auto presentModes = renderer->GetPresentModes();
        auto multisampleModes = renderer->GetMultisampleCounts();

        while (!window->GetCloseRequested()) {
            frameCounter.BeginFrame();

            physics.Update(frameCounter.GetDeltaTime());

            input->Poll([imgui](SDL_Event& event) {
                imgui->Process(event);
            });

            if (keyboard.GetKeyDown(bl::Scancode::Escape)) {
                mouse.SetCaptured(window, false);
                ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            }

            if (mouse.IsButtonDown(bl::MouseButton::Left) && window->GetFocused() && !ImGui::GetIO().WantCaptureMouse) {
                mouse.SetCaptured(window, true);
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
            }

            bl::Extent2D extent = window->GetExtent();

            audio->Update();

            grassMaterial->SetVector4("material.playerParams", glm::vec4{flyCamNode->GetWorldPosition(), 5.0f});
            grassMaterial->SetVector4("material.colorSmall", {sinf(bl::Time::Current()), sinf(bl::Time::Current() + bl::Math::Pi), 0.9f, 1.0f});

            rootNode->Update(frameCounter.GetDeltaTime());

            auto objectFunc = [&](bl::RenderData& rd) {
                rootNode->Draw(rd);
            };

            auto renderFunc = [&](bl::RenderData& rd){
                auto extent = window->GetExtent();

                imgui->BeginFrame();
                editor.Draw(rd);

                ImGui::Begin("Settings");

                if (ImGui::TreeNode("Renderer")) {
                    for (int i = 0; i < presentModes.size(); i++) {
                        if (presentModes[i] == VK_PRESENT_MODE_FIFO_LATEST_READY_EXT)
                            ImGui::BeginDisabled();

                        if (ImGui::RadioButton(bl::ToString(presentModes[i]).data(), presentModes[i] == renderer->GetPresentMode())) {
                            renderer->SetPresentMode(presentModes[i]);
                        }

                        if (presentModes[i] == VK_PRESENT_MODE_FIFO_LATEST_READY_EXT)
                            ImGui::EndDisabled();
                    }

                    ImGui::Separator();

                    for (int i = 0; i < multisampleModes.size(); i++) {
                        if (ImGui::RadioButton(bl::ToString(multisampleModes[i]).data(), multisampleModes[i] == renderer->GetMultisampleCount())) {
                            renderer->SetMultisampleCount(multisampleModes[i]);
                        }
                    }

                    ImGui::TreePop();
                }

                ImGui::End();

                imgui->EndFrame(rd.GetCommandBuffer());
            };

            renderer->Render(renderFunc, objectFunc);

            frameCounter.EndFrame();
        }

        graphics.GetDevice()->WaitForDevice();

        resourceMgr->UnloadAll();

    } catch (std::exception& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception Error", e.what(), nullptr);
        return EXIT_FAILURE;
    }

    return 0;
}
