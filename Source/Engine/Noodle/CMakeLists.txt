list(APPEND NoodleSources
    "Source/Noodle.cpp"
    "Source/NoodleLexer.cpp"
    "Source/NoodleToken.cpp")

set(libType "STATIC") 
if (BLUEMETAL_NOODLE_BUILD_SHARED)
    set(libType "SHARED")
endif()

add_library(BLNoodle ${libType} ${NoodleSources})
add_library(bl::Noodle ALIAS BLNoodle)
target_include_directories(BLNoodle PUBLIC "Include")

# Generate the export header for exported functions and classes to use.
include(GenerateExportHeader)
generate_export_header(
    BLNoodle
    BASE_NAME "BLUEMETAL_NOODLE"
    EXPORT_MACRO_NAME "BLUEMETAL_NOODLE_API"
    DEPRECATED_MACRO_NAME "BLUEMETAL_DEPRECATED"
    EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/Include/Noodle/Export.h")

# Use C++ Standard Version 20
target_compile_features(BLNoodle PUBLIC cxx_std_20)

# Add the precompiled header
target_precompile_headers(BLNoodle PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include/Noodle/Precompiled.hpp")