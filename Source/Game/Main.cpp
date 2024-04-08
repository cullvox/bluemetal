#include "Core/Time.h"
#include "Engine/Engine.h"
#include "Graphics/Shader.h"
#include "Graphics/Pipeline.h"
#include "imgui/imgui_impl_bluemetal.h"
#include "imgui/imgui_impl_sdl2.h"

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
    
    bl::Engine engine{};

    auto graphics = engine.getGraphics();
    auto audio = engine.getAudio();

    auto renderer = graphics->getRenderer();

    auto sound = audio->createSound("Resources/Audio/Music/Aria Math.flac");
    auto listener = audio->createListener();
    auto source = audio->createSource();

    source->setSound(sound.get());
    source->play();

    // float last = 0.0f;
    // float current = bl::Time::current();

    ImGui_ImplBluemetal_InitInfo initInfo = {
        graphics->getInstance(),
        graphics->getPhysicalDevice(),
        graphics->getDevice(),
        graphics->getRenderer()->getUserInterfacePass(),
        graphics->getWindow()
    };

    ImGui_ImplBluemetal_Init(&initInfo);

    // bl::GfxShader::CreateInfo shaderCreateInfo{
    //     VK_SHADER_STAGE_VERTEX_BIT,
    //     
// 
    // };
    // std::shared_ptr<bl::GfxShader> vertShader = std::make_shared<bl::GfxShader>();

    bl::FrameCounter frameCounter;

    bool running = true;
    bool minimized = false;
    while (running) {
        frameCounter.beginFrame();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

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
                case SDL_WINDOWEVENT_EXPOSED:
                case SDL_WINDOWEVENT_RESTORED:
                case SDL_WINDOWEVENT_MAXIMIZED: {
                    minimized = false;
                    break;
                }
                }
                break;
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

        graphics->getRenderer()->render([&frameCounter, &graphics, &audio](VkCommandBuffer cmd){
            ImGui_ImplBluemetal_BeginFrame();

            ImGui::Begin("Debug Info");

            if (ImGui::CollapsingHeader("Graphics")) {
                ImGui::Text("Graphics Device: %s", graphics->getPhysicalDevice()->getDeviceName().c_str()); 
                ImGui::Text("Graphics Vendor: %s", graphics->getPhysicalDevice()->getVendorName().c_str()); 
                ImGui::Text("F/S: %d", frameCounter.getFramesPerSecond()); 
                ImGui::Text("MS/F: %.2f", frameCounter.getMillisecondsPerFrame()); 
                ImGui::Text("Average F/S (Over 10 Seconds): %.1f", frameCounter.getAverageFramesPerSecond(10));
                ImGui::Text("Average MS/F (Over 144 Frames): %.2f", frameCounter.getAverageMillisecondsPerFrame(144)); 
                // ImGui::Text("Present Mode: %s", string_VkPresentModeKHR(currentPresentMode)); ImGui::Text("Surface Format: (%s, %s)", string_VkFormat(currentSurfaceFormat.format), string_VkColorSpaceKHR(currentSurfaceFormat.colorSpace));
            }

            if (ImGui::CollapsingHeader("Audio")) {
                ImGui::Text("Audio Driver: %s", audio->getSystem()->getDriverName().c_str());
                ImGui::Text("Num Channels: %d", audio->getSystem()->getNumChannelsPlaying());
            }

            ImGui::End();

            ImGui::Begin("Advanced Configuration");
            ImGui::SeparatorText("ImGui Interface");

            static float alpha = 1.0f;
            if (ImGui::SliderFloat("UI Alpha Transparency", &alpha, 0.1f, 1.0f)) {
                ImGui::GetStyle().Alpha = alpha;
            }

            ImGui::SeparatorText("Graphics");


            // if (ImGui::CollapsingHeader("Physical Device"))
            // {
            //     for (size_t i = 0; i < physicalDevices)
            // }

            if (ImGui::CollapsingHeader("Surface Formats")) {

                // static int surfaceFormatSelected = 0;
                // for (size_t i = 0; i < surfaceFormats.size(); i++)
                // {
                //     HelpMarker("Changes to this value will require a restart to change.");
                //     std::string name = fmt::format("({}, {})", string_VkFormat(surfaceFormats[i].format),
                //     string_VkColorSpaceKHR(surfaceFormats[i].colorSpace)); if
                //     (ImGui::RadioButton(name.c_str(), &surfaceFormatSelected, i))
                //     {
                //        //Engine::get().getConfig()["render"]["presentMode"] = (int)i;
                //     }
                // }
            }

            if (ImGui::CollapsingHeader("Present Modes"))
            {
                // static int selectedPresentMode = 0;
                // for (size_t i = 0; i < presentModes.size(); i++)
                // {
                //     std::string name = fmt::format("({})", string_VkPresentModeKHR(presentModes[i]));
                //     if (ImGui::RadioButton(name.c_str(), &selectedPresentMode, i))
                //     {
                //         // static int value = 0;
                //         // value = i;
                //         // _postRenderCommands.push([&](){
                //         //     m_swapchain->recreate(presentModes[value],
                //         // _swapchain->getSurfaceFormat();
                //         //     m_renderer->resize(m_swapchain->getExtent());
                //         // });
                //     }
                // }
            }

            ImGui::End();

            ImGui::ShowDemoWindow();

            ImGui_ImplBluemetal_EndFrame(cmd);
        });

        }

        frameCounter.endFrame();
    }

    return 0;
}
