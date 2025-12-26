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
endif()

# find_package(OpenMP REQUIRED)

if(WINDOWS AND OpenMP_CXX_FOUND)
    target_link_libraries(app PRIVATE OpenMP::OpenMP_CXX)
    message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
endif()

target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(app PRIVATE ImGui-SFML::ImGui-SFML)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler path: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")