#pragma once

#include "Display.h"
#include "Export.h"
#include "Graphics/Instance.h"
#include "Math/Vector2.h"
#include "Precompiled.h"
#include "VideoMode.h"

namespace bl {

struct WindowCreateInfo {
  GraphicsInstance *pInstance;
  VideoMode mode;
  std::string title = "Window";
  Display* display = nullptr;
};

class BLUEMETAL_API Window {
public:
  /** @brief Default Constructor */
  Window();

  /** @brief Create Constructor */
  Window(const WindowCreateInfo &createInfo);

  /** @brief Move Constructor */
  Window(Window &&rhs);

  /** @brief Default Destructor */
  ~Window();

  /** @brief Creates this window. */
  bool create(const WindowCreateInfo &createInfo);

  /** @brief Destroys this window. */
  void destroy();

public:
  /** @brief Gets the current window extent in pixels. */
  Extent2D getExtent();

  /** @brief Returns the Vulkan surface created with this window. */
  VkSurfaceKHR getSurface();

  /** @brief Returns the underlying SDL window object. */
  SDL_Window *getHandle();

private:
  bool createWindow(const VideoMode &videoMode, const std::string &title, Display* display);
  bool createSurface();

  WindowCreateInfo _createInfo;
  GraphicsInstance* _pInstance;
  SDL_Window* _pWindow;
  VkSurfaceKHR _surface;
};

} // namespace bl
