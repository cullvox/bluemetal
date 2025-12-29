#include "Editor.h"
#include "Graphics/RenderData.h"
#include "ImGui/ImGuiSystem.h"
#include "Engine/Engine.h"
#include "Graphics/GraphicsSystem.h"
#include "Graphics/VulkanWindow.h"
#include "Window/Window.h"

namespace bl {

Editor::Editor(Engine& engine)
    : System(engine)
    , _debug(*this)
{
}

Editor::~Editor()
{
}

void Editor::Draw(RenderData& rd)
{
    ImGui::BeginMainMenuBar();

    if (ImGui::BeginMenu("File")) {

        if (ImGui::MenuItem("Exit")) {
            GetEngine().GetGraphics().GetWindow()->RequestClose();
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
        static bool showDebug = false;
        ImGui::MenuItem("Debug Window", nullptr, &showDebug);
        _debug.Show(showDebug);

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    _debug.Draw(rd);
}

void Editor::DrawDebug(RenderData& rd)
{
    (void) rd;
}

}