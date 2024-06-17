add_library(FMOD SHARED IMPORTED GLOBAL)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GCC")
    set(FMOD_IMPORTED_LOCATION       "${CMAKE_CURRENT_LIST_DIR}/api/core/lib/linux/x86_64/libfmod.so.13.21")
    set(FMOD_IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_LIST_DIR}/api/core/lib/linux/x86_64/libfmodL.so.13.21")
  else()
    message(FATAL_ERROR "Unsupported Linux compiler for FMOD, create an issue or add it yourself!")
  endif()

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin") # Apple
  if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(FMOD_IMPORTED_LOCATION       "${CMAKE_CURRENT_LIST_DIR}/api/core/lib/macos/libfmod.dylib")
    set(FMOD_IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_LIST_DIR}/api/core/lib/macos/libfmodL.dylib")
  else()
    message(FATAL_ERROR "Unsupported Apple compiler for FMOD, create an issue or add it yourself!")
  endif()

elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(FMOD_IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/api/core/lib/windows/x64/fmod_vc.lib")
    set(FMOD_IMPORTED_LOCATION "{CMAKE_CURRENT_LIST_DIR}/api/core/lib/windows/x64/fmod.dll")
    set(FMOD_IMPORTED_IMPLIB_DEBUG "${CMAKE_CURRENT_LIST_DIR}/api/core/lib/windows/x64/fmodL_vc.lib")
    set(FMOD_IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_LIST_DIR}/api/core/lib/windows/x64/fmodL.dll")
  else()
    message(FATAL_ERROR "Unsupported Windows compiler for FMOD, create an issue or add it yourself!")
  endif()
else()
  message("Unsupported system for FMOD, create an issue or add it yourself!")
endif()

set_target_properties(
  FMOD
  PROPERTIES
    IMPORTED_IMPLIB "${FMOD_IMPORTED_IMPLIB}"
    IMPORTED_LOCATION "${FMOD_IMPORTED_LOCATION}"
    IMPORTED_IMPLIB_DEBUG "${FMOD_IMPORTED_IMPLIB_DEBUG}"
    IMPORTED_LOCATION_DEBUG "${FMOD_IMPORTED_LOCATION_DEBUG}"
    IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/api/core/inc/")