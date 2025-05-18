
#===============================================================================
# setup cmake
#===============================================================================

cmake_minimum_required(VERSION 3.24)

project(bluemetal C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules")

include(bluemetallog)

#===============================================================================
# options
#===============================================================================

option(BLUEMETAL_DEVELOPMENT ON "Enables development mode and splits modules into shared libraries.")

# spit out the current options
bl_message("This is a development build.")

if (BLUEMETAL_DEVELOPMENT)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DBLUEMETAL_DEVELOPMENT=1")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DBLUEMETAL_DEVELOPMENT=1")
endif()

#===============================================================================
# find libraries
#===============================================================================

# find Vulkan
find_package(Vulkan)

if (NOT Vulkan_FOUND)
  message(FATAL_ERROR "Vulkan SDK was not found!")
endif()

bl_message("Found Vulkan - ${Vulkan_INCLUDE_DIR}")


#===============================================================================
# build third party
#===============================================================================

# build without specific compile options

include(CMake/CPM.cmake)

include(FetchContent)

set(SDL_TEST OFF)
set(SPIRV_REFLECT_EXAMPLES OFF CACHE BOOL "")
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "")

set(BUILD_SHARED_LIBS OFF)
set(SPNG_SHARED OFF)

set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ZLIB_LIBRARIES zlibstatic)


add_subdirectory("ThirdParty/zlib")
add_subdirectory("ThirdParty/fmod") # FMOD is special


CPMAddPackage("gh:p-ranav/argparse@3.0")
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:libsdl-org/sdl#release-3.2.8")
CPMAddPackage("gh:fmtlib/fmt#10.2.1")
CPMAddPackage("gh:khronosgroup/spirv-reflect#vulkan-sdk-1.3.283.0")
CPMAddPackage("gh:nlohmann/json@3.11.3")
CPMAddPackage("gh:assimp/assimp#@5.4.3")
CPMAddPackage("gh:auburn/fastnoise2@0.10.0-alpha")
CPMAddPackage("gh:zeux/volk#1.4.304")
CPMAddPackage("gh:gpuopen-librariesandsdks/vulkanmemoryallocator@3.1.0")
CPMAddPackage("gh:cullvox/qoixx#cmake_and_windows_fix")


#===============================================================================
# add bluemetal compile options
#===============================================================================

# requires cpp20
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

# define compile platforms
message(${CMAKE_CXX_COMPILER_ID})

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_GNU")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_GNU")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_GNU")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_GNU")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_MSVC")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_MSVC")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_MSVC")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_MSVC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_APPLE_CLANG")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_APPLE_CLANG")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_APPLE_CLANG")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -DBLUEMETAL_COMPILER_APPLE_CLANG")
else()
  message(FATAL_ERROR "Compiler needs specification.")
endif()

# export compile commands for clangd on some platforms
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# enable pedantic on gcc and msvc
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

  bl_message("Enabling GNU Pedantic (-Werror -Wall -Wpedantic)")
  
  if (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror -Wall -Wpedantic")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Werror -Wall -Wpedantic")
  else()
    bl_message("Enabling GNU Optimization(-O2)")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG}")
  endif()

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

  bl_message("Disabling MSVC Permissive (/W4 /permissive)")
  bl_message("Enabling MSVC New Preprocessor (/Zc:preprocessor)") # Enables Cpp20 __VA_OPT__
  bl_message("Disabling MSVC Warning 4251 (/wd4251)") # Warnings for Shared Libraries 
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /permissive /Zc:preprocessor /wd4251")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /W4 /permissive /Zc:preprocessor /wd4251")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /W4 /permissive /Zc:preprocessor /wd4251")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} /W4 /permissive /Zc:preprocessor /wd4251")
endif()

# Any apple product requires the use of the Vulkan Portability Subset.
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  bl_message("Enabling Vulkan Portability Subset for MacOS")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DBLUEMETAL_VULKAN_PORTABILITY")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DBLUEMETAL_VULKAN_PORTABILITY")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -DBLUEMETAL_VULKAN_PORTABILITY")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -DBLUEMETAL_VULKAN_PORTABILITY")
endif()

#===============================================================================
# build source
#===============================================================================

bl_message("BUILD_TYPE - ${CMAKE_BUILD_TYPE}")
bl_message("COMPILE_FLAGS - ${CMAKE_CXX_FLAGS_DEBUG}")

add_subdirectory("Source") # some compile options are applied in this folder


