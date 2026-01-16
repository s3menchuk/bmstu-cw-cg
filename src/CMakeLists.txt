add_executable(app
    Main.cpp
    Renderer.cpp
    Object.cpp
    Color.cpp
    GUI.cpp
    Benchmark.cpp
)

if(APPLE)
    set(LIBOMP_PREFIX "/opt/homebrew/opt/libomp")
    target_compile_options(app PRIVATE -Xpreprocessor -fopenmp)
    target_include_directories(app PRIVATE "${LIBOMP_PREFIX}/include")
    target_link_directories(app PRIVATE "${LIBOMP_PREFIX}/lib")
    target_link_libraries(app PRIVATE omp)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(OpenMP REQUIRED)
    target_link_libraries(app PRIVATE OpenMP::OpenMP_CXX)
endif()

# find_package(OpenGL REQUIRED)
# target_include_directories(app PRIVATE OpenGL)
# target_link_libraries()

target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(app PRIVATE ImGui-SFML::ImGui-SFML)

if (MSVC)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/FS>)
endif()

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler path: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")