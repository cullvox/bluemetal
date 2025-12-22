#pragma once

#include "Engine/System.h"
#include "DebugEditor.h"

namespace bl {

class RenderData;

class Editor : public System {
    DebugEditor _debug;

    void DrawDebug(RenderData& rd);
    void DrawMainMenu(RenderData& rd);
    void DrawHeirarchy(RenderData& rd);

public:
    Editor(Engine& engine);
    ~Editor();

    void Draw(RenderData& rd);

};

}