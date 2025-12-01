add_executable(app
    Main.cpp
    Renderer.cpp
    Object.cpp
)

target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(app PRIVATE ImGui-SFML::ImGui-SFML)