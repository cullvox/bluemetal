list(APPEND EngineSources
    "Source/Engine.cpp")

add_library(BloodLustEngine SHARED ${EngineSources})
add_library(bl::Engine ALIAS BloodLustEngine)
target_include_directories(BloodLustEngine PUBLIC "Include")
target_link_libraries(BloodLustEngine bl::Core bl::Math bl::Window bl::Render bl::Input)


install(TARGETS BloodLustEngine EXPORT 
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})