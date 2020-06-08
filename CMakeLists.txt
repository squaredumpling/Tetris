cmake_minimum_required(VERSION 3.14)
project(tetris)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
find_package(SDL2 REQUIRED)
find_package(SDL2TTF REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${SDL2TTF_INCLUDE_DIR})

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY})
configure_file("Andale Mono.ttf" "Andale.ttf" COPYONLY)