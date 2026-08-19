#include "GLUtil_Ben16Tex.h"

void MeshBuffer8::destroy()
{
    if ( VAO ) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
    if ( VBO ) { glDeleteBuffers(1, &VBO); VBO = 0; }
    if ( IBO ) { glDeleteBuffers(1, &IBO); IBO = 0; }
}

void MeshBuffer8::draw() const
{
    if (VAO) { glBindVertexArray( VAO ); }

    if (VBO)
    {
        GLenum const primType = de::gpu::PrimitiveType::toOpenGL( PrimType );
        if ( IBO )
        {
            glDrawElements( primType, GLint(Indices.size()), GL_UNSIGNED_INT, nullptr );
        }
        else
        {
            glDrawArrays( primType, 0, GLint(Vertices.size()) );
        }
    }

    if (VAO) { glBindVertexArray(0); }
}

void MeshBuffer8::upload( bool bForceUpload )
{
    bool bNeedUpload = bForceUpload;
    if ( !VAO ) { glGenVertexArrays(1, &VAO); bNeedUpload = true; }
    if ( !VBO ) { glGenBuffers(1, &VBO); bNeedUpload = true; }
    if ( !IBO && Indices.size() > 0 ) { glGenBuffers(1, &IBO); bNeedUpload = true; }

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
            glVertexAttribPointer( 0, 4, GL_HALF_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(0) );
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
Shader8::Shader8()
// ===========================================================================
    : m_modelMat(1.0f)
    , m_viewMat(1.0f)
    , m_projMat(1.0f)
    , m_camera(nullptr)
    , m_programId(0)
    , m_u_modelMat(-1)
    , m_u_viewMat(-1)
    , m_u_projMat(-1)
    , m_blend(false)
{
    // u_modelMat = M4(1.0f);
    // u_viewMat = glm::translate(M4(1.0f), V3(500.0f, 500.0f, 1000.0f));
    // u_projMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 38000.0f);
}

// ===========================================================================
Shader8::~Shader8()
// ===========================================================================
{

}
// ===========================================================================
void Shader8::ensureShader()
// ===========================================================================
{
    if (m_programId)
    {
        return;
    }

    const char* g_vs = R"(
        #version 330 core

        //precision highp float;

        layout(location = 0) in vec4 a_spaceTime; // x,y,z,t Position attribute
        
        uniform mat4 u_mvp;

        out float v_tex;

        void main()
        {
            gl_PointSize = 5.0;
            vec3 a_pos = a_spaceTime.xyz;
            gl_Position = u_mvp * vec4(a_pos, 1.0);
            v_tex = a_spaceTime.w;
        }
    )";

    const char* g_fs = R"(
        #version 330 core

        //precision highp float;

        out vec4 o_fragColor;
        
        in float v_tex;

        uniform Sampler2D u_tex;

        void main()
        {
            o_fragColor = texture( u_tex, vec2( v_tex, 0.5f ) );
        }
    )";


    m_programId = GLUtil_createShaderProgram( g_vs, g_fs );
    m_u_modelMat = glGetUniformLocation(m_programId, "u_modelMat");
    m_u_viewMat = glGetUniformLocation(m_programId, "u_viewMat");
    m_u_projMat = glGetUniformLocation(m_programId, "u_projMat");
    //m_u_color = glGetUniformLocation(m_programId, "u_color");
}

void Shader8::setCamera( de::gpu::Camera* camera )
{
    m_camera = camera;
}

void Shader8::setBlend( bool bBlend )
{
    m_blend = bBlend;
}

void Shader8::drawAt( const MeshBuffer & mesh, float x, float y, float z )
{
    setModelPos(V3(x,y,z));
    draw(mesh);

}
void Shader8::drawAt( const MeshBuffer8 & mesh, const V3& pos )
{
    setModelPos(pos);
    draw(mesh);
}

void Shader8::draw( const MeshBuffer8 & mesh )
{
    ensureShader();

    //GT_upload( m, bForceUpload );

    if (m_camera)
    {
        m_viewMat = m_camera->getViewMatrix();
        m_projMat = m_camera->getProjectionMatrix();
    }
    else
    {
        //m_viewMat = glm::mat4(1.0f);
        //m_projMat = glm::mat4(1.0f);
    }
    glUseProgram(m_programId);
    glUniformMatrix4fv(m_u_modelMat, 1, GL_FALSE, glm::value_ptr(m_modelMat));
    glUniformMatrix4fv(m_u_viewMat, 1, GL_FALSE, glm::value_ptr(m_viewMat));
    glUniformMatrix4fv(m_u_projMat, 1, GL_FALSE, glm::value_ptr(m_projMat));
    //glUniform4f(m_u_color, 0.0f, 0.8f, 1.0f, 1.0f); // Solid color

    if (m_blend)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }

    mesh.draw();
}

void Shader8::deinit()
{
    glDeleteProgram(m_programId);
}
void Shader8::resetModelMat()
{
    m_modelMat = glm::mat4(1.0f);
}
void Shader8::setModelMat( glm::mat4 const & modelMat )
{
    m_modelMat = modelMat;
}
void Shader8::setModelPos( glm::vec3 const & modelPos )
{
    m_modelMat = glm::translate(glm::mat4(1.0f), modelPos);
}
void Shader8::setModelScale( glm::vec3 const & scale )
{
    m_modelMat = glm::scale(glm::mat4(1.0f), scale);
}
void Shader8::setModelEuler( glm::vec3 const & eulerAngles )
{
    auto R_x = glm::rotate( glm::mat4(1.0f), eulerAngles.x, glm::vec3(1,0,0) );
    auto R_y = glm::rotate( glm::mat4(1.0f), eulerAngles.y, glm::vec3(0,1,0) );
    auto R_z = glm::rotate( glm::mat4(1.0f), eulerAngles.z, glm::vec3(0,0,1) );
    auto R = R_x * (R_y * R_z);
    m_modelMat = R;
}
