#include "H3_ObjRenderer.h"
#include <H3/H3_Game.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

// ===========================================================================
H3_ObjMeshBuffer::H3_ObjMeshBuffer()
// ===========================================================================
{
    m_primitiveType = de::gpu::PrimitiveType::Triangles;
    m_vao = 0;
    m_vbo_vertices = 0;
    m_vbo_indices = 0;
    m_vbo_instanceMat = 0;
    m_vbo_instanceColor = 0;
}

// static
de::gpu::SMeshBuffer
H3_ObjMeshBuffer::toSMesh( const H3_ObjMeshBuffer& tmesh )
{
    de::gpu::SMeshBuffer smesh;

    //smesh.name = tmesh.name;
    smesh.primitiveType = tmesh.m_primitiveType;

    smesh.vertices.reserve( tmesh.m_vertices.size() );
    for ( const auto & v : tmesh.m_vertices)
    {
        smesh.vertices.emplace_back( v.pos, v.normal, v.color, v.tex );
    }
    smesh.indices.reserve( tmesh.m_indices.size() );
    for ( const auto & i : tmesh.m_indices)
    {
        smesh.indices.emplace_back(i);
    }

    smesh.recalculateBoundingBox();
    smesh.material.Lighting = 1;
    //smesh.material.DiffuseMap = H3_getTex( game, H3_Tex::Wood );

    return smesh;
}

// static
H3_ObjMeshBuffer
H3_ObjMeshBuffer::toTMesh( const de::gpu::SMeshBuffer& smesh )
{
    H3_ObjMeshBuffer tmesh;

    //smesh.name = tmesh.name;
    tmesh.m_primitiveType = smesh.primitiveType;
    tmesh.m_vertices.reserve( smesh.vertices.size() );
    tmesh.m_indices.reserve( smesh.indices.size() );

    for ( const auto & v : smesh.vertices)
    {
        glm::vec3 tangente(1,0,0);
        glm::vec3 bitangente(0,1,0);
        tmesh.m_vertices.emplace_back( v.pos, tangente, bitangente, v.normal, v.color, v.tex );
    }

    for ( const auto & i : smesh.indices)
    {
        tmesh.m_indices.emplace_back(i);
    }

    //tmesh.recalculateBoundingBox();
    //tmesh.material.Lighting = 1;
    //smesh.material.DiffuseMap = H3_getTex( game, H3_Tex::Wood );

    return tmesh;
}

//
//     B
//    / \
//   /   \
//  / -Y  \
// A-------C
//
void
H3_ObjMeshBuffer::addTriangle( glm::vec3 A, glm::vec3 B, glm::vec3 C,
                              glm::vec2 uvA, glm::vec2 uvB, glm::vec2 uvC )
{
    auto n = glm::normalize( glm::cross( B-A, C-A ) );
    auto t = glm::vec3(1,0,0);
    auto b = glm::vec3(0,1,0);
    m_vertices.emplace_back( A, n, t, b, 0xFFFFFFFF, uvA );
    m_vertices.emplace_back( B, n, t, b, 0xFFFFFFFF, uvB );
    m_vertices.emplace_back( C, n, t, b, 0xFFFFFFFF, uvC );
}

//
// B-------C
// |       |
// |  -Y   |
// |       |
// A-------D
//
void
H3_ObjMeshBuffer::addQuad( glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D,
                          float u, float v )
{
    auto n = glm::normalize( glm::cross( B-A, C-A ) );
    auto t = glm::vec3(1,0,0);
    auto b = glm::vec3(0,1,0);
    m_vertices.emplace_back( A, n, t, b, 0xFFFFFFFF, glm::vec2(0,v) );
    m_vertices.emplace_back( B, n, t, b, 0xFFFFFFFF, glm::vec2(0,0) );
    m_vertices.emplace_back( C, n, t, b, 0xFFFFFFFF, glm::vec2(u,0) );

    m_vertices.emplace_back( A, n, t, b, 0xFFFFFFFF, glm::vec2(0,v) );
    m_vertices.emplace_back( C, n, t, b, 0xFFFFFFFF, glm::vec2(u,0) );
    m_vertices.emplace_back( D, n, t, b, 0xFFFFFFFF, glm::vec2(u,v) );
}

void
H3_ObjMeshBuffer::initRoadGeometry( H3_ObjMeshBuffer & o, const glm::vec3& dim)
{
    //
    //                F---------------------------------------G  --- y1
    //               /|                                      /|
    //              / |                  +Z                 / |
    //             /  |                                    /  |
    //    z1 ---  /   E-----------------------------------/---H  --- y0
    //           /   /                                   /   /
    //          /   /                                   /   /
    //         B---------------------------------------C   /
    //         |  /                                    |  /
    //         | /                -Z                   | /
    //         |/                                      |/
    // z0 ---  A---------------------------------------D
    //
    //         |                                       |
    //         x0                                      x1
    //
    const float dx = dim.x * 0.5f;
    const float dy = dim.y * 0.5f;
    const float dz = dim.z * 0.5f;
    const auto a = glm::vec3( -dx, -dy, -dz );
    const auto b = glm::vec3( -dx,  dy, -dz );
    const auto c = glm::vec3(  dx,  dy, -dz );
    const auto d = glm::vec3(  dx, -dy, -dz );
    const auto e = glm::vec3( -dx, -dy,  dz );
    const auto f = glm::vec3( -dx,  dy,  dz );
    const auto g = glm::vec3(  dx,  dy,  dz );
    const auto h = glm::vec3(  dx, -dy,  dz );
    o.addQuad( a,b,c,d, 6,1); // NegZ = Front quad ABCD
    o.addQuad( h,g,f,e, 6,1); // PosZ = Back quad HGFE

    o.addQuad( e,f,b,a, 1,1); // NegX = Left quad EFBA
    o.addQuad( d,c,g,h, 1,1); // PosX = Right quad DCGH

    o.addQuad( d,h,e,a, 6,1); // NegY = Bottom quad DHEA
    o.addQuad( b,f,g,c, 6,1); // PosY = Top quad BFGC

    for (auto & v : o.m_vertices)
    {
        v.pos += glm::vec3(0,dy,0);
    }
}

void
H3_ObjMeshBuffer::initFarmGeometry( H3_ObjMeshBuffer & o, const glm::vec3& dim)
{
    //                        __J__          --- y2
    //                     __/ /   \__
    //                  __/   /       \__
    //                 /     /           \
    //                F-----/-------------G  --- y1
    //               /|    /             /|
    //              / |   /     +Y      / |
    //             /  |__I__           /  |
    //    z1 ---  / __E--|--\__-------/---H  --- y0
    //           /_/ /   |     \__   / X /
    //          /   /    |        \ / + /
    //         B---------+---------C   /
    //         |  /                |  /
    //         | /       -Y        | /
    //         |/                  |/
    // z0 ---  A-------------------D   ---
    //
    //         |         |         |
    //         x0        x1        x2
    //
    // const auto farmSize = glm::vec3( 20, 20, 20 );

    const float dx = dim.x;
    const float dy = dim.y;
    const float dz = dim.z;

    const float x0 = dx * 0.0f;
    const float x1 = dx * 0.5f;
    const float x2 = dx * 1.0f;

    const float y0 = dy * 0.0f;
    const float y1 = dy * 0.5f;
    const float y2 = dy * 1.0f;

    const float z0 = dz * 0.0f;
    const float z1 = dz * 1.0f;

    const auto a = glm::vec3( x0, y0, z0 );
    const auto b = glm::vec3( x0, y1, z0 );
    const auto c = glm::vec3( x2, y1, z0 );
    const auto d = glm::vec3( x2, y0, z0 );
    const auto e = glm::vec3( x0, y0, z1 );
    const auto f = glm::vec3( x0, y1, z1 );
    const auto g = glm::vec3( x2, y1, z1 );
    const auto h = glm::vec3( x2, y0, z1 );

    const auto i = glm::vec3( x1, y2, z0 );
    const auto j = glm::vec3( x1, y2, z1 );

    //o.addQuad( a,e,h,d, 6,1); // NegY = Bottom quad AEHD
    //o.addQuad( b,f,g,c, 6,1); // PosY = Top quad BFGC
    o.addQuad( a,b,c,d, 6,3); // NegZ = Front quad ABCD
    o.addQuad( h,g,f,e, 6,3); // PosZ = Back quad HGFE
    o.addQuad( e,f,b,a, 6,3); // NegX = Left quad EFBA
    o.addQuad( d,c,g,h, 6,3); // PosX = Right quad DCGH

    o.addQuad( f,j,i,b, 6,6); // Roof Left FJIB
    o.addQuad( c,i,j,g, 6,6); // Roof Right CIJG

    const auto uvA = glm::vec2(0,1);
    const auto uvB = glm::vec2(0.5f,0);
    const auto uvC = glm::vec2(1,1);
    o.addTriangle(b,i,c,uvA,uvB,uvC); // Attic Front BIJ
    o.addTriangle(g,j,f,uvA,uvB,uvC); // Attic Back GJF

    for (auto & v : o.m_vertices)
    {
        v.pos += glm::vec3(-dx*0.5f, 0.0f, -dz*0.5f);
    }
}

void
H3_ObjMeshBuffer::initCityGeometry( H3_ObjMeshBuffer & obj, const glm::vec3& dim)
{
    //                        __J__          --- y3
    //                     __/ /   \__
    //                  __/   /       \__
    //                 /     /           \
    //                F-----/-------------G  --- y2
    //               /|    /             /|
    //              / |   /     +Y      / |
    //             /  |__I__           /  |
    //    z1 ---  / __E--|--\__-------/---H  --- y1
    //           /_/ /   |     \__   / X /
    //          /   /    |        \ / + /
    //         B---------+---------C   /
    //         |  /                |  /
    //         | /       -Y        | /
    //         |/                  |/
    // z0 ---  A-------------------D   ---
    //
    //                E---------+---------H-------------------N  --- y1
    //               /|        /         /                   /|
    //              / |       /         /   +Y              / |
    //             /  |      /         /                   /  |
    //    z1 ---  /   P-----/---------/-------------------/---O  --- y0
    //           /   /     /         /                   /   /
    //          /   /     /         /                   /   /
    //         A---------+---------D-------------------K   /
    //         |  /                                    |  /
    //         | /                -Y                   | /
    //         |/                                      |/
    // z0 ---  M---------------------------------------L
    //
    //         |         |         |                   |
    //         x0        x1        x2                  x3
    //

    const float dx = dim.x;
    const float dy = dim.y;
    const float dz = dim.z;

    const float x0 = dx * 0.0f;
    const float x1 = dx * 0.25f;
    const float x2 = dx * 0.5f;
    const float x3 = dx * 1.0f;

    const float y0 = dy * 0.0f;
    const float y1 = dy * 0.5f;
    const float y2 = dy * 0.75f;
    const float y3 = dy * 1.0f;

    const float z0 = dz * 0.0f;
    const float z1 = dz * 1.0f;

    const auto a = glm::vec3( x0, y1, z0 );
    const auto b = glm::vec3( x0, y2, z0 );
    const auto c = glm::vec3( x2, y2, z0 );
    const auto d = glm::vec3( x2, y1, z0 );
    const auto e = glm::vec3( x0, y1, z1 );
    const auto f = glm::vec3( x0, y2, z1 );
    const auto g = glm::vec3( x2, y2, z1 );
    const auto h = glm::vec3( x2, y1, z1 );

    const auto i = glm::vec3( x1, y3, z0 );
    const auto j = glm::vec3( x1, y3, z1 );

    const auto k = glm::vec3( x3, y1, z0 );
    const auto l = glm::vec3( x3, y0, z0 );
    const auto m = glm::vec3( x0, y0, z0 );

    const auto n = glm::vec3( x3, y1, z1 );
    const auto o = glm::vec3( x3, y0, z1 );
    const auto p = glm::vec3( x0, y0, z1 );

    //obj.addQuad( a,e,h,d, 6,1); // NegY = Bottom quad AEHD
    //obj.addQuad( b,f,g,c, 6,1); // PosY = Top quad BFGC
    obj.addQuad( a,b,c,d, 6,3); // NegZ = Front quad ABCD
    obj.addQuad( h,g,f,e, 6,3); // PosZ = Back quad HGFE
    obj.addQuad( e,f,b,a, 6,3); // NegX = Left quad EFBA
    obj.addQuad( d,c,g,h, 6,3); // PosX = Right quad DCGH

    obj.addQuad( f,j,i,b, 6,6); // Roof Left FJIB
    obj.addQuad( c,i,j,g, 6,6); // Roof Right CIJG

    const auto uvA = glm::vec2(0,1);
    const auto uvB = glm::vec2(0.5f,0);
    const auto uvC = glm::vec2(1,1);
    obj.addTriangle(b,i,c,uvA,uvB,uvC); // Attic Front BIJ
    obj.addTriangle(g,j,f,uvA,uvB,uvC); // Attic Back GJF

    //obj.addQuad( a,e,h,d, 6,1);   // NegY = Bottom quad AEHD
    obj.addQuad( d,h,n,k, 6,6);     // PosY = Top quad DHNK
    obj.addQuad( m,a,k,l, 12,6);    // NegZ = Front quad MAKL
    obj.addQuad( o,n,e,p, 12,6);    // PosZ = Back quad ONEP
    obj.addQuad( p,e,a,m, 6,6);     // NegX = Left quad PEAM
    obj.addQuad( l,k,n,o, 6,6);     // PosX = Right quad LKNO

    for (auto & v : obj.m_vertices)
    {
        v.pos += glm::vec3(-dx*0.5f, 0.0f, -dz*0.5f);
    }
}


void
H3_ObjMeshBuffer::upload()
{
    //DE_WARN("Upload ", m_vertices.size(), " vertices.")
    //DE_WARN("sizeof(Vertex) = ", sizeof(H3_ObjVertex))
    //DE_WARN("Upload ", m_instanceMat.size(), " modelInstances.")
    //DE_WARN("Upload ", m_instanceColor.size(), " colorInstances.")

    if (!m_vbo_vertices)
    {
        glGenBuffers(1, &m_vbo_vertices);
        //DE_TRACE("m_vbo_vertices = ",m_vbo_vertices)
    }

    if (!m_vbo_indices && m_indices.size())
    {
        glGenBuffers(1, &m_vbo_indices);
        //DE_TRACE("m_vbo_indices = ",m_vbo_indices)
    }

    if (!m_vbo_instanceMat)
    {
        glGenBuffers(1, &m_vbo_instanceMat);
        //DE_TRACE("m_vbo_instanceMat = ",m_vbo_instanceMat)
    }

    if (!m_vbo_instanceColor)
    {
        glGenBuffers(1, &m_vbo_instanceColor);
        //DE_TRACE("m_vbo_instanceColor = ",m_vbo_instanceColor)
    }

    if (!m_vao)
    {
        glGenVertexArrays(1, &m_vao);
        //DE_TRACE("m_vao = ",m_vao)
    }

    glBindVertexArray(m_vao);

    // vbo_vertices
    const auto & vertices = m_vertices;
    const size_t vertexCount = vertices.size();
    const size_t vertexSize = sizeof(H3_ObjVertex);
    const size_t vertexBytes = vertexCount * vertexSize;
    const uint8_t* pVertices = reinterpret_cast< const uint8_t* >( vertices.data() );
    //std::cout << "Upload " << vertexBytes << " bytes" << std::endl;

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), pVertices, GL_STATIC_DRAW);

    // a_pos
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(H3_ObjVertex), (void*)(0) );
    glEnableVertexAttribArray( 0 );
    // a_normal
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(H3_ObjVertex), (void*)(12) );
    glEnableVertexAttribArray( 1 );
    // a_tangent
    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof(H3_ObjVertex), (void*)(24) );
    glEnableVertexAttribArray( 2 );
    // a_bitangent
    glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(H3_ObjVertex), (void*)(36) );
    glEnableVertexAttribArray( 3 );
    // a_color
    glVertexAttribPointer( 4, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(H3_ObjVertex), (void*)(48) );
    glEnableVertexAttribArray( 4 );
    // a_tex
    glVertexAttribPointer( 5, 2, GL_FLOAT, GL_FALSE, sizeof(H3_ObjVertex), (void*)(52) );
    glEnableVertexAttribArray( 5 );

    // m_vbo_instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    //glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0));
    glVertexAttribDivisor(6, 1);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(16));
    glVertexAttribDivisor(7, 1);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(32));
    glVertexAttribDivisor(8, 1);
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(48));
    glVertexAttribDivisor(9, 1);
    glEnableVertexAttribArray(9);

    // m_vbo_instanceColor:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceColor);
    //glBufferData(GL_ARRAY_BUFFER, m_instanceColor.size() * sizeof(uint32_t), m_instanceColor.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(10, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint32_t), (void*)(0));
    glVertexAttribDivisor(10, 1);
    glEnableVertexAttribArray(10);

    if (m_vbo_indices)
    {
        const auto & indices = m_indices;
        const size_t indexCount = indices.size();
        const size_t indexSize = sizeof(uint32_t);
        const size_t indexBytes = indexCount * indexSize;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBytes), m_indices.data(), GL_STATIC_DRAW);

        //DE_WARN("Upload ", m_indices.size(), " indices.")
        //DE_WARN("sizeof(Index) = ", sizeof(uint32_t))
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (m_vbo_indices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    GL_VALIDATE;
}

void
H3_ObjMeshBuffer::uploadInstances()
{
    if (m_instanceMat.empty())
    {
        return; // Nothing to upload.
    }

    // m_vbo_instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);

    // m_vbo_instanceColor:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceColor);
    glBufferData(GL_ARRAY_BUFFER, m_instanceColor.size() * sizeof(uint32_t), m_instanceColor.data(), GL_STATIC_DRAW);
}

void
H3_ObjMeshBuffer::render() const
{
    if (m_instanceMat.empty())
    {
        return; // Nothing to draw.
    }

    glBindVertexArray(m_vao);

    const GLenum primType = de::gpu::PrimitiveType::toOpenGL( m_primitiveType );
    if (m_indices.size() > 0)
    {
        glDrawElementsInstanced(primType, GLsizei(m_indices.size()), GL_UNSIGNED_INT, nullptr, m_instanceMat.size());
    }
    else
    {
        glDrawArraysInstanced(primType, 0, m_vertices.size(), m_instanceMat.size());
    }

    glBindVertexArray(0);
    GL_VALIDATE;
}


// ===========================================================================
H3_ObjShader::H3_ObjShader()
// ===========================================================================
{
    m_driver = nullptr;
    m_shader = nullptr;
    m_texWood = nullptr;
}

//H3_ObjShader::~H3_ObjShader();

void H3_ObjShader::init( H3_Game & game )
{
    m_driver = game.getDriver();
    initShader();
    initTexture( game );
}

void H3_ObjShader::initShader()
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (m_shader) return;

    std::string const & shaderName = "H3_ObjShader";

    std::string vs = R"(

    layout(location = 0) in vec3 a_pos;
    layout(location = 1) in vec3 a_normal;
    layout(location = 2) in vec3 a_tangent;
    layout(location = 3) in vec3 a_bitangent;
    layout(location = 4) in lowp vec4 a_color;
    layout(location = 5) in vec2 a_texCoords;

    layout(location = 6) in mat4 a_instanceMat; // (mat4 spans 4 attribute locations)
    layout(location = 10) in lowp vec4 a_instanceColor;

    uniform mat4 u_projMat;
    uniform mat4 u_viewMat;
    //uniform mat3 u_normalMat;
    uniform vec3 u_lightPos0;

    out vec2 v_texCoords;
    out vec4 v_color;

    out vec3 v_fragPos;
    out vec3 v_fragNormal;
    out vec3 v_fragPosLight0;

    void main()
    {
        //const vec4 cMin = vec4( 0,0,0,0 );
        //const vec4 cMax = vec4( 0,0,0,0 );
        // Instance color
        vec4 ci = a_instanceColor / 255.0;
        // Vertex color
        vec4 cv = a_color / 255.0;
        // Combined color
        v_color = ci * cv;
        v_texCoords = a_texCoords;

        vec4 fragPos = u_viewMat * a_instanceMat * vec4(a_pos, 1.0);

        gl_Position = u_projMat * fragPos;

        v_fragPos = vec3(fragPos);

        mat3 normalMat = mat3(transpose(inverse(u_viewMat * a_instanceMat)));

        v_fragNormal = normalMat * a_normal; // Only rotation (3x3)

        v_fragPosLight0 = vec3(u_viewMat * vec4(u_lightPos0, 1.0));
    }
    )";

    std::string fs = R"(

    out vec4 fragColor;

    in vec4 v_color;
    in vec2 v_texCoords;

    in vec3 v_fragPos;
    in vec3 v_fragNormal;
    in vec3 v_fragPosLight0;

    uniform sampler2D u_diffuseMap;
    uniform vec3 u_lightColor0;

    void main()
    {
        // ObjectColor:
        vec4 objectColor = texture(u_diffuseMap, v_texCoords) * v_color;

        // AmbientColor:
        float ambientStrength = 0.25;
        vec3 ambientColor = ambientStrength * u_lightColor0;

        // DiffuseColor:
        vec3 normal = normalize(v_fragNormal);
        vec3 lightDir0 = normalize(v_fragPosLight0 - v_fragPos);
        float diff0 = max(dot(normal, lightDir0), 0.0);
        vec3 diffuseColor = u_lightColor0 * diff0;

        // SpecularColor:
        float specularStrength = 0.5;
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

void
H3_ObjShader::initTexture( H3_Game & game )
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (m_texWood) return;

    m_texWood = game.getTex(H3_Tex::Wood, "H3_ObjShader::initTexture()" ).tex;
}

void H3_ObjShader::setMaterial()
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (!m_shader) { DE_ERROR("No shader") return; }

    if (!m_texWood) { DE_ERROR("No texWood") return; }

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
    m_driver->useTexture(m_texWood, 0);
    glUniform1i(m_uloc_diffuseMap, 0);

    // [Uniform] u_lightPos0 + u_lightColor0
    const auto & l0 = m_driver->getLights().at(0);
    glUniform3fv(m_uloc_lightPos0, 1, glm::value_ptr( l0.pos ));
    glUniform3fv(m_uloc_lightColor0, 1, glm::value_ptr( l0.color ));

    de::gpu::State state;
    state.culling = de::gpu::Culling::disabled();
    state.blend = de::gpu::Blend::disabled();
    m_driver->setState( state );
}

void H3_ObjShader::unsetMaterial()
{
    m_driver->useTexture(m_texWood, 0);
}

// ===========================================================================
H3_ObjRenderer::H3_ObjRenderer()
// ===========================================================================
{
}

// H3_ObjRenderer::~H3_ObjRenderer() { }

void
H3_ObjRenderer::init( H3_Game & game )
{
    m_driver = game.getDriver();
    m_shader.init( game );
}

void H3_ObjRenderer::render( const H3_ObjMeshBuffer& mesh )
{
    if (mesh.m_instanceMat.empty())
    {
        return; // Nothing to draw.
    }

    m_shader.setMaterial();
    mesh.render();
    m_shader.unsetMaterial();
}

/*
void H3_ObjRenderer::upload(const H3_Game& game)
{
    const auto & g_tiles = game.m_tiles;
    const auto & g_edges = game.m_edges;
    const auto & g_corners = game.m_corners;

    const auto & g_tileSize = game.m_tileSize;
    const auto & g_edgeSize = game.m_edgeSize;
    const auto & g_cornerSize = game.m_cornerSize;

    const auto & g_roadSize = game.m_roadSize;
    const auto & g_farmSize = game.m_farmSize;
    const auto & g_citySize = game.m_citySize;

    //========================================================================
    // Update Roads on Edges:
    //========================================================================

    m_road.m_instanceMat.clear();
    m_road.m_instanceColor.clear();

    for (const auto& edge : g_edges)
    {
        if (edge.owner < 1) continue;

        H3_TRS trs;
        trs.pos = edge.trs.pos + glm::vec3(0,g_roadSize.y,0);
        trs.rotate = edge.trs.rotate;
        trs.scale = glm::vec3(1,1,1);
        trs.update();
        m_road.m_instanceMat.push_back( trs.trs );

        uint32_t playerColor = game.getPlayerColor( edge.owner );

        m_road.m_instanceColor.push_back( playerColor );
    }

    //========================================================================
    // Update Farms on Corners:
    //========================================================================

    m_farm.m_instanceMat.clear();
    m_farm.m_instanceColor.clear();

    for (const auto& corner : g_corners)
    {
        if (corner.owner < 1) continue;
        if (corner.building_type != 1) continue;

        H3_TRS trs;
        trs.pos = corner.trs.pos + glm::vec3(0,g_farmSize.y,0);
        trs.rotate = corner.trs.rotate;
        trs.scale = glm::vec3(1,1,1);
        trs.update();
        m_farm.m_instanceMat.push_back( trs.trs );

        uint32_t playerColor = game.getPlayerColor( corner.owner );

        m_farm.m_instanceColor.push_back( playerColor );
    }

    //========================================================================
    // Update Cities on Corners:
    //========================================================================

    m_city.m_instanceMat.clear();
    m_city.m_instanceColor.clear();

    for (const auto& corner : g_corners)
    {
        if (corner.owner < 1) continue;
        if (corner.building_type != 2) continue;

        H3_TRS trs;
        trs.pos = corner.trs.pos + glm::vec3(0,g_farmSize.y + g_citySize.y,0);
        trs.rotate = corner.trs.rotate;
        trs.scale = glm::vec3(1,1,1);
        trs.update();
        m_city.m_instanceMat.push_back( trs.trs );

        uint32_t playerColor = game.getPlayerColor( corner.owner );

        m_city.m_instanceColor.push_back( playerColor );
    }

    m_road.upload();
    m_farm.upload();
    m_city.upload();
}
*/

