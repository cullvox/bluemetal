#pragma once

#include "VulkanWindow.h"
#include "VulkanViewport.h"

namespace bl {

class VulkanViewportWindow : public VulkanViewport, public VulkanWindow {
public:
    VulkanViewportWindow();
    ~VulkanViewportWindow();

};

} // namespace bl