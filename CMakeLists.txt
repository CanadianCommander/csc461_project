cmake_minimum_required (VERSION 3.10)
project(csc461)

if(APPLE)
    find_library(CORE_GRAPHICS_LIBRARY CoreGraphics)
    find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    include_directories(${GTK3_INCLUDE_DIRS})
endif()

set(CMAKE_CXX_STANDARD 14)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})

# Add files here
SET(SRC_FILES

        #main
        main.cpp
        Debug.cpp
        Program.cpp

        #graphics
        Graphics/Graphics.cpp
        Graphics/Texture.cpp

        #io
        IO/ScreenCapture.cpp
        IO/ImageRGB.cpp
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
set_target_properties(csc461 PROPERTIES SUFFIX ".a")
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

SET(LIBRARIES ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
if(APPLE)
    SET(LIBRARIES ${LIBRARIES} ${CORE_GRAPHICS_LIBRARY} ${CORE_FOUNDATION_LIBRARY})
elseif(UNIX)
    SET(LIBRARIES ${LIBRARIES} ${GTK3_LIBRARIES})
endif()
target_link_libraries(csc461 ${LIBRARIES})
