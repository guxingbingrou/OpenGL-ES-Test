#ifndef _HELLO_FBO_H__
#define _HELLO_FBO_H__

#include "GLesUtils/GlesUtil.h"

class HelloFBO : public GLES_context
{
private:
    GLuint m_program = 0;
    GLuint m_fbo = 0;
    GLuint m_tex = 0;
    GLuint m_vertex_buffer = 0;
    GLuint m_compose_pos = 0;
    GLuint m_width_pos = 0;
    GLuint m_height_pos = 0;
    GLuint m_tex_pos = 0;
    struct Vertex
    {
        /* data */
        float pos[3];
        float coord[2];
    };
    
public:
    HelloFBO(/* args */);
    ~HelloFBO();

    bool Init() final;
    void Draw() final;
    void Shutdown() final;
   

};


#endif