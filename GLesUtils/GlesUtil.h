#ifndef _GLES_UTIL_H__
#define _GLES_UTIL_H__

#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <EGL/eglext.h>
#include <memory>
#include <string>
#include <iostream>
class GLES_context
{
public:
    GLES_context();
    ~GLES_context();
    GLint m_width = 0;
    GLint m_height = 0;
    EGLNativeWindowType m_egl_native_window = 0;
    EGLNativeDisplayType m_egl_native_display;
    EGLDisplay m_egl_display;
    EGLContext m_context;
    EGLSurface m_window_surface;
    EGLSurface m_pb_surface;

    GLuint LoadShader ( GLenum type, const char *shaderSrc );
    virtual void Draw() = 0;
    virtual void Shutdown() = 0;
    virtual bool Init() = 0;

};

class GlesUtil
{
public:
    GlesUtil(/* args */);
    ~GlesUtil();

    bool InitContext(std::shared_ptr<GLES_context>& gles_context, GLint width, GLint height, std::string title);
    virtual bool CreateWindow(std::string title) = 0;
    void MainLoop();
    virtual bool UserInterrupt() = 0;

protected:
    std::shared_ptr<GLES_context> m_gles_context;
};









#endif