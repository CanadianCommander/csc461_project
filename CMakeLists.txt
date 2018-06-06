cmake_minimum_required (VERSION 3.11)
project(csc461)
add_definitions(-std=gnu++14)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
add_subdirectory(nativeScreen)

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(csc461 main.cpp)
include_directories(${SDL2_INCLUDE_DIRS} ./nativeScreen/)
target_link_libraries(csc461 ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} nativeScreen)
set_target_properties(csc461 PROPERTIES SUFFIX ".a")
