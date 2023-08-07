list(APPEND DarkRedSources
    "Main.cpp")

add_executable(DarkRed ${DarkRedSources})
target_link_libraries(DarkRed Bluemetal)