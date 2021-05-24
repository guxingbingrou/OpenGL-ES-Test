#include "HelloFBO.h"
#include <stdio.h>
HelloFBO::HelloFBO(/* args */)
{
}

HelloFBO::~HelloFBO()
{

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

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

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
    

}

void HelloFBO::Shutdown(){
    glDeleteProgram(m_program);
    glDeleteTextures(1, &m_tex);
    glDeleteFramebuffers(1, &m_fbo);
}

