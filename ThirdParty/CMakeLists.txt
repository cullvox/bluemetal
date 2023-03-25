SET(SPIRV_REFLECT_EXECUTABLE OFF CACHE BOOL "")
SET(SPIRV_REFLECT_EXAMPLES OFF CACHE BOOL "")
SET(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "")
add_subdirectory("SPIRV-Reflect")

add_subdirectory("Delegate-1.0.1")


include(FetchContent)
FetchContent_Declare(vcpkg GIT_REPOSITORY https://github.com/microsoft/vcpkg/ GIT_TAG 2022.09.27)
FetchContent_MakeAvailable(vcpkg)
set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "")


find_package(PkgConfig)