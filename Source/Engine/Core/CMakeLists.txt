include(bluemetallib)

list(APPEND CoreSources
    "Source/Debug.cpp")

bl_add_library(BLCore Core "${CoreSources}")

# Find the required libraries
find_package(fmt CONFIG REQUIRED)

target_link_libraries(BLCore PUBLIC fmt::fmt)

# Disable optimizations when debugging on GNU compilers
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(
        BloodLustCore
        PUBLIC 
            -Og)
endif()

# When using clangd for language server it requires some additional info
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()
