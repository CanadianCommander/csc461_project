cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(csc461)
add_definitions(-std=gnu++14 -Ofast)

if(APPLE)
    find_library(CORE_GRAPHICS_LIBRARY CoreGraphics)
    find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    include_directories(${GTK3_INCLUDE_DIRS})
endif()

find_package(SDL2 REQUIRED)
list(APPEND LIBRARIES ${SDL2_LIBRARIES})
list(APPEND INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
list(APPEND LIBRARIES ${OPENGL_LIBRARIES})
list(APPEND INCLUDE_DIRS ${OPENGL_INCLUDE_DIRS})

# OpenMP
if(APPLE)
    # Use HomeBrew libomp
    add_definitions("-Xpreprocessor -fopenmp")
    list(APPEND LIBRARIES "/usr/local/opt/libomp/lib/libomp.dylib")
    list(APPEND INCLUDE_DIRS "/usr/local/opt/libomp/include")
elseif(UNIX)
    add_definitions("-fopenmp")
    list(APPEND LIBRARIES omp)
endif()

# libde265
list(APPEND INCLUDE_DIRS ./3rdParty/libde265)
list(APPEND LINK_DIRS ./3rdParty/libde265/libde265/)
list(APPEND LIBRARIES de265)

# openg264
list(APPEND INCLUDE_DIRS ./3rdParty/openh264/codec/api/svc/)
list(APPEND LINK_DIRS ./3rdParty/openh264)
list(APPEND LIBRARIES openh264)

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
        IO/ImageBGRA.cpp

        #codec
        Codec/Codec.cpp
        Codec/Transcoders/DE265Transcoder.cpp
        Codec/Transcoders/Open264Transcoder.cpp
        Codec/Packets/OpenH264Packet.cpp
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

link_directories(${LINK_DIRS})
add_executable(csc461 ${SRC_FILES})
set_target_properties(csc461 PROPERTIES SUFFIX ".a")
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

list(APPEND LIBRARIES ${ENCODER_LIBRARIES})
if(APPLE)
    list(APPEND LIBRARIES ${CORE_GRAPHICS_LIBRARY} ${CORE_FOUNDATION_LIBRARY})
elseif(UNIX)
    list(APPEND LIBRARIES ${GTK3_LIBRARIES})
endif()

include_directories(${INCLUDE_DIRS})
target_link_libraries(csc461 ${LIBRARIES})
