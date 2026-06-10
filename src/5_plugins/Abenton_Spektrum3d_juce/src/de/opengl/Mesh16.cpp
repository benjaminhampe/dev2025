#include "Mesh16.h"
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

Mesh16::Mesh16()
    : PrimType(de::gpu::PrimitiveType::Points)
#ifdef USE_MESH16_INDICES_32BIT
    , IndexType(de::gpu::IndexType::U32)
#else
    , IndexType(de::gpu::IndexType::U16)
#endif
    , VAO(0)
    , VBO(0)
    , IBO(0)
{
}

void Mesh16::addIndex(uint32_t index)
{
#ifndef USE_MESH16_INDICES_32BIT
    if (index > 65535)
    {
        DE_ERROR("Got index ",index," > 16-bit")
        return;
    }
#endif
    Indices.push_back( index );
}

void Mesh16::addIndexedLine(uint32_t A,uint32_t B)
{
    addIndex( A );   // A - 0
    addIndex( B );   // C - 3
}

void Mesh16::addIndexedTriangle(uint32_t A,uint32_t B,uint32_t C)
{
    addIndex( A );   // A - 0
    addIndex( C );   // C - 3
    addIndex( B );   // B - 1
}

void Mesh16::addIndexedQuad(uint32_t A,uint32_t B,uint32_t C,uint32_t D)
{
    addIndex( A );   // A - 0
    addIndex( C );   // C - 3
    addIndex( B );   // B - 1

    addIndex( A );   // A - 0
    addIndex( D );   // D - 2
    addIndex( C );   // C - 3
}

void Mesh16::destroy()
{
    if ( VAO ) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
    if ( VBO ) { glDeleteBuffers(1, &VBO); VBO = 0; }
    if ( IBO ) { glDeleteBuffers(1, &IBO); IBO = 0; }
    GL_VALIDATE
}

void Mesh16::draw() const
{
    GL_VALIDATE

    if (VAO) { glBindVertexArray( VAO ); }

    if (VBO)
    {
        const GLenum primType = de::gpu::PrimitiveType::toOpenGL( PrimType );
        if ( IBO )
        {
            // const GLenum indexType = de::gpu::IndexType::toOpenGL( IndexType );
#ifdef USE_MESH16_INDICES_32BIT
            const GLenum indexType = GL_UNSIGNED_INT;
#else
            const GLenum indexType = GL_UNSIGNED_SHORT;
#endif
            glDrawElements( primType, GLsizei(Indices.size()), indexType, nullptr );
        }
        else
        {
            glDrawArrays( primType, 0, GLsizei(Vertices.size()) );
        }
    }

    if (VAO) { glBindVertexArray(0); }

    GL_VALIDATE
}

void Mesh16::upload( bool bNeedVertexUpload, bool bNeedIndexUpload )
{
    GL_VALIDATE

    bool bNeedRebind = false;
    if ( !VAO )
    {
        glGenVertexArrays(1, &VAO);
        bNeedRebind = true;
        bNeedVertexUpload = true;
        bNeedIndexUpload = true;
    }
    if ( !VBO )
    {
        glGenBuffers(1, &VBO);
        bNeedRebind = true;
        bNeedVertexUpload = true;
    }
    if ( !IBO && Indices.size() > 0 )
    {
        glGenBuffers(1, &IBO);
        bNeedRebind = true;
        bNeedIndexUpload = true;
    }

    if ( bNeedRebind )
    {
        glBindVertexArray(VAO);
        // VBO
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glEnableVertexAttribArray( 0 );
#if 0
            glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof(Mesh16_Vertex), reinterpret_cast<void*>(0) );
#else
            glVertexAttribPointer( 0, 4, GL_HALF_FLOAT, GL_FALSE, sizeof(Mesh16_Vertex), reinterpret_cast<void*>(0) );
#endif
            auto n = Vertices.size() * sizeof(Mesh16_Vertex);
            auto p = reinterpret_cast< const uint8_t* >( Vertices.data() );
            glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(n), p, GL_STATIC_DRAW);
            if (n<1) { DE_WARN("Uploading 0 vertices") }
            DE_OK("Upload ",n," vertices")
        }

        if (IBO)
        {
#ifdef USE_MESH16_INDICES_32BIT
            auto n = Indices.size() * sizeof(uint32_t);
            auto p = reinterpret_cast< const uint8_t* >( Indices.data() );
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(n), p, GL_STATIC_DRAW);
#else // 16-bit
            auto n = Indices.size() * sizeof(uint16_t);
            auto p = reinterpret_cast< const uint8_t* >( Indices.data() );
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(n), p, GL_STATIC_DRAW);
#endif
            if (n<1) { DE_WARN("Uploading 0 indices") }
            DE_OK("Upload ",n," indices")
        }
        glBindVertexArray(0);
        bNeedVertexUpload = false;
        bNeedIndexUpload = false;
    }

    if ( bNeedVertexUpload )
    {
        auto n = Vertices.size() * sizeof(Mesh16_Vertex);
        auto p = reinterpret_cast< const uint8_t* >( Vertices.data() );
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(n), p, GL_STATIC_DRAW);
    }

    if ( IBO && bNeedIndexUpload )
    {
#ifdef USE_MESH16_INDICES_32BIT
        auto n = Indices.size() * sizeof(uint32_t);
        auto p = reinterpret_cast< const uint8_t* >( Indices.data() );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(n), p, GL_STATIC_DRAW);
#else // 16-bit
        auto n = Indices.size() * sizeof(uint16_t);
        auto p = reinterpret_cast< const uint8_t* >( Indices.data() );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(n), p, GL_STATIC_DRAW);
#endif
    }
    GL_VALIDATE
}

#if 0

void Mesh16::upload( bool bForceUpload, bool bNeedIndexUpload )
{
    bool bNeedUpload = bForceUpload;
    if ( !VAO )
    {
        glGenVertexArrays(1, &VAO);
        bNeedUpload = true;
        bNeedIndexUpload = true;
    }
    if ( !VBO )
    {
        glGenBuffers(1, &VBO);
        bNeedUpload = true;
        bNeedIndexUpload = true;
    }
    if ( !IBO && Indices.size() > 0 )
    {
        glGenBuffers(1, &IBO);
        bNeedUpload = true;
        bNeedIndexUpload = true;
    }

    if ( bNeedUpload )
    {
        glBindVertexArray(VAO);

        if ( VBO )
        {
            const size_t vertexCount = Vertices.size();
            const size_t vertexSize = sizeof(Mesh16_Vertex);
            const size_t vertexBytes = vertexCount * vertexSize;
            const uint8_t* pVertices = reinterpret_cast< const uint8_t* >( Vertices.data() );
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), pVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray( 0 );
            glVertexAttribPointer( 0, 4, GL_HALF_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(0) );
        }

        if ( IBO && bNeedIndexUpload )
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


void Mesh16::initVAO( bool bUseIndices )
{
    bool bNeedBindVAO = false;
    if ( !VAO )
    {
        glGenVertexArrays(1, &VAO);
        bNeedBindVAO = true;
    }
    if ( !VBO )
    {
        glGenBuffers(1, &VBO);
        bNeedBindVAO = true;
    }

    if ( !IBO && bUseIndices )
    {
        glGenBuffers(1, &IBO);
        bNeedBindVAO = true;
    }

    if ( bNeedBindVAO )
    {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glEnableVertexAttribArray( 0 );
    #if 0
        glVertexAttribPointer( 0, 4, GL_SHORT, GL_TRUE, sizeof(Mesh16_Vertex), reinterpret_cast<void*>(0) );
    #else
        glVertexAttribPointer( 0, 4, GL_HALF_FLOAT, GL_FALSE, sizeof(Mesh16_Vertex), reinterpret_cast<void*>(0) );
    #endif

        if ( bUseIndices )
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        }

        glBindVertexArray(0);
    }
}

void Mesh16::uploadVertices()
{
    if ( !VAO )
    {
        DE_ERROR("Malformed VAO")
        return;
    }

    if ( !VBO )
    {
        DE_ERROR("Malformed VAO and VBO")
        return;
    }

    const size_t vertexCount = Vertices.size();
    const size_t vertexSize = sizeof(Mesh16_Vertex);
    const size_t vertexBytes = vertexCount * vertexSize;
    const uint8_t* pVertices = reinterpret_cast< const uint8_t* >( Vertices.data() );
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), pVertices, GL_STATIC_DRAW);
}

void Mesh16::uploadIndices()
{
    if ( !VAO )
    {
        DE_ERROR("Malformed VAO")
        return;
    }

    if ( !IBO )
    {
        DE_ERROR("Malformed VAO and IBO")
        return;
    }

    if ( Indices.empty() )
    {
        DE_ERROR("No indices")
        return;
    }

    const size_t indexCount = Indices.size();
    const size_t indexSize = sizeof(uint32_t);
    const size_t indexBytes = indexCount * indexSize;
    const uint8_t* pIndices = reinterpret_cast< const uint8_t* >( Indices.data() );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBytes), pIndices, GL_STATIC_DRAW);
}
#endif

/*
#version 330 core

uniform int rows;
uniform int cols;
uniform vec3 size;
uniform sampler2D heightTex;

ivec2 gridCoord(int id, int cols) {
    int z = id / cols;
    int x = id - z * cols;
    return ivec2(x, z);
}

void main() {
    ivec2 c = gridCoord(gl_VertexID, cols);

    float fx = float(c.x) / float(cols - 1);
    float fz = float(c.y) / float(rows - 1);

    float y = texture(heightTex, vec2(fx, fz)).r;

    vec3 pos = vec3(fx, y, fz) * size;

    gl_Position = vec4(pos, 1.0);
}
*/

// ===========================================================================
Mesh16_Shader3D::Mesh16_Shader3D()
// ===========================================================================
    : m_driver(nullptr)
    , m_shader(nullptr)
    , m_u_mvp(-1)
    , m_u_tex(-1)
{
}

void Mesh16_Shader3D::destroy()
{
    //glDeleteProgram(m_programId);
}

void Mesh16_Shader3D::setDriver( de::gpu::VideoDriver* driver )
{
    m_driver = driver;
}

void Mesh16_Shader3D::setMaterial( const Mesh16_Material & material, const glm::mat4& modelMat )
{
    GL_VALIDATE

    if (!m_driver)
    {
        DE_ERROR("No shader")
        return;
    }

    if (!material.tex0.tex)
    {
        DE_ERROR("No tex0")
        return;
    }

    if (!m_shader)
    {
        const char* g_vs = R"(
            //#version 330 core
            //precision highp float;
            layout(location = 0) in vec4 a_pos; // x,y,z,t Position attribute
            uniform mat4 u_mvp;
            out float v_tex;
            void main()
            {
                gl_PointSize = 5.0;
                gl_Position = u_mvp * vec4(a_pos.xyz, 1.0);
                v_tex = a_pos.w;
            }
        )";

        const char* g_fs = R"(
            //#version 330 core
            //precision highp float;
            out vec4 o_fragColor;
            in float v_tex;
            uniform sampler2D u_tex;
            void main()
            {
                o_fragColor = texture( u_tex, vec2( v_tex, 0.0f ) );
            }
        )";
        m_shader = m_driver->createShader( "3DMesh16", g_vs, g_fs );
        if (m_shader)
        {
            DE_BENNI("Created shader 3DMesh16")
            m_driver->useShader(m_shader);
            m_u_mvp = glGetUniformLocation(m_shader->id, "u_mvp");
            m_u_tex = glGetUniformLocation(m_shader->id, "u_tex");
            GL_VALIDATE
        }
        else
        {
            DE_ERROR("No shader 3DMesh16")
            return;
        }
    }
    else
    {
        m_driver->useShader(m_shader);
    }

    // u_mvp
    glm::mat4 viewProjMat(1.0f);
    auto camera = m_driver->getCamera();
    if (camera)
    {
        viewProjMat = camera->getViewProjectionMatrix();
    }
    glm::mat4 u_mvp = viewProjMat * modelMat;
    glUniformMatrix4fv(m_u_mvp, 1, GL_FALSE, glm::value_ptr(u_mvp));
    GL_VALIDATE

    // u_tex
    glBindTextureUnit( 0, material.tex0.tex->id() );
    glUniform1i(m_u_tex, 0);
    GL_VALIDATE

    // if (material.alpha < 1.0f)
    // {
    //     m_driver->setBlend( de::gpu::Blend::alphaBlend() );
    // }
    // else
    // {
         m_driver->setBlend( de::gpu::Blend::disabled() );
    // }

    GL_VALIDATE
}



// ===========================================================================
Mesh16_Shader2D::Mesh16_Shader2D()
// ===========================================================================
    : m_driver(nullptr)
    , m_shader(nullptr)
    , m_u_screenSize(-1)
    , m_u_tex(-1)
{
}

void Mesh16_Shader2D::destroy()
{
    //glDeleteProgram(m_programId);
}

void Mesh16_Shader2D::setDriver( de::gpu::VideoDriver* driver )
{
    m_driver = driver;
}

void Mesh16_Shader2D::setMaterial(
        const Mesh16_Material & material,
        const de::Rectf& pos )
{
    GL_VALIDATE

    if (!m_driver)
    {
        DE_ERROR("No shader")
        return;
    }

    if (!material.tex0.tex)
    {
        DE_ERROR("No tex0")
        return;
    }

    if (!m_shader)
    {
        const char* g_vs = R"(
            //#version 330 core
            //precision highp float;
            layout(location = 0) in vec4 a_pos; // x,y,z,t Position attribute

            uniform vec2 u_screenSize;

            uniform vec4 u_posTransform;

            out float v_tex;
            void main()
            {
                gl_PointSize = 5.0;

                vec2 T = u_posTransform.xy;
                vec2 S = u_posTransform.zw;
                vec2 pos = S * a_pos.xy + T;

                vec2 ndc = (2.0 * vec2( pos.x, u_screenSize.y - 1.0 - pos.y ) / u_screenSize ) - 1.0;
                gl_Position = vec4( ndc, 0.0, 1.0 );

                v_tex = a_pos.w;
            }
        )";

        const char* g_fs = R"(
            //#version 330 core
            //precision highp float;

            out vec4 fragColor;
            in float v_tex;

            uniform sampler2D u_tex;
         // uniform float u_alpha;
         // uniform vec4 u_texTransform;

            void main()
            {
                // if (v_tex < u_alpha)
                // {
                //     fragColor.a = 0.0;
                // }
                // else
                // {
                    vec4 Td = texture( u_tex, vec2( v_tex, 0.0f ) );
                    fragColor = Td;
                //}
            }
        )";
        m_shader = m_driver->createShader( "2DMesh16", g_vs, g_fs );
        if (m_shader)
        {
            DE_BENNI("Created shader 2DMesh16")
            m_driver->useShader(m_shader);
            m_u_screenSize = glGetUniformLocation(m_shader->id, "u_screenSize");
            m_u_tex = glGetUniformLocation(m_shader->id, "u_tex");
            m_u_posTransform = glGetUniformLocation(m_shader->id, "u_posTransform");
            //m_u_texTransform = glGetUniformLocation(m_shader->id, "u_texTransform");
            //m_u_alpha = glGetUniformLocation(m_shader->id, "u_alpha");
            GL_VALIDATE
        }
        else
        {
            DE_ERROR("No shader 2DMesh16")
            return;
        }
    }
    else
    {
        m_driver->useShader(m_shader);
    }
    // u_screenSize
    const int w = m_driver->getScreenWidth();
    const int h = m_driver->getScreenHeight();
    glm::vec2 u_screenSize{ w, h };
    glUniform2fv(m_u_screenSize, 1, glm::value_ptr( u_screenSize ));
    GL_VALIDATE

    // u_posTransform
    glm::vec4 u_posTransform{ pos.x(),pos.y(),pos.w(),pos.h() };
    glUniform4fv(m_u_posTransform, 1, glm::value_ptr( u_posTransform ));
    GL_VALIDATE

    // u_tex
    glBindTextureUnit( 0, material.tex0.tex->id() );
    glUniform1i(m_u_tex, 0);
    GL_VALIDATE

    // u_alpha
    //glUniform1f(m_u_alpha, material.alpha);

    // u_texTransform
    //glm::vec4 u_texTransform{ 0,0,1,1 };
    //glUniform4fv(m_u_texTransform, 1, glm::value_ptr( u_texTransform ));

    // if (material.alpha < 1.0f)
    // {
    //     m_driver->setBlend( de::gpu::Blend::alphaBlend() );
    // }
    // else
    // {
        m_driver->setBlend( de::gpu::Blend::disabled() );
    //}
    GL_VALIDATE
}


/*
void Mesh16_Shader::setModelMat( glm::mat4 const & modelMat )
{
    m_modelMat = modelMat;
}
void Mesh16_Shader::setModelPos( glm::vec3 const & modelPos )
{
    m_modelMat = glm::translate(glm::mat4(1.0f), modelPos);
}
void Mesh16_Shader::setModelPos( float x, float y, float z )
{
    setModelPos( V3(x,y,z) );
}
void Mesh16_Shader::setModelScale( glm::vec3 const & scale )
{
    m_modelMat = glm::scale(glm::mat4(1.0f), scale);
}
void Mesh16_Shader::setModelScale( float x, float y, float z )
{
    setModelScale( V3(x,y,z) );
}
void Mesh16_Shader::setModelEuler( glm::vec3 const & eulerAngles )
{
    auto R_x = glm::rotate( glm::mat4(1.0f), eulerAngles.x, glm::vec3(1,0,0) );
    auto R_y = glm::rotate( glm::mat4(1.0f), eulerAngles.y, glm::vec3(0,1,0) );
    auto R_z = glm::rotate( glm::mat4(1.0f), eulerAngles.z, glm::vec3(0,0,1) );
    auto R = R_x * (R_y * R_z);
    m_modelMat = R;
}
void Mesh16_Shader::setModelEuler( float x, float y, float z )
{
    setModelEuler( V3(x,y,z) );
}
*/

#if 0
void Mesh16::createWavMatrix(Mesh16 & m, glm::vec3 const & d,
                                DE_AlignedFloatShiftMatrix const & table,
                                DE_AlignedFloatVector& rowBuffer )
{
    const size_t cols = table.columnCount();
    const size_t rows = table.rowCount();

    m.PrimType = de::gpu::PrimitiveType::Triangles;
    m.Vertices.clear();
    m.Vertices.reserve( /* top */ cols * rows +
                       /* front */ 2*cols );
    m.Indices.clear();
    m.Indices.reserve( /* top */ ((cols - 1) * (rows - 1) * 6) +
                      /* front */((cols - 1) * 6) );

    const float dx = d.x / float(cols-1);
    const float dy = d.y / 2.0f;
    const float dz = d.z / float(rows-1);

    // [Top] out vertices: pcm expected in range {-1,1}:
    for (size_t j = 0; j < rows; j++)
    {
        const float* row = table.m_view[j];
        for (size_t i = 0; i < cols; i++)
        {
            const float s = row[i]; // expected in range [-1,1]
            const float t = std::clamp((0.5f * s) + 0.5f, 0.f, 1.f); // expected in range [0,1]
            const float x = dx * i;
            const float y = dy * s;
            const float z = dz * j;
            m.Vertices.push_back( Mesh16_Vertex( x, y, z, t ) );
        }
    }
    // [Top] out quads:
    for (size_t j = 0; j < rows-1; j++)
    {
        for (size_t i = 0; i < cols-1; i++)
        {
            const uint32_t A = (cols * j) + i;           // A - 0
            const uint32_t B = (cols * (j+1)) + i;       // B - 1
            const uint32_t C = (cols * (j+1)) + i+1; // C - 3
            const uint32_t D = (cols * j) + i+1;     // D - 2
            m.addIndexedQuad( A,B,C,D );
        }
    }

    // [Front] vertices:
    const size_t nF = m.Vertices.size();
    for (size_t i = 0; i < cols; i++)
    {
        auto vF = m.Vertices[i];
        auto vO = vF;
        vO.set_y(0.0f);
        vO.set_t(0.0f);
        m.Vertices.push_back( vO );
        m.Vertices.push_back( vF );
    }
    // [Front] indices:
    for (size_t i = 0; i < cols - 1; ++i)
    {
        const uint32_t A = nF + 2*i;          // A - 0
        const uint32_t B = nF + 2*i + 1;      // B - 1
        const uint32_t C = nF + 2*(i+1) + 1;  // C - 3
        const uint32_t D = nF + 2*(i+1);      // D - 2
        m.addIndexedQuad( A,B,C,D );
    }

    // [Left] vertices:
    const size_t nL = m.Vertices.size();
    for (size_t i = 0; i < rows; i++)
    {
        auto vL = m.Vertices[cols * (rows-1-i)];
        auto vO = vL;
        vO.set_y(0.0f);
        vO.set_t(0.0f);
        m.Vertices.push_back( vO );
        m.Vertices.push_back( vL );
    }
    // [Left] indices:
    for (size_t i = 0; i < rows - 1; i++)
    {
        const uint32_t A = nL + 2*i;          // A - 0
        const uint32_t B = nL + 2*i + 1;      // B - 1
        const uint32_t C = nL + 2*(i+1) + 1;  // C - 3
        const uint32_t D = nL + 2*(i+1);      // D - 2
        m.addIndexedQuad( A,B,C,D );
    }

    // [Right] vertices:
    const size_t nR = m.Vertices.size();
    for (size_t i = 0; i < rows; i++)
    {
        auto vR = m.Vertices[cols * i + cols - 1];
        auto vO = vR;
        vO.set_y(0.0f);
        vO.set_t(0.0f);
        m.Vertices.push_back( vO );
        m.Vertices.push_back( vR );
    }
    // Right indices:
    for (size_t i = 0; i < rows - 1; i++)
    {
        const uint32_t A = nR + 2*i;          // A - 0
        const uint32_t B = nR + 2*i + 1;      // B - 1
        const uint32_t C = nR + 2*(i+1) + 1;  // C - 3
        const uint32_t D = nR + 2*(i+1);      // D - 2
        m.addIndexedQuad( A,B,C,D );
    }

    m.upload( true );
}

void Mesh16::createWavMatrix01(Mesh16 & m,
                                  glm::vec3 const & d,
                                  DE_AlignedFloatShiftMatrix const & table,
                                  DE_AlignedFloatVector& rowBuffer )
{
    const size_t cols = table.columnCount();
    const size_t rows = table.rowCount();

    m.PrimType = de::gpu::PrimitiveType::Triangles;
    m.Vertices.clear();
    m.Vertices.reserve( /* top */   cols * rows +
                       /* front */ 2*cols +
                       ///* back */ 2*cols +
                       /* left */  2*rows +
                       /* right */ 2*rows
                       );
    m.Indices.clear();
    m.Indices.reserve(  /* top */   (cols - 1) * (rows - 1) * 6 +
                      /* front */ (cols - 1) * 6 +
                      ///* back */ (cols - 1) * 6 +
                      /* left */  (rows - 1) * 6 +
                      /* right */ (rows - 1) * 6
                      );

    const float dx = d.x / float(cols-1);
    const float dy = d.y;
    const float dz = d.z / float(rows-1);

    // Top:
    for (size_t j = 0; j < rows; j++)
    {
        const float* row = table.m_view[j];
        for (size_t i = 0; i < cols; i++)
        {
            const float s = row[i]; // expected in range [0,1]
            const float t = std::abs(s); // expected in range [0,1]
            const float x = dx * i;
            const float y = dy * s;
            const float z = dz * j;
            m.Vertices.push_back( Mesh16_Vertex( x, y, z, t ) );
        }
    }
    // Top:
    for (size_t j = 0; j < rows-1; j++)
    {
        for (size_t i = 0; i < cols-1; i++)
        {
            const uint32_t A = (cols * j) + i;           // A - 0
            const uint32_t B = (cols * j) + i + 1;       // B - 1
            const uint32_t C = (cols * (j + 1)) + i + 1; // C - 3
            const uint32_t D = (cols * (j + 1)) + i;     // D - 2
            m.addIndexedQuad(A,B,C,D);
        }
    }

    // [Front]:
    const uint32_t vF = m.Vertices.size();

    table.getFrontVector(rowBuffer);
    for (size_t i = 0; i < rowBuffer.size(); ++i)
    {
        const float s = rowBuffer[i];
        const float t = std::abs(s); // expected in range [0,1]
        const float x = dx * i;
        const float y = dy * s;
        const float z = 0.0f;
        m.Vertices.push_back( Mesh16_Vertex( x, 0, z, 0 ) );
        m.Vertices.push_back( Mesh16_Vertex( x, y, z, t ) );
    }
    for (size_t i = 0; i < rowBuffer.size() - 1; ++i)
    {
        const uint32_t A =  vF + 2*i;          // A - 0
        const uint32_t B =  vF + 2*i + 1;      // B - 1
        const uint32_t C =  vF + 2*(i+1) + 1;  // C - 3
        const uint32_t D =  vF + 2*(i+1);      // D - 2
        m.addIndexedQuad(A,B,C,D);
    }

    // [Left] vertices:
    const uint32_t vL = m.Vertices.size();

    table.getLeftVector( rowBuffer );
    for (size_t i = 0; i < rowBuffer.size(); i++)
    {
        const float s = rowBuffer[i];
        const float t = std::abs(s); // expected in range [0,1]
        const float x = 0.0f;
        const float y = dy * s;
        const float z = d.z - dz * i;
        m.Vertices.push_back( Mesh16_Vertex( x, 0, z, 0 ) );
        m.Vertices.push_back( Mesh16_Vertex( x, y, z, t ) );
    }
    // [Left] indices:
    for (size_t i = 0; i < rowBuffer.size() - 1; i++)
    {
        const uint32_t A = vL + 2*i;          // A - 0
        const uint32_t B = vL + 2*i + 1;      // B - 1
        const uint32_t C = vL + 2*(i+1) + 1;  // C - 3
        const uint32_t D = vL + 2*(i+1);      // D - 2
        m.addIndexedQuad(A,B,C,D);
    }

    // [Right] vertices:
    const uint32_t vR = m.Vertices.size();

    table.getRightVector( rowBuffer );
    for (size_t i = 0; i < rowBuffer.size(); i++)
    {
        const float s = rowBuffer[i];
        const float t = std::abs(s); // expected in range [0,1]
        const float x = d.x;
        const float y = dy * s;
        const float z = dz * i;
        m.Vertices.push_back( Mesh16_Vertex( x, 0, z, 0 ) );
        m.Vertices.push_back( Mesh16_Vertex( x, y, z, t ) );
    }
    // [Right] indices:
    for (size_t i = 0; i < rowBuffer.size() - 1; i++)
    {
        const uint32_t A = vR + 2*i;          // A - 0
        const uint32_t B = vR + 2*i + 1;      // B - 1
        const uint32_t C = vR + 2*(i+1) + 1;  // C - 3
        const uint32_t D = vR + 2*(i+1);      // D - 2
        m.addIndexedQuad(A,B,C,D);
    }

    m.upload( true );
}


// size3d.x = 2000.0f;
// size3d.y = 500.0f;
// size3d.z = 300.0f;
void Mesh16::createFftMatrix(Mesh16 & m, glm::vec3 const & d,
                                DE_AlignedFloatShiftMatrix const & table,
                                DE_AlignedFloatVector& rowBuffer )
{
    const float dBmin = -120.0f;
    const float dBmax = 60.0f;

    auto cols = table.columnCount();
    auto rows = table.rowCount();
    if ( rows < 1 || cols < 1 )
    {
        DE_ERROR("No rows or cols")
        return;
    }

    float dx = d.x / float ( cols - 1 ); // / (sampleRate_over_fftSize * colCount);
    float dy = d.y;
    float dz = d.z / float ( rows - 1 );

    uint32_t mode = 1;
    uint32_t scaleXmode = 1;

    int ch = 0;

    m.PrimType = de::gpu::PrimitiveType::Triangles;

    m.Vertices.clear();
    m.Vertices.reserve(  /* top */   rows * cols
                       // + /* front */ 2*cols
                       // + /* back */  2*cols
                       // + /* left */  2*rows
                       // + /* right */ 2*rows
                       );
    m.Indices.clear();
    m.Indices.reserve(  /* top */   6*(rows) * (cols)
                      // + /* front */ 6*(cols-1)
                      // + /* back */  6*(cols-1)
                      // + /* left */  6*(rows-1)
                      // + /* right */ 6*(rows-1)
                      );

    // X-axis is scaled logarithmicly.
    if (cols != m.XMap.size())
    {
        m.XMap.resize(cols);

        float xInv = 1.0f / log10f( float(cols) );

        for ( size_t col = 0; col < cols; col++ )
        {
            float k = log10f( float(col+1) ) * xInv; //  - 1.5f -1 = shift by 10^-1
            m.XMap[ col ] = d.x * k;
        }
    }

    float dBrange = dBmax - dBmin;
    if ( dBrange < 1.0f ) dBrange = 1.0f;
    float dBrangeInv = 1.0f / dBrange;

    // auto bb = table.getMinMax();
    // //DE_WARN("table.getMinMax(", bb.m_min,",",bb.m_max,")")

    // float dB = 0;
    // float dBmin = bb.m_min;
    // float dBmax = bb.m_max;
    // Matrix Top

    if ( scaleXmode == 0 ) // X-axis is linear.
    {
        for ( size_t row = 0; row < rows; row++ )
        {
            for ( size_t col = 0; col < cols; col++ )
            {
                float dB = table.getPixel( col, row );  // The row data
                float t = (dB - dBmin) * dBrangeInv;
                float x = dx * col;
                float y = dy * t;
                float z = dz * row;
                m.Vertices.push_back( Mesh16_Vertex(x,y,z,t) );
            }
        }
    }
    else if ( scaleXmode == 1 ) // X-axis is scaled logarithmicly.
    {
        // Matrix Top
        for ( size_t row = 0; row < rows; row++ )
        {
            for ( size_t col = 0; col < cols; col++ )
            {
                float dB = table.getPixel( col, row );  // The row data
                float t = (dB - dBmin) * dBrangeInv;
                float x = m.XMap[ col ];
                float y = dy * t;
                float z = dz * row;
                m.Vertices.push_back( Mesh16_Vertex(x,y,z,t) );
            }
        }
    }

    for ( size_t j = 0; j < rows-1; j++ )
    {
        for ( size_t i = 0; i < cols-1; i++ )
        {
            const uint32_t A = (cols * j) + i;       // A - 0
            const uint32_t B = (cols * (j+1)) + i;   // B - 1
            const uint32_t C = (cols * (j+1)) + i+1; // C - 3
            const uint32_t D = (cols * j) + i+1;     // D - 2
            m.addIndexedQuad( A,B,C,D );
        }
    }

#if 0
    // [Front]:
    const size_t nF = m.Vertices.size();
    for (size_t i = 0; i < cols; i++)
    {
        auto vF = m.Vertices[i];
        auto vO = vF;
        vO.set_y(0.0f);
        vO.set_t(0.0f);
        m.Vertices.push_back( vO );
        m.Vertices.push_back( vF );
    }
    for (size_t i = 0; i < cols - 1; i++)
    {
        const uint32_t A = nF + 2*i;           // A - 0
        const uint32_t B = nF + 2*i + 1;       // B - 1
        const uint32_t C = nF + 2*(i+1) + 1;   // C - 3
        const uint32_t D = nF + 2*(i+1);       // D - 2
        m.addIndexedQuad(A,B,C,D);
    }

    // [Left]:
    const size_t nL = m.Vertices.size();
    for (size_t i = 0; i < rows; i++)
    {
        auto vL = m.Vertices[cols * (rows-1-i)];
        auto vO = vL;
        vO.set_y(0.0f);
        vO.set_t(0.0f);
        m.Vertices.push_back( vO );
        m.Vertices.push_back( vL );
    }
    for (size_t i = 0; i < rows - 1; i++)
    {
        const uint32_t A = nL + 2*i;           // A - 0
        const uint32_t B = nL + 2*i + 1;       // B - 1
        const uint32_t C = nL + 2*(i+1) + 1;   // C - 3
        const uint32_t D = nL + 2*(i+1);       // D - 2
        m.addIndexedQuad(A,B,C,D);
    }

    // [Right]:
    const size_t nR = m.Vertices.size();
    for (size_t i = 0; i < rows; i++)
    {
        auto vR = m.Vertices[cols * i + cols - 1];
        auto vO = vR;
        vO.set_y(0.0f);
        vO.set_t(0.0f);
        m.Vertices.push_back( vO );
        m.Vertices.push_back( vR );
    }
    for (size_t i = 0; i < rows - 1; i++)
    {
        const uint32_t A = nR + 2*i;           // A - 0
        const uint32_t B = nR + 2*i + 1;       // B - 1
        const uint32_t C = nR + 2*(i+1) + 1;   // C - 3
        const uint32_t D = nR + 2*(i+1);       // D - 2
        m.addIndexedQuad(A,B,C,D);
    }

    //de::gpu::SMeshBufferTool::computeNormals( o );
#endif
    m.upload( true );
}
#endif
