add_executable(hello_fbo
    HelloFBOTest.cpp
    HelloFBO.cpp
    ${PROJECT_SOURCE_DIR}/GLesUtils/GlesUtil.cpp
    ${PROJECT_SOURCE_DIR}/GLesUtils/X11Util.cpp
    )

target_link_libraries(hello_fbo
    ${OPENGLES3_LIBRARY} 
    ${EGL_LIBRARY} 
    ${X11_LIBRARIES})

install(TARGETS hello_fbo
        DESTINATION bin)