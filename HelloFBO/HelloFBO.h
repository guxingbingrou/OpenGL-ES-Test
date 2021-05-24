#ifndef _HELLO_TRIANGLE_H__
#define _HELLO_TRIANGLE_H__

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
    uint8_t* m_rgb_data = nullptr;
    uint8_t* m_yuv_data = nullptr;
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

    void RGB2YUV(uint8_t* input);

    

};


#endif