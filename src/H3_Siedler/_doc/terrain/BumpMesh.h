#pragma once
#include <de/gpu/VideoDriver.h>

#include <de/terrain/Hillplane.h>

namespace de {
namespace gpu {

#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct BumpVertex // 20 Bytes if packed	
// ===========================================================================
{
	typedef float T;
	typedef glm::vec2 V2;
	typedef glm::vec3 V3;
		
    BumpVertex() : pos(), tex() {}
    BumpVertex( T x, T y, T z, T u, T v ) : pos( x,y,z ) , tex( u,v ) {}
    BumpVertex( const V3& p, const V2& t ) : pos( p ), tex( t ) {}
    BumpVertex( const BumpVertex& o ) : pos( o.pos ), tex( o.tex ) {}
	
    V3 pos;
    V2 tex;    
};

#pragma pack( pop )

// ===========================================================================
struct BumpMaterial
// ===========================================================================
{
	TexRef diffuseMap;
	TexRef normalMap;
	TexRef bumpMap;
	float bumpScale = 1.0f;
};

// ===========================================================================
struct BumpMesh
// ===========================================================================
{
    typedef float T;
    typedef glm::vec2 V2;
    typedef glm::vec3 V3;
    typedef glm::vec4 V4;
    typedef glm::mat4 M4;

    // CPU side
    std::vector<BumpVertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<M4> m_instanceMat;
    BumpMaterial m_material;

    // GPU side
    PrimitiveType m_primitiveType;
    uint32_t m_vao;
    uint32_t m_vbo_vertices;
    uint32_t m_vbo_indices;
    uint32_t m_vbo_instanceMat;

    BumpMesh();
    //~BumpMesh();

    void upload();
    void render();

    static BumpMesh createDesert( VideoDriver* driver, V3 tileSize = V3(100,10,100) )
    {
        BumpMesh mesh;
        mesh.addHexagon( tileSize );
        mesh.upload();

        if (!driver) { DE_ERROR("No driver.") }

        DE_PERF_MARKER

        std::string mediaDir = "media/H3/gfx/tiles_2025/Sand/";

        std::string uriDiffuseMap = mediaDir + "sand_d.png";
        std::string uriBumpMap = mediaDir + "sand_h.png";
        std::string uriNormalMap = mediaDir + "sand_n.png";

        // ==========================================================
        Image imgDiffuseMap;
        if (!dbLoadImage(imgDiffuseMap, uriDiffuseMap))
        {
            DE_ERROR("No image ", uriDiffuseMap)
        }

        DE_BENNI("Loaded imgDiffuseMap = ", dbStrBytes(imgDiffuseMap.computeMemoryConsumption()) )

        imgDiffuseMap = imgDiffuseMap.convert( PixelFormat::RGB_24 );

        DE_BENNI("Converted imgDiffuseMap = ", dbStrBytes(imgDiffuseMap.computeMemoryConsumption()) )

        // ==========================================================
        Image imgBumpMap;
        if (!dbLoadImage(imgBumpMap, uriBumpMap))
        {
            DE_ERROR("No image ", uriBumpMap)
        }

        DE_BENNI("Loaded imgBumpMap = ", dbStrBytes(imgBumpMap.computeMemoryConsumption()) )

        imgBumpMap = imgBumpMap.convert( PixelFormat::R8 );

        DE_BENNI("Converted imgBumpMap = ", dbStrBytes(imgBumpMap.computeMemoryConsumption()) )

        // ==========================================================
        Image imgNormalMap;
        if (!dbLoadImage(imgNormalMap, uriNormalMap))
        {
            DE_ERROR("No image ", uriNormalMap)
        }

        DE_BENNI("Loaded imgNormalMap = ", dbStrBytes(imgNormalMap.computeMemoryConsumption()) )

        imgNormalMap = imgNormalMap.convert( PixelFormat::RGB_24 );

        DE_BENNI("Converted imgNormalMap = ", dbStrBytes(imgNormalMap.computeMemoryConsumption()) )

        //BumpMapUtil::saturateHeightmap( imgBumpMap );

        //DE_BENNI("Saturated imgBumpMap = ", dbStrBytes(imgBumpMap.computeMemoryConsumption()) )

        mesh.m_material.bumpMap
            = driver->createTexture2D("DesertParaBumpMap", imgBumpMap );

        mesh.m_material.bumpScale = 1.0f;

        mesh.m_material.diffuseMap
            = driver->createTexture2D("DesertParaDiffuseMap", imgDiffuseMap );

        mesh.m_material.normalMap
            = driver->createTexture2D("DesertParaNormalMap", imgNormalMap );

        return mesh;
    }

    static BumpMesh createErz( VideoDriver* driver )
    {
        BumpMesh mesh;
        mesh.addHexagon( V3(100,10,100) );
        mesh.upload();

        if (!driver) { DE_ERROR("No driver.") }

        DE_PERF_MARKER

                std::string mediaDir = "media/H3/gfx/tiles_2025/";

        std::string uriDiffuseMap = mediaDir + "Erz2_DiffuseMap.png";
        std::string uriBumpMap = mediaDir + "Erz2_BumpMap.exr";

        Image imgDiffuseMap;
        if (!dbLoadImage(imgDiffuseMap, uriDiffuseMap))
        {
            DE_ERROR("No image ", uriDiffuseMap)
        }

        DE_BENNI("Loaded imgDiffuseMap = ", dbStrBytes(imgDiffuseMap.computeMemoryConsumption()) )

        Image imgBumpMap;
        if (!dbLoadImage(imgBumpMap, uriBumpMap))
        {
            DE_ERROR("No image ", uriBumpMap)
        }

        DE_BENNI("Loaded imgBumpMap = ", dbStrBytes(imgBumpMap.computeMemoryConsumption()) )

        imgBumpMap = imgBumpMap.convert( PixelFormat::R32F );

        DE_BENNI("Converted imgBumpMap = ", dbStrBytes(imgBumpMap.computeMemoryConsumption()) )

        BumpMapUtil::saturateHeightmap( imgBumpMap );

        DE_BENNI("Saturated imgBumpMap = ", dbStrBytes(imgBumpMap.computeMemoryConsumption()) )

        imgBumpMap = ImageResizef::resizeBilinearf( imgBumpMap, 1024, 1024 );

        DE_BENNI("Resized imgBumpMap = ", dbStrBytes(imgBumpMap.computeMemoryConsumption()) )

        mesh.m_material.bumpMap
            = driver->createTexture2D("BShdErz_BumpMap", imgBumpMap );

        mesh.m_material.bumpScale = 1.0f;

        mesh.m_material.diffuseMap
            = driver->createTexture2D("BShdErz_DiffuseMap", imgDiffuseMap );


        return mesh;
    }

    void addVertex( const BumpVertex& v )
    {
        m_vertices.push_back( v );
    }

    // ===============================================
    /// @brief The Hexagon
    // ===============================================
    ///
    ///               M| x=0 | y=0 | z=0 | u=.5| v=.5|
    ///       D       -|-----|-----|-----|-----|-----|
    ///      / \      A|   0 |  0  | -.5 | .5  | 0   |
    ///   C /   \ E   B| -.5 |  0  |-.25 |  0  | 0.25|
    ///    |-----|    C| -.5 |  0  | .25 |  0  | 0.75|
    ///    |  M  |    D| 0.0 |  0  |  .5 |  .5 | 1   |
    ///    |-----|    E| 0.5 |  0  | .25 |  1  | 0.75|
    ///   B \   / F   F| 0.5 |  0  |-.25 |  1  | 0.25|
    ///      \ /
    ///       A       triangles: BFA, BEF, BCE, CDE
    ///
    void addHexagon( const V3& tileSize )
    {
        m_primitiveType = PrimitiveType::Triangles;

        const float w = tileSize.x * 0.5f;
        const float h = tileSize.z * 0.5f;
        const BumpVertex A( V3(     0,0,-.5f*h ), V2(.5f, 1 ) );
        const BumpVertex B( V3(-.5f*w,0,-.25f*h), V2(0, .75f ));
        const BumpVertex C( V3(-.5f*w,0, .25f*h), V2(0, .25f ));
        const BumpVertex D( V3(     0,0, .5f*h ), V2(.5f, 0 ) );
        const BumpVertex E( V3(0.5f*w,0, .25f*h), V2(1, .25f ));
        const BumpVertex F( V3(0.5f*w,0,-.25f*h), V2(1, .75f ));

        addVertex( B ); addVertex( F ); addVertex( A ); // Triangle BAF
        addVertex( B ); addVertex( E ); addVertex( F ); // Triangle BFE
        addVertex( B ); addVertex( C ); addVertex( E ); // Triangle BEC
        addVertex( C ); addVertex( D ); addVertex( E ); // Triangle CED

        // if ( useIndices )
        // {
        //     size_t v = getVertexCount();
        //     addVertex( A ); addVertex( B ); addVertex( C );
        //     addVertex( D ); addVertex( E ); addVertex( F );
        //     //addIndexedTriangle( B, A, F );
        //     addIndexedTriangle( v+1, v+5, v+0 );
        //     //addIndexedTriangle( B, F, E );
        //     //addIndex( v+1 ); addIndex( v+5 ); addIndex( v+4 );
        //     addIndexedTriangle( v+1, v+4, v+5 );
        //     //addIndexedTriangle( B, E, C );
        //     //addIndex( v+1 ); addIndex( v+4 ); addIndex( v+2 );
        //     addIndexedTriangle( v+1, v+2, v+4 );
        //     //addIndexedTriangle( C, E, D );
        //     //addIndex( v+2 ); addIndex( v+4 ); addIndex( v+3 );
        //     addIndexedTriangle( v+2, v+3, v+4 );

        // }
    }
    //
    //     B
    //    / \
    //   /   \
    //  / -Y  \
    // A-------C
    //
    void addTriangle( glm::vec3 A, glm::vec3 B, glm::vec3 C,
                      glm::vec2 uvA, glm::vec2 uvB, glm::vec2 uvC )
    {
        auto n = glm::normalize( glm::cross( B-A, C-A ) );
        auto t = glm::vec3(1,0,0);
        auto b = glm::vec3(0,1,0);
        m_vertices.emplace_back( A, uvA );
        m_vertices.emplace_back( B, uvB );
        m_vertices.emplace_back( C, uvC );
    }

    //
    // B-------C
    // |       |
    // |  -Y   |
    // |       |
    // A-------D
    //
    void addQuad( glm::vec3 A, glm::vec3 B, glm::vec3 C, glm::vec3 D,
                  float u, float v )
    {
        auto n = glm::normalize( glm::cross( B-A, C-A ) );
        auto t = glm::vec3(1,0,0);
        auto b = glm::vec3(0,1,0);
        m_vertices.emplace_back( A, glm::vec2(0,v) );
        m_vertices.emplace_back( B, glm::vec2(0,0) );
        m_vertices.emplace_back( C, glm::vec2(u,0) );

        m_vertices.emplace_back( A, glm::vec2(0,v) );
        m_vertices.emplace_back( C, glm::vec2(u,0) );
        m_vertices.emplace_back( D, glm::vec2(u,v) );
    }
};

} // end namespace gpu.
} // end namespace de.
