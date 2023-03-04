
# On windows/macos or platforms without package managers 
# use recursively cloned libraries using it.

SET(SPIRV_REFLECT_EXECUTABLE OFF CACHE BOOL "")
SET(SPIRV_REFLECT_EXAMPLES OFF CACHE BOOL "")
SET(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "")

add_subdirectory("SPIRV-Reflect")