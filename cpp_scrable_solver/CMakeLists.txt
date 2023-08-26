cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3 -Wall")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXPORT_COMPILE_COMMANDS on)

project(ScrabbleSolver VERSION 0.1)
add_executable(ScrabbleSolver main.cpp)
add_subdirectory(src)

target_link_libraries(ScrabbleSolver PUBLIC Scrabble)
target_include_directories(ScrabbleSolver PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/src")
