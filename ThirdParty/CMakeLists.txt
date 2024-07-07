include(FetchContent)

set(SDL_TEST OFF)
set(SPIRV_REFLECT_EXAMPLES OFF CACHE BOOL "")
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "")

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ZLIB_USE_STATIC_LIBS ON)
set(ZLIB_INSTALL ON)

set(SPNG_SHARED OFF)

CPMAddPackage("gh:p-ranav/argparse@3.0")
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:libsdl-org/sdl#release-2.30.4")
CPMAddPackage("gh:fmtlib/fmt#10.2.1")
CPMAddPackage("gh:khronosgroup/spirv-reflect#vulkan-sdk-1.3.283.0")
CPMAddPackage("gh:nlohmann/json@3.11.3")
CPMAddPackage("gh:jimmy-park/zlib-cmake#1.3.1")
CPMAddPackage("gh:randy408/libspng@0.7.4")
CPMAddPackage("gh:auburn/fastnoise2@0.10.0-alpha")
CPMAddPackage("gh:zeux/volk#1.3.270")
CPMAddPackage("gh:gpuopen-librariesandsdks/vulkanmemoryallocator@3.1.0")
CPMAddPackage("gh:cullvox/qoixx#cmake_and_windows_fix")

add_subdirectory("fmod") # FMOD is specificly 
