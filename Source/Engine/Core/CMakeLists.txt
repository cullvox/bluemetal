include(bluemetallib)

list(APPEND CoreSources "Source/Log.cpp")

bl_add_library(BLCore Core "${CoreSources}")

# Find the required libraries
find_package(fmt CONFIG REQUIRED)

target_link_libraries(BLCore PUBLIC fmt::fmt)
