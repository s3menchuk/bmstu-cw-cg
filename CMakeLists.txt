cmake_minimum_required(VERSION 3.15)
project(GraphicsEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(SFML_DIR "/opt/homebrew/opt/sfml/lib/cmake/SFML")

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)


set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(SFML
	GIT_REPOSITORY https://github.com/SFML/SFML.git
	GIT_TAG 3.0.2)


FetchContent_Declare(imgui
	GIT_REPOSITORY https://github.com/ocornut/imgui.git
	GIT_TAG v1.91.9b)


FetchContent_Declare(imgui-sfml
	GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
	GIT_TAG v3.0)

FetchContent_MakeAvailable(SFML)
FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(imgui-sfml)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(app
    src/Main.cpp
    src/Color.cpp
    src/Interval.cpp
    src/Math.cpp
    src/Object.cpp
    src/Renderer.cpp
    src/Vec3.cpp

    #    external/imgui/imgui.cpp
    #    external/imgui-sfml/imgui-SFML.cpp
)

target_link_libraries(app PRIVATE
    SFML
    imgui
    imgui-sfml)
