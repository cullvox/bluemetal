include(bluemetallib)

list(APPEND CoreSources
    "Source/Debug.cpp")

bl_add_library(BLCore Core "${CoreSources}")

# Find the required libraries
find_package(fmt CONFIG REQUIRED)

target_link_libraries(BLCore PUBLIC fmt::fmt)

# When using clangd for language server it requires some additional info
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#if (CMAKE_EXPORT_COMPILE_COMMANDS)
#  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
#endif()
