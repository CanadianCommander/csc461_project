cmake_minimum_required(VERSION 3.10)
set(NAME AppleInterop)
project(${NAME})
set(CMAKE_C_FLAGS "-x objective-c++")

set(SRC_FILES
        AVCaptureSession.h
        AVCaptureSession.mm
        )

add_library(${NAME} SHARED ${SRC_FILES})

target_link_libraries(${NAME}
        "-framework CoreData"
        "-framework CoreFoundation"
        "-framework Foundation"
        "-framework AVFoundation"
        )