#include <Audio/AudioSystem.h>
#include <Core/FrameCounter.h>
#include <Core/Print.h>
#include <Core/Time.h>
#include <Core/Profiler.h>
#include <Editor/Editor.h>
#include <Engine/Engine.h>
#include <Graphics/GraphicsSystem.h>
#include <Graphics/UniformData.h>
#include <Graphics/Vertex.h>
#include <Physics/ObjectLayers.h>
#include <Physics/PhysicsSystem.h>
#include <Resources/Material.h>
#include <Resources/Model.h>
#include <Resources/ResourceSystem.h>
#include <Resources/Shader.h>
#include <Resources/Texture2D.h>
#include <Resources/NoiseTexture2D.h>
#include <Scene/AudioSource3D.h>
#include <Scene/MeshInstance3D.h>
#include <Scene/PhysicsBody3D.h>
#include <Window/Input.h>
#include <Window/Keyboard.h>
#include <Window/Mouse.h>
#include <Scene/CharacterBody3D.h>
#include <Scene/FlyCamera3D.h>
#include <Scene/MultiMeshInstance3D.h>
#include <Scene/Orbit3D.h>
#include <Physics/PhysicsRenderer.h>
#include <Social/Discord.h>
#include <Scene/NodeFilterIterator.h>

#include "ImGui/implot.h"

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
        physicsBody->SetMassProperties(80.0f);
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
        auto presentModes = renderer->GetPresentModes();
        auto multisampleModes = renderer->GetMultisampleCounts();

        ImPlot::CreateContext();

        auto& profiler = bl::GetGlobalProfiler();
        bool enableEditor = false;

        auto& discord = engine.GetDiscord();

        bl::DiscordActivity activity;
        activity.applicationID = 763767974469042178;
        activity.details = "Testing discord rich presence";
        activity.type = bl::DiscordActivityType::ePlaying;
        activity.state = "Programming infinitely...";
        activity.startTime = std::time(nullptr);
        activity.endTime = 0;
        activity.art.smallImage = "retrofox";
        activity.art.smallImageTooltip = "Look it's mini me!";
        activity.art.largeImage = "corruptedcanyons";
        activity.art.largeImageTooltip = "I call these, corrupted canyons.";

        discord.UpdateActivity(activity);

        auto physFrameCounter = physics.GetPhysFrameCounter();

        std::function<void(bl::Node*)> doChildren;

        bl::Node* selectedNode = nullptr;

        doChildren = [&](bl::Node* node){
            auto& children = node->GetVecChildren();

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
            if (node == selectedNode)
                flags |= ImGuiTreeNodeFlags_Selected;

            if (children.empty())
                flags |= ImGuiTreeNodeFlags_Leaf;

            if (ImGui::TreeNodeEx(node->GetName().c_str(), flags))
            {
                ImGui::SameLine();
                ImGui::TextColored({0.45f, 0.34f, 0.45f, 1.0f}, "%s", node->GetClassName().data());

                if (ImGui::IsItemClicked())
                {
                    selectedNode = node;
                }

                for (const auto& child : children)
                {
                    doChildren(child.get());
                }
                ImGui::TreePop();
            } else {
                ImGui::SameLine();
                ImGui::TextColored({0.45f, 0.34f, 0.45f, 1.0f}, "%s", node->GetClassName().data());
            }
        };


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

            rootNode->Update(frameCounter.GetDeltaTime());
            profiler.EndProfile("Update");

            renderer->SetView(cameraNode->GetViewMatrix());
            renderer->SetProjection(cameraNode->GetProjectionMatrix());

            auto objectFunc = [&](bl::RenderData& rd) {
                rootNode->Draw(rd);
            };

            auto renderFunc = [&](bl::RenderData& rd){
                //auto extent = window->GetExtent();

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

                if (enableEditor) {
                    ImGui::Begin("Heirarchy");


                    doChildren(rootNode.get());

                    //ImGui::Text("Objects in Scene: %d", rootNode->GetChildCount());
                    ImGui::End();

                    ImGui::Begin("Inspector");

                    //for (int i = 0; i < engine.GetClassDB())

                    ImGui::End();
                }

                ImGui::Begin("Settings");

                if (ImGui::TreeNode("Editor")) {
                    ImGui::Checkbox("Editor", &enableEditor);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Renderer")) {
                    for (std::size_t i = 0; i < presentModes.size(); i++) {
                        if (ImGui::RadioButton(bl::ToString(presentModes[i]).data(), presentModes[i] == renderer->GetPresentMode())) {
                            renderer->SetPresentMode(presentModes[i]);
                        }
                    }

                    ImGui::Separator();

                    for (std::size_t i = 0; i < multisampleModes.size(); i++) {
                        if (ImGui::RadioButton(bl::ToString(multisampleModes[i]).data(), multisampleModes[i] == renderer->GetMultisampleCount())) {
                            renderer->SetMultisampleCount(multisampleModes[i]);
                        }
                    }

                    static bool enablePhysDebugRenderer = false;
                    ImGui::Checkbox("Enable Physics Debug", &enablePhysDebugRenderer);
                    physicsRenderer->SetEnable(enablePhysDebugRenderer);

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Audio")) {
                    static float volume = 1.0f;
                    if (ImGui::SliderFloat("MASTER", &volume, 0.0f, 1.0f))
                    {
                        source->SetVolume(volume);
                    }

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Profiler")) {


                    bool enableProfiling = profiler.IsProfilingEnabled();
                    ImGui::Checkbox("Enable Profiling", &enableProfiling);
                    profiler.EnableProfiling(enableProfiling);

                    ImGui::Text("Frame Time: %.2f ms", frameCounter.GetDeltaTime() * 1000.0f);

                    // Plot a frame pie chart of the profiler data
                    static std::vector<float> values;
                    static std::vector<const char*> labels;

                    if (enableProfiling) {
                        profiler.GetProfileTimes(values);
                        profiler.GetProfileNames(labels);
                    } else {
                        values.clear();
                        labels.clear();
                    }

                    if (ImPlot::BeginPlot("##Pie1", ImVec2(ImGui::GetTextLineHeight()*16,ImGui::GetTextLineHeight()*16), ImPlotFlags_Equal | ImPlotFlags_NoMouseText | ImPlotFlags_NoInputs)) {
                        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
                        ImPlot::SetupAxesLimits(0, 1, 0, 1, ImPlotCond_None);

                        ImPlot::PlotPieChart(labels.data(), values.data(), static_cast<int>(values.size()), 0.5, 0.5, 0.4, "%.2f", 90, ImPlotPieChartFlags_Normalize);
                        ImPlot::EndPlot();
                    }

                    ImGui::TreePop();
                }

                ImPlot::ShowDemoWindow();

                ImGui::End();

                imgui->EndFrame(rd.GetCommandBuffer());
            };

            profiler.StartProfile("Render");
            renderer->Render(renderFunc, imguiFunc, objectFunc);
            profiler.EndProfile("Render");
            frameCounter.EndFrame();
            profiler.EndFrame();
        }

        graphics.GetDevice()->WaitForDevice();

        resourceMgr->UnloadAll();

    } catch (std::exception& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception Error", e.what(), nullptr);
        return EXIT_FAILURE;
    }

    return 0;
}
