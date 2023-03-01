
# Find the libraries we need on the system.
# If we don't find them ask the user to pull recursively.


find_library (LIB_SPDLOG "spdlog")
find_library (LIB_GLFW "glfw")
find_package (GLM)


if (NOT LIB_SPDLOG)
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/spdlog")
        add_subdirectory("spdlog")
    else ()
        message (FATAL_ERROR "Could not find required library: \"spdlog\". Consider recursivly cloning the project.")
    endif ()
endif ()

if (NOT LIB_GLFW)
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/glfw")
        add_subdirectory("glfw")
    else ()
        message (FATAL_ERROR "Could not find required library: \"glfw\". Consider recursivly cloning the project.")
    endif ()
endif ()

if (NOT GLM_FOUND)
    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/glm")
        add_subdirectory("glm")
    else ()
        message (FATAL_ERROR "Could not find required library: \"glm\". Consider recursivly cloning the project.")
    endif ()
endif ()


