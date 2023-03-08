
list(APPEND MathSources
    "Source/Vector2.cpp"
    "Source/Vector3.cpp")

add_library(BloodLustMath SHARED ${MathSources})
add_library(bl::Math ALIAS BloodLustMath)
target_include_directories(BloodLustMath PUBLIC "Include")
target_link_libraries(BloodLustMath bl::Core)

