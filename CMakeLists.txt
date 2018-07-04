cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(csc461)

## System libraries
##  - We assume these are installed on your system

if(APPLE)
    find_library(CORE_GRAPHICS_LIBRARY CoreGraphics)
    find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
    list(APPEND LIBRARIES ${CORE_GRAPHICS_LIBRARY} ${CORE_FOUNDATION_LIBRARY})
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    list(APPEND INCLUDE_DIRS ${GTK3_INCLUDE_DIRS})
    list(APPEND LIBRARIES ${GTK3_LIBRARIES})
endif()

# SDL2
find_package(SDL2 REQUIRED)
list(APPEND LIBRARIES ${SDL2_LIBRARIES})
list(APPEND INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})

# OpenGL
find_package(OpenGL REQUIRED)
list(APPEND LIBRARIES ${OPENGL_LIBRARIES})
list(APPEND INCLUDE_DIRS ${OPENGL_INCLUDE_DIRS})

# OpenMP
list(APPEND LIBRARIES omp)
if(APPLE)
    add_definitions("-Xpreprocessor -fopenmp")
    # HomeBrew
    list(APPEND LINK_DIRS /usr/local/opt/libomp/lib/)
    list(APPEND INCLUDE_DIRS /usr/local/opt/libomp/include)
elseif(UNIX)
    add_definitions("-fopenmp")
    set(CMAKE_EXE_LINKER_FLAGS -fopenmp)
endif()

## 3rd Party
##  - We assume you compiled these in their respective folders after cloning

# libde265
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/libde265)
list(APPEND LINK_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/libde265/libde265/)
list(APPEND LIBRARIES de265)

# openh264
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/openh264/codec/api/svc/)
list(APPEND LINK_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/openh264)
list(APPEND LIBRARIES openh264)

# vpx
list(APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/libvpx/)
list(APPEND LINK_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/libvpx/)
list(APPEND LIBRARIES vpx)

## Project files
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
        IO/ImageBGRA.cpp
        IO/ImageYUV444.cpp
        IO/ImageYUV420.cpp

        #codec
        Codec/Transcoders/DE265Transcoder.cpp
        Codec/Transcoders/Open264Transcoder.cpp
        Codec/Transcoders/VPXTranscoder.cpp
        Codec/Transcoders/VPXHelper.c
        Codec/Packets/OpenH264Packet.cpp
        Codec/Packets/VPXPacket.cpp
        )

## Black magic so we can use a source directory
set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
FUNCTION(PREPEND var prefix)
    set(listVar "")
    FOREACH(f ${ARGN})
        list(APPEND listVar "${prefix}/${f}")
    ENDFOREACH(f)
    set(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)
prepend(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${SRC_FILES})

## Targets & Includes
link_directories(${LINK_DIRS})
add_executable(csc461 ${SRC_FILES})
set_target_properties(csc461 PROPERTIES SUFFIX ".a")
include_directories(${INCLUDE_DIRS})
target_link_libraries(csc461 ${LIBRARIES})