list(APPEND DarkRedSources
    "Main.cpp")

add_executable(DarkRed ${DarkRedSources})
target_link_libraries(DarkRed Bluemetal)

add_custom_command(TARGET DarkRed POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:DarkRed> $<TARGET_RUNTIME_DLLS:DarkRed>
  COMMAND_EXPAND_LISTS
)