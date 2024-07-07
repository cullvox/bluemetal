#pragma once

#include "Graphics/WindowInterface.h"

namespace bl {

class WindowGLFW : public WindowInterface {
public:
    WindowGLFW();
    ~WindowGLFW();
};

} // namespace bl