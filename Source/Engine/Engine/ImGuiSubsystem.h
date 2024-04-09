#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "imgui/imgui.h"

#include "Export.h"
#include "Graphics/Sdl.h"
#include "Graphics/Vulkan.h"

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
    ImGuiSubsystem(Engine& engine);
    ~ImGuiSubsystem();

    void process(const SDL_Event& event);
    void beginFrame();
    void endFrame(VkCommandBuffer cmd);

private:
    void init();
    void unload();

    Engine& _engine;
    VkDescriptorPool _descriptorPool;
};

} // namespace bl