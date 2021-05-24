#ifndef _HELLO_TRIANGLE_H__
#define _HELLO_TRIANGLE_H__

#include "GLesUtils/GlesUtil.h"

class HelloTriangle : public GLES_context
{
private:
    GLuint m_program = 0;
    uint8_t* m_rgb_data = nullptr;
    uint8_t* m_yuv_data = nullptr;
public:
    HelloTriangle(/* args */);
    ~HelloTriangle();

    bool Init() final;
    void Draw() final;
    void Shutdown() final;

    void RGB2YUV(uint8_t* input);

    

};


#endif