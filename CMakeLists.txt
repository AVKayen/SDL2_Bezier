cmake_minimum_required(VERSION 3.30)
project(BezierGraphics C)

find_package(SDL2 REQUIRED)

set(CMAKE_C_STANDARD 23)

add_executable(BezierGraphics main.c)
target_link_libraries(BezierGraphics PRIVATE SDL2::SDL2)
