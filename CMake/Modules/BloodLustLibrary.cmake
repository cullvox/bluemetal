
function(bl_add_library lib_name lib_short_name lib_sources)

	
    # Add the actual CMake library. 

    add_library(${lib_name} SHARED ${lib_sources})
    add_library(bl::${lib_short_name} ALIAS ${lib_name})
    target_include_directories(${lib_name} PUBLIC "Include")


    # Generate the export header for exported functions and classes to use.

    string(TOUPPER ${lib_short_name} lib_short_name_upper)

    include(GenerateExportHeader)
    generate_export_header(
        ${lib_name}
        BASE_NAME "BLOODLUST_${lib_short_name_upper}"
        EXPORT_MACRO_NAME "BLOODLUST_${lib_short_name_upper}_API"
        DEPRECATED_MACRO_NAME "BLOODLUST_DEPRECATED"
        EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/Include/${lib_short_name}/Export.h")

    if(NOT BUILD_SHARED_LIBS)
      target_compile_definitions(${lib_name} PRIVATE "BLOODLUST_BUILD_STATIC")
    endif()

    # Add debug definitions
    if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
        target_compile_definitions(${lib_name} PRIVATE "BLOODLUST_DEBUG")
    endif()

    # Use C++ Standard Version 20
    target_compile_features(${lib_name} PUBLIC cxx_std_20)


endfunction()