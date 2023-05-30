include(bluemetallib)

list(APPEND MathSources
    "Source/Vector2.cpp"
    "Source/Vector3.cpp")

bl_add_library(BLMath Math "${MathSources}")
target_link_libraries(BLMath bl::Core)

