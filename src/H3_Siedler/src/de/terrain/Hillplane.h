#pragma once
#include <de/gpu/smesh/SMesh.h>
#include <de/gpu/VideoDriver.h>
#include <de/image/ImagePainter.h>

#if 1

namespace de {
namespace gpu {

// #include <glm/glm.hpp>
// #include <vector>
// #include <iostream>

struct BumpMapUtil
{
    typedef glm::vec2 V2;
    typedef glm::vec3 V3;
    typedef glm::vec4 V4;
    typedef S3DVertex V;

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
    addPlaneXZ( SMeshBuffer & mesh, const V4& plane, float size );

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
    comparePos( const S3DVertex& a, const S3DVertex& b);

    static void
    addTriangles(SMeshBuffer & o, const Triangles & triangles);

    static void
    makeVerticesUnique( SMeshBuffer & o );

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
    windowVertices( SMeshBuffer & o );

    static void
    smoothNormals( SMeshBuffer & o );

/*
    static void
    testMask();

    static void
    test1_4x4( SMeshBuffer & o, VideoDriver* driver, V3 tileSize );

    static void
    test2_4x4( SMeshBuffer & o, VideoDriver* driver, V3 tileSize );

    // Desert,A,B,C,D,E
    static void
    testImpl( SMeshBuffer & o, VideoDriver* driver, V3 tileSize,
              std::string uriBumpMap, std::string uriDiffMap );

    // Desert
    static void
    testNevada( SMeshBuffer & o, VideoDriver* driver );

    // A
    static void
    testMountWilder( SMeshBuffer & o, VideoDriver* driver );

    // B
    static void
    testLehm( SMeshBuffer & o, VideoDriver* driver );

    // C
    static void
    testThuringen( SMeshBuffer & o, VideoDriver* driver );

    // D
    static void
    testWeizen( SMeshBuffer & o, VideoDriver* driver );

    // E
    static void
    testAlpen( SMeshBuffer & o, VideoDriver* driver );
*/
};

/*
// =======================================================================
struct Hillplane
// =======================================================================
{
    static void
    addTriangles(SMeshBuffer & o, const Image & heightMap,
                 glm::vec3 siz = glm::vec3(1000.0, 200.0, 1000.0 ));

    static void
    addTriangles(SMeshBuffer & o, std::string uri,
                 glm::vec3 siz = glm::vec3(1000.0, 200.0, 1000.0 ))
    {
        Image heightMap;
        dbLoadImage( heightMap, uri );
        addTriangles( o, heightMap, siz );
    }

    static void
    addIndexedTriangles(SMeshBuffer & o, Image const & heightMap,
        glm::vec3 siz = glm::vec3(1000.0, 200.0, 1000.0 ));

    static void
    addIndexedTriangles(SMeshBuffer & o, std::string uri,
        glm::vec3 siz = glm::vec3(1000.0, 200.0, 1000.0 ))
    {
        Image heightMap;
        dbLoadImage( heightMap, uri );
        addIndexedTriangles( o, heightMap, siz );
    }
};
*/

} // end namespace gpu.
} // end namespace de.

#endif
