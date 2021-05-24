#include "HelloTriangle.h"

HelloTriangle::HelloTriangle(/* args */)
{
}

HelloTriangle::~HelloTriangle()
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

bool HelloTriangle::Init(){

    char vShaderStr[] =
        "#version 300 es                          \n"
        "layout(location = 0) in vec4 vPosition;  \n"
        "void main()                              \n"
        "{                                        \n"
        "   gl_Position = vPosition;              \n"
        "}                                        \n";

    char fShaderStr[] =
        "#version 300 es                              \n"
        "precision mediump float;                     \n"
        "out vec4 fragColor;                          \n"
        "void main()                                  \n"
        "{                                            \n"
        "   fragColor = vec4 ( 0.0, 1.0, 1.0, 1.0 );  \n"
        "}                                            \n";

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

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   return true;
}

void HelloTriangle::Draw(){
    GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f,
                            -0.5f, -0.5f, 0.0f,
                            0.5f, -0.5f, 0.0f,
                         };

    // Set the viewport
    glViewport ( 0, 0, m_width, m_height );

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT );

    // Use the program object
    glUseProgram ( m_program );

    // Load the vertex data
    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
    glEnableVertexAttribArray ( 0 );

    glDrawArrays ( GL_TRIANGLES, 0, 3 );

    if(!m_rgb_data){
        m_rgb_data = (uint8_t*)malloc(m_width * m_height * 4);
        glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_rgb_data);

        if(!m_yuv_data)
            m_yuv_data = (uint8_t*)malloc(m_width * m_height * 3 /2 );
        RGB2YUV(m_rgb_data);
        FILE* file = fopen("test.yuv", "wb");
        fwrite(m_yuv_data, m_width* m_height*3/2, 1, file);
        fclose(file);
    }

    glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_rgb_data);

}

void HelloTriangle::Shutdown(){
    glDeleteProgram(m_program);
}

void HelloTriangle::RGB2YUV(uint8_t* input_buffer){

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

        	m_yuv_data[y_index++] = (uint8_t)(Y);
        	if(i%2 == 0 && j%2 == 0){
        		m_yuv_data[u_index++] = (uint8_t)(U);
        		m_yuv_data[v_index++] = (uint8_t)(V);
        	}
        }
    }

}