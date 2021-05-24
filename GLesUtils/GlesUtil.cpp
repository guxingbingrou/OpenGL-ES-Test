#include "GlesUtil.h"
GLES_context::GLES_context(){

}

GLES_context::~GLES_context(){

}
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint GLES_context::LoadShader ( GLenum type, const char *shaderSrc )
{
    GLuint shader;
    GLint compiled;

   // Create the shader object
    shader = glCreateShader ( type );

    if ( shader == 0 )
    {
        return 0;
    }

   // Load the shader source
    glShaderSource ( shader, 1, &shaderSrc, NULL );

   // Compile the shader
    glCompileShader ( shader );

   // Check the compile status
    glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

    if ( !compiled ){
        GLint infoLen = 0;

        glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

        if ( infoLen > 1 ){
            char *infoLog = (char*)malloc ( sizeof ( char ) * infoLen );

            glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
            std::cout << "can not compile shader: " << infoLog << std::endl;

            free ( infoLog );
        }

        glDeleteShader ( shader );
        return 0;
    }

    return shader;

}

GlesUtil::GlesUtil(){
    
}

GlesUtil::~GlesUtil(){

}
bool GlesUtil::InitContext(std::shared_ptr<GLES_context>& gles_context, GLint width, GLint height, std::string title){
    EGLint major_verison, minor_version;
    m_gles_context = gles_context;
    m_gles_context->m_width = width;
    m_gles_context->m_height = height;

    if(!CreateWindow(title)){
        std::cout << "can not create native window\n";
        return false;
    }
    m_gles_context->m_egl_display = eglGetDisplay(m_gles_context->m_egl_native_display);

    if(m_gles_context->m_egl_display == EGL_NO_DISPLAY){
        std::cout << "egl can not get display \n";
        return false;
    }
    if(!eglInitialize(m_gles_context->m_egl_display, &major_verison, &minor_version)){
        std::cout << "get can not initialized \n";
        return false;
    }

    EGLint num_configs = 0;
    EGLConfig config;
    EGLint attribute_list[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8 ,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };
    if(!eglChooseConfig(m_gles_context->m_egl_display, attribute_list, &config, 1, &num_configs)){
        std::cout << "egl can not choose suitable config \n";
        return false;
    }

    if(num_configs < 1){
        std::cout << "egl get configs num less than 1 \n";
        return false;
    }

    m_gles_context->m_window_surface = eglCreateWindowSurface ( m_gles_context->m_egl_display, config, 
                                                    m_gles_context->m_egl_native_window, NULL );

    if ( m_gles_context->m_window_surface == EGL_NO_SURFACE )
    {
        std::cout << "egl can not create window surface\n";
        return false;
    }

    EGLint context_attibute[] = {EGL_CONTEXT_MAJOR_VERSION, 2,
                                 EGL_CONTEXT_MINOR_VERSION, 0,
                                 EGL_NONE };

    EGLContext context = eglCreateContext(m_gles_context->m_egl_display, config, EGL_NO_CONTEXT, context_attibute);
    if(context == EGL_NO_CONTEXT){
        std::cout << "egt can not create context \n";
        return false;
    }

    m_gles_context->m_context = context;

    // EGLint pb_attribute[] = {EGL_WIDTH, width,
    //                          EGL_HEIGHT, height,
    //                          EGL_NONE};
    // EGLSurface pbuf_surface = eglCreatePbufferSurface(display,config,pb_attribute);
    // if(pbuf_surface == EGL_NO_SURFACE){
    //     std::cout << "egl can not create pbuffer surface\n";
    //     return false;
    // }

    // m_gles_context->m_pb_surface = pbuf_surface;


   // Make the context current
    if (!eglMakeCurrent(m_gles_context->m_egl_display, m_gles_context->m_window_surface, 
                          m_gles_context->m_window_surface, m_gles_context->m_context)){
        std::cout << "can not make current context\n";
        return false;
    }


    if(!m_gles_context->Init()){
        std::cout << "can not initialize context\n";
        return false;
    }

    return true;
}

void GlesUtil::MainLoop(){
    while(!UserInterrupt()){
        m_gles_context->Draw();
        eglSwapBuffers(m_gles_context->m_egl_display, m_gles_context->m_window_surface);
    }

    m_gles_context->Shutdown();
}