#include "Core/Time.h"
#include "Core/FrameCounter.h"
#include "Core/Print.h"
#include "Engine/Engine.h"
#include "Graphics/VulkanShader.h"
#include "Graphics/VulkanPhysicalDevice.h"
#include "Graphics/VulkanPipeline.h"
#include "Graphics/Vertex.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/VulkanConversions.h"
#include "Graphics/Texture2D.h"
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

static inline float randomValue() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

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
    
    auto vert = resourceMgr->Load<bl::VulkanShader>("Resources/Shaders/Default.vert.spv");
    auto frag = resourceMgr->Load<bl::VulkanShader>("Resources/Shaders/Default.frag.spv");

    std::vector<bl::Vertex> cubeVertices{
        // front
        { {-1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, {1.0f, 0.0f} },
        { {1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },
        { {1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 1.0f} },
        { {-1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, {1.0f, 1.0f} },
        // back
        { {-1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, {1.0f, 0.0f} },
        { {1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 0.0f} },
        { {1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, {0.0f, 1.0f} },
        { {-1.0f,  1.0f, -1.f }, { 0.0f, 0.0f, 0.0f }, {1.0f, 1.0f} },
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

    auto mesh = std::make_shared<bl::StaticMesh>(graphics->GetDevice(), cubeVertices, cubeIndices);



    auto renderer = engine.GetRenderer();

    bl::VulkanPipelineStateInfo psi{};
    psi.vertexInput.vertexInputBindings = bl::Vertex::GetBindingDescriptions();
    psi.vertexInput.vertexInputAttribs = bl::Vertex::GetBindingAttributeDescriptions();
    psi.stages = { vert.Get(), frag.Get() };

    auto material = std::make_unique<bl::Material>(graphics->GetDevice(), renderer->GetRenderPass(), 0, psi);
    material->SetVector4("material.color", { 1.0f, 0.0f, 0.0, 1.0f});
    
    auto window = engine.GetWindow();
    auto presentModes = graphics->GetPhysicalDevice()->GetPresentModes(window);

    auto globalBuffer = std::make_unique<bl::VulkanBuffer>(graphics->GetDevice(), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, sizeof(bl::GlobalUBO));

    void* globalBufferMap = nullptr;
    globalBuffer->Map(&globalBufferMap);

    std::vector<VkDescriptorSetLayoutBinding> bindings{1};
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    auto layout = graphics->GetDevice()->CacheDescriptorSetLayout(bindings);

    VkDescriptorSet globalSet = graphics->GetDevice()->AllocateDescriptorSet(layout);

    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = globalBuffer->Get();
    bufferInfo.offset = 0;
    bufferInfo.range = VK_WHOLE_SIZE;

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.pNext = nullptr;
    write.dstSet = globalSet;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(graphics->GetDevice()->Get(), 1, &write, 0, nullptr);

    bl::FrameCounter frameCounter;

    bl::ObjectPC object{};
    object.model = glm::identity<glm::mat4>();
    object.model = glm::translate(object.model, glm::vec3{0.0f, 0.0f, 0.0f});

    auto texture = resourceMgr->Load<bl::Texture2D>("Resources/Textures/fox.png");
    auto sampler = bl::VulkanSampler{graphics->GetDevice(), VK_FILTER_NEAREST};

    
    material->SetSampledImage2D("image", &sampler, texture.Get()->GetImage());

    bool firstMouse = true;
    glm::ivec2 lastMouse{};
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -10.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
    glm::mat4 view = glm::identity<glm::mat4>();
    float yaw = -90.0f, pitch = 0.0f;
    float walkingSpeed = 9.0f;
    bool mouseCaptured = false;
    bool windowFocused = false;
    glm::vec3 direction;

    auto extent = window->GetExtent();
    auto extenti = glm::ivec2{(int)extent.width, (int)extent.height};
    auto extentf = glm::vec2{(float)extent.width, (float)extent.height};

    view = glm::lookAt(cameraPos, cameraPos - cameraFront, cameraUp);
    auto projection = glm::perspectiveFov(70.0f, extentf.x, extentf.y, 1.0f, 100.0f);

    bl::GlobalUBO globalUBO = {
        0.0f,
        0.0f,
        extenti,
        {},
        view,
        projection
    };

    std::memcpy(globalBufferMap, &globalUBO, sizeof(globalUBO));

    bool running = true;
    bool minimized = false;
    while (running) 
    {
        frameCounter.BeginFrame();


        glm::vec2 mouseRelativeMovement = {};
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type) 
            {
            case SDL_WINDOWEVENT:
                switch (event.window.event) 
                {
                case SDL_WINDOWEVENT_CLOSE:
                    running = false;
                    break;
                case SDL_WINDOWEVENT_MINIMIZED:
                    minimized = true;
                    break;
                case SDL_WINDOWEVENT_RESTORED:
                    minimized = false;
                    break;
                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    windowFocused = true;
                    break;
                case SDL_WINDOWEVENT_FOCUS_LOST:
                    windowFocused = false;
                    break;
                }
                break;
            case SDL_MOUSEMOTION:
                mouseRelativeMovement.x = (float)event.motion.xrel;
                mouseRelativeMovement.y = (float)event.motion.yrel;
            }

            imgui->Process(event);
        }

        SDL_PumpEvents();

        const uint8_t* keystate = SDL_GetKeyboardState(NULL);    
        if(keystate[SDL_SCANCODE_W])
            cameraPos += -walkingSpeed * cameraFront * frameCounter.GetDeltaTime();
        if (keystate[SDL_SCANCODE_S])
            cameraPos += walkingSpeed * cameraFront * frameCounter.GetDeltaTime();
        if (keystate[SDL_SCANCODE_A])
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * walkingSpeed * frameCounter.GetDeltaTime();
        if (keystate[SDL_SCANCODE_D])
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * -walkingSpeed * frameCounter.GetDeltaTime();
        if (keystate[SDL_SCANCODE_ESCAPE])
        {
            mouseCaptured = false;
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }

        glm::ivec2 mouse;
        uint32_t buttons = SDL_GetMouseState(&mouse.x, &mouse.y);

        if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT) && windowFocused && !ImGui::GetIO().WantCaptureMouse)
        {
            mouseCaptured = true;
            SDL_SetRelativeMouseMode(SDL_TRUE);
        }

        if (mouseCaptured)
        {
            if (firstMouse)
            {
                lastMouse = mouse;
                firstMouse = false;
            }

            float sensitivity = 0.1f;
            mouseRelativeMovement *= sensitivity;

            yaw += mouseRelativeMovement.x;
            pitch -= mouseRelativeMovement.y;

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(direction);

            blInfo("Camera direction: {}, {}, {}", direction.x, direction.y, direction.z);
        }

        view = glm::lookAt(cameraPos, cameraPos - cameraFront, cameraUp);

        blInfo("Mouse: {}, {}, {}", mouse.x, mouse.y, firstMouse);
        blInfo("Camera Pos: {}, {}, {}", cameraPos.x, cameraPos.y, cameraPos.z);

        auto extent = window->GetExtent();
        auto extentf = glm::vec2{(float)extent.width, (float)extent.height};
        // auto extentf = glm::vec2{(float)extent.width, (float)extent.height};

        bl::GlobalUBO globalUBO = {
            0.0f,
            0.0f,
            extentf,
            {},
            view,
            projection
        };

        std::memcpy(globalBufferMap, &globalUBO, sizeof(globalUBO));

        // object.model = glm::rotate(object.model, frameCounter.GetDeltaTime() * glm::radians(180.0f), glm::vec3{0.f, 1.f, 1.f});

        glm::vec3 position{ sinf(bl::Time::current() / 1000.f) * 10.f, 0.0f, 10.0f };
        glm::vec3 velocity{ cosf(bl::Time::current() / 1000.f) * 1 / 100.f, 0.0f, 0.0f };

        glm::vec4 color = { 1.f, 0.5f, 0.f, 1.0f };
        
        glm::vec4 val{};

        std::memcpy(&val, &color, sizeof(glm::vec4));

        // glm::vec4 color = { 1.0f, 0.0f, 0.0f, 1.0f};
        material->SetVector4("material.color", color);

        source->Set3DAttributes(position, velocity);
        audio->Update();

        if (!minimized) {

        
        material->UpdateUniforms();

        renderer->Render([&](VulkanRenderData& rd){

            auto extent = window->GetExtent();

            VkViewport viewport;
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)extent.width;
            viewport.height = (float)extent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(cmd, 0, 1, &viewport);

            VkRect2D scissor;
            scissor.offset = {0, 0};
            scissor.extent = extent;

            vkCmdSetViewportWithCountEXT(cmd, 1, &viewport);
            vkCmdSetScissorWithCountEXT(cmd, 1, &scissor);

            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipeline()->GetLayout(), 0, 1, &globalSet, 0, nullptr);
            material->Bind(rd);
            material->PushConstant(cmd, 0, sizeof(bl::ObjectPC), &object);
            mesh->bind(cmd);
            mesh->draw(cmd);

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

            if (ImGui::CollapsingHeader("Input"))
            {
                ImGui::Text("Camera Direction: %f, %f, %f", direction.x, direction.y, direction.z);
                ImGui::Text("Mouse Relative: %f, %f", mouseRelativeMovement.x, mouseRelativeMovement.y);
            }

            ImGui::Text("x: %f, y: %f, z: %f", cameraPos.x, cameraPos.y, cameraPos.z);

            if (ImGui::CollapsingHeader("Graphics")) {

                ImGui::Text("Graphics Device: %s", graphics->GetPhysicalDevice()->GetDeviceName().c_str()); 
                ImGui::Text("Graphics Vendor: %s", graphics->GetPhysicalDevice()->GetVendorName().c_str()); 
                ImGui::Text("F/S: %d", frameCounter.GetFramesPerSecond()); 
                ImGui::Text("MS/F: %.2f", frameCounter.GetMillisecondsPerFrame()); 
                ImGui::Text("Average F/S (Over 10 Seconds): %.1f", frameCounter.GetAverageFramesPerSecond(10));
                ImGui::Text("Average MS/F (Over 144 Frames): %.2f", frameCounter.GetAverageMillisecondsPerFrame(144)); 
                ImGui::Text("Present Mode: %s", bl::vk::ToString(window->GetSwapchain()->GetPresentMode())); 
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
                                    ImGui::Text("%s", bl::vk::ToString(mode));

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

    globalBuffer->Unmap();

    } catch (std::exception& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception Error", e.what(), nullptr);
        return EXIT_FAILURE;
    }

    return 0;
}
