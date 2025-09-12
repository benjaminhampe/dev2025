#include <de/terrain/HTerrain.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gpu {

// ===========================================================================
HMesh::HMesh()
// ===========================================================================
{
    m_primitiveType = PrimitiveType::Triangles;
    m_vao = 0;
    m_vbo_vertices = 0;
    m_vbo_indices = 0;
    m_vbo_instanceMat = 0;
}

void
HMesh::upload( bool bForceUpload )
{
    bool bNeedUpload = bForceUpload;

    if (!m_vbo_vertices)
    {
        glGenBuffers(1, &m_vbo_vertices);
        bNeedUpload = true;
    }

    if (!m_vbo_indices && m_indices.size())
    {
        glGenBuffers(1, &m_vbo_indices);
        bNeedUpload = true;
    }

    if (!m_vbo_instanceMat)
    {
        glGenBuffers(1, &m_vbo_instanceMat);
        bNeedUpload = true;
    }

    if (!m_vao)
    {
        glGenVertexArrays(1, &m_vao);
        bNeedUpload = true;
    }

    if (!bNeedUpload)
    {
        return;
    }

    glBindVertexArray(m_vao);

    int k = 0;
	
    // [vbo] vertices
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER,
                 m_vertices.size() * sizeof(HVertex),
                 m_vertices.data(), GL_STATIC_DRAW);
    // a_pos
    glVertexAttribPointer( k, 3, GL_SHORT, GL_TRUE, sizeof(HVertex), (void*)(0) );
    glEnableVertexAttribArray( k ); k++;
    // a_normal
    glVertexAttribPointer( k, 3, GL_SHORT, GL_TRUE, sizeof(HVertex), (void*)(6) );
    glEnableVertexAttribArray( k ); k++;
    // a_tex
    glVertexAttribPointer( k, 2, GL_SHORT, GL_TRUE, sizeof(HVertex), (void*)(12) );
    glEnableVertexAttribArray( k ); k++;

    // [vbo] instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), 
					m_instanceMat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0));
    glVertexAttribDivisor(k, 1);
    glEnableVertexAttribArray( k ); k++;
    glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(16));
    glVertexAttribDivisor(k, 1);
    glEnableVertexAttribArray( k ); k++;
    glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(32));
    glVertexAttribDivisor(k, 1);
    glEnableVertexAttribArray( k ); k++;
    glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(48));
    glVertexAttribDivisor(k, 1);
    glEnableVertexAttribArray( k ); k++;

    if (m_vbo_indices)
    {
        const auto & indices = m_indices;
        const size_t indexCount = indices.size();
        const size_t indexSize = sizeof(uint32_t);
        const size_t indexBytes = indexCount * indexSize;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBytes), 
		              m_indices.data(), GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (m_vbo_indices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    GL_VALIDATE;
}

void HMesh::draw() const
{
    if (m_instanceMat.empty()) { DE_WARN("No instance") return; }

    glBindVertexArray(m_vao);

    const GLenum primType = PrimitiveType::toOpenGL( m_primitiveType );

    if (m_indices.empty())
    {
        glDrawArraysInstanced(primType, 0, m_vertices.size(), m_instanceMat.size());
    }
    else
    {
        glDrawElementsInstanced(primType, GLsizei(m_indices.size()), GL_UNSIGNED_INT, nullptr, m_instanceMat.size());
    }
    GL_VALIDATE;

    glBindVertexArray(0);
}

void
HMesh::addVertex( const HVertex& v )
{
    m_vertices.push_back( v );
}

void
HMesh::computeBoundingBox()
{
    if ( m_vertices.empty() )
    {
        m_boundingBox.reset( glm::vec3(0,0,0) );
        return;
    }

    m_boundingBox.reset( m_vertices[ 0 ].pos() );

    for ( size_t i = 1; i < m_vertices.size(); ++i )
    {
        m_boundingBox.addInternalPoint( m_vertices[ i ].pos() );
    }
}

//static
HMesh
HMesh::fromSMesh( const SMeshBuffer & smesh )
{
    HMesh hmesh;
    hmesh.m_name = smesh.name;
    hmesh.m_primitiveType = smesh.primitiveType;
    hmesh.m_boundingBox = smesh.boundingBox;
    hmesh.m_material.diffuseMap = smesh.material.DiffuseMap;

    auto f = glm::vec3(2,1,2) / glm::vec3(100,10,100);

    hmesh.m_vertices.reserve( smesh.vertices.size() );
    for ( const auto & v : smesh.vertices )
    {
        hmesh.m_vertices.emplace_back( v.pos * f, v.normal, v.tex );
    }

    hmesh.m_indices.reserve( smesh.indices.size() );
    for ( const auto & i : smesh.indices )
    {
        hmesh.m_indices.emplace_back( i );
    }

    //hmesh.upload();

    hmesh.computeBoundingBox();

    DE_WARN(hmesh.str())

    return hmesh;
}


// [MemorySizeInBytes]
size_t
HMesh::computeByteConsumption() const
{
    size_t byteCount = sizeof( *this );
    byteCount += m_vertices.capacity() * sizeof( HVertex );
    byteCount += m_indices.capacity() * sizeof( uint32_t );
    return byteCount;
}

std::string
HMesh::str( bool withMaterial ) const
{
    std::stringstream o;
    o << "(" << m_name << ")|";
    o << dbStrBytes( computeByteConsumption() ) << "|";
    o << "p:" << PrimitiveType::getShortString( m_primitiveType ) << "|";
    o << "v:" << m_vertices.size() << "|";
    o << "i:" << m_indices.size() << "|";
    o << "bb:" << m_boundingBox.str();
    //if ( withFVF )
    //   o << "fvf(" << getFVF().toString() << "), ";
    //else
    //   o << "fvf(" << getFVF().getByteSize() << "), ";

    //      for ( size_t i = 0; i < getMaterial().getTextureCount(); ++i )
    //      {
    //         if ( !getMaterial().getTexture( i ).empty() )
    //         {
    //            s << ", tex" << i << "(" << getMaterial().getTexture( i ).toString() << ")";
    //         }
    //      }
    return o.str();
}


HMeshRenderer::HMeshRenderer()
    : m_driver(nullptr)
    , m_shader(nullptr)
{
}

void HMeshRenderer::init( VideoDriver * driver )
{
    m_driver = driver;
    initShader();
}

void HMeshRenderer::draw( const HMesh & hmesh )
{
    setMaterial( hmesh.m_material );
    hmesh.draw();
}

void HMeshRenderer::setMaterial( const HMaterial & material )
{
    if (!m_driver) { DE_ERROR("No driver") return; }
    if (!m_shader) { DE_ERROR("No shader") return; }

    // ==== Shader ====
    m_driver->useShader(m_shader);

    // [Uniform] projMat + viewMat
    glm::mat4 projMat = glm::mat4(1.0f);
    glm::mat4 viewMat = glm::mat4(1.0f);
    //glm::mat3 normalMat = glm::mat3(1.0f);
    auto camera = m_driver->getCamera();
    if (camera)
    {
        projMat = camera->getProjectionMatrix();
        viewMat = camera->getViewMatrix();
        //normalMat = glm::mat3( glm::transpose( glm::inverse( viewMat * modelMat ) ) );
    }
    glUniformMatrix4fv(m_uloc_projMat, 1, GL_FALSE, glm::value_ptr( projMat ));
    glUniformMatrix4fv(m_uloc_viewMat, 1, GL_FALSE, glm::value_ptr( viewMat ));

    // [Uniform] u_diffuseMap
    int stage = 0;
	m_driver->useTexture(material.diffuseMap.tex, stage);
    glUniform1i(m_uloc_diffuseMap, stage);

    // [Uniform] u_lightPos0 + u_lightColor0
    const auto & l0 = m_driver->getLights().at(0);
    glUniform3fv(m_uloc_lightPos0, 1, glm::value_ptr( l0.pos ));
    glUniform3fv(m_uloc_lightColor0, 1, glm::value_ptr( l0.color ));
	
    State state;
    state.culling = Culling();
    state.blend = Blend::disabled();
    m_driver->setState( state );	
}

void HMeshRenderer::initShader()
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (m_shader) return;

    const std::string shaderName = "HTerrain";

    const std::string vs = R"(

    layout(location = 0) in vec3 a_pos;
    layout(location = 1) in vec3 a_normal;
    layout(location = 2) in vec2 a_tex;
    layout(location = 3) in mat4 a_instanceMat;

    uniform mat4 u_projMat;
    uniform mat4 u_viewMat;
    uniform vec3 u_lightPos0;

    out vec2 v_tex;
    out vec3 v_fragPos;
    out vec3 v_fragNormal;
    out vec3 v_fragPosLight0;
    
    void main()
    {
        v_tex = a_tex;

        vec4 fragPos = u_viewMat * a_instanceMat * vec4(a_pos, 1.0);

        gl_Position = u_projMat * fragPos;

        v_fragPos = vec3(fragPos);

        mat3 normalMat = mat3(transpose(inverse(u_viewMat * a_instanceMat)));

        v_fragNormal = normalMat * a_normal; // Only rotation (3x3)

        v_fragPosLight0 = vec3(u_viewMat * vec4(u_lightPos0, 1.0));
    }
    )";

    const std::string fs = R"(

    out vec4 fragColor;

    in vec2 v_tex;

    in vec3 v_pos;
    in vec3 v_normal;

    in vec3 v_fragPos;
    in vec3 v_fragNormal;
    in vec3 v_fragPosLight0;

    uniform sampler2D u_diffuseMap;
    uniform vec3 u_lightColor0;

    void main()
    {
        // ObjectColor:
        vec4 objectColor = texture(u_diffuseMap, v_tex);

        // AmbientColor:
        float ambientStrength = 0.15;
        vec3 ambientColor = ambientStrength * u_lightColor0;

        // DiffuseColor:
        vec3 normal = normalize(v_fragNormal);
        vec3 lightDir0 = normalize(v_fragPosLight0 - v_fragPos);
        float diff0 = max(dot(normal, lightDir0), 0.0);
        vec3 diffuseColor = u_lightColor0 * diff0;

        // SpecularColor:
        float specularStrength = 0.25;
        vec3 viewDir = normalize(-v_fragPos);
        vec3 reflectDir0 = reflect(-lightDir0, normal);
        float spec0 = pow(max(dot(viewDir, reflectDir0), 0.0), 32);
        vec3 specularColor = specularStrength * spec0 * u_lightColor0;

        // Result:
        fragColor = objectColor * vec4(ambientColor + diffuseColor + specularColor,1.0);
    }
    )";

    m_shader = m_driver->createShader( shaderName, vs, fs );
    m_uloc_diffuseMap = glGetUniformLocation(m_shader->id, "u_diffuseMap");
    m_uloc_projMat = glGetUniformLocation(m_shader->id, "u_projMat");
    m_uloc_viewMat = glGetUniformLocation(m_shader->id, "u_viewMat");
    //m_uloc_normalMat = glGetUniformLocation(m_shader->id, "u_normalMat");
    m_uloc_lightPos0 = glGetUniformLocation(m_shader->id, "u_lightPos0");
    m_uloc_lightColor0 = glGetUniformLocation(m_shader->id, "u_lightColor0");
}

} // end namespace gpu.
} // end namespace de.
