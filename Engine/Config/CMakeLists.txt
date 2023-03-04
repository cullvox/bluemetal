
list(APPEND ConfigSource
    "Source/Config.cpp"
    "Source/ConfigLexer.cpp"
    "Source/ConfigParser.cpp"
    "Source/ConfigToken.cpp")

add_library(BloodLustConfig SHARED ${ConfigSource})
add_library(bl::Config ALIAS BloodLustConfig)
target_include_directories(BloodLustConfig PUBLIC "Include")
target_link_libraries(BloodLustConfig bl::Core)