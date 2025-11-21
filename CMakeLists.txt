cmake_minimum_required(VERSION 3.15)
project(GraphicsEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_STATIC_LIBRARIES OFF CACHE BOOL "" FORCE)

include(FetchContent)

# FetchContent_Declare(
#     SFML
#     GIT_REPOSITORY https://github.com/SFML/SFML.git
#     GIT_TAG 3.0.2
# )
# FetchContent_MakeAvailable(SFML)

# FetchContent_Declare(
#     imgui
#     GIT_REPOSITORY https://github.com/ocornut/imgui.git
#     GIT_TAG v1.91.9b
# )
# FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG v3.0
)
FetchContent_MakeAvailable(imgui-sfml)

target_link_libraries(GraphicsEngine PRIVATE
    SFML
    imgui
    imgui-sfml
)

target_include_directories(GraphicsEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(GraphicsEngine
    src/Main.cpp
    src/Renderer.cpp
    src/Object.cpp
    src/Vec3.cpp
    src/Color.cpp
    src/Math.cpp
    src/Interval.cpp
)
