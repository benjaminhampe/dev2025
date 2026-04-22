#include "BenMesh.h"

#if 0
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
















void GL_Spectrum3D::createVLines( BenMeshBuffer & m, glm::vec3 const & d,
                                  DE_AlignedFloatVector const & pcm )
{
    const size_t n = pcm.size();
    const float* p = pcm.data();

    m.PrimType = de::gpu::PrimitiveType::Lines;
    m.Vertices.clear();
    m.Vertices.reserve( n*2 );
    m.Indices.clear();
    m.Indices.reserve( n*2 );

    const float dx = d.x / float(n - 1);
    const float dy = d.y * 0.5f;
    const auto c0 = m_wav_colorGradient.getColor32(0.5f);

    for (size_t i = 0; i < n; i++)
    {
        const float s = p[i];
        const float t = std::clamp((s * 0.5f) + 0.5f, 0.f, 1.f);
        const float x = (dx * i);
        const float y = (dy * s) + dy;
        const float z = 0.0f;
        const auto c = m_wav_colorGradient.getColor32(t);
        m.Vertices.emplace_back( x, y, z, c );
        m.Vertices.emplace_back( x, dy, z, c0 );
        m.Indices.emplace_back( 2*i );
        m.Indices.emplace_back( 2*i+1 );
    }
    m.upload( true );
}

void GL_Spectrum3D::createVLines(BenMeshBuffer & m, glm::vec3 const & d,
                                DE_AlignedFloatShiftMatrix const & table )
{
    auto rowCount = table.rowCount();
    auto colCount = table.columnCount();

    if ( rowCount < 1 || colCount < 1 )
    {
        return;
    }

    m.PrimType = de::gpu::PrimitiveType::Triangles;
    m.Vertices.clear();
    m.Indices.clear();
    m.Vertices.reserve( rowCount * colCount * 2 );
    m.Indices.reserve( rowCount * colCount * 2 );

    float dx = d.x / float(colCount-1);
    float dy = d.y * 0.5f;
    float dz = d.z / float(rowCount-1);
    uint32_t c0 = m_wav_colorGradient.getColor32( 0.5f );

    // Matrix Top
    uint32_t v0 = 0;
    for ( size_t j = 0; j < rowCount; ++j )
    {
        auto row = table.m_view[ j ];  // The row vector
        for ( size_t i = 0; i < colCount; ++i )
        {
            float s = row[i];
            float t = std::clamp( (0.5f*s) + 0.5f, 0.f, 1.f);
            float x = dx * i;
            float y = dy * s + dy;
            float z = dz * j;

            uint32_t c1 = m_wav_colorGradient.getColor32( t );
            //uint32_t c1 = de::RainbowColor::computeColor32( std::fabs( 1.0f - 0.5f * r ) );

            m.Vertices.push_back( BenVertex(x,dy,z,c0) );
            m.Vertices.push_back( BenVertex(x, y,z,c1) );
            m.Indices.push_back( v0 );
            m.Indices.push_back( v0+1 );

            v0 += 2;
        }
    }

    m.upload( true );
}
void GL_Spectrum3D::createLineBox(BenMeshBuffer & m, de::BBox3f const & bbox, uint32_t color )
{
    m.PrimType = de::gpu::PrimitiveType::Lines;
    m.Vertices.clear();
    m.Vertices.reserve( 8 );
    m.Indices.clear();
    m.Indices.reserve( 24 );

    const float x1 = bbox.m_min.x;
    const float y1 = bbox.m_min.y;
    const float z1 = bbox.m_min.z;
    const float x2 = bbox.m_max.x;
    const float y2 = bbox.m_max.y;
    const float z2 = bbox.m_max.z;

    m.Vertices.push_back( BenVertex( x1, y1, z1, color ) );
    m.Vertices.push_back( BenVertex( x1, y2, z1, color ) );
    m.Vertices.push_back( BenVertex( x2, y2, z1, color ) );
    m.Vertices.push_back( BenVertex( x2, y2, z1, color ) );
    m.Vertices.push_back( BenVertex( x1, y1, z2, color ) );
    m.Vertices.push_back( BenVertex( x1, y2, z2, color ) );
    m.Vertices.push_back( BenVertex( x2, y2, z2, color ) );
    m.Vertices.push_back( BenVertex( x2, y2, z2, color ) );

    uint32_t v0 = 0;
    m.addIndexedLine(v0 + 0, v0 + 1); // AB
    m.addIndexedLine(v0 + 1, v0 + 2); // BC
    m.addIndexedLine(v0 + 2, v0 + 3); // CD
    m.addIndexedLine(v0 + 3, v0 + 0); // DA

    m.addIndexedLine(v0 + 4, v0 + 5); // EF
    m.addIndexedLine(v0 + 5, v0 + 6); // FG
    m.addIndexedLine(v0 + 6, v0 + 7); // GH
    m.addIndexedLine(v0 + 7, v0 + 4); // HE

    m.addIndexedLine(v0 + 0, v0 + 4); // AE
    m.addIndexedLine(v0 + 1, v0 + 5); // BF
    m.addIndexedLine(v0 + 2, v0 + 6); // CG
    m.addIndexedLine(v0 + 3, v0 + 7); // DH

    m.upload( true );
}

void GL_Spectrum3D::createLineBox(BenMeshBuffer & m, de::BBox3f const & bbox )
{
    m.PrimType = de::gpu::PrimitiveType::Lines;
    m.Vertices.clear();
    m.Vertices.reserve( 8 );
    m.Indices.clear();
    m.Indices.reserve( 24 );

    const float x1 = bbox.m_min.x;
    const float y1 = bbox.m_min.y;
    const float z1 = bbox.m_min.z;
    const float x2 = bbox.m_max.x;
    const float y2 = bbox.m_max.y;
    const float z2 = bbox.m_max.z;

    m.Vertices.push_back( BenVertex( x1, y1, z1, 255,100,100 ) );
    m.Vertices.push_back( BenVertex( x1, y2, z1, 255,255,255 ) );
    m.Vertices.push_back( BenVertex( x2, y2, z1, 100,255,100 ) );
    m.Vertices.push_back( BenVertex( x2, y2, z1, 255,255,255 ) );
    m.Vertices.push_back( BenVertex( x1, y1, z2, 255,200,200 ) );
    m.Vertices.push_back( BenVertex( x1, y2, z2, 255,255,255 ) );
    m.Vertices.push_back( BenVertex( x2, y2, z2, 200,255,200 ) );
    m.Vertices.push_back( BenVertex( x2, y2, z2, 255,255,255 ) );

    uint32_t v0 = 0;
    m.addIndexedLine(v0 + 0, v0 + 1); // AB
    m.addIndexedLine(v0 + 1, v0 + 2); // BC
    m.addIndexedLine(v0 + 2, v0 + 3); // CD
    m.addIndexedLine(v0 + 3, v0 + 0); // DA

    m.addIndexedLine(v0 + 4, v0 + 5); // EF
    m.addIndexedLine(v0 + 5, v0 + 6); // FG
    m.addIndexedLine(v0 + 6, v0 + 7); // GH
    m.addIndexedLine(v0 + 7, v0 + 4); // HE

    m.addIndexedLine(v0 + 0, v0 + 4); // AE
    m.addIndexedLine(v0 + 1, v0 + 5); // BF
    m.addIndexedLine(v0 + 2, v0 + 6); // CG
    m.addIndexedLine(v0 + 3, v0 + 7); // DH

    m.upload( true );
}

void GL_Spectrum3D::createBksLines(BenMeshBuffer & m, float size3d )
{
    float d = size3d;
    m.PrimType = de::gpu::PrimitiveType::Lines;
    m.Vertices.clear();
    m.Vertices.reserve( 6 );
    m.Vertices.push_back( BenVertex( 0, 0, 0, 255,  0,  0 ) );
    m.Vertices.push_back( BenVertex( d, 0, 0, 255,255,255 ) );
    m.Vertices.push_back( BenVertex( 0, 0, 0,   0,255,  0 ) );
    m.Vertices.push_back( BenVertex( 0, d, 0, 255,255,255 ) );
    m.Vertices.push_back( BenVertex( 0, 0, 0,   0,  0,255 ) );
    m.Vertices.push_back( BenVertex( 0, 0, d, 255,255,255 ) );
    m.upload( true );
}

void GL_Spectrum3D::createBksLinesX(BenMeshBuffer & m, float d )
{
    m.PrimType = de::gpu::PrimitiveType::Lines;

    int n=100;

    m.Vertices.clear();
    m.Vertices.reserve( 6 );

    float dx = 10.0f;
    float dz = 50.0f;
    for (int i = 0; i < 100; ++i)
    {
        m.Vertices.push_back( BenVertex( dx * i, 0, 0, 255,  0,  0 ) );
        m.Vertices.push_back( BenVertex( dx * i, 0, -dz, 255,255,255 ) );
    }

    dx = 100.0f;
    dz = 100.0f;
    for (int i = 0; i < 10; ++i)
    {
        m.Vertices.push_back( BenVertex( dx * i, 0, 0, 255,  0,  0 ) );
        m.Vertices.push_back( BenVertex( dx * i, 0, -dz, 255,255,255 ) );
    }

    dx = 1000.0f;
    dz = 200.0f;
    for (int i = 0; i < 2; ++i)
    {
        m.Vertices.push_back( BenVertex( dx * i, 0, 0, 255,  0,  0 ) );
        m.Vertices.push_back( BenVertex( dx * i, 0, -dz, 255,255,255 ) );
    }

    m.upload( true );
}

void GL_Spectrum3D::createBksLinesZ(BenMeshBuffer & m, float d )
{
    m.PrimType = de::gpu::PrimitiveType::Lines;

    int n=100;

    m.Vertices.clear();
    m.Vertices.reserve( 6 );

    float dz = 10.0f;
    float dx = 50.0f;
    for (int i = 0; i < 100; ++i)
    {
        m.Vertices.push_back( BenVertex(   0, 0, dz * i, 0,  0,  255 ) );
        m.Vertices.push_back( BenVertex( -dx, 0, dz * i, 255,255,255 ) );
    }

    dz = 100.0f;
    dx = 100.0f;
    for (int i = 0; i < 10; ++i)
    {
        m.Vertices.push_back( BenVertex(   0, 0, dz * i, 0,  0,  255 ) );
        m.Vertices.push_back( BenVertex( -dx, 0, dz * i, 255,255,255 ) );
    }

    dz = 1000.0f;
    dx = 200.0f;
    for (int i = 0; i < 2; ++i)
    {
        m.Vertices.push_back( BenVertex(   0, 0, dz * i, 0,  0,  255 ) );
        m.Vertices.push_back( BenVertex( -dx, 0, dz * i, 255,255,255 ) );
    }

    m.upload( true );
}

void GL_Spectrum3D::createBksLinesY(BenMeshBuffer & m, float d )
{
    m.PrimType = de::gpu::PrimitiveType::Lines;

    int n=100;

    m.Vertices.clear();
    m.Vertices.reserve( 6 );

    float dy = 10.0f;
    float dz = 50.0f;
    for (int i = 0; i < 100; ++i)
    {
        m.Vertices.push_back( BenVertex( 0, dy * i, 0, 0,  255,  0 ) );
        m.Vertices.push_back( BenVertex( 0, dy * i, -dz, 255,255,255 ) );
    }

    dy = 100.0f;
    dz = 100.0f;
    for (int i = 0; i < 10; ++i)
    {
        m.Vertices.push_back( BenVertex( 0, dy * i, 0, 0,  255,  0 ) );
        m.Vertices.push_back( BenVertex( 0, dy * i, -dz, 255,255,255 ) );
    }

    dy = 1000.0f;
    dz = 200.0f;
    for (int i = 0; i < 2; ++i)
    {
        m.Vertices.push_back( BenVertex( 0, dy * i, 0, 0,  255,  0 ) );
        m.Vertices.push_back( BenVertex( 0, dy * i, -dz, 255,255,255 ) );
    }

    m.upload( true );
}
#endif