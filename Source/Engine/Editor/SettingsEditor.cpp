#include "SettingsEditor.h"

#include "Audio/AudioSystem.h"
#include "Core/Profiler.h"
#include "Engine/Engine.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/VulkanConversions.h"
#include "Graphics/VulkanPhysicalDevice.h"
#include "ImGui/ImGuiSystem.h"
#include "ImGui/implot.h"
#include "Physics/PhysicsSystem.h"
#include "Physics/PhysicsRenderer.h"
#include "Core/FrameCounter.h"


namespace bl {

SettingsEditor::SettingsEditor(Engine& engine)
    : _engine(engine)
    , _audio(engine.GetAudio())
    , _graphics(_engine.GetGraphics())
    , _profiler(_engine.GetProfiler())
    , _physics(engine.GetPhysics())
    , _renderer(engine.GetRenderer())
    , _physicsRenderer(engine.GetPhysics().GetPhysicsRenderer())
    , _frameCounter(_engine.GetFrameCounter())
{
    _presentModes = _graphics.GetPhysicalDevice()->GetPresentModes(_graphics.GetWindow());
    _multisampleModes = _renderer->GetMultisampleCounts();
}

SettingsEditor::~SettingsEditor()
{
}

void SettingsEditor::SetShow(bool show)
{
    _show = show;
}

void SettingsEditor::SetCollapsed(bool collapsed)
{
    _collapsed = collapsed;
}

static const char* UserFriendlyPresentModeName(VkPresentModeKHR mode)
{
    switch (mode) {
        case VK_PRESENT_MODE_IMMEDIATE_KHR: return "Immediate (VSync Off)";
        case VK_PRESENT_MODE_FIFO_KHR: return "VSync";
        case VK_PRESENT_MODE_MAILBOX_KHR: return "Triple Buffered VSync (Recommended)";
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR: return "Relaxed VSync";
        default: return bl::ToString(mode).data();
    }
}

static const char* UserFriendlyPresentModeDescription(VkPresentModeKHR mode)
{
    switch (mode) {
        case VK_PRESENT_MODE_IMMEDIATE_KHR: return "Displays a frame as soon as it's available. The fastest experience with likely screen tearing.";
        case VK_PRESENT_MODE_FIFO_KHR: return "Displays frames in the order they were rendered and synced to your monitor. A smooth experience without screen tearing.";
        case VK_PRESENT_MODE_MAILBOX_KHR: return "Displays the latest frame synced to your monitor. A smooth and fast experience without screen tearing.";
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR: return "Displays a frame usually on the vertical sync, sometimes presents off sync if waiting too long. A smooth-ish and fast-ish experience with possible screen tearing.";
        default: return "No witty description available.";
    }
}

static const char* UserFriendlyMultisampleModeName(VkSampleCountFlagBits samples)
{
    switch (samples)
    {
    case VK_SAMPLE_COUNT_1_BIT: return "x1 (Off)";
    case VK_SAMPLE_COUNT_2_BIT: return "x2 (Low)";
    case VK_SAMPLE_COUNT_4_BIT: return "x4 (Medium)";
    case VK_SAMPLE_COUNT_8_BIT: return "x8 (High)";
    case VK_SAMPLE_COUNT_16_BIT: return "x16 (Ultra)";
    case VK_SAMPLE_COUNT_32_BIT: return "x32 (Super Duper)";
    case VK_SAMPLE_COUNT_64_BIT: return "x64 (What hardware supports this? Email me!)";
    default: return "Undefined Sample Count";
    }
}

void SettingsEditor::Draw(RenderData& renderData)
{
    if (!_show) {
        return;
    }

    ImGui::Begin("SettingsEditor", &_collapsed);

    if (ImGui::TreeNode("Editor")) {
        ImGui::Checkbox("Editor", &_enableEditor);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Renderer")) {

        ImGui::SeparatorText("Present Mode");
        for (std::size_t i = 0; i < _presentModes.size(); i++) {
            if (ImGui::RadioButton(UserFriendlyPresentModeName(_presentModes[i]), _presentModes[i] == _renderer->GetPresentMode())) {
                _renderer->SetPresentMode(_presentModes[i]);
            }
            ImGui::HelpMarker(UserFriendlyPresentModeDescription(_presentModes[i]));
        }

        ImGui::SeparatorText("Multisample Anti-aliasing (MSAA)");

        for (std::size_t i = 0; i < _multisampleModes.size(); i++) {
            if (ImGui::RadioButton(UserFriendlyMultisampleModeName(_multisampleModes[i]), _multisampleModes[i] == _renderer->GetMultisampleCount())) {
                _renderer->SetMultisampleCount(_multisampleModes[i]);
            }
            if (i != _multisampleModes.size() - 1)
                ImGui::SameLine();
        }

        ImGui::HelpMarker("Reduces jagged edges by averaging samples together. Looks good, may cause performance reduction.");

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Audio")) {
        static float masterBusVolume = 1.0f;
        if (ImGui::SliderFloat("MASTER", &masterBusVolume, 0.0f, 1.0f))
        {
            _audio->SetBusVolume(AudioBus::eMaster, masterBusVolume);
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Physics")) {
        static bool enablePhysDebugRenderer = false;
        ImGui::Checkbox("Enable Physics Debug", &enablePhysDebugRenderer);
        ImGui::HelpMarker("You may notice stuttered debug lines, caused by the physics rate not aligning with the frame. This is normal, linear interpolation smooths the movement of physics of objects.");
        _physicsRenderer->SetEnable(enablePhysDebugRenderer);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Profiler")) {


        bool enableProfiling = _profiler.IsProfilingEnabled();
        ImGui::Checkbox("Enable Profiling", &enableProfiling);
        _profiler.EnableProfiling(enableProfiling);

        static bool enableFrameLimiter = false;
        static int frameLimitMax = 144;
        if (ImGui::Checkbox("Enable FPS Limiter", &enableFrameLimiter)) {
            _frameCounter.SetFrameLimiterEnabled(enableFrameLimiter);
        }

        if (ImGui::SliderInt("Set FPS", &frameLimitMax, 1, 256)) {
            _frameCounter.SetFrameLimiterFPS(frameLimitMax);
        }

        ImGui::Text("Frame Time: %.2f ms", _frameCounter.GetDeltaTime() * 1000.0f);

        ImGui::Text("Phys Frac: %.4f", _physics.GetPhysicsInterpolationFraction());

        // Plot a frame pie chart of the profiler data
        static std::vector<float> values;
        static std::vector<const char*> labels;

        if (enableProfiling) {
            _profiler.GetProfileTimes(values);
            _profiler.GetProfileNames(labels);
        } else {
            values.clear();
            labels.clear();
        }

        if (ImPlot::BeginPlot("##Pie1", ImVec2(ImGui::GetTextLineHeight()*16,ImGui::GetTextLineHeight()*16), ImPlotFlags_Equal | ImPlotFlags_NoMouseText | ImPlotFlags_NoInputs)) {
            ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
            ImPlot::SetupAxesLimits(0, 1, 0, 1, ImPlotCond_None);

            ImPlot::PlotPieChart(labels.data(), values.data(), static_cast<int>(values.size()), 0.5, 0.5, 0.4, "%.2f", 90, ImPlotPieChartFlags_None);
            ImPlot::EndPlot();
        }

        ImGui::TreePop();
    }

    ImPlot::ShowDemoWindow();

    ImGui::End();

}



} // namespace bl