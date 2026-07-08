#include <Audio/AudioSystem.h>
#include <Core/ClassDB.h>
#include <Core/FrameCounter.h>
#include <Core/Print.h>
#include <Core/Profiler.h>
#include <Core/Time.h>
#include <Editor/Editor.h>
#include <Engine/Engine.h>
#include <Graphics/GraphicsSystem.h>
#include <Graphics/Renderer.h>
#include <Graphics/UniformData.h>
#include <Graphics/Vertex.h>
#include <ImGui/ImGuiSystem.h>
#include <ImGui/imgui_impl_vulkan.h>
#include <ImGui/implot.h>
#include <Physics/ObjectLayers.h>
#include <Physics/PhysicsRenderer.h>
#include <Physics/PhysicsSystem.h>
#include <Resources/Material.h>
#include <Resources/Model.h>
#include <Resources/NoiseTexture2D.h>
#include <Resources/ResourceSystem.h>
#include <Resources/Shader.h>
#include <Resources/Texture2D.h>
#include <Scene/AudioSource3D.h>
#include <Scene/CharacterBody3D.h>
#include <Scene/FlyCamera3D.h>
#include <Scene/MeshInstance3D.h>
#include <Scene/MultiMeshInstance3D.h>
#include <Scene/NodeFilterIterator.h>
#include <Scene/Orbit3D.h>
#include <Scene/PhysicsBody3D.h>
#include <Social/Discord.h>
#include <Scene/Sky3D.h>
#include <Graphics/WindowViewport.h>
#include <Window/Input.h>
#include <Window/Keyboard.h>
#include <Window/Mouse.h>
#include <Editor/EditorSystem.h>

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <glm/trigonometric.hpp>

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
        auto& editor = engine.GetEditorSystem();
        auto physicsRenderer = physics.GetPhysicsRenderer();

        auto sound = resourceMgr->Load<bl::Sound>("Resources/Audio/Music/Aaron Cherof - Anagnorisis - 06 Mare Marginis.ogg");
        auto source = std::make_unique<bl::AudioSource3D>(engine);

        source->Play(sound, true);

        auto model = resourceMgr->Load<bl::Model>("Resources/Models/low_poly_fox.glb");
        auto cube = resourceMgr->Load<bl::Model>("Resources/Models/cube.glb");
        auto grass = resourceMgr->Load<bl::Model>("Resources/Models/Grass.glb");

        auto rootNode = std::make_unique<bl::Node3D>(engine);
        rootNode->SetName("Root");

        auto characterNode = model.lock()->GetTree()->Clone();
        characterNode->SetName("Character");
        characterNode->SetPosition({ 0.0f, -0.6f, 0.0f });

        JPH::Ref<JPH::CapsuleShape> shape = new JPH::CapsuleShape(0.5f, 0.3f);
        auto physicsBody = std::make_unique<bl::CharacterBody3D>(engine);
        physicsBody->SetName("CharacterBody");
        physicsBody->SetPosition({ 0.0f, 0.0f, -5.0f });
        physicsBody->SetShape(shape.GetPtr());
        physicsBody->SetFriction(0.2f);
        physicsBody->SetMass(80.0f);
        physicsBody->SetRestitution(0.0f);
        physicsBody->ResetBody();
        physicsBody->SetDOF(true, true, true, false, false, false); // Lock rotation around X and Z axis
        physicsBody->AddChild(characterNode);
        rootNode->AddChild(std::move(physicsBody));

        auto floorMaterial = resourceMgr->Load<bl::Material>("Resources/Materials/Default.mat");
        auto floorTexture = resourceMgr->Load<bl::Texture2D>("Resources/Textures/floor.jpg");
        auto defaultTexture = resourceMgr->Load<bl::Texture2D>("Resources/Textures/Default.png");
        auto defaultSampler = resourceMgr->Load<bl::Sampler>("Resources/Samplers/Default.json");
        auto nearestSampler = resourceMgr->Load<bl::Sampler>("Resources/Samplers/Nearest.json");
        auto noiseTexture = resourceMgr->Load<bl::NoiseTexture2D>("Resources/Textures/Noise.json");
        auto grssMaterial = resourceMgr->Load<bl::Material>("Resources/Materials/Grass.mat");
        auto physDebugFlatMaterial = resourceMgr->Load<bl::Material>("Resources/Materials/PhysicsDebugFlat.mat");
        auto skyMaterial = resourceMgr->Load<bl::Material>("Resources/Materials/Sky.mat");

        auto skyMat = skyMaterial.lock()->CreateInstance();
        skyMat->SetScaler("material.offsetHorizon", 0.0f);

        skyMat->SetScaler("material.horizonIntensity", -3.3f);
        skyMat->SetVector4("material.sunSet", { 1.0f, 0.8f, 1.0f, 1.0f });
        skyMat->SetVector4("material.horizonColorDay", { 0.0f, 0.8f, 1.0f, 1.0f });
        skyMat->SetVector4("material.horizonColorNight", { 0.0f, 0.8f, 1.0f, 1.0f });

        // Sun
        skyMat->SetVector4("material.cSun", glm::vec4{1.0});
        skyMat->SetVector3("material.sunDirection", glm::radians(glm::vec3{45.0f, 0.0f, 0.0f}));
        skyMat->SetScaler("material.sunRadius", 0.5f);
        skyMat->SetScaler("material.bFlatSun", true);

        // Moon
        skyMat->SetVector4("material.cMoon", glm::vec4{1.0f});
        skyMat->SetScaler("material.moonRadius", 0.15f);
        skyMat->SetScaler("material.moonCrescent", -0.3f);
        skyMat->SetScaler("material.darkFalloff", 4.0f);

        // Day Background Colors
        skyMat->SetVector4("material.cDayBottom", {0.4f, 1.0f, 1.0f, 1.0f});
        skyMat->SetVector4("material.cDayTop", {0.0f, 0.8f, 1.0f, 1.0f});

        // Night Background Colors
        skyMat->SetVector4("material.cNightBottom", {0.0f, 0.0f, 0.2f, 1.0f});
        skyMat->SetVector4("material.cNightTop", {0.0f, 0.0f, 0.0f, 1.0f});

        // stars
        skyMat->SetScaler("material.baseNoiseScale", 0.2f);
        skyMat->SetScaler("material.starsSpeed", 0.3f);
        skyMat->SetScaler("material.starsCutoff", 0.08f);
        skyMat->SetVector4("material.cStarsSky", {0.0f, 0.2f, 0.1f, 1.0f});
        skyMat->SetScaler("material.offsetStars", 0.083f);
        skyMat->SetScaler("material.starsIntensity", -2.829f);
        skyMat->SetScaler("material.starFalloff", 1.79f);
        skyMat->SetScaler("material.starsFadeModulation", 0.91f);

        skyMat->SetSampledTexture2D("starsTexture", defaultSampler, noiseTexture);
        skyMat->SetSampledTexture2D("baseNoiseTexture", defaultSampler, noiseTexture);

        resourceMgr->Add("Materials/SkyInstance.mat", skyMat);

        physicsRenderer->SetMaterial(physDebugFlatMaterial.lock()->GetVulkanMaterial());

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

        resourceMgr->Add("Materials/GrassInstance.mat", grassMaterial);

        auto multimesh = std::make_unique<bl::MultiMeshInstance3D>(engine);
        multimesh->SetName("GrassMultiMesh");
        multimesh->SetMesh(grass.lock()->GetMeshes()[0]);
        multimesh->SetMaterial(grassMaterial);
        multimesh->SetInstanceCount(6000);

        for (int i = 0; i < 6000; i++) {
            float x = static_cast<float>(rand()) / ( static_cast<float>(RAND_MAX/(80.0f)));
            float z = static_cast<float>(rand()) / ( static_cast<float>(RAND_MAX/(80.0f)));
            float rot_x = static_cast<float>(rand()) / ( static_cast<float>(RAND_MAX/(360.0f)));

            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(x - 40.0f, -4.f, z - 40.0f));
            transform = glm::rotate(transform, glm::radians(rot_x), glm::vec3(0.0f, 1.0f, 0.0f));
            transform = glm::scale(transform, glm::vec3(1.0f, 1.0f, 1.0f));
            multimesh->SetInstanceTransform(i, transform);
        }
        rootNode->AddChild(std::move(multimesh));

        floorMaterial.lock()->SetSampledTexture2D("inAlbedo", defaultSampler, floorTexture);
        floorMaterial.lock()->SetBool("material.useTriplanar", true);

        auto floorNode = cube.lock()->GetTree()->Clone();
        floorNode->SetName("Floor");
        floorNode->SetScale({ 100.0f, 1.0f, 100.0f });

        floorNode->GetChild("Cube")->As<bl::MeshInstance3D>()->SetMaterial(floorMaterial);

        JPH::Ref<JPH::Shape> floorShape = new JPH::BoxShape({ 100.0f, 1.0f, 100.0f });
        auto floorStaticBody = std::make_unique<bl::PhysicsBody3D>(engine);
        floorStaticBody->SetName("FloorBody");
        floorStaticBody->SetMotionType(JPH::EMotionType::Static);
        floorStaticBody->SetObjectLayer(bl::ObjectLayers::STATIC);
        floorStaticBody->SetShape(floorShape);
        floorStaticBody->SetPosition({ 0.0f, -5.0f, 0.0f });
        floorStaticBody->SetFriction(0.8f);
        floorStaticBody->ResetBody();
        floorStaticBody->SetDOF(true, true, true, false, true, false); // Lock rotation around Z axis

        floorStaticBody->AddChild(std::move(floorNode));

        rootNode->AddChild(std::move(floorStaticBody));

        // Add Sky3D node
        auto skyNode = std::make_unique<bl::Sky3D>(engine);
        skyNode->SetName("Sky");
        skyNode->SetSkyMaterial(skyMat.get());
        
        rootNode->AddChild(std::move(skyNode));


        //auto flycam = std::make_unique<bl::FlyCamera3D>(engine);
        //flycam->SetName("FlyCam");
        //flycam->SetPosition({ 0.0f, 0.0f, 5.0f });
        //rootNode->AddChild(std::move(flycam));

        //auto flyCamNode = rootNode->GetChild("FlyCam")->As<bl::FlyCamera3D>();
        auto playerNode = rootNode->GetChild("CharacterBody")->As<bl::Node3D>();

        auto cameraOrbit = std::make_unique<bl::Orbit3D>(engine);
        cameraOrbit->SetName("Orbiter");
        playerNode->AddChild(std::move(cameraOrbit));

        auto followCamera = std::make_unique<bl::Camera3D>(engine);
        followCamera->SetName("FollowCam");
        //followCamera->SetPosition({ 0.0f, 10.0f, -10.0f });
        followCamera->SetRotationEuler({ 0.0f, 0.0f, 0.0f});
        followCamera->SetProjection(bl::CameraProjection::ePerspective);
        followCamera->SetFOV(65.0f);
        followCamera->SetNearClip(0.01f);
        followCamera->SetFarClip(1000.0f);

        auto orbiter = playerNode->GetChild("Orbiter")->As<bl::Orbit3D>();

        orbiter->AddChild(std::move(followCamera));
        auto cameraNode = orbiter->GetChild("FollowCam")->As<bl::Camera3D>();

        bl::FrameCounter& frameCounter = engine.GetFrameCounter();
        auto presentModes = std::vector<VkPresentModeKHR>{};//renderer->GetPresentModes();
        //auto multisampleModes = renderer->GetMultisampleCounts();

        ImPlot::CreateContext();

        auto& profiler = bl::GetGlobalProfiler();
        bool enableEditor = true;

        auto& discord = engine.GetDiscord();

        bl::DiscordActivity activity;
        activity.applicationID = 763767974469042178;
        activity.details = "Testing discord rich presence";
        activity.type = bl::DiscordActivityType::ePlaying;
        activity.state = "Programming infinitely...";
        activity.startTime = std::time(nullptr);
        activity.endTime = 0;
        activity.party.id = "party1234";
        activity.party.sizes.currentSize = 1;
        activity.party.sizes.maxSize = 4;
        activity.art.smallImage = "retrofox";
        activity.art.smallImageTooltip = "Look it's mini me!";
        activity.art.largeImage = "corruptedcanyons";
        activity.art.largeImageTooltip = "I call these, corrupted canyons.";

        discord. UpdateActivity(activity);

        auto physFrameCounter = physics.GetPhysFrameCounter();

        editor.GetHierarchyEditor().SetRootNode(rootNode.get());

        auto& classDB = engine.GetClassDB();
        auto classNames = classDB.GetClassNames();

        auto viewport = graphics.GetViewport();

        while (!window->GetCloseRequested()) {
            profiler.StartFrame();
            frameCounter.BeginFrame();

            discord.RunCallbacks();

            profiler.StartProfile("Input");
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

            profiler.EndProfile("Input");

            profiler.StartProfile("Audio");
            audio->Update();
            profiler.EndProfile("Audio");

            profiler.StartProfile("Physics");

            auto physUpdater = [&](){
                rootNode->PhysicsUpdate();
            };

            bool physUpdate = physics.Update(frameCounter.GetDeltaTime(), physUpdater);

            profiler.EndProfile("Physics");

            profiler.StartProfile("Update");
            grassMaterial->SetVector4("material.playerParams", glm::vec4{playerNode->GetWorldPosition(), 2.0f});
            grassMaterial->SetVector4("material.colorSmall", {sinf(bl::Time::Current()), sinf(bl::Time::Current() + bl::Math::Pi), 0.9f, 1.0f});

            // skyMat->SetVector3("material.sunDirection", glm::normalize(glm::vec3{cosf(bl::Time::Current() * 0.1f), sinf(bl::Time::Current() * 0.1f), 0.0f}));

            glm::vec3 cameraDirection = cameraNode->GetWorldRotationEuler();
            cameraDirection.z = 0.0f; // Remove roll for skybox calculations.

            skyMat->SetVector3("material.eyeDirection", glm::radians(cameraDirection));

            rootNode->Update(frameCounter.GetDeltaTime());
            profiler.EndProfile("Update");


            viewport->SetView(cameraNode->GetViewMatrix());
            viewport->SetProjection(cameraNode->GetProjectionMatrix());

            auto objectFunc = [&](bl::RenderData& rd) {
                rootNode->Draw(rd);

                renderer->DrawLine(playerNode->GetWorldPosition(), {0.0f, 0.0f, 0.0f});
                if (physUpdate)
                {
                    physicsRenderer->SetCameraPosition(cameraNode->GetWorldPosition());
                    physicsRenderer->Reset();
                    physics.Draw();
                    physicsRenderer->WriteInstances();
                    physUpdate = false;
                }
                physicsRenderer->RecordCommands();
            };

            auto imguiFunc = [&](bl::RenderData& rd){
                imgui->BeginFrame();
                editor.Draw(rd);
                imgui->EndFrame(rd.GetCommandBuffer());
            };

            renderer->SetObjectFunction(objectFunc);
            renderer->SetGUIFunction(imguiFunc);

            profiler.StartProfile("Render");
            renderer->RenderFrame();
            profiler.EndProfile("Render");


            frameCounter.EndFrame();

            profiler.EndFrame();
        }

        graphics.GetDevice()->WaitForDevice();

        resourceMgr->ReleaseAll();

    } catch (std::exception& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception Error", e.what(), nullptr);
        return EXIT_FAILURE;
    }

    return 0;
}
