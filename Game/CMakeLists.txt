list(APPEND DarkRedSources
    "Main.cpp")

add_executable(DarkRed ${DarkRedSources})
target_link_libraries(DarkRed bl::Config bl::Render bl::Input)

add_custom_command(TARGET DarkRed POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:DarkRed> $<TARGET_FILE_DIR:DarkRed>
  COMMAND_EXPAND_LISTS
  )