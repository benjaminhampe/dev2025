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

namespace {

static void APIENTRY GLUtil_DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::ostringstream o;
    o << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") <<
        " type = 0x" << std::hex << type <<
        ", severity = 0x" << severity <<
        ", message = " << message << std::endl;
    DE_ERROR(o.str())
}

}

void
GLUtil_init()
{
    ensureDesktopOpenGL(); // initGlew()

    // Enable OpenGL debug output
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(GLUtil_DebugMessageCallback, 0);

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
GLUtil_compileShader(uint32_t type, const char* source)
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
GLUtil_createShaderProgram( const char* vsText, const char* fsText )
{
    uint32_t vsShaderId = GLUtil_compileShader(GL_VERTEX_SHADER, vsText);
    uint32_t fsShaderId = GLUtil_compileShader(GL_FRAGMENT_SHADER, fsText);

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

// ===============================================================

void BenMeshBuffer::destroy()
{
    if ( VAO )
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    if ( VBO )
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if ( IBO )
    {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }
}

void BenMeshBuffer::draw() const
{
    if (VAO)
    {
        glBindVertexArray( VAO );
    }

    if (VBO)
    {
        GLenum const primType = de::gpu::PrimitiveType::toOpenGL( PrimType );
        if ( IBO )
        {
            glDrawElements( primType,
                           GLint(Indices.size()),
                           GL_UNSIGNED_INT,
                           nullptr );
        }
        else
        {
            glDrawArrays(   primType,
                         0,
                         GLint(Vertices.size()) );
        }
    }

    if (VAO)
    {
        glBindVertexArray(0);
    }
}

void BenMeshBuffer::upload( bool bForceUpload )
{
    bool bNeedUpload = bForceUpload;
    if ( !VAO )
    {
        glGenVertexArrays(1, &VAO);
        bNeedUpload = true;
    }

    if ( !VBO )
    {
        glGenBuffers(1, &VBO);
        bNeedUpload = true;
    }

    if ( !IBO && Indices.size() > 0 )
    {
        glGenBuffers(1, &IBO);
        bNeedUpload = true;
    }

    if ( bNeedUpload )
    {
        glBindVertexArray(VAO);

        if ( VBO )
        {
            const size_t vertexCount = Vertices.size();
            const size_t vertexSize = sizeof(BenVertex);
            const size_t vertexBytes = vertexCount * vertexSize;
            const uint8_t* pVertices = reinterpret_cast< const uint8_t* >( Vertices.data() );
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), pVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray( 0 );
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(0) );
            glEnableVertexAttribArray( 1 );
            glVertexAttribIPointer( 1, 1, GL_UNSIGNED_INT, vertexSize, reinterpret_cast<void*>(12) );
        }

        if ( IBO )
        {
            const size_t indexCount = Indices.size();
            const size_t indexSize = sizeof(uint32_t);
            const size_t indexBytes = indexCount * indexSize;
            const uint8_t* pIndices = reinterpret_cast< const uint8_t* >( Indices.data() );
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBytes), pIndices, GL_STATIC_DRAW);
        }

        glBindVertexArray(0);
    }
}


// ===========================================================================
BenShader::BenShader()
// ===========================================================================
    : m_modelMat(1.0f)
    , m_driver(nullptr)
    , m_shader(nullptr)
    , m_u_modelMat(-1)
    , m_u_viewMat(-1)
    , m_u_projMat(-1)
    , m_blend(false)
{
    // u_modelMat = M4(1.0f);
    // u_viewMat = glm::translate(M4(1.0f), V3(500.0f, 500.0f, 1000.0f));
    // u_projMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 38000.0f);
}

BenShader::~BenShader()
{
}

void BenShader::setDriver( de::gpu::VideoDriver* driver )
{
    m_driver = driver;
}

void BenShader::ensureShader()
{
    if (m_shader)
    {
        return;
    }
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }
    //if (m_programId)
    //{
    //    return;
    //}

    const char* g_vs = R"(
        //#version 330 core

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
        //#version 330 core

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

    m_shader = m_driver->createShader( "BenShader", g_vs, g_fs );
    if (m_shader)
    {
        //m_programId = GLUtil_createShaderProgram( g_vs, g_fs );
        m_u_modelMat = glGetUniformLocation(m_shader->id, "u_modelMat");
        m_u_viewMat = glGetUniformLocation(m_shader->id, "u_viewMat");
        m_u_projMat = glGetUniformLocation(m_shader->id, "u_projMat");
        //m_u_color = glGetUniformLocation(m_shader->id, "u_color");
    }
    else
    {
        DE_WARN("No shader!")
    }
}


void BenShader::setBlend( bool bBlend )
{
    m_blend = bBlend;
}
void BenShader::drawAt( const BenMeshBuffer & mesh, float x, float y, float z )
{
    setModelPos(V3(x,y,z));
    draw(mesh);
}
void BenShader::drawAt( const BenMeshBuffer & mesh, const V3& pos )
{
    setModelPos(pos);
    draw(mesh);
}
void BenShader::draw( const BenMeshBuffer & mesh )
{
    ensureShader();

    //GT_upload( m, bForceUpload );

    M4 m_viewMat = M4(1.0f);
    M4 m_projMat = M4(1.0f);

    auto camera = m_driver->getCamera();
    if (camera)
    {
        m_viewMat = camera->getViewMatrix();
        m_projMat = camera->getProjectionMatrix();
    }

    m_driver->useShader(m_shader);
    // glUseProgram(m_programId);
    glUniformMatrix4fv(m_u_modelMat, 1, GL_FALSE, glm::value_ptr(m_modelMat));
    glUniformMatrix4fv(m_u_viewMat, 1, GL_FALSE, glm::value_ptr(m_viewMat));
    glUniformMatrix4fv(m_u_projMat, 1, GL_FALSE, glm::value_ptr(m_projMat));
    //glUniform4f(m_u_color, 0.0f, 0.8f, 1.0f, 1.0f); // Solid color

    if (m_blend)
    {
        auto state = m_driver->getState();
        state.blend = de::gpu::Blend::alphaBlend();
        m_driver->setState(state);
    }

    mesh.draw();
}

void BenShader::deinit()
{
    // glDeleteProgram(m_programId);
    // m_driver->shader
}
void BenShader::resetModelMat()
{
    m_modelMat = glm::mat4(1.0f);
}
void BenShader::setModelMat( glm::mat4 const & modelMat )
{
    m_modelMat = modelMat;
}
void BenShader::setModelPos( glm::vec3 const & modelPos )
{
    m_modelMat = glm::translate(glm::mat4(1.0f), modelPos);
}
void BenShader::setModelScale( glm::vec3 const & scale )
{
    m_modelMat = glm::scale(glm::mat4(1.0f), scale);
}
void BenShader::setModelEuler( glm::vec3 const & eulerAngles )
{
    auto R_x = glm::rotate( glm::mat4(1.0f), eulerAngles.x, glm::vec3(1,0,0) );
    auto R_y = glm::rotate( glm::mat4(1.0f), eulerAngles.y, glm::vec3(0,1,0) );
    auto R_z = glm::rotate( glm::mat4(1.0f), eulerAngles.z, glm::vec3(0,0,1) );
    auto R = R_x * (R_y * R_z);
    m_modelMat = R;
}
