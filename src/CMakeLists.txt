add_executable(app
    Main.cpp
    Renderer.cpp
    Object.cpp
    Color.cpp
    GUI.cpp
)

# if(APPLE)
#     set(CMAKE_C_COMPILER /opt/homebrew/opt/llvm/bin/clang)
#     set(CMAKE_CXX_COMPILER /opt/homebrew/opt/llvm/bin/clang++)

#     target_include_directories(app PRIVATE /opt/homebrew/opt/libomp/include)
#     target_link_directories(app PRIVATE /opt/homebrew/opt/libomp/lib)

#     target_compile_options(app PRIVATE -Xpreprocessor -fopenmp)
#     target_link_libraries(app PRIVATE omp)
# endif()

# target_compile_options(app PRIVATE -stdlib=libc++)
# target_link_libraries(app PRIVATE c++)

find_package(OpenMP REQUIRED)

if(OpenMP_CXX_FOUND)
    target_link_libraries(app PRIVATE OpenMP::OpenMP_CXX)
    message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
endif()

target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(app PRIVATE ImGui-SFML::ImGui-SFML)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler path: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
