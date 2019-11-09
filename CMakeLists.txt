cmake_minimum_required(VERSION 3.14)
project(tetris)

set(CMAKE_CXX_STANDARD 14)
find_package(Curses REQUIRED)

add_executable(tetris main.cpp)
target_link_libraries(tetris ${CURSES_LIBRARIES})