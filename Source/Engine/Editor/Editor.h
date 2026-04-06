#pragma once

#include "Engine/System.h"
#include "DebugEditor.h"
#include "SettingsEditor.h"

namespace bl {

class RenderData;

class Editor : public System {
    DebugEditor _debug;
    SettingsEditor _settings;

    void DrawDebug(RenderData& rd);
    void DrawMainMenu(RenderData& rd);
    void DrawHeirarchy(RenderData& rd);

public:
    Editor(Engine& engine);
    ~Editor();

    void Draw(RenderData& rd);

};

}