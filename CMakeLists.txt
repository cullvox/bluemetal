
#===============================================================================
# setup cmake
#===============================================================================

cmake_minimum_required(VERSION 3.24)

project(bluemetal)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules")

include(bluemetallog)

#===============================================================================
# options
#===============================================================================


option(BLUEMETAL_DEVELOPMENT ON "Enables development mode and splits modules into shared libraries.")


# spit out the current options
bl_message("BLUEMETAL_DEVELOPMENT = ${BLUEMETAL_DEVELOPMENT}")

#===============================================================================
# find libraries
#===============================================================================


# find SDL2
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/ThirdParty/SDL2/CMakeLists.txt")
  add_subdirectory("ThirdParty/SDL2")
  bl_message("Using SDL2 as a recursive submodule, ensure you have the proper libraries installed or SDL might throw at runtime.")
else()
  find_package(SDL2 CONFIG REQUIRED)
  bl_message("Found SDL2 - ${SDL2_INCLUDE_DIR}")
endif()

# find Vulkan
find_package(Vulkan REQUIRED)
bl_message("Found Vulkan - ${Vulkan_INCLUDE_DIR}")


#===============================================================================
# build third party
#===============================================================================

# build without specific compile options
add_subdirectory("ThirdParty")

#===============================================================================
# compile options
#===============================================================================

# export compile commands for clangd on some platforms
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# be a little pedantic
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

  bl_message("Enabling GNU Pedantic (-Werror -Wall -Wpedantic)")
  
  if (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror -Wall -Wpedantic")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Werror -Wall -Wpedantic")
  else()
    bl_message("Enabling GNU Optimization(-O2)")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -Werror -Wall -Wpedantic -O2")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -Werror -Wall -Wpedantic -O2")
  endif()

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

  bl_message("Disabling MSVC Permissive ( /W4 /Wall /permissive)")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /Wall /permissive")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /W4 /Wall /permissive")
endif()

#===============================================================================
# build source
#===============================================================================

add_subdirectory("Source") # some compile options are applied in this folder


