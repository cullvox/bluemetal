list(APPEND CoreSources
    "Source/Debug.cpp")

add_library(BloodLustCore SHARED ${CoreSources})
add_library(bl::Core ALIAS BloodLustCore)
target_include_directories(BloodLustCore PUBLIC "Include")

# Find the required libraries
pkg_check_modules(SPDLOG REQUIRED IMPORTED_TARGET spdlog)
pkg_check_modules(FMT REQUIRED IMPORTED_TARGET fmt)

target_include_directories(BloodLustCore PUBLIC ${FMT_INCLUDE_DIR} ${SPDLOG_INCLUDE_DIR})
target_link_libraries(BloodLustCore ${FMT_LIBRARIES} ${SPDLOG_LIBRARIES})
target_compile_definitions(BloodLustCore PUBLIC SPDLOG_FMT_EXTERNAL)

set_target_properties(
    BloodLustCore
    PROPERTIES
        CXX_STANDARD 20)

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

