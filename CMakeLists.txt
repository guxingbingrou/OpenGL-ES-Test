cmake_minimum_required(VERSION 3.10)
project(OpenGLES_test)

find_package(X11)
find_library( OPENGLES3_LIBRARY GLESv2 "OpenGL ES v3.0 library")
find_library( EGL_LIBRARY EGL "EGL 1.4 library" )

include_directories(${PROJECT_SOURCE_DIR})
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
add_subdirectory(HelloTriangle)
add_subdirectory(HelloFBO)

