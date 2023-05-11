include(BloodLustLibrary)

list(APPEND ConfigSources
    "Source/Noodle.cpp"
    "Source/NoodleLexer.cpp"
    "Source/NoodleToken.cpp")

bl_add_library(BloodLustConfig Config "${ConfigSources}")
target_link_libraries(BloodLustConfig bl::Core)