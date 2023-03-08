list(APPEND CoreSources
    "Source/Debug.cpp"
    "Source/Delegate.cpp")

add_library(BloodLustCore SHARED ${CoreSources})
add_library(bl::Core ALIAS BloodLustCore)
target_include_directories(BloodLustCore PUBLIC "Include")
target_precompile_headers(BloodLustCore PUBLIC "Include/Core/Precompiled.hpp")

# Generate the export header
include(GenerateExportHeader)

generate_export_header(
    BloodLustCore
    EXPORT_MACRO_NAME "BLOODLUST_CORE_API"
    EXPORT_FILE_NAME "include/Core/Export.h")

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(BloodLustCore PUBLIC "BLOODLUST_BUILD_STATIC")
endif()

target_include_directories(BloodLustCore PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include")

# Use C++ Standard Version 20
target_compile_features(BloodLustCore PUBLIC cxx_std_20)

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

