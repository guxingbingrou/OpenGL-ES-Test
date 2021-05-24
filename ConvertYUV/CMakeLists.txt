add_executable(convertYUV
    ConverYUVTest.cpp
    ConverYUV.cpp
    ${PROJECT_SOURCE_DIR}/GLesUtils/GlesUtil.cpp
    ${PROJECT_SOURCE_DIR}/GLesUtils/X11Util.cpp
    )

target_link_libraries(convertYUV
    ${OPENGLES3_LIBRARY} 
    ${EGL_LIBRARY} 
    ${X11_LIBRARIES})

install(TARGETS convertYUV
        DESTINATION bin)