#pragma once
#include <H3/H3_Board.h>

struct H3_Game;

#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct H3_ObjVertex
// ===========================================================================
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    uint32_t color;
    glm::vec2 tex;    // 5*12 = 60 Bytes if packed
public:
    H3_ObjVertex()
        : pos()
        , normal( 0, 1, 0 ), tangent( 1, 0, 0 ), bitangent( 0, 0, 1 )
        , color( 0xFFFF00FF ), tex()
    {}
    H3_ObjVertex( float x, float y, float z,
                    float nx, float ny, float nz,
                    float tx, float ty, float tz,
                    float bx, float by, float bz,
                    uint32_t crgba, float u, float v )
        : pos( x,y,z )
        , normal( nx,ny,nz ), tangent( tx,ty,tz ), bitangent( bx,by,bz )
        , color( crgba ), tex( u,v )
    {}
    H3_ObjVertex( const glm::vec3 &p,
                    const glm::vec3 &nrm,
                    const glm::vec3 &tan,
                    const glm::vec3 &bitan,
                    const uint32_t crgba, const glm::vec2 &tex0 )
        : pos( p )
        , normal( nrm ), tangent( tan ), bitangent( bitan )
        , color( crgba ), tex( tex0 )
    {}
};
#pragma pack( pop )


// ===========================================================================
struct H3_ObjMeshBuffer
// ===========================================================================
{
    // CPU side
    std::vector<H3_ObjVertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<glm::mat4> m_instanceMat;
    std::vector<uint32_t> m_instanceColor;

    // GPU side
    de::gpu::PrimitiveType m_primitiveType;
    uint32_t m_vao;
    uint32_t m_vbo_vertices;
    uint32_t m_vbo_indices;
    uint32_t m_vbo_instanceMat;
    uint32_t m_vbo_instanceColor;

    H3_ObjMeshBuffer();
    //~H3_ObjMeshBuffer();

    void upload();
    void uploadInstances();
    void render() const;

    static de::gpu::SMeshBuffer toSMesh( const H3_ObjMeshBuffer& tmesh );

    static H3_ObjMeshBuffer toTMesh( const de::gpu::SMeshBuffer& smesh );
    //
    //     B
    //    / \
    //   /   \
    //  / -Y  \
    // A-------C
    //
    void addTriangle( glm::vec3 A, glm::vec3 B, glm::vec3 C,
                     glm::vec2 uvA, glm::vec2 uvB, glm::vec2 uvC );

    //
    // B-------C
    // |       |
    // |  -Y   |
    // |       |
    // A-------D
    //
    void addQuad( glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D,
                 float u, float v );

    static void initRoadGeometry( H3_ObjMeshBuffer & o, const glm::vec3& dim );
    static void initFarmGeometry( H3_ObjMeshBuffer & o, const glm::vec3& dim );
    static void initCityGeometry( H3_ObjMeshBuffer & o, const glm::vec3& dim );

};

// ===========================================================================
struct H3_ObjShader
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver;
    de::gpu::Shader* m_shader;
    int m_uloc_diffuseMap;
    int m_uloc_projMat;
    int m_uloc_viewMat;
    int m_uloc_normalMat;

    int m_uloc_lightPos0;
    int m_uloc_lightColor0;

    de::gpu::Texture* m_texWood;

    H3_ObjShader();
    //~H3_ObjShader();

    void init( H3_Game & game );
    void initShader();
    void initTexture( H3_Game & game );

    void setMaterial();
    void unsetMaterial();

    // const glm::mat4& projViewMat, glm::vec3 const& u_cameraPos;
};

// ===========================================================================
struct H3_ObjRenderer
// ===========================================================================
{
    de::gpu::VideoDriver* m_driver;
    H3_ObjShader m_shader;

    H3_ObjRenderer();
    //~H3_ObjRenderer();
    
    void init( H3_Game & game );

    //void upload( const H3_Game & game );

    void render( const H3_ObjMeshBuffer& mesh );

};

