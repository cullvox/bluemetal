#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "imgui/imgui.h"
#include "imgui/imgui_impl_bluemetal.h"
#include "Export.h"

namespace bl
{

///////////////////////////////
// Forward Declarations
///////////////////////////////

class Engine;

///////////////////////////////
// Classes
///////////////////////////////

class ImGuiSubsystem
{
public:
    ImGuiSubsystem(Engine* pEngine);
    ~ImGuiSubsystem();

    void init();
    void shutdown();

private:
    Engine* m_pEngine;
};

} // namespace bl