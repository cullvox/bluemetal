#include "DebugEditor.h"

#include "Core/FrameCounter.h"
#include "Core/Version.h"
#include "Editor.h"
#include "Engine/Engine.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/VulkanConversions.h"
#include "Graphics/RenderData.h"
#include "Graphics/VulkanWindow.h"
#include "Physics/PhysicsSystem.h"
#include "ImGui/ImGuiSystem.h"
#include "Audio/AudioSystem.h"

namespace bl {

DebugEditor::DebugEditor(Editor& editor)
    : _editor(editor)
    , _frameCounter(editor.GetEngine().GetFrameCounter())
{
    _vulkanInstanceVersion = volkGetInstanceVersion();
}

DebugEditor::~DebugEditor()
{
}

void DebugEditor::Show(bool visible)
{
    _open = visible;
}

void DebugEditor::Draw(RenderData& rd)
{
    Engine& engine = _editor.GetEngine();
    FrameCounter& counter = engine.GetFrameCounter();
    GraphicsSystem& graphics = engine.GetGraphics();
    AudioSystem* audio = engine.GetAudio();
    VulkanWindow* window = graphics.GetWindow();
    FrameCounter& physFrameCounter = engine.GetPhysics().GetPhysFrameCounter();

    if (!_open) return;

    if (!ImGui::Begin("Debug Editor", &_open)) {
        ImGui::End();
        return;
    }

    ImGui::SeparatorText("Versions");

    ImGui::Text("Bluemetal");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4 { 0.2f, 0.4f, 0.8f, 1.0f }, "%s", bl::engineVersion.ToString().c_str());

    ImGui::Text("SDL");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4 { 0.2f, 0.4f, 0.8f, 1.0f }, BL_STRINGIFY(SDL_MAJOR_VERSION) "." BL_STRINGIFY(SDL_MINOR_VERSION));

    ImGui::Text("Vulkan Header");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4 { 0.2f, 0.4f, 0.8f, 1.0f }, "%d", VK_HEADER_VERSION);

    ImGui::Text("Vulkan Version");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4 { 0.7f, 0.1f, 0.1f, 1.0f }, "%d.%d.%d", VK_VERSION_MAJOR(_vulkanInstanceVersion), VK_VERSION_MINOR(_vulkanInstanceVersion), VK_VERSION_PATCH(_vulkanInstanceVersion));

    ImGui::Text("ImGui");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4 { 0.2f, 0.4f, 0.8f, 1.0f }, "%s", ImGui::GetVersion());

    ImGui::SeparatorText("Graphics");

    ImGui::Text("Graphics Device: %s", graphics.GetPhysicalDevice()->GetDeviceName().c_str());
    ImGui::SameLine();
    ImGui::HelpMarker("Your graphics card.");
    ImGui::Text("Graphics Vendor: %s", graphics.GetPhysicalDevice()->GetVendorName().c_str());
    ImGui::Text("F/S: %d", counter.GetFramesPerSecond());
    ImGui::Text("MS/F: %.2f", counter.GetMillisecondsPerFrame());
    ImGui::Text("Average F/S (Over 10 Seconds): %.1f", counter.GetAverageFramesPerSecond(10));
    ImGui::Text("Average MS/F (Over 144 Frames): %.2f", counter.GetAverageMillisecondsPerFrame(144));
    ImGui::Text("Present Mode: %s", bl::ToString(window->GetSwapchain()->GetPresentMode()).data());
    ImGui::Text("Surface Format: %s", bl::ToString(window->GetSwapchain()->GetSurfaceFormat().format).data());
    ImGui::Text("Surface Color Space: %s", bl::ToString(window->GetSwapchain()->GetSurfaceFormat().colorSpace).data());

    ImGui::Text("Phys F/S: %d", physFrameCounter.GetFramesPerSecond());

    if (ImGui::TreeNode("Physical Devices")) {
        auto physicalDevices = graphics.GetInstance()->GetPhysicalDevices();

        for (size_t i = 0; i < physicalDevices.size(); i++) {
            auto& physicalDevice = physicalDevices[i];

            bool treeOpened = false;
            if ((treeOpened = ImGui::TreeNode((void*)(intptr_t)i, "%s", physicalDevice->GetDeviceName().c_str()))) {
                const char* deviceType = "";
                switch (physicalDevice->GetType()) {
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    deviceType = "Integrated";
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    deviceType = "Discrete";
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    deviceType = "CPU";
                    break;
                default:
                    deviceType = "Unknown";
                    break;
                }

                ImGui::SameLine();
                ImGui::TextColored({ 0.2f, 0.8f, 0.4f, 1.0f }, "%s", deviceType);

                if (ImGui::TreeNode("Present Modes")) {
                    for (VkPresentModeKHR mode : physicalDevice->GetPresentModes(window))
                        ImGui::Text("%s", bl::ToString(mode).data());

                    ImGui::TreePop();
                }

                ImGui::TreePop();
            }

            if (!treeOpened && physicalDevice == graphics.GetPhysicalDevice()) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4 { 0.2f, 0.5f, 0.8f, 1.0f }, "Current");
            }
        }

        ImGui::TreePop();
    }

    if (ImGui::CollapsingHeader("Audio")) {
        ImGui::Text("Audio Driver: %s", audio->GetDriverName().c_str());
        ImGui::Text("Num Channels: %d", audio->GetNumChannelsPlaying());
    }

    ImGui::End();
}

} // namespace bl
