#pragma once

#include "Engine/System.h"
#include "DebugEditor.h"

namespace bl {

struct VulkanRenderData;

class Editor : public System {
    DebugEditor _debug;

    void DrawDebug(VulkanRenderData& rd);
    void DrawMainMenu(VulkanRenderData& rd);
    void DrawHeirarchy(VulkanRenderData& rd);

public:
    Editor(Engine& engine);
    ~Editor();

    void Draw(VulkanRenderData& rd);

};

}