include(BloodLustLibrary)

list(APPEND MathSources
    "Source/Vector2.cpp"
    "Source/Vector3.cpp")

bl_add_library(BloodLustMath Math ${MathSources})

