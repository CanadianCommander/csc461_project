cmake_minimum_required (VERSION 3.10)
project(csc461)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add files here
SET(SRC_FILES main.cpp)

# Prepend the /src directory to each file
SET(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
FUNCTION(PREPEND var prefix)
    SET(listVar "")
    FOREACH(f ${ARGN})
        LIST(APPEND listVar "${prefix}/${f}")
    ENDFOREACH(f)
    SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)
PREPEND(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${SRC_FILES})

add_executable(csc461 ${SRC_FILES})
target_link_libraries(csc461 ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
