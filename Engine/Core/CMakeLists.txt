list(APPEND CoreSources
    "Source/Debug.cpp")

add_library(BloodLustCore SHARED ${CoreSources})
add_library(bl::Core ALIAS BloodLustCore)
target_include_directories(BloodLustCore PUBLIC "Include")
target_precompile_headers(BloodLustCore PUBLIC "Include/Core/Precompiled.hpp")

# Generate the export header
include(GenerateExportHeader)

generate_export_header(
    BloodLustCore
    BASE_NAME "BLOODLUST"
    EXPORT_MACRO_NAME "BLOODLUST_API"
    DEPRECATED_MACRO_NAME "BLOODLUST_DEPRECATED"
    EXPORT_FILE_NAME "include/Core/Export.h")

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(BloodLustCore PUBLIC "BLOODLUST_BUILD_STATIC")
endif()

target_include_directories(BloodLustCore PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include")

# Add debug definitions
if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    target_compile_definitions(BloodLustCore PUBLIC "BLOODLUST_DEBUG")
endif()

# Use C++ Standard Version 20
target_compile_features(BloodLustCore PUBLIC cxx_std_20)

# Find the required libraries
find_package(fmt CONFIG REQUIRED)

target_link_libraries(BloodLustCore PUBLIC fmt::fmt)

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

install(TARGETS BloodLustCore EXPORT 
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})