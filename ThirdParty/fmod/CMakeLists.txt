
add_library(FMOD SHARED IMPORTED GLOBAL)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")

  message("FMOD On Linux ${CMAKE_CURRENT_LIST_DIR}")
  set_target_properties(
    FMOD
    PROPERTIES
      IMPORTED_LOCATION "api/core/lib/linux/x86_64/libfmod.so"
      IMPORTED_LOCATION_DEBUG "api/core/linux/x86_64/libfmodL.so"
      IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/api/core/inc/")

elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")

  set_target_properties(
    FMOD
    PROPERTIES
      IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/api/core/lib/windows/x64/fmod_vc.lib"
      IMPORTED_LOCATION "{CMAKE_CURRENT_LIST_DIR}/api/core/lib/windows/x64/fmod.dll"
      
      IMPORTED_IMPLIB_DEBUG "${CMAKE_CURRENT_LIST_DIR}/api/core/lib/windows/x64/fmodL_vc.lib"
      IMPORTED_LOCATION_DEBUG "${CMAKE_CURRENT_LIST_DIR}/api/core/lib/windows/x64/fmodL.dll"
             
      IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/api/core/inc/")
  
endif()