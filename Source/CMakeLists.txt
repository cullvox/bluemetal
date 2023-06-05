
# When using clangd for language server it requires some additional info
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES 
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

if (BLUEMETAL_DEVELOPMENT)
    # Add -O0 to remove optimizations when using gcc
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message("Disabling GNU Optimizations (-O0)")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
        set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
    endif()
endif()

add_subdirectory("Engine")
add_subdirectory("Game")