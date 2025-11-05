#include "Core/Time.h"
#include "Core/FrameCounter.h"
#include "Core/Print.h"
#include "Math/Transform.h"
#include "Engine/Engine.h"
#include "Resources/StaticMesh.h"
#include "Graphics/VulkanShader.h"
#include "Graphics/VulkanPhysicalDevice.h"
#include "Graphics/VulkanPipeline.h"
#include "Graphics/Vertex.h"
#include "Resources/Material.h"
#include "Graphics/VulkanConversions.h"
#include "Resources/Texture2D.h"
#include "Resources/Shader.h"
#include "Graphics/UniformData.h"

#include "Scene/AudioSource3D.h"
#include <Window/Keyboard.h>
#include <Window/Mouse.h>




static inline float randomValue() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

int main(int argc, const char** argv)
{
    (void)argc;
    (void)argv;

    try
    {
    auto engine = bl::Engine{};
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
    //auto model = resourceMgr->Load<bl::StaticModel>("Models/red_fox_skull.glb");
    //auto dragonModel = resourceMgr->Load<bl::StaticModel>("Models/dragon_quick_sculpt.glb");
    auto material = resourceMgr->Load<bl::Material>("Resources/Materials/Default.mat");

    auto renderer = engine.GetRenderer();

    bl::VulkanPipelineStateInfo psi{};
    psi.rasterizerState.cullMode = VK_CULL_MODE_BACK_BIT;
    psi.stages.shaders = { vert.Get()->Get(), frag.Get()->Get() };

    auto window = engine.GetWindow();
    auto vulkanWindow = dynamic_cast<bl::VulkanWindow*>(window);

    // auto [pass, subpass] = renderer->GetRenderPass(bl::RenderPassType::eGeometry);

    // auto material = std::make_unique<bl::Material>(graphics->GetDevice(), pass, subpass, psi, vulkanWindow->GetSwapchain()->GetImageCount(), 1);
    // material->SetVector4("material.color", { 1.0f, 0.0f, 0.0, 1.0f});

    auto presentModes = graphics->GetPhysicalDevice()->GetPresentModes(vulkanWindow);

    bl::FrameCounter frameCounter;

    bl::ObjectPC object{};
    object.model = glm::identity<glm::mat4>();
    object.model = glm::translate(object.model, glm::vec3{0.0f, 0.0f, 0.0f});

    auto texture = resourceMgr->Load<bl::Texture2D>("Resources/Textures/furry.qoi");
    auto sampler = bl::VulkanSampler{graphics->GetDevice(), VK_FILTER_LINEAR};

    // material->SetSampledImage2D("inAlbedo", &sampler, texture.Get()->GetImage());

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -10.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::mat4 view = glm::identity<glm::mat4>();
    float yaw = -90.0f, pitch = 0.0f;
    float walkingSpeed = 9.0f;
    glm::vec3 direction;

    auto extent = window->GetExtent();
    auto extenti = glm::ivec2{(int)extent.width, (int)extent.height};
    auto extentf = glm::vec2{(float)extent.width, (float)extent.height};

    view = glm::lookAt(cameraPos, cameraPos - cameraFront, cameraUp);
    auto projection = glm::perspectiveFov(70.0f, extentf.x, extentf.y, 0.1f, 100.0f);



    while (!window->GetCloseRequested())
    {
        frameCounter.BeginFrame();

        input->Poll([imgui](SDL_Event& event){
            imgui->Process(event);
        });

        if(keyboard.GetKeyDown(bl::Scancode::W))
            cameraPos += -walkingSpeed * cameraFront * frameCounter.GetDeltaTime();
        if (keyboard.GetKeyDown(bl::Scancode::S))
            cameraPos += walkingSpeed * cameraFront * frameCounter.GetDeltaTime();
        if (keyboard.GetKeyDown(bl::Scancode::A))
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * walkingSpeed * frameCounter.GetDeltaTime();
        if (keyboard.GetKeyDown(bl::Scancode::D))
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * -walkingSpeed * frameCounter.GetDeltaTime();
        if (keyboard.GetKeyDown(bl::Scancode::Escape))
        {
            mouse.SetCaptured(window, false);
        }

        auto mousePos = mouse.GetMousePosition();
        auto mouseDelta = mouse.GetMouseDelta();

        if (mouse.IsButtonDown(bl::MouseButton::Left) && window->GetFocused() && !ImGui::GetIO().WantCaptureMouse)
        {
            mouse.SetCaptured(window, true);
        }

        if (mouse.GetCaptured(window))
        {
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

        view = glm::lookAt(cameraPos, cameraPos - cameraFront, cameraUp);

        bl::Print::Info("Mouse: {}, {}", mousePos.x, mousePos.y);
        bl::Print::Info("Camera Pos: {}, {}, {}", cameraPos.x, cameraPos.y, cameraPos.z);

        extent = window->GetExtent();
        extentf = glm::vec2{(float)extent.width, (float)extent.height};
        projection = glm::perspectiveFov(70.0f, extentf.x, extentf.y, 0.1f, 1000.0f);
        // auto extentf = glm::vec2{(float)extent.width, (float)extent.height};

        renderer->SetView(view);
        renderer->SetProjection(projection);

        // object.model = glm::rotate(object.model, frameCounter.GetDeltaTime() * glm::radians(180.0f), glm::vec3{0.f, 1.f, 1.f});

        glm::vec3 position{ sinf(bl::Time::Current() / 1000.f) * 10.f, 0.0f, 10.0f };
        glm::vec3 velocity{ cosf(bl::Time::Current() / 1000.f) * 1 / 100.f, 0.0f, 0.0f };

        glm::vec4 color = { 1.f, 0.5f, 0.f, 1.0f };
        glm::vec4 val{};

        std::memcpy(&val, &color, sizeof(glm::vec4));

        // glm::vec4 color = { 1.0f, 0.0f, 0.0f, 1.0f};
        // material->SetVector4("material.color", color);

        audio->Update();

        if (!window->GetMinimized()) {

        renderer->Render([&](bl::VulkanRenderData& rd){

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
            scissor.offset = {0, 0};
            scissor.extent = {extent.width, extent.height};
            vkCmdSetScissor(rd.cmd, 0, 1, &scissor);

            material->Bind(rd);
            material->PushConstant(rd, 0, sizeof(bl::ObjectPC), &object);

            //model.Get()->Draw(rd, material->GetMaterial());

            material->PushConstant(rd, 0, sizeof(bl::ObjectPC), &object);
            // dragonModel.Get()->Draw(rd, material->GetMaterial());

            imgui->BeginFrame();

            ImGui::Begin("Debug Info");

            if (ImGui::CollapsingHeader("Version"))
            {
                ImGui::Text("Compiled " __DATE__ " " __TIME__);
                ImGui::Text("Compiler ");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, "%s", bl::compiler.c_str());

                ImGui::Text("Bluemetal");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, "%s", bl::to_string(bl::engineVersion).c_str());

                ImGui::Text("SDL");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, BL_STRINGIFY(SDL_MAJOR_VERSION) "." BL_STRINGIFY(SDL_MINOR_VERSION));

                ImGui::Text("Vulkan Header");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, "%d", VK_HEADER_VERSION);

                ImGui::Text("Vulkan Version");
                ImGui::SameLine();
                auto instanceVersion = volkGetInstanceVersion();
                ImGui::TextColored(ImVec4{0.7f, 0.1f, 0.1f, 1.0f}, "%d.%d.%d", VK_VERSION_MAJOR(instanceVersion), VK_VERSION_MINOR(instanceVersion), VK_VERSION_PATCH(instanceVersion));

                ImGui::Text("ImGui");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, "%s", ImGui::GetVersion());
            }

            if (ImGui::CollapsingHeader("Input"))
            {
                ImGui::Text("Camera Direction: %f, %f, %f", direction.x, direction.y, direction.z);
                ImGui::Text("Mouse Relative: %f, %f", mouseDelta.x, mouseDelta.y);
                ImGui::Text("x: %f, y: %f, z: %f", cameraPos.x, cameraPos.y, cameraPos.z);
            }


            if (ImGui::CollapsingHeader("Graphics")) {

                ImGui::Text("Graphics Device: %s", graphics->GetPhysicalDevice()->GetDeviceName().c_str()); 
                ImGui::SameLine();
                ImGui::HelpMarker("Your graphics card.");
                ImGui::Text("Graphics Vendor: %s", graphics->GetPhysicalDevice()->GetVendorName().c_str()); 
                ImGui::Text("F/S: %d", frameCounter.GetFramesPerSecond()); 
                ImGui::Text("MS/F: %.2f", frameCounter.GetMillisecondsPerFrame()); 
                ImGui::Text("Average F/S (Over 10 Seconds): %.1f", frameCounter.GetAverageFramesPerSecond(10));
                ImGui::Text("Average MS/F (Over 144 Frames): %.2f", frameCounter.GetAverageMillisecondsPerFrame(144)); 
                ImGui::Text("Presenting: (%s | %s, %s)", bl::ToString(vulkanWindow->GetSwapchain()->GetPresentMode()).data(), bl::ToString(vulkanWindow->GetSwapchain()->GetSurfaceFormat().format).data(), bl::ToString(vulkanWindow->GetSwapchain()->GetSurfaceFormat().colorSpace).data()); 
                // ImGui::Text("Surface Format: (%s, %s)", string_VkFormat(currentSurfaceFormat.format), string_VkColorSpaceKHR(currentSurfaceFormat.colorSpace));

                if (ImGui::TreeNode("Physical Devices")) {
                    auto physicalDevices = graphics->GetInstance()->GetPhysicalDevices();

                    for (size_t i = 0; i < physicalDevices.size(); i++) {
                        auto& physicalDevice = physicalDevices[i];

                        if (ImGui::TreeNode((void*)(intptr_t)i, "%s", physicalDevice->GetDeviceName().c_str())) {
                            const char* deviceType = "";
                            switch (physicalDevice->GetType()) {
                            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: deviceType = "Integrated"; break;
                            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: deviceType = "Discrete"; break;
                            case VK_PHYSICAL_DEVICE_TYPE_CPU: deviceType = "CPU"; break;
                            default: deviceType = "Unknown"; break;
                            }

                            ImGui::SameLine();
                            ImGui::TextColored({0.2f, 0.8f, 0.4f, 1.0f}, "%s", deviceType);

                            if (physicalDevice == graphics->GetPhysicalDevice()) {
                                ImGui::SameLine();
                                ImGui::TextColored(ImVec4{0.2f, 0.5f, 0.8f, 1.0f}, "Current");
                            }

                            if (ImGui::TreeNode("Present Modes")) {
                                for (VkPresentModeKHR mode : physicalDevice->GetPresentModes(vulkanWindow))
                                    ImGui::Text("%s", bl::ToString(mode).data());

                                ImGui::TreePop();
                            }

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
            }

            if (ImGui::CollapsingHeader("Audio")) {
                ImGui::Text("Audio Driver: %s", audio->GetDriverName().c_str());
                ImGui::Text("Num Channels: %d", audio->GetNumChannelsPlaying());
            }

            ImGui::End();

            ImGui::ShowDemoWindow();

            imgui->EndFrame(rd.cmd);
        });
        }

        frameCounter.EndFrame();
    }

    graphics->GetDevice()->WaitForDevice();

    resourceMgr->UnloadAll();

    }
    catch (std::exception& e) 
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception Error", e.what(), nullptr);
        return EXIT_FAILURE;
    }

    return 0;
}
