#include "Engine/Engine.h"
#include "Core/Time.h"
#include "imgui/imgui_impl_bluemetal.h"

//Helper to display a little (?) mark which shows a tooltip when hovered.
//In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
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

    bl::Engine engine;
    
    engine.init(bl::eSubsystemGraphicsBit | bl::eSubsystemAudioBit);
    
    auto audio = engine.getAudio();
    //auto graphics = engine.getGraphics();

    //auto renderer = graphics->getRenderer();

    auto sound = audio->createSound("Resources/Audio/Music/Aria Math.flac");
    auto listener = audio->createListener();
    auto source = audio->createSource();
    

    source->setSound(sound.get());
    source->play();

    //float last = 0.0f;
    //float current = bl::Time::current();

    while (true)
    {
        //last = current;
        //current = bl::Time::current();
        //auto dt = current - last;


        bl::Vector3f position{ sinf(bl::Time::current() / 1000.f) * 10.f, 0.0f, 10.0f };
        bl::Vector3f velocity{ cosf(bl::Time::current() / 1000.f) * 1/100.f, 0.0f, 0.0f };

        source->set3DAttributes(position, velocity);
        
        audio->update();
    }

    //auto& graphics = engine.getGraphicsSubsystem();



    // ImGui_ImplBluemetal_BeginFrame();
// 
    // auto surfaceFormats = graphics.getSurfaceFormats();
    // auto presentModes = graphics.getPresentModes();
// 
    // ImGui::Begin("Debug Info");
    // ImGui::Text("Graphics Device: %s", Engine::get().getPhysicalDevice()->getDeviceName().c_str());
    // ImGui::Text("Graphics Vendor: %s", Engine::get().getPhysicalDevice()->getVendorName().c_str());
    // ImGui::Text("F/S: %d", Engine::get().getFrameCounter().getFramesPerSecond());
    // ImGui::Text("MS/F: %.2f", Engine::get().getFrameCounter().getMillisecondsPerFrame());
    // ImGui::Text("Average F/S (Over 10 Seconds): %.1f", Engine::get().getFrameCounter().getAverageFramesPerSecond(10));
    // ImGui::Text("Average MS/F (Over 144 Frames): %.2f", Engine::get().getFrameCounter().getAverageMillisecondsPerFrame(144));
    // ImGui::Text("Present Mode: %s", string_VkPresentModeKHR(currentPresentMode));
    // ImGui::Text("Surface Format: (%s, %s)", string_VkFormat(currentSurfaceFormat.format), string_VkColorSpaceKHR(currentSurfaceFormat.colorSpace));
// 
    // ImGui::End();
// 
    // ImGui::Begin("Advanced Configuration");
// 
    // ImGui::SeparatorText("ImGui Interface");
// 
    // static float alpha = 1.0f;
    // if (ImGui::SliderFloat("UI Alpha Transparency", &alpha, 0.1f, 1.0f))
    // {
    //     ImGui::GetStyle().Alpha = alpha;
    // }
// 
    // ImGui::SeparatorText("Graphics");
// 
// 
    // // if (ImGui::CollapsingHeader("Physical Device"))
    // // {
    // //     for (size_t i = 0; i < physicalDevices)
    // // }
// 
    // if (ImGui::CollapsingHeader("Surface Formats"))
    // {
// 
    //     static int surfaceFormatSelected = 0;
    //     for (size_t i = 0; i < surfaceFormats.size(); i++)
    //     {
    //         HelpMarker("Changes to this value will require a restart to change.");
    //         std::string name = fmt::format("({}, {})", string_VkFormat(surfaceFormats[i].format), string_VkColorSpaceKHR(surfaceFormats[i].colorSpace));
    //         if (ImGui::RadioButton(name.c_str(), &surfaceFormatSelected, i))
    //         {
    //             Engine::get().getConfig()["render"]["presentMode"] = (int)i;
    //         }
    //     }
    // }
// 
    // if (ImGui::CollapsingHeader("Present Modes"))
    // {
    //     static int selectedPresentMode = 0;
    //     for (size_t i = 0; i < presentModes.size(); i++)
    //     {
    //         std::string name = fmt::format("({})", string_VkPresentModeKHR(presentModes[i]));
    //         if (ImGui::RadioButton(name.c_str(), &selectedPresentMode, i))
    //         {
    //             // static int value = 0;
    //             // value = i;
    //             // _postRenderCommands.push([&](){
    //             //     m_swapchain->recreate(presentModes[value], m_swapchain->getSurfaceFormat());
    //             //     m_renderer->resize(m_swapchain->getExtent());
    //             // });
    //         }
    //     }
    // }
// 
    // ImGui::End();
// 
    // ImGui::ShowDemoWindow();
    // 
    // ImGui_ImplBluemetal_EndFrame(cmd);

    return 0;
}