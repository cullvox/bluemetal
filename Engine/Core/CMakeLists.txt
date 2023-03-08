list(APPEND CoreSources
    "Source/Debug.cpp"
    "Source/Delegate.cpp")

add_library(BloodLustCore SHARED ${CoreSources})
add_library(bl::Core ALIAS BloodLustCore)
target_include_directories(BloodLustCore PUBLIC "Include")
target_precompile_headers(BloodLustCore PUBLIC "Include/Core/Precompiled.hpp")

set_target_properties(
    BloodLustCore
    PROPERTIES
        CXX_STANDARD 20)

# Find the required libraries
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

target_link_libraries(BloodLustCore PUBLIC fmt::fmt spdlog::spdlog)
target_compile_definitions(BloodLustCore PUBLIC "SPDLOG_FMT_EXTERNAL")



# Disable optimizations when debugging on GNU compilers.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(
        BloodLustCore
        PUBLIC 
            -Og)
endif()

# When using clangd for language server it requires some additional info.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

