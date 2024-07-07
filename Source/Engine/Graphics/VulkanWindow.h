#pragma once

#include "Precompiled.h"
#include "Core/NonCopyable.h"
#include "Engine/SDLInitializer.h"
#include "Display.h"
#include "VideoMode.h"
#include "VulkanSwapchain.h"

namespace bl {

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;

class VulkanWindow : public WindowInterface {
public:
    VulkanRenderWindow(VulkanDevice* device);
    ~VulkanRenderWindow();

    SDL_Window* Get() const; /** @brief Returns the underlying window handle. */
    VkExtent2D GetExtent() const; /** @brief Returns a Vulkan usable extent for swapchain. */
    VideoMode GetCurrentVideoMode() const; /** @brief Returns the current video mode. */
    void SetTitle(const std::string& title); /** @brief Changes the title displayed on the top of a windowed window. */
    void SetVideoMode(const VideoMode& mode); /** @brief Changes the windows dimensions and video mode. */
    VkSurfaceKHR GetSurface() const; /** @brief Returns the vulkan surface associated with this window. */
    VulkanMutableReference<VulkanSwapchain> GetSwapchain() const; /** @brief Returns the swapchain associated with this window. */

public:
    static void UseTemporaryWindow(const std::function<void(SDL_Window*)>& func); /** @brief Creates a temporary window that can be used for various non-display purposes. */
    static std::vector<const char*> GetSurfaceExtensions(); /** @brief Returns the surface extensions required for this device. */
    static void UseTemporarySurface(VulkanInstance* instance, const std::function<void(VkSurfaceKHR)>& func); /** @brief Creates a temporary surface for instance/device creation usage. */

private:
    VulkanDevice* _device;
    SDL_Window* _window;
    VkSurfaceKHR _surface;
    VulkanSwapchain _swapchain;
};

} // namespace bl
