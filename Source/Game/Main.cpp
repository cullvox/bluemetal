#include "Core/Time.h"
#include "Core/FrameCounter.h"
#include "Engine/Engine.h"
#include "Graphics/Shader.h"
#include "Graphics/PhysicalDevice.h"
#include "Graphics/Pipeline.h"
#include "Graphics/Vertex.h"
#include "Graphics/Mesh.h"
#include "Material/UniformData.h"

#include "Math/Transform.h"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see
// docs/FONTS.md)
// static void HelpMarker(const char* desc)
// {
//     ImGui::TextDisabled("(?)");
//     if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
//     {
//         ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
//         ImGui::TextUnformatted(desc);
//         ImGui::PopTextWrapPos();
//         ImGui::EndTooltip();
//     }
// }

int main(int argc, const char** argv)
{
    (void)argc;
    (void)argv;
    
    try {
    bl::Engine engine;

    auto resourceMgr = engine.GetResourceManager();
    resourceMgr->LoadFromManifest("Resources/Manifest.json");

    auto audio = engine.GetAudio();
    auto sound = resourceMgr->Load<bl::Sound>("Resources/Audio/Music/Taswell.flac");
    auto listener = audio->CreateListener();
    auto source = audio->CreateSource();

    source->SetSound(sound);
    source->Play();

    auto graphics = engine.GetGraphics();
    auto imgui = engine.GetImGui();
    
    auto vert = resourceMgr->Load<bl::Shader>("Resources/Shaders/Default.vert.spv");
    auto frag = resourceMgr->Load<bl::Shader>("Resources/Shaders/Default.frag.spv");

    std::vector<bl::Vertex> cubeVertices{
        // front
        { {-1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },
        { {1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },
        { {1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },
        { {-1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },
        // back
        { {-1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },
        { {1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },
        { {1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },
        { {-1.0f,  1.0f, -1.f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },
    };

    std::vector<uint32_t> cubeIndices{
        // front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
    };

    auto mesh = std::make_shared<bl::Mesh>(graphics->GetDevice(), cubeVertices, cubeIndices);

    VkVertexInputBindingDescription vertexInputBinding{0, sizeof(bl::Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
    std::vector<VkVertexInputAttributeDescription> attributes = {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, 12},
        {2, 0, VK_FORMAT_R32G32_SFLOAT, 24},
    };

    auto renderer = engine.GetRenderer();

    bl::PipelineCreateInfo pci{};
    pci.renderPass = renderer->GetUIPass();
    pci.subpass = 0;
    pci.vertexInputBindings = {vertexInputBinding};
    pci.vertexInputAttribs = attributes;
    pci.shaders = {vert, frag};
    pci.setLayoutCache = graphics->GetDescriptorCache();
    pci.pipelineLayoutCache = graphics->GetPipelineLayoutCache();

    auto pipeline = std::make_shared<bl::Pipeline>(graphics->GetDevice(), pci);
    auto window = engine.GetWindow();
    auto presentModes = graphics->GetPhysicalDevice()->GetPresentModes(window);

    auto globalBuffer = std::make_shared<bl::Buffer>(graphics->GetDevice(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sizeof(bl::GlobalUBO));
    auto objectBuffer = std::make_shared<bl::Buffer>(graphics->GetDevice(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sizeof(bl::ObjectUBO));

    

    bl::FrameCounter frameCounter;
    glm::vec3 cameraPos { 0.0f, 0.0f, -10.f};

    bool running = true;
    bool minimized = false;
    while (running) {
        frameCounter.BeginFrame();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            imgui->Process(event);

            switch (event.type) {
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    running = false;
                    break;
                case SDL_WINDOWEVENT_MINIMIZED:
                    minimized = true;
                    break;
                case SDL_WINDOWEVENT_RESTORED:
                    minimized = false;
                    break;
                }
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_F12:
                    // renderDoc->beginCapture();
                    break;
                }
                break;
            }
        }

        auto extent = window->GetExtent();
        auto extenti = glm::ivec2{(int)extent.width, (int)extent.height};
        auto extentf = glm::vec2{(float)extent.width, (float)extent.height};

        bl::GlobalUBO globalUBO = {
            0.0f,
            0.0f,
            extenti,
            {},
            glm::translate(glm::identity<glm::mat4>(), cameraPos),
            glm::perspectiveFov(70.0f, extentf.x, extentf.y, 0.01f, 100.0f)
        };

        globalBuffer->Upload((void*)&globalUBO);

        bl::ObjectUBO objectUBO = {
            glm::identity<glm::mat4>()
        };

        objectBuffer->Upload((void*)&objectUBO);

        glm::vec3 position { sinf(bl::Time::current() / 1000.f) * 10.f, 0.0f, 10.0f };
        glm::vec3 velocity { cosf(bl::Time::current() / 1000.f) * 1 / 100.f, 0.0f, 0.0f };

        source->Set3DAttributes(position, velocity);
        audio->Update();

        if (!minimized) {

        renderer->Render([&](VkCommandBuffer cmd){
            
            auto extent = window->GetExtent();

            VkViewport vp;
            vp.x = 0.0f;
            vp.y = 0.0f;
            vp.width = (float)extent.width;
            vp.height = (float)extent.height;
            vp.minDepth = 0.0f;
            vp.maxDepth = 1.0f;
            vkCmdSetViewport(cmd, 0, 1, &vp);

            VkRect2D scissor;
            scissor.offset = {0, 0};
            scissor.extent = extent;
            vkCmdSetScissor(cmd, 0, 1, &scissor);


            // mesh->bind(cmd);
// 
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->Get());
            vkCmdDraw(cmd, 3, 1, 0, 0);

            imgui->BeginFrame();

            ImGui::Begin("Debug Info");

            ImGui::Text("bluemetal");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, "%s", bl::to_string(bl::engineVersion).c_str());

            ImGui::Text("sdl2");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, BL_STRINGIFY(SDL_MAJOR_VERSION) "." BL_STRINGIFY(SDL_MINOR_VERSION));

            ImGui::Text("vulkan header");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, "%d", VK_HEADER_VERSION);

            ImGui::Text("imgui");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4{0.2f, 0.4f, 0.8f, 1.0f}, "%s", ImGui::GetVersion());

            if (ImGui::CollapsingHeader("Graphics")) {

                ImGui::Text("Graphics Device: %s", graphics->GetPhysicalDevice()->GetDeviceName().c_str()); 
                ImGui::Text("Graphics Vendor: %s", graphics->GetPhysicalDevice()->GetVendorName().c_str()); 
                ImGui::Text("F/S: %d", frameCounter.GetFramesPerSecond()); 
                ImGui::Text("MS/F: %.2f", frameCounter.GetMillisecondsPerFrame()); 
                ImGui::Text("Average F/S (Over 10 Seconds): %.1f", frameCounter.GetAverageFramesPerSecond(10));
                ImGui::Text("Average MS/F (Over 144 Frames): %.2f", frameCounter.GetAverageMillisecondsPerFrame(144)); 
                ImGui::Text("Present Mode: %s", string_VkPresentModeKHR(window->GetSwapchain()->GetPresentMode())); 
                // ImGui::Text("Surface Format: (%s, %s)", string_VkFormat(currentSurfaceFormat.format), string_VkColorSpaceKHR(currentSurfaceFormat.colorSpace));

                if (ImGui::TreeNode("Physical Devices")) {
                    auto physicalDevices = graphics->GetPhysicalDevices();

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
                            ImGui::TextColored(ImVec4{0.2f, 0.8f, 0.4f, 1.0f}, "%s", deviceType);

                            if (physicalDevice == graphics->GetPhysicalDevice()) {
                                ImGui::SameLine();
                                ImGui::TextColored(ImVec4{0.2f, 0.5f, 0.8f, 1.0f}, "Current");
                            }

                            if (ImGui::TreeNode("Present Modes")) {
                                for (VkPresentModeKHR mode : physicalDevice->GetPresentModes(window))
                                    ImGui::Text("%s", string_VkPresentModeKHR(mode));

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

            imgui->EndFrame(cmd);
        });

        }

        frameCounter.EndFrame();
    }

    } catch (std::exception& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception Error", e.what(), nullptr);
        return EXIT_FAILURE;
    }


    system("pause");

    return 0;
}
