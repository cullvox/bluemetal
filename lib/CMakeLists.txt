
# On windows/macos or platforms without package managers 
# use recursively cloned libraries using it.

set(SPIRV_REFLECT_EXECUTABLE OFF)
set(SPIRV_REFLECT_EXAMPLES OFF)
set(SPIRV_REFLECT_STATIC_LIB ON)
add_subdirectory("SPIRV-Reflect")

if (DARKRED_USE_PACKAGE_MANAGER)
    message("Using package manager")
    find_package(glfw3)
    find_package(glm)
    find_package(spdlog)
else()
    add_subdirectory("spdlog") # spdlog v1.10.0+
    add_subdirectory("glfw") # glfw 3.4+
    add_subdirectory("glm") # glm 9.9.9.8+ (go to 1.0 already...)
endif()
