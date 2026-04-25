#include <de/gpu/renderer/Line3D_Renderer.h>
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>

namespace de {
namespace gpu {

void
Line3D_MeshBuffer::destroy()
{
    if ( m_vao )
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    if ( m_vbo_vertices )
    {
        glDeleteBuffers(1, &m_vbo_vertices);
        m_vbo_vertices = 0;
    }

    if ( m_vbo_indices )
    {
        glDeleteBuffers(1, &m_vbo_indices);
        m_vbo_indices = 0;
    }
}

void
Line3D_MeshBuffer::upload( bool bForceUpload )
{
    bool bNeedUpload = bForceUpload | m_needUpload;
    if ( !m_vao ) { bNeedUpload = true; }
    if ( !m_vbo_vertices ) { bNeedUpload = true; }
    if ( !m_vbo_indices && m_indices.size() > 0 ) { bNeedUpload = true; }

    m_needUpload = false;

    if ( !bNeedUpload )
    {
        return;
    }

    if ( !m_vao )
    {
        glGenVertexArrays(1, &m_vao);
        //DE_DEBUG("Create m_vao")
    }

    if ( !m_vbo_vertices )
    {
        glGenBuffers(1, &m_vbo_vertices);
        //DE_DEBUG("Create m_vbo_vertices")
    }

    if ( !m_vbo_indices && m_indices.size() > 0 )
    {
        glGenBuffers(1, &m_vbo_indices);
        //DE_DEBUG("Create m_vbo_indices")
    }

    glBindVertexArray(m_vao);

    // VertexBufferDefinition:
    // VertexSize = 16 Bytes per vertex.
    // VertexAttributeLocation[0] :: XYZ Position3D (vec3f), 12 Bytes.
    // VertexAttributeLocation[1] :: RGBA Color (vec4ub), 4 Bytes.
    if ( m_vbo_vertices )
    {
        const auto nVertices = m_vertices.size();
        const auto pVertices = reinterpret_cast< const uint8_t* >( m_vertices.data() );
        glBindBuffer(GL_ARRAY_BUFFER,
                     m_vbo_vertices);
        glBufferData(GL_ARRAY_BUFFER,
                     GLsizeiptr(nVertices * sizeof(Line3D_Vertex)),
                     pVertices,
                     GL_STATIC_DRAW);
        glVertexAttribPointer( 0, 3,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(Line3D_Vertex),
                              reinterpret_cast<void*>(0) );
        glEnableVertexAttribArray( 0 );
        glVertexAttribIPointer( 1, 1,
                               GL_UNSIGNED_INT,
                               sizeof(Line3D_Vertex),
                               reinterpret_cast<void*>(12) );
        glEnableVertexAttribArray( 1 );
    }

    // IndexBufferDefinition:
    // IndexSize = 4 Bytes per index.
    if ( m_vbo_indices )
    {
        const auto nIndices = m_indices.size();
        const auto pIndices = reinterpret_cast< const uint8_t* >( m_indices.data() );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                     m_vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     GLsizeiptr(nIndices * sizeof(uint32_t)),
                     pIndices,
                     GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

void
Line3D_MeshBuffer::draw()
{
    //upload( meshBuffer );

    if (m_vao)
    {
        glBindVertexArray( m_vao );
    }

    if (m_vbo_vertices)
    {
        const GLenum primType = PrimitiveType::toOpenGL( m_primType );
        if ( m_vbo_indices )
        {
            glDrawElements( primType,
                            GLint(m_indices.size()),
                            GL_UNSIGNED_INT,
                            nullptr );
        }
        else
        {
            glDrawArrays(   primType,
                            0,
                            GLint(m_vertices.size()) );
        }
    }

    if (m_vao)
    {
        glBindVertexArray(0);
    }
}


// ===========================================================================
void Line3D_Renderer::initShader()
// ===========================================================================
{
    if (!m_driver) { DE_ERROR("No driver!") return; }

    if (m_shader)
    {
        return;
    }

    std::string vs = R"(
        //precision highp float;

        layout(location = 0) in vec3 a_pos; // XYZ 3D position attribute
        layout(location = 1) in uint a_color; // RGBA 32-bit | in lowp vec4 a_color;

        uniform mat4 u_modelMat;
        uniform mat4 u_viewMat;
        uniform mat4 u_projMat;

        out vec4 v_color;

        void main()
        {
            v_color = unpackUnorm4x8(a_color); // = clamp( vec4( a_color ) * (1.0 / 255.0), vec4(0), vec4(1) );
            gl_PointSize = 5.0;
            gl_Position = u_projMat * (u_viewMat * (u_modelMat * vec4(a_pos, 1.0)));
        }
    )";

    std::string fs = R"(
        //precision highp float;

        out vec4 fragColor;

        in vec4 v_color;

        void main()
        {
            fragColor = clamp( v_color, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
        }
    )";

    m_shader = m_driver->createShader( "Line3D_Shd", vs, fs );

    // int modelLoc = glGetUniformLocation(m_shaderId, "u_modelMat");
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_modelMat));

    // glm::mat4 m_viewMat = pCamera->getViewMatrix();
    // int viewLoc = glGetUniformLocation(m_shaderId, "u_viewMat");
    // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMat));

    // glm::mat4 m_projMat = pCamera->getProjectionMatrix();
    // int projLoc = glGetUniformLocation(m_shaderId, "u_projMat");

    // m_u_modelMat =
    // m_modelMat = glm::mat4(1.0f);

    // m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, 500.0f, 1000.0f));

    // m_projMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 38000.0f);
}

// ===========================================================================
void Line3D_Renderer::init( VideoDriver* driver )
// ===========================================================================
{
    m_driver = driver;

    if (!m_driver) { DE_ERROR("No driver!") return; }

    initShader();
}

void Line3D_Renderer::setMaterial( bool bBlend )
{
    // =========================== bind ===========================
    m_driver->useShader(m_shader);

    // =========================== uniforms ===========================
    m_shader->setMat4f("u_modelMat", m_driver->getModelMatrix());
    glm::mat4 viewMat(1.0f);
    glm::mat4 projMat(1.0f);
    auto camera = m_driver->getCamera();
    if (camera)
    {
        viewMat = camera->getViewMatrix();
        projMat = camera->getProjectionMatrix();
    }
    m_shader->setMat4f("u_viewMat",viewMat);
    m_shader->setMat4f("u_projMat",projMat);

    // =========================== state ===========================
    auto state = m_driver->getState();
    if (bBlend)
    {
        state.blend = Blend::alphaBlend();
    }
    else
    {
        state.blend = Blend::disabled();
    }

    m_driver->setState(state);
}

void Line3D_Renderer::draw3DLine( const glm::vec3& A, const glm::vec3& B,
                                 const uint32_t cA, const uint32_t cB )
{
    Line3D_MeshBuffer o;
    o.m_primType = PrimitiveType::Lines;
    o.m_vertices.emplace_back( A.x, A.y, A.z, cA );
    o.m_vertices.emplace_back( B.x, B.y, B.z, cB );
    o.upload();

    setMaterial(true);
    o.draw();
    o.destroy();
}

void Line3D_Renderer::draw3DLineBox( const glm::vec3& minPos, const glm::vec3& maxPos,
                                    const uint32_t color, bool bUseIndices )
{
    Line3D_Box box;
    auto s = maxPos - minPos;
    box.a.pos = minPos + V3( 0, 0, 0 );
    box.b.pos = minPos + V3( 0, s.y, 0 );
    box.c.pos = minPos + V3( s.x, s.y, 0 );
    box.d.pos = minPos + V3( s.x, 0, 0 );
    box.e.pos = minPos + V3( 0, 0, s.z );
    box.f.pos = minPos + V3( 0, s.y, s.z );
    box.g.pos = minPos + V3( s.x, s.y, s.z );
    box.h.pos = minPos + V3( s.x, 0, s.z );

    box.a.color = color;
    box.b.color = color;
    box.c.color = color;
    box.d.color = color;
    box.e.color = color;
    box.f.color = color;
    box.g.color = color;
    box.h.color = color;

    draw3DLineBox( box, bUseIndices );
}

void Line3D_Renderer::draw3DLineBox( const Line3D_Box& box, bool bUseIndices )
{
    Line3D_MeshBuffer o;
    o.m_primType = PrimitiveType::Lines;

    if (bUseIndices)
    {
        const auto v = o.m_vertices.size();

        o.m_vertices.emplace_back(box.a);
        o.m_vertices.emplace_back(box.b);
        o.m_vertices.emplace_back(box.c);
        o.m_vertices.emplace_back(box.d);
        o.m_vertices.emplace_back(box.e);
        o.m_vertices.emplace_back(box.f);
        o.m_vertices.emplace_back(box.g);
        o.m_vertices.emplace_back(box.h);

        auto addLine = [&]( uint32_t iA, uint32_t iB )
        {
            o.m_indices.push_back( v + iA );
            o.m_indices.push_back( v + iB );
        };

        addLine(0,1); addLine(1,2); addLine(2,3); addLine(3,0); // AB, BC, CD, DA
        addLine(0,4); addLine(1,5); addLine(2,6); addLine(3,7); // AE, BF, CG, DH
        addLine(4,5); addLine(5,6); addLine(6,7); addLine(7,4);
    }
    else
    {
        auto addLine = [&]( Line3D_Vertex a, Line3D_Vertex b )
        {
            o.m_vertices.emplace_back(a); o.m_vertices.emplace_back(b); // AB
        };

        addLine( box.a,box.b ); addLine( box.b,box.c ); addLine( box.c,box.d ); addLine( box.d,box.a ); // AB, BC, CD, DA
        addLine( box.a,box.e ); addLine( box.b,box.f ); addLine( box.c,box.g ); addLine( box.d,box.h ); // AE, BF, CG, DH
        addLine( box.e,box.f ); addLine( box.f,box.g ); addLine( box.g,box.h ); addLine( box.h,box.e ); // EF, FG, GH, HE

    }
    o.upload();

    setMaterial(true);
    o.draw();
    o.destroy();
}

} // end namespace gpu.
} // end namespace de.
