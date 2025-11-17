cmake_minimum_required(VERSION 3.15)
project(MyApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "/opt/homebrew/opt/sfml/lib/cmake/SFML")
find_package(SFML 3 COMPONENTS Graphics Window System Audio REQUIRED)

add_executable(MyApp
    src/Main.cpp
    src/Color.cpp
    src/Interval.cpp
    src/Math.cpp
    src/Object.cpp
    src/Renderer.cpp
    src/Vec3.cpp

    external/imgui/imgui.cpp
    external/imgui-sfml/imgui-sfml.cpp
)

# Добавляем директории с заголовками
target_include_directories(MyApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    external/imgui
    external/imgui-sfml
)

target_link_libraries(MyApp PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

# Для macOS: фреймворки Cocoa и OpenGL
if(APPLE)
    find_library(COCOA Cocoa)
    find_library(OpenGL OpenGL)
    find_library(IOKIT IOKit)
    find_library(COREVIDEO CoreVideo)
    target_link_libraries(MyApp PRIVATE ${COCOA} ${OpenGL} ${IOKIT} ${COREVIDEO})
endif()
