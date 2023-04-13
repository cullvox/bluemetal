include(BloodLustLibrary)

list(APPEND ConfigSources
    "Source/Config.cpp"
    "Source/ConfigLexer.cpp"
    "Source/ConfigParser.cpp"
    "Source/ConfigToken.cpp")

bl_add_library(BloodLustConfig Config "${ConfigSources}")
target_link_libraries(BloodLustConfig bl::Core)