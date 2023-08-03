
add_library(FMOD STATIC IMPORTED GLOBAL)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")

  message("FMOD On Linux ${CMAKE_CURRENT_LIST_DIR}")
  set_target_properties(
    FMOD
    PROPERTIES
      IMPORTED_LOCATION "api/core/lib/libfmod.so"
      IMPORTED_LOCATION_DEBUG "api/core/libfmodL.so"
      IMPORTED_CONFIGURATIONS "RELEASE;DEBUG"
      INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/api/core/inc/")

else()

  message(ERROR "Requires import!")

endif()