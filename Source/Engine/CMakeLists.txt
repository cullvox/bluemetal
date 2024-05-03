set(Sources "")

list(APPEND Sources

  # Noodle
  # "Noodle/Noodle.cpp"
  # "Noodle/NoodleLexer.cpp"
  # "Noodle/NoodleToken.cpp"

  "Core/FrameCounter.cpp"
  
  # Audio
  "Audio/AudioSystem.cpp"
  "Audio/Source.cpp"
  "Audio/Listener.cpp"
  "Audio/Sound.cpp"

  "Graphics/volk.cpp"
  "Graphics/Display.cpp"
  "Graphics/Window.cpp"
  # "Graphics/RenderDoc.cpp"
  "Graphics/Instance.cpp"
  "Graphics/PhysicalDevice.cpp"
  # "Graphics/RenderWindow.cpp"
  "Graphics/Device.cpp"
  "Graphics/Buffer.cpp"
  "Graphics/Image.cpp"
  "Graphics/Shader.cpp"
  #"Graphics/ShaderReflection.cpp"
  "Graphics/DescriptorSetLayoutCache.cpp"
  #"Graphics/DescriptorSetAllocator.cpp"
  "Graphics/PipelineLayoutCache.cpp"
  #"Graphics/DescriptorAllocator.cpp"
  "Graphics/Pipeline.cpp"
  "Graphics/Swapchain.cpp"
  "Graphics/Mesh.cpp"
  "Graphics/PresentPass.cpp"
  "Graphics/Renderer.cpp"
  "Graphics/GraphicsSystem.cpp"
  # "Graphics/GeometryPass.cpp"

  "World/RenderActor.cpp"

  # ImGui
  "ImGui/imgui.cpp"
  "ImGui/imgui_demo.cpp"
  "ImGui/imgui_draw.cpp"
  "ImGui/imgui_tables.cpp"
  "ImGui/imgui_widgets.cpp"
  "ImGui/imgui_impl_sdl2.cpp"
  "ImGui/imgui_impl_vulkan.cpp"
  "ImGui/ImGuiSystem.cpp"

  "Engine/SDLInitializer.cpp"
  
  "Engine/Engine.cpp"
  )

add_library(Bluemetal STATIC ${Sources})

target_include_directories(Bluemetal PUBLIC ${Vulkan_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(Bluemetal ${Vulkan_LIBRARY} glm::glm-header-only spirv-reflect-static SDL2::SDL2 fmt::fmt FMOD)

# generate export header
include(GenerateExportHeader)

generate_export_header(
  Bluemetal
  BASE_NAME "BLUEMETAL"
  EXPORT_MACRO_NAME "BLUEMETAL_API"
  DEPRECATED_MACRO_NAME "BLUEMETAL_DEPRECATED"
  EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/Export.h")