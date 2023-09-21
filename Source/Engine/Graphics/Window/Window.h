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
  bool create(const WindowCreateInfo &createInfo) noexcept;

  /** @brief Destroys this window. */
  void destroy() noexcept;

public:
  /** @brief Gets the current window extent in pixels. */
  Extent2D getExtent() const noexcept;

  /** @brief Returns the Vulkan surface created with this window. */
  VkSurfaceKHR getSurface() const noexcept;

  /** @brief Returns the underlying SDL window object. */
  SDL_Window *getHandle() const noexcept;

  /** @brief Changes the title displayed on the top of a windowed window. */
  void setTitle(const std::string_view& title) noexcept;

private:
  bool createWindow(const VideoMode &videoMode, const std::string &title, Display* display) noexcept;
  bool createSurface() noexcept;

  WindowCreateInfo _createInfo;
  GraphicsInstance* _pInstance;
  SDL_Window* _pWindow;
  VkSurfaceKHR _surface;
};

} // namespace bl
