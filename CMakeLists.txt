cmake_minimum_required (VERSION 3.11)
project(csc461)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(csc461 src/main.cpp)
target_link_libraries(csc461 ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
