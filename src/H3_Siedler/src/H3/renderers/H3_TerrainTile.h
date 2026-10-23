#pragma once
#include <de/smesh/SMesh.h>
#include <de/gpu/VideoDriver.h>
#include <de/image/ImagePainter.h>

namespace de {
namespace gpu {

// ============================================
struct BumpMapUtil
// ============================================
{
    typedef glm::vec2 V2;
    typedef glm::vec3 V3;
    typedef glm::vec4 V4;
    typedef smesh::S3DVertex V;

    // **Definition der Triangle-Struct**
    struct BBox
    {
        V3 minEdge;
        V3 maxEdge;
    };

    // **Definition der Triangle-Struct**
    struct Triangle
    {
        V A, B, C;
        Triangle(V a, V b, V c) : A(a), B(b), C(c) {}

        BBox getBoundingBox() const
        {
            BBox bbox;
            bbox.minEdge = A.pos;
            bbox.minEdge = glm::min(bbox.minEdge, B.pos);
            bbox.minEdge = glm::min(bbox.minEdge, C.pos);
            bbox.maxEdge = A.pos;
            bbox.maxEdge = glm::max(bbox.maxEdge, B.pos);
            bbox.maxEdge = glm::max(bbox.maxEdge, C.pos);
            return bbox;
        }
    };

    typedef std::vector<Triangle> Triangles;

    struct Crop
    {
        int l = 0; // left
        int t = 0; // top
        int r = 0; // right
        int b = 0; // bottom
        int w = 0; // width
        int h = 0; // height

        std::string str() const
        {
            std::ostringstream o; o <<
            "_w" << w << "_h" << h <<
            "_l" << l << "_t" << t <<
            "_r" << r << "_b" << b;
            return o.str();
        }

        void scale( int w2, int h2 )
        {
            const float fX = float(w2) / float(w);
            const float fY = float(h2) / float(h);
            l = std::lround( fX * l );
            r = std::lround( fX * r );
            t = std::lround( fY * t );
            b = std::lround( fY * b );
            w = w2;
            h = h2;
        }
    };

    static Crop
    computeSquaredCenter( const Image & img );

    static Image
    crop( const Image & img, Crop crop );

    static BBox
    computeBBox( const Image & heightMap );

    static BBox
    computeBBox( const Triangles& triangles );

    static void
    centerTrianglesXZ(Triangles& triangles);

    // RGBA32 images (PNG files)
    static void
    createTriangles( Triangles & o, const Image & heightMap, V3 tileSize);

    // RGBA32F images (EXR files)
    static void
    createTrianglesf( Triangles & o, const Image & heightMap, V3 tileSize);

/*
    static V2
    getNormalXZ( const V3& normal );

    static V2
    getOrtho_CCW( const V2& normal );

    static V2
    getOrtho_CW( const V2& normal );

    static V3
    getPointOnPlane( const V4& plane );

    static bool
    getPerpendicularVectors1(V3 normal, V3 & tangent, V3 & bitangent );

    // Function to validate that the computed point satisfies the plane equation
    bool testPointOnPlane(const V4& plane, const V3& p)
    {
        float result = glm::dot(V3(plane.x, plane.y, plane.z), p); // Compute dot product
        return glm::epsilonEqual(result, plane.w, EPSILON); // Check if result matches offset D
    }
*/

    // Function to check if two vectors are collinear
    static bool
    areCollinear(const V3& v1, const V3& v2);

    // Function to compute two perpendicular vectors for any 3D normal
    static bool
    getPerpendicularVectors(V3 normal, V3 & tangent, V3 & bitangent );

    static void
    addPlaneXZ( smesh::SMeshBuffer & mesh, const V4& plane, float size );

    static void
    translateHeightmap( Image & img, float offset );

    static void
    scaleHeightmap( Image & img, float scale );

    static void
    saturateHeightmap( Image & img );

    // **Berechnung des Schnittpunkts einer Kante mit einer Clipping-Ebene**
    static V
    computeIntersection(const V& a, const V& b, const V4& plane);

    // **Clipping eines Dreiecks gegen eine einzelne Clipping-Ebene**
    static Triangles
    clipTriangle(const Triangle& tri, const V4& plane);

    // **Rekursives Clipping eines Dreiecks gegen alle sechs Ebenen**
    static Triangles
    clipTriangles(const Triangles& in_triangles, const V4& plane);

    // **Rekursives Clipping eines Dreiecks gegen alle sechs Ebenen**
    static Triangles
    clipTriangles(const Triangles& tri, const std::vector<V4>& planes);

    // static V4
    // createPlaneFromPoints(const V3& a, const V3& b);

    static V4
    computePlaneFromXZ( const V3& a, const V3& b );

    // =========================================================================
    /// @brief The Hexagon ( ver_2018 ):
    // =========================================================================
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
    ///       A       triangles: ABF, BCE, BEF, CDE
    ///

    static void
    clipTrianglesHexagon(const Triangles& in_triangles,
                        Triangles& out_triangles, const V3& tileSize );

    static bool
    comparePos( const smesh::S3DVertex& a, const smesh::S3DVertex& b);

    static void
    addTriangles( smesh::SMeshBuffer & o, const Triangles & triangles);

    static void
    makeVerticesUnique( smesh::SMeshBuffer & o );

    static glm::vec3
    computeNormal( const Triangle & o );

    static void
    repairWinding( Triangles & o );

    static void
    drawHexagon( Image & img, Recti pos, uint32_t color, bool blend = false );

    static Image
    createHexagonFilter( int w );

    static Image
    createHexagonFilterf( int w );

    static void
    filterGrey( Image & img, const Image & mask );

    static void
    windowVertices( smesh::SMeshBuffer & o );

    static void
    smoothNormals( smesh::SMeshBuffer & o );

};

// glm::uint16 float16 = glm::packHalf1x16(1.0f);   // Float -> half
// float value = glm::unpackHalf1x16(float16);      // Half -> float

/*
inline uint16_t f32_to_f16( const float value )
{
    uint32_t bits = *(uint32_t*)&value;
    uint16_t sign = (bits >> 16) & 0x8000;
    int exp = ((bits >> 23) & 0xFF) - 127 + 15;
    uint16_t mant = (bits >> 13) & 0x3FF;
    exp = std::clamp( exp, 0, 31 );
    return sign | (exp << 10) | mant;
}

inline float f16_to_f32( const uint16_t half )
{
    uint32_t sign = (half & 0x8000) << 16;
    int exp = ((half >> 10) & 0x1F) - 15 + 127;
    uint32_t mant = (half & 0x3FF) << 13;
    uint32_t bits = sign | (exp << 23) | mant;
    return *(float*)&bits;
}
*/

#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct HVertex // 6 Bytes if packed
// ===========================================================================
{
    int16_t m_x;  // 2 Bytes
    int16_t m_y;  // 2 Bytes
    int16_t m_z;  // 2 Bytes
    int16_t m_nx; // 2 Bytes
    int16_t m_ny; // 2 Bytes
    int16_t m_nz; // 2 Bytes
    int16_t m_u;  // 2 Bytes
    int16_t m_v;  // 2 Bytes = 16 Bytes

    HVertex( const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& tex )
        : m_x( glm::packSnorm1x16( pos.x ) )
        , m_y( glm::packSnorm1x16( pos.y ) )
        , m_z( glm::packSnorm1x16( pos.z ) )
        , m_nx(glm::packSnorm1x16( normal.x ) )
        , m_ny(glm::packSnorm1x16( normal.y ) )
        , m_nz(glm::packSnorm1x16( normal.z ) )
        , m_u( glm::packSnorm1x16( tex.x ) )
        , m_v( glm::packSnorm1x16( tex.y ) )
    {}

    HVertex( const HVertex& o )
        : m_x( o.m_x )
        , m_y( o.m_y )
        , m_z( o.m_z )
        , m_nx(o.m_nx )
        , m_ny(o.m_ny )
        , m_nz(o.m_nz )
        , m_u( o.m_u )
        , m_v( o.m_v )
    {}

    float x() const { return glm::unpackSnorm1x16( m_x ); }
    float y() const { return glm::unpackSnorm1x16( m_y ); }
    float z() const { return glm::unpackSnorm1x16( m_z ); }
    float nx() const{ return glm::unpackSnorm1x16( m_nx); }
    float ny() const{ return glm::unpackSnorm1x16( m_ny); }
    float nz() const{ return glm::unpackSnorm1x16( m_nz); }
    float u() const { return glm::unpackSnorm1x16( m_u ); }
    float v() const { return glm::unpackSnorm1x16( m_v ); }

    glm::vec3 pos() const { return glm::vec3( x(), y(), z() ); }
    glm::vec3 normal() const { return glm::vec3( nx(), ny(), nz() ); }
    glm::vec2 tex() const { return glm::vec2( u(), v() ); }
};

#pragma pack( pop )

// ===========================================================================
struct HMaterial
// ===========================================================================
{
    TexRef diffuseMap;
};

// ===========================================================================
struct HMesh
// ===========================================================================
{
    typedef glm::vec2 V2;
    typedef glm::vec3 V3;

    std::string m_name;
    Box3f m_boundingBox;
    HMaterial m_material;
    std::vector<HVertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<glm::mat4> m_instanceMat;

    // GPU side
    PrimitiveType m_primitiveType;
    uint32_t m_vao;
    uint32_t m_vbo_vertices;
    uint32_t m_vbo_indices;
    uint32_t m_vbo_instanceMat;

    HMesh();
    ~HMesh();
    void destroy();
    void upload( bool bForceUpload = false );
    void draw() const;
    void addVertex( const HVertex& v );
    void computeBoundingBox();
    size_t computeByteConsumption() const;
    std::string str( bool withMaterial = false ) const;

    static HMesh fromSMesh( const smesh::SMeshBuffer & smesh );

};

// ===========================================================================
struct HMeshRenderer
// ===========================================================================
{
    VideoDriver* m_driver;
    Shader* m_shader;

    int m_uloc_diffuseMap;
    int m_uloc_projMat;
    int m_uloc_viewMat;
    //int m_uloc_normalMat;
    int m_uloc_lightPos0;
    int m_uloc_lightColor0;

    HMeshRenderer();
    //~HMeshRenderer();

    void init( VideoDriver * driver );
    void draw( const HMesh & hmesh );
    void setMaterial( const HMaterial & material );


protected:
    void initShader();
};

} // end namespace gpu.
} // end namespace de.
