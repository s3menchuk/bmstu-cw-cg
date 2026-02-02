cmake_minimum_required(VERSION 3.16)
project(OpenGLApp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------- GLFW ----------
include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
)

FetchContent_MakeAvailable(glfw)

# ---------- GLM ----------
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
)

FetchContent_MakeAvailable(glm)

# ---------- GLAD ----------
add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# ---------- Executable ----------
add_executable(app
    main.cpp
)

# ---------- Link ----------
target_link_libraries(app
    glfw
    glad
    glm
)

target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/include)
