cmake_minimum_required (VERSION 3.10)
project(csc461)

set(CMAKE_CXX_STANDARD 11)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})

# Add files here
SET(SRC_FILES
        main.cpp
        debug.cpp debug.h
        program.cpp program.h
        graphics/graphics.h graphics/graphics.cpp graphics/_graphics.h
        graphics/graphics_resource.h
        graphics/texture2d.h graphics/texture2d.cpp
        )

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
