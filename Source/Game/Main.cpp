#include "Core/Time.h"
#include "Engine/Engine.h"
#include "Graphics/Shader.h"
#include <Graphics/PhysicalDevice.h>
#include "Graphics/Pipeline.h"
#include "Graphics/Vertex.h"
#include "Graphics/Mesh.h"

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
    bl::Engine engine{};
 
    auto graphics = engine.getGraphics();
    auto audio = engine.getAudio();

    auto renderDoc = graphics->getRenderDoc();
    auto renderer = graphics->getRenderer();

    auto sound = audio->createSound("Resources/Audio/Music/Mutation.flac");
    auto listener = audio->createListener();
    auto source = audio->createSource();

    source->setSound(sound.get());
    source->play();

    // float last = 0.0f;
    // float current = bl::Time::current();

    auto imgui = engine.getImGui();


    auto loadShader = [=](const char* path, VkShaderStageFlagBits stage){
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return std::make_shared<bl::GfxShader>(graphics->getDevice(), stage, buffer);
    };

    auto vert = loadShader("Resources/Shaders/Default.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    auto frag = loadShader("Resources/Shaders/Default.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    auto descriptorCache = graphics->getDescriptorCache();
    auto layoutCache = graphics->getPipelineLayoutCache();

    std::vector<VkDescriptorSetLayoutBinding> bindings = {
        VkDescriptorSetLayoutBinding{
            0,
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            1,
            VK_SHADER_STAGE_VERTEX_BIT,
            nullptr
        }
    };

    VkDescriptorSetLayout bufferSetLayout = descriptorCache->acquire(bindings);
    auto layout = graphics->getPipelineLayoutCache()->acquire({bufferSetLayout, bufferSetLayout}, {});

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

    auto mesh = std::make_shared<bl::Mesh>(graphics->getDevice(), cubeVertices, cubeIndices);

    VkVertexInputBindingDescription vertexInputBinding{0, sizeof(bl::Vertex), VK_VERTEX_INPUT_RATE_VERTEX};

    std::vector<VkVertexInputAttributeDescription> attributes = {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, 12},
        {2, 0, VK_FORMAT_R32G32_SFLOAT, 24},
    };

    bl::GfxPipeline::CreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.layout = layout;
    pipelineCreateInfo.renderPass = renderer->getUserInterfacePass();
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.vertexInputBindings = {vertexInputBinding};
    pipelineCreateInfo.vertexInputAttribs = attributes;
    pipelineCreateInfo.shaders = {vert, frag};

    auto pipeline = std::make_shared<bl::GfxPipeline>(graphics->getDevice(), pipelineCreateInfo);
    auto window = graphics->getWindow();
    auto presentModes = graphics->getPhysicalDevice()->getPresentModes(graphics->getWindow());

    bl::FrameCounter frameCounter;

    bool running = true;
    bool minimized = false;
    while (running) {
        frameCounter.beginFrame();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            imgui->process(event);

            switch (event.type) {
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    running = false;
                    break;
                case SDL_WINDOWEVENT_MINIMIZED: {
                    minimized = true;
                    break;
                }
                case SDL_WINDOWEVENT_RESTORED: {
                    minimized = false;
                    break;
                }
                }
                break;
                case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                    case SDLK_F12:
                        renderDoc->beginCapture();
                    }
                    break;
                }
            }

        }

        // last = current;
        // current = bl::Time::current();
        // auto dt = current - last;

        glm::vec3 position { sinf(bl::Time::current() / 1000.f) * 10.f, 0.0f, 10.0f };
        glm::vec3 velocity { cosf(bl::Time::current() / 1000.f) * 1 / 100.f, 0.0f, 0.0f };

        source->set3DAttributes(position, velocity);
        audio->update();

        if (!minimized) {

        
        graphics->getRenderer()->render([&](VkCommandBuffer cmd){
            
            auto extent = window->getExtent();

            VkViewport vp{};
            vp.x = 0.0f;
            vp.y = 0.0f;
            vp.width = (float)extent.x;
            vp.height = (float)extent.y;
            vp.minDepth = 0.0f;
            vp.maxDepth = 1.0f;
            vkCmdSetViewport(cmd, 0, 1, &vp);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = {(uint32_t)extent.x, (uint32_t)extent.y};
            vkCmdSetScissor(cmd, 0, 1, &scissor);

            vkCmdBindVertexBuffers(cmd, 0, 1, &)

            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->get());
            vkCmdDraw(cmd, 3, 1, 0, 0);

            imgui->beginFrame();

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

                ImGui::Text("Graphics Device: %s", graphics->getPhysicalDevice()->getDeviceName().c_str()); 
                ImGui::Text("Graphics Vendor: %s", graphics->getPhysicalDevice()->getVendorName().c_str()); 
                ImGui::Text("F/S: %d", frameCounter.getFramesPerSecond()); 
                ImGui::Text("MS/F: %.2f", frameCounter.getMillisecondsPerFrame()); 
                ImGui::Text("Average F/S (Over 10 Seconds): %.1f", frameCounter.getAverageFramesPerSecond(10));
                ImGui::Text("Average MS/F (Over 144 Frames): %.2f", frameCounter.getAverageMillisecondsPerFrame(144)); 
                ImGui::Text("Present Mode: %s", string_VkPresentModeKHR(graphics->getSwapchain()->getPresentMode())); 
                // ImGui::Text("Surface Format: (%s, %s)", string_VkFormat(currentSurfaceFormat.format), string_VkColorSpaceKHR(currentSurfaceFormat.colorSpace));

                if (ImGui::TreeNode("Physical Devices")) {
                    auto physicalDevices = graphics->getPhysicalDevices();

                    for (size_t i = 0; i < physicalDevices.size(); i++) {
                        auto& physicalDevice = physicalDevices[i];

                        if (ImGui::TreeNode((void*)(intptr_t)i, "%s", physicalDevice->getDeviceName().c_str())) {
                            

                            const char* deviceType = "";
                            switch (physicalDevice->getType()) {
                            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: deviceType = "Integrated"; break;
                            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: deviceType = "Discrete"; break;
                            case VK_PHYSICAL_DEVICE_TYPE_CPU: deviceType = "CPU"; break;
                            default: deviceType = "Unknown"; break;
                            }

                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4{0.2f, 0.8f, 0.4f, 1.0f}, "%s", deviceType);

                            if (physicalDevice == graphics->getPhysicalDevice()) {
                                ImGui::SameLine();
                                ImGui::TextColored(ImVec4{0.2f, 0.5f, 0.8f, 1.0f}, "Current");
                            }

                            if (ImGui::TreeNode("Present Modes")) {
                                for (VkPresentModeKHR mode : physicalDevice->getPresentModes(window))
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
                ImGui::Text("Audio Driver: %s", audio->getSystem()->getDriverName().c_str());
                ImGui::Text("Num Channels: %d", audio->getSystem()->getNumChannelsPlaying());
            }

            ImGui::End();

            ImGui::ShowDemoWindow();

            imgui->endFrame(cmd);
        });

        }

        frameCounter.endFrame();
    }

    } catch (std::exception& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception Error", e.what(), nullptr);
        return EXIT_FAILURE;
    }


    system("pause");

    return 0;
}
