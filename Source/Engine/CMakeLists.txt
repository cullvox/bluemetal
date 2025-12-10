set(Sources "")

list(APPEND Sources

  # Core
  "Core/FrameCounter.cpp"
  "Core/Print.cpp"
  
  # Audio
  "Audio/AudioSystem.cpp"

  "Graphics/VulkanWindow.cpp"
  "Graphics/VulkanConversions.cpp"
  "Graphics/VulkanInstance.cpp"
  "Graphics/VulkanPhysicalDevice.cpp"
  "Graphics/VulkanDevice.cpp"
  "Graphics/VulkanBuffer.cpp"
  "Graphics/VulkanImage.cpp"
  "Graphics/VulkanSampler.cpp"
  "Graphics/VulkanShader.cpp"
  "Graphics/VulkanReflectedBlock.cpp"
  "Graphics/VulkanReflectedDescriptorSet.cpp"
  "Graphics/VulkanDescriptorSetLayoutCache.cpp"
  "Graphics/VulkanReflectedPushConstant.cpp"
  "Graphics/VulkanReflectedPipeline.cpp"
  "Graphics/VulkanPipelineLayoutCache.cpp"
  "Graphics/VulkanPipeline.cpp"
  "Graphics/VulkanDescriptorSetAllocatorCache.cpp"
  "Graphics/VulkanSwapchain.cpp"
  "Graphics/VulkanMaterial.cpp"
  "Graphics/VulkanMaterialInstance.cpp"
  "Graphics/Renderer.cpp"
  "Graphics/GraphicsSystem.cpp"

  "Window/Display.cpp"
  "Window/Window.cpp"
  "Window/Input.cpp"
  "Window/Keyboard.cpp"
  "Window/Mouse.cpp"

  # ImGui
  "ImGui/imgui.cpp"
  "ImGui/imgui_demo.cpp"
  "ImGui/imgui_draw.cpp"
  "ImGui/imgui_tables.cpp"
  "ImGui/imgui_widgets.cpp"
  "ImGui/imgui_impl_sdl3.cpp"
  "ImGui/imgui_impl_vulkan.cpp"
  "ImGui/ImGuiSystem.cpp"

  "Physics/PhysicsSystem.cpp"
  "Physics/BroadPhaseLayerImpl.cpp"
  "Physics/ObjectLayerPairFilterImpl.cpp"
  "Physics/ObjectVsBroadPhaseLayerFilterImpl.cpp"



  "Resources/ResourceSystem.cpp"
  "Resources/Resource.cpp"
  "Resources/Sampler.cpp"
  "Resources/Sound.cpp"
  "Resources/Shader.cpp"
  "Resources/Material.cpp"
  "Resources/Texture.cpp"
  "Resources/Texture2D.cpp"
  "Resources/MaterialInstance.cpp"
  "Resources/Mesh.cpp"
  "Resources/Model.cpp"

  "Scene/Node.cpp"
  "Scene/Node3D.cpp"
  "Scene/AudioSource3D.cpp"
  "Scene/MeshInstance3D.cpp"
  "Scene/Camera3D.cpp"
  "Scene/PhysicsBody3D.cpp"

  "Editor/DebugEditor.cpp"
  "Editor/Editor.cpp"

  "Engine/SDL.cpp"
  "Engine/System.cpp"
  
  "Engine/Engine.cpp"
  )

add_library(Bluemetal STATIC ${Sources})

set_property(TARGET Bluemetal PROPERTY CXX_STANDARD 20)

target_include_directories(Bluemetal PUBLIC ${Vulkan_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(Bluemetal PUBLIC 
  glm::glm-header-only
  spirv-reflect-static
  argparse
  SDL3::SDL3 
  fmt::fmt
  FMOD
  volk
  GPUOpen::VulkanMemoryAllocator
  nlohmann_json
  qoixx::qoixx
  tinygltf
  Jolt::Jolt)
