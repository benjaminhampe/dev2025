#include "GL_Utils.h"

#include <de_opengl.h>

#include <de/Core.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#else
    #define APIENTRY
#endif

// static
PrimitiveType
PrimitiveType::fromOpenGL( uint32_t const primitiveType )
{
    switch ( primitiveType )
    {
    case GL_LINES: return PrimitiveType::Lines;
    case GL_LINE_STRIP: return PrimitiveType::LineStrip;
    case GL_LINE_LOOP: return PrimitiveType::LineLoop;
    case GL_TRIANGLES: return PrimitiveType::Triangles;
    case GL_TRIANGLE_STRIP: return PrimitiveType::TriangleStrip;
    case GL_TRIANGLE_FAN: return PrimitiveType::TriangleFan;
    case GL_QUADS: return PrimitiveType::Quads;
    default: return PrimitiveType::Points;
    }
}

// static
uint32_t
PrimitiveType::toOpenGL( PrimitiveType const primitiveType )
{
    switch ( primitiveType )
    {
    case PrimitiveType::Lines: return GL_LINES;
    case PrimitiveType::LineStrip: return GL_LINE_STRIP;
    case PrimitiveType::LineLoop: return GL_LINE_LOOP;
    case PrimitiveType::Triangles: return GL_TRIANGLES;
    case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
    case PrimitiveType::TriangleFan: return GL_TRIANGLE_FAN;
    case PrimitiveType::Quads: return GL_QUADS;
    default: return GL_POINTS;
    }
}

static void APIENTRY GT_DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::ostringstream o;
    o << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") <<
        " type = 0x" << std::hex << type <<
        ", severity = 0x" << severity <<
        ", message = " << message << std::endl;
    DE_ERROR(o.str())
}

void
GT_init()
{
    ensureDesktopOpenGL(); // initGlew()

    // Enable OpenGL debug output
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GT_DebugMessageCallback, 0);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Smooth shading (optional for older OpenGL versions)
    //glShadeModel(GL_SMOOTH);
}

uint32_t
GT_compileShader(uint32_t type, const char* source)
{
    uint32_t shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &source, NULL);
    glCompileShader(shaderId);

    int ok = 0;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        DE_ERROR("SHADER::COMPILATION_FAILED:\n", infoLog)
    }

    return shaderId;
}

uint32_t
GT_createShaderProgram( const char* vsText, const char* fsText )
{
    uint32_t vsShaderId = GT_compileShader(GL_VERTEX_SHADER, vsText);
    uint32_t fsShaderId = GT_compileShader(GL_FRAGMENT_SHADER, fsText);

    uint32_t programId = glCreateProgram();
    glAttachShader(programId, vsShaderId);
    glAttachShader(programId, fsShaderId);
    glLinkProgram(programId);

    int ok = 0;
    char infoLog[512];
    glGetProgramiv(programId, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        DE_ERROR("SHADER::PROGRAM::LINKING_FAILED:\n", infoLog)
    }

    glDeleteShader(vsShaderId);
    glDeleteShader(fsShaderId);

    return programId;
}


void
GT_destroy( SMeshBuffer & meshBuffer )
{
    if ( meshBuffer.VAO )
    {
        glDeleteVertexArrays(1, &meshBuffer.VAO);
        meshBuffer.VAO = 0;
    }

    if ( meshBuffer.VBO )
    {
        glDeleteBuffers(1, &meshBuffer.VBO);
        meshBuffer.VBO = 0;
    }

    if ( meshBuffer.IBO )
    {
        glDeleteBuffers(1, &meshBuffer.IBO);
        meshBuffer.IBO = 0;
    }
}

void
GT_upload( SMeshBuffer & meshBuffer, bool bForceUpload )
{
    bool bNeedUpload = bForceUpload;
    if ( !meshBuffer.VAO )
    {
        glGenVertexArrays(1, &meshBuffer.VAO);
        //DE_DEBUG("Create meshBuffer.VAO")
        bNeedUpload = true;
    }

    if ( !meshBuffer.VBO )
    {
        glGenBuffers(1, &meshBuffer.VBO);
        //DE_DEBUG("Create meshBuffer.VBO")
        bNeedUpload = true;
    }

    if ( !meshBuffer.IBO && meshBuffer.Indices.size() > 0 )
    {
        glGenBuffers(1, &meshBuffer.IBO);
        //DE_DEBUG("Create meshBuffer.IBO")
        bNeedUpload = true;
    }

    if ( bNeedUpload )
    {
        //DE_DEBUG("Upload meshBuffer.VAO")

        //shouldUpload = false;

        glBindVertexArray(meshBuffer.VAO);

        if ( meshBuffer.VBO )
        {
            //DE_DEBUG("Upload meshBuffer.VBO")

            const size_t vertexCount = meshBuffer.Vertices.size();
            const size_t vertexSize = sizeof(S3DVertex);
            const size_t vertexBytes = vertexCount * vertexSize;
            const uint8_t* pVertices = reinterpret_cast< const uint8_t* >( meshBuffer.Vertices.data() );

            glBindBuffer(GL_ARRAY_BUFFER, meshBuffer.VBO);
            glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), pVertices, GL_STATIC_DRAW);

            // VertexAttribute[0] = Position3D (vec3f)
            // VertexAttribute[1] = Normal3D (vec3f)
            // VertexAttribute[2] = ColorRGBA (vec4ub)
            // VertexAttribute[3] = Tex2DCoord0 (vec2f)

            glEnableVertexAttribArray( 0 );
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(0) );

            glEnableVertexAttribArray( 1 );
            glVertexAttribIPointer( 1, 1, GL_UNSIGNED_INT, vertexSize, reinterpret_cast<void*>(12) );

            //glEnableVertexAttribArray( 1 );
            //glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(12) );

            //glEnableVertexAttribArray( 2 );
            //glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_FALSE, vertexSize, reinterpret_cast<void*>(24) );

            //glEnableVertexAttribArray( 3 );
            //glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(28) );

            //DE_DEBUG("Upload ", vertexCount, " vertices")
            //DE_DEBUG("Upload ", vertexBytes, " bytes")
            //DE_DEBUG("Upload ", vertexSize, " stride")
        }

        if ( meshBuffer.IBO )
        {
            //DE_DEBUG("Upload meshBuffer.IBO")

            const size_t indexCount = meshBuffer.Indices.size();
            const size_t indexSize = sizeof(uint32_t);
            const size_t indexBytes = indexCount * indexSize;
            const uint8_t* pIndices = reinterpret_cast< const uint8_t* >( meshBuffer.Indices.data() );

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffer.IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBytes), pIndices, GL_STATIC_DRAW);

            //std::cout << "Upload " << vertexCount << " vertices" << std::endl;
            //std::cout << "Upload " << vertexBytes << " bytes" << std::endl;
            //std::cout << "Upload " << vertexSize << " stride" << std::endl;
        }

        glBindVertexArray(0);
    }
}

void
GT_draw( SMeshBuffer & meshBuffer )
{
    //upload( meshBuffer );

    if (meshBuffer.VAO)
    {
        glBindVertexArray( meshBuffer.VAO );

        //DE_DEBUG("Bind meshBuffer.VAO")
    }

    if (meshBuffer.VBO)
    {
        GLenum const primType = PrimitiveType::toOpenGL( meshBuffer.PrimType );
        if ( meshBuffer.IBO )
        {
            //DE_DEBUG("Draw meshBuffer.IBO")

            glDrawElements( primType, GLint(meshBuffer.Indices.size()), GL_UNSIGNED_INT, nullptr );
        }
        else
        {
            //DE_DEBUG("Draw meshBuffer.VBO, GL_PrimType = ", primType)

            glDrawArrays( primType, 0, GLint(meshBuffer.Vertices.size()) );
        }
    }

    if (meshBuffer.VAO)
    {
        glBindVertexArray(0);

        //DE_DEBUG("Unbind meshBuffer.VAO")
    }
}


// ===========================================================================
void LineShader::init()
// ===========================================================================
{
    const char* g_vs = R"(
        #version 330 core

        //precision highp float;

        layout(location = 0) in vec3 a_pos; // Position attribute
        layout(location = 1) in uint a_color; // Guaranteed to be 32-bit

        //layout(location = 1) in vec3 a_normal;
        //layout(location = 1) in lowp vec4 a_color;
        //layout(location = 3) in vec2 a_tex;

        uniform mat4 u_modelMat;
        uniform mat4 u_viewMat;
        uniform mat4 u_projMat;

        //out vec3 v_pos;
        //out vec3 v_normal;
        out vec4 v_color;
        //out vec2 v_tex;

        void main()
        {
            gl_PointSize = 5.0;
            //v_pos = a_pos;
            //v_normal = a_normal;
            //v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
            v_color = unpackUnorm4x8(a_color);
            //v_tex = a_tex;
            gl_Position = u_projMat * (u_viewMat * (u_modelMat * vec4(a_pos, 1.0)));
        }
    )";

    const char* g_fs = R"(
        #version 330 core

        //precision highp float;

        //in vec3 v_pos;
        //in vec3 v_normal;
        in vec4 v_color;
        //in vec2 v_tex;

        out vec4 FragColor;

        void main()
        {
            FragColor = clamp( v_color, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
        }
    )";

    m_programId = GT_createShaderProgram( g_vs, g_fs );

    // m_modelMat = glm::mat4(1.0f);

    // m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, 500.0f, 1000.0f));

    // m_projMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 38000.0f);
}

void LineShader::useShader( de::gpu::Camera* pCamera, bool bBlend )
{
    glUseProgram(m_programId);

    glm::mat4 m_modelMat = glm::mat4(1.0f);
    int modelLoc = glGetUniformLocation(m_programId, "u_modelMat");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_modelMat));

    glm::mat4 m_viewMat = pCamera->getViewMatrix();
    int viewLoc = glGetUniformLocation(m_programId, "u_viewMat");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMat));

    glm::mat4 m_projMat = pCamera->getProjectionMatrix();
    int projLoc = glGetUniformLocation(m_programId, "u_projMat");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projMat));

    //int colorLoc = glGetUniformLocation(m_programId, "u_color");
    //glUniform4f(colorLoc, 0.0f, 0.8f, 1.0f, 1.0f); // Solid color

    if (bBlend)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }

}

void LineShader::deinit()
{
    glDeleteProgram(m_programId);
}
