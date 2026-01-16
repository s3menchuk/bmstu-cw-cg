cmake_minimum_required(VERSION 3.15)

project(glfw-opengl-test)


add_executable(app main.cpp)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

target_link_libraries(app PRIVATE OpenGL::GL glfw glad)
