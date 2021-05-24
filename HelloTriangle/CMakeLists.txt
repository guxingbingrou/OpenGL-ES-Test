add_executable(hello_triangle
    HelloTriangleTest.cpp
    HelloTriangle.cpp
    ${PROJECT_SOURCE_DIR}/GLesUtils/GlesUtil.cpp
    ${PROJECT_SOURCE_DIR}/GLesUtils/X11Util.cpp
    )

target_link_libraries(hello_triangle
    ${OPENGLES3_LIBRARY} 
    ${EGL_LIBRARY} 
    ${X11_LIBRARIES})

install(TARGETS hello_triangle
        DESTINATION bin)