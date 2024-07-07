#pragma once

#include "WindowPlatform.h"

namespace bl {

class WindowPlatformGLFW : public WindowPlatformInterface {
public:
    WindowPlatformGLFW();
    virtual ~WindowPlatformGLFW();
    virtual std::vector<const DisplayInterface*> GetDisplays() override;
    virtual std::unique_ptr<WindowInterface> CreateWindow(std::string_view name, Rect2D rect, FullscreenMode mode) override;
    virtual InputInterface* GetInput() override;`

private:
    InputGLFW _input;
};

} // namespace bl