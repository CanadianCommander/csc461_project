cmake_minimum_required (VERSION 3.11)

if(APPLE)
  find_library(CORE_GRAPHICS_LIBRARY CoreGraphics)
  find_library(CORE_FOUNDATION_LIBRARY CoreFoundation)
elseif(UNIX)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
endif()


add_library(nativeScreen STATIC Screen.cpp TextureRGB.cpp)
include_directories(${GTK3_INCLUDE_DIRS})

if(APPLE)
  target_link_libraries(nativeScreen ${CORE_GRAPHICS_LIBRARY} ${CORE_FOUNDATION_LIBRARY})
elseif(UNIX)
  target_link_libraries(nativeScreen ${GTK3_LIBRARIES} )
endif()
