list(APPEND AssetProcessorSources
    "Main.cpp")

add_executable(AssetProcessor ${AssetProcessorSources})
target_link_libraries(AssetProcessor Bluemetal)

add_custom_command(TARGET AssetProcessor POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:AssetProcessor> $<TARGET_RUNTIME_DLLS:AssetProcessor>
  COMMAND_EXPAND_LISTS
)