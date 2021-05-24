#include "HelloFBO.h"
#include <stdio.h>
HelloFBO::HelloFBO(/* args */)
{
}

HelloFBO::~HelloFBO()
{
    if(m_rgb_data){
        free(m_rgb_data);
        m_rgb_data = nullptr;
    }
    if(m_yuv_data){
        free(m_yuv_data);
        m_yuv_data = nullptr;
    }
}

bool HelloFBO::Init(){

    Vertex vVertices[] = {  {{1.0, -1.0, 0.0}, {1.0, 0.0}},
                            {{1.0, 1.0, 0.0}, {1.0, 1.0}},
                            {{-1.0, 1.0, 0.0}, {0.0, 1.0}},
                            {{-1.0, -1.0, 0.0}, {0.0, 0.0}}
                         };
    
    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);



    glGenFramebuffers(1, &m_fbo);
    if(m_fbo == 0){
        std::cout << "glGenFrameBuffer failed\n";
        return false;
    }

    glGenTextures(1, &m_tex);
    if(m_tex == 0){
        std::cout << "glGenTextures failed\n";
        return false;
    }    
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);
    GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE){
        std::cout << "gl framebuffer status error: " << status << std::endl;
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    char vShaderStr[] =
        "#version 300 es                          \n"
        "layout(location = 0) in vec4 vPosition;  \n"
        "layout(location = 1) in vec2 inCoord;\n"
        "out vec2 vTextureCo;\n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = vPosition;              \n"
        "   vTextureCo = inCoord;                   \n"
        "}                                        \n";
#if 0
    char fShaderStr[] =
        "#version 300 es                                  \n"
        "precision mediump float;                         \n"
        "in lowp vec2 vTextureCo;                         \n"
        "uniform sampler2D uTexture;                           \n"
        "out vec4 fragColor;                              \n"
        "uniform int composeMode;                         \n"
        "void main()                                      \n"
        "{                                                \n"
        "   if(composeMode == 2){                         \n"
        "       fragColor = texture2D(uTexture, vTextureCo);   \n"
        "   } else if(composeMode == 0){                  \n"
        "       fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
        "   }else{                                        \n"
        "       fragColor = vec4 ( 0.0, 1.0, 0.0, 1.0 );  \n"
        "   }                                             \n"
        "}                                                \n";
#else
    char fShaderStr[] =
        "#version 300 es                                  \n"
        "precision highp float;                           \n"
        "precision highp int;                           \n"
        "in vec2 vTextureCo;                           \n"
        "uniform sampler2D uTexture;                           \n"
        "out vec4 fragColor;                              \n"
        "uniform int composeMode;                         \n"
        "uniform float uWidth;                            \n"
        "uniform float uHeight;                           \n"
        "float cY(float x,float y){                       \n"
        "    vec4 c=texture2D(uTexture,vec2(x,y));        \n"
        "    return c.r*0.257+c.g*0.504+c.b*0.098+0.0625; \n"
        "}                                                \n"
        "vec4 cC(float x,float y,float dx,float dy){      \n"
        "    vec4 c0=texture2D(uTexture,vec2(x,y));       \n"
        "    vec4 c1=texture2D(uTexture,vec2(x+dx,y));    \n"
        "    vec4 c2=texture2D(uTexture,vec2(x,y+dy));    \n"
        "    vec4 c3=texture2D(uTexture,vec2(x+dx,y+dy)); \n"
        "    return (c0+c1+c2+c3)/4.;                     \n"
        "}                                                \n"  
        "float cU(float x,float y,float dx,float dy){     \n"
        "    vec4 c=cC(x,y,dx,dy);                        \n"
        "    return -0.148*c.r - 0.291*c.g + 0.439*c.b+0.5000;\n"
        "}                                                \n"
        "                                                 \n"
        "float cV(float x,float y,float dx,float dy){     \n"
        "    vec4 c=cC(x,y,dx,dy);                        \n"
        "    return 0.439*c.r - 0.368*c.g - 0.071*c.b+0.5000;\n"
        "}                                                \n"
        "                                                 \n"
        "vec2 cPos(float t,float shiftx,float gy){        \n"
        "    vec2 pos=vec2(floor(uWidth*vTextureCo.x),floor(uHeight*gy));\n"
        "    return vec2(mod(pos.x*shiftx,uWidth),(pos.y*shiftx+floor(pos.x*shiftx/uWidth))*t);\n"
        "}                                                \n"
        "                                                 \n"
        "vec4 calculateY(){                               \n"
        "    vec2 pos=cPos(1.,4.,vTextureCo.y);           \n"
        "    vec4 oColor=vec4(0);                         \n"
        "    float textureYPos=pos.y/uHeight;             \n"
        "    oColor[0]=cY(pos.x/uWidth,textureYPos);      \n"
        "    oColor[1]=cY((pos.x+1.)/uWidth,textureYPos); \n"
        "    oColor[2]=cY((pos.x+2.)/uWidth,textureYPos); \n"
        "    oColor[3]=cY((pos.x+3.)/uWidth,textureYPos); \n"
        "    return oColor;                               \n"
        "}                                                \n"
        "vec4 calculateU(float gy,float dx,float dy){     \n"
        "    vec2 pos=cPos(2.,8.,vTextureCo.y-gy);        \n"
        "    vec4 oColor=vec4(0);                         \n"
        "    float textureYPos=pos.y/uHeight;             \n"
        "    oColor[0]= cU(pos.x/uWidth,textureYPos,dx,dy);\n"
        "    oColor[1]= cU((pos.x+2.)/uWidth,textureYPos,dx,dy);\n"
        "    oColor[2]= cU((pos.x+4.)/uWidth,textureYPos,dx,dy);\n"
        "    oColor[3]= cU((pos.x+6.)/uWidth,textureYPos,dx,dy);\n"
        "    return oColor;\n"
        "}\n"
        "vec4 calculateV(float gy,float dx,float dy){     \n"
        "    vec2 pos=cPos(2.,8.,vTextureCo.y-gy);        \n"
        "    vec4 oColor=vec4(0);                         \n"
        "    float textureYPos=pos.y/uHeight;             \n"
        "    oColor[0]=cV(pos.x/uWidth,textureYPos,dx,dy);\n"
        "    oColor[1]=cV((pos.x+2.)/uWidth,textureYPos,dx,dy);\n"
        "    oColor[2]=cV((pos.x+4.)/uWidth,textureYPos,dx,dy);\n"
        "    oColor[3]=cV((pos.x+6.)/uWidth,textureYPos,dx,dy);\n"
        "    return oColor;                               \n"
        "}                                                \n"
        "void main()                                      \n"
        "{                                                \n"
        "   if(composeMode == 2){                         \n"
        "       if(vTextureCo.y<0.2500){                  \n"
        "           fragColor=calculateY();               \n"
        "       }else if(vTextureCo.y<0.3125){            \n"
        "           fragColor=calculateU(0.2500,1./uWidth,1./uHeight);\n"
        "       }else if(vTextureCo.y<0.3750){            \n"
        "           fragColor=calculateV(0.3125,1./uWidth,1./uHeight);\n"
        "       }else{                                    \n"
        "           fragColor=vec4(0,0,0,1);           \n"
        "       }                                         \n"
        "   } else if(composeMode == 0){                  \n"
        "       fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
        "   }else{                                        \n"
        "       fragColor = vec4 ( 0.0, 1.0, 0.0, 1.0 );  \n"
        "   }                                             \n"
        "}                                                \n";
#endif

    GLuint vertexShader;
    GLuint fragmentShader;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );
    fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );

   // Create the program object
    m_program = glCreateProgram ( );

    if ( m_program == 0 )
    {
        return 0;
    }

    glAttachShader ( m_program, vertexShader );
    glAttachShader ( m_program, fragmentShader );

    // Link the program
    glLinkProgram ( m_program );

    // Check the link status
    glGetProgramiv ( m_program, GL_LINK_STATUS, &linked );

    if ( !linked ) {
        GLint infoLen = 0;

        glGetProgramiv ( m_program, GL_INFO_LOG_LENGTH, &infoLen );

        if ( infoLen > 1 ) {
            char *infoLog = (char*)malloc ( sizeof ( char ) * infoLen );

            glGetProgramInfoLog ( m_program, infoLen, NULL, infoLog );
            std::cout << "Error linking program: " << infoLog << std::endl;

            free ( infoLog );
        }

        glDeleteProgram ( m_program );
        return false;
    }

   // Store the program object
    m_compose_pos = glGetUniformLocation(m_program, "composeMode");
    m_width_pos = glGetUniformLocation(m_program, "uWidth");
    m_height_pos = glGetUniformLocation(m_program, "uHeight");
    m_tex_pos = glGetUniformLocation(m_program, "uTexture");
    glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
    return true;
}

void HelloFBO::Draw(){
    const GLubyte kIndices[] = {0, 1, 2, 2, 3, 0};
    GLfloat vVertices1[] = {  -1.0f,  -1.0f, 0.0f,
                            1.0f, -1.0f, 0.0f,
                            0.0f, 1.0f, 0.0f,
                         };
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Clear the color bufferGLuint m_tex = 0;
    // Use the program object
    glUseProgram ( m_program );

    
    glUniform1i(m_compose_pos, 0);

    // Load the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices1 );
    glEnableVertexAttribArray ( 0 );


    glDrawArrays ( GL_TRIANGLES, 0, 3 );
    // static int count = 0;
    // if(!m_rgb_data )
    //     m_rgb_data = (uint8_t*)malloc(m_width * m_height * 4);
    // glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_rgb_data);

    // if(!m_yuv_data)
    //     m_yuv_data = (uint8_t*)malloc(m_width * m_height * 3 /2 );
    // RGB2YUV(m_rgb_data);
    // static FILE* file1 = fopen("test.yuv", "wb");
    // fwrite(m_yuv_data, m_width* m_height*3/2, 1, file1);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    // glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glUniform1i(m_compose_pos, 2);
    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0 );
    glEnableVertexAttribArray ( 0 );
    glVertexAttribPointer ( 1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(static_cast<unsigned long long>(sizeof(float) * 3)) );
    glEnableVertexAttribArray ( 1 );
    glUniform1f(m_width_pos, 500);
    glUniform1f(m_height_pos, 400);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glUniform1i(m_tex_pos, 0);

    glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, (const GLvoid*)kIndices);
    
    // glReadPixels(0, 0, m_width, m_height*3/8, GL_RGBA, GL_UNSIGNED_BYTE, m_rgb_data);

    // static FILE* file2 = fopen("test2.yuv", "wb");
    // fwrite(m_rgb_data, m_width* m_height*3/2, 1, file2);


}

void HelloFBO::Shutdown(){
    glDeleteProgram(m_program);
    glDeleteTextures(1, &m_tex);
    glDeleteFramebuffers(1, &m_fbo);
}

void HelloFBO::RGB2YUV(uint8_t* input_buffer){

    int y_size = m_width * m_height;
    int y_index = 0;
    int u_index = y_size;
    int v_index = u_index + y_size / 4;
    int R, G, B, Y, U, V;

    for(int i=0; i<m_height; i++){
        for(int j=0; j<m_width; j++){
        	R = input_buffer[(i*m_width + j) * 4 + 0];
        	G = input_buffer[(i*m_width + j) * 4 + 1];
        	
        	B = input_buffer[(i*m_width + j) * 4 + 2];
        	Y = ((66*R + 129*G + 25*B) >> 8 ) + 16;
        	U = ((-38*R - 74*G + 112*B) >> 8 ) + 128;
        	V = ((112*R - 94*G - 18*B) >> 8 ) + 128;

            // Y = R*0.257+G*0.504+B*0.098+0.0625;
        	// U = -0.148*R - 0.291*G + 0.439*B+0.5000;
        	// V = 0.439*R - 0.368*G - 0.071*B+0.5000;
        	m_yuv_data[y_index++] = (uint8_t)(Y);
        	if(i%2 == 0 && j%2 == 0){
        		m_yuv_data[u_index++] = (uint8_t)(U);
        		m_yuv_data[v_index++] = (uint8_t)(V);
        	}
        }
    }

}
