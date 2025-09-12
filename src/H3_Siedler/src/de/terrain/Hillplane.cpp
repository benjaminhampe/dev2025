#include <de/terrain/Hillplane.h>

#if 1

#include <de/gpu/VideoDriver.h>

namespace de {
namespace gpu {

void
BumpMapUtil::createTriangles( Triangles & o, const Image & bumpMapRGB, glm::vec3 siz )
{
    int32_t w = bumpMapRGB.w();
    int32_t h = bumpMapRGB.h();
    if ( w < 2 ) { DE_ERROR("Not enough image width") return; }
    if ( h < 2 ) { DE_ERROR("Not enough image height") return; }

    if ( bumpMapRGB.pixelFormat() == PixelFormat::R32F )
    {
        DE_ERROR("[Benni] Got R32F GeoTIFF heightmap")
    }

    DE_BENNI("Given triangles ", o.size())
    DE_BENNI("Given image w ", bumpMapRGB.w())
    DE_BENNI("Given image h ", bumpMapRGB.h())

    //====================
    // Temporary vertices
    //====================
    std::vector< S3DVertex > vertices;
    vertices.reserve( size_t( w ) * size_t( h ) );

    float cell_x = siz.x / float( w - 1 );
    float cell_y = siz.y;
    float cell_z = siz.z / float( h - 1 );
    for ( int32_t j = 0; j < h; j++ )
    {
        for ( int32_t i = 0; i < w; i++ )
        {
            const uint32_t color = bumpMapRGB.getPixel( i, j );
            const float r = dbRGBA_R( color );
            const float g = dbRGBA_G( color );
            const float b = dbRGBA_B( color );
            const float f = (r + g + b) * (1.0f / (3.0f * 255.0f));
            const float x = cell_x * i;
            const float y = cell_y * f;
            const float z = cell_z * j;
            const float u = float( i ) / float( w - 1 );
            const float v = float( j ) / float( h - 1 );
            vertices.emplace_back( x,y,z, 0,1,0, 0xFFFFFFFF, u,v );
        }
    }

    DE_BENNI("Created vertices ", vertices.size())

    //=======================
    // Create triangle quads
    //=======================
    o.clear();
    o.reserve( 2 * size_t( w-1 ) * size_t( h-1 ) );

    auto computeLinearIndex = [w] (int x, int y)
    {
        return (size_t( w ) * size_t( y )) + size_t( x );
    };

    for ( int32_t j = 0; j < h - 1; ++j )
    {
        for ( int32_t i = 0; i < w - 1; ++i )
        {
            size_t iA = computeLinearIndex( i, j );
            size_t iB = computeLinearIndex( i, j+1 );
            size_t iC = computeLinearIndex( i+1, j+1 );
            size_t iD = computeLinearIndex( i+1, j );
            Triangle ABC( vertices[iA], vertices[iB], vertices[iC] );
            Triangle ACD( vertices[iA], vertices[iC], vertices[iD] );
            o.push_back( ABC );
            o.push_back( ACD );
        }
    }

    DE_BENNI("Created triangles ", o.size())
}

void
BumpMapUtil::createTrianglesf( Triangles & o, const Image & bumpMap, glm::vec3 siz )
{
    int32_t w = bumpMap.w();
    int32_t h = bumpMap.h();
    if ( w < 2 ) { DE_ERROR("Not enough bumpMap width") return; }
    if ( h < 2 ) { DE_ERROR("Not enough bumpMap height") return; }

    if ( bumpMap.pixelFormat() == PixelFormat::RGBA32F )
    {
        DE_ERROR("[Benni] Got RGBA32F exr heightmap")
    }

    //====================
    // Temporary vertices
    //====================
    std::vector< S3DVertex > vertices;
    vertices.reserve( size_t( w ) * size_t( h ) );

    float cell_x = siz.x / float( w - 1 );
    float cell_y = siz.y;
    float cell_z = siz.z / float( h - 1 );
    for ( int32_t j = 0; j < h; ++j )
    {
        for ( int32_t i = 0; i < w; ++i )
        {
            const float f = bumpMap.getPixelf( i, j );
            const float x = cell_x * i;
            const float y = cell_y * f;
            const float z = cell_z * j;
            const float u = float( i ) / float( w - 1 );
            const float v = float( j ) / float( h - 1 );
            vertices.emplace_back( x,y,z, 0,1,0, 0xFFFFFFFF, u,v );
        }
    }

    //=====================
    // Create vertex quads
    //=====================
    o.clear();
    o.reserve( 2 * size_t( w-1 ) * size_t( h-1 ) );

    for ( int32_t j = 0; j < h - 1; ++j )
    {
        for ( int32_t i = 0; i < w - 1; ++i )
        {
            size_t iA = size_t( i ) + size_t( w ) * size_t( j );
            size_t iB = iA + size_t( w );
            size_t iC = iB + 1;
            size_t iD = iA + 1;
            // Triangle ABC
            o.emplace_back( vertices[iA], vertices[iB], vertices[iC] );
            // Triangle ACD
            o.emplace_back( vertices[iA], vertices[iC], vertices[iD] );
        }
    }
}

bool
BumpMapUtil::comparePos( const S3DVertex& a, const S3DVertex& b)
{
    const auto e_pos = 1.0e-6f;
    const auto e_tex = 1.0e-5f;

    if (   std::abs(a.pos.x - b.pos.x) <= e_pos
        && std::abs(a.pos.y - b.pos.y) <= e_pos
        && std::abs(a.pos.z - b.pos.z) <= e_pos)
    {
        // if (glm::epsilonEqual(a.tex.x, b.tex.x, e_tex))
        // {
        //     DE_WARN
        // }
        return true;
    }
    else
    {
        return false;
    }
}

void
BumpMapUtil::addTriangles(SMeshBuffer & o, const Triangles & triangles)
{
    o.setPrimitiveType( PrimitiveType::Triangles );

    //const auto n = o.vertices.size();

    //=====================
    // Create vertex quads
    //=====================
    auto & v = o.vertices;

    v.clear();
    v.reserve( (triangles.size() * 3) );

    /*
    for ( size_t i = 0; i < triangles.size(); ++i )
    {
        const Triangle& t = triangles[ i ];
        o.vertices.emplace_back( t.A );
        o.vertices.emplace_back( t.B );
        o.vertices.emplace_back( t.C );
        o.indices.push_back( v + 3*i + 0 );
        o.indices.push_back( v + 3*i + 1 );
        o.indices.push_back( v + 3*i + 2 );
        //DE_OK("Triangle[",i,"] A(", glm::ivec3(t.A.pos), "), "
        //       "B(", glm::ivec3(t.B.pos), "), "
        //       "C(", glm::ivec3(t.C.pos), ")")
    }
    */

    for ( size_t i = 0; i < triangles.size(); ++i )
    {
        const Triangle& t = triangles[ i ];

        auto itA = std::find_if( v.begin(), v.end(), [&] (const S3DVertex& c) { return comparePos(c,t.A); } );
        if (itA == v.end())
        {
            o.indices.push_back( o.vertices.size() );
            o.vertices.emplace_back( t.A );
        }
        else
        {
            o.indices.push_back( static_cast<uint32_t>(
                std::distance( v.begin(), itA ) ) );
        }

        auto itB = std::find_if( v.begin(), v.end(), [&] (const S3DVertex& c) { return comparePos(c,t.B); } );
        if (itB == v.end())
        {
            o.indices.push_back( o.vertices.size() );
            o.vertices.emplace_back( t.B );
        }
        else
        {
            o.indices.push_back( static_cast<uint32_t>(
                std::distance( v.begin(), itB ) ) );
        }

        auto itC = std::find_if( v.begin(), v.end(), [&] (const S3DVertex& c) { return comparePos(c,t.C); } );
        if (itC == v.end())
        {
            o.indices.push_back( o.vertices.size() );
            o.vertices.emplace_back( t.C );
        }
        else
        {
            o.indices.push_back( static_cast<uint32_t>(
                std::distance( v.begin(), itC ) ) );
        }
    }

    //o.vertices.shrink_to_fit();
    //o.indices.shrink_to_fit();

    o.recalculateBoundingBox();

    // SMeshBufferTool::computeNormals( o );
}


// static
BumpMapUtil::Crop
BumpMapUtil::computeSquaredCenter( const Image & img )
{
    Crop crop;
    crop.w = img.w();
    crop.h = img.h();

    if (crop.w == crop.h)
    {
        return crop; // Already squared
    }

    if (crop.w > crop.h)
    {
        int dx = crop.w - crop.h;
        crop.l = dx / 2;
        crop.r = dx - crop.l;
    }
    else if (crop.h > crop.w)
    {
        int dy = crop.h - crop.w;
        crop.t = dy / 2;
        crop.b = dy - crop.t;
    }

    return crop; // Already squared
}

// static
Image
BumpMapUtil::crop( const Image & img, Crop crop )
{
    int w = img.w();
    int h = img.h();
    if ((w != crop.w) || (h != crop.h))
    {
        DE_OK("Before scale: ", crop.str())
        crop.scale( w, h );
        DE_OK("After scale: ", crop.str())
    }

    const int dx = crop.l + crop.r;
    const int dy = crop.t + crop.b;

    const int w2 = w - dx;
    const int h2 = h - dy;

    if (w2 < 1)
    {
        DE_OK("w2 < 1", crop.str())
        return img;
    }

    if (h2 < 1)
    {
        DE_OK("h2 < 1", crop.str())
        return img;
    }

    Image dst( w2, h2, img.pixelFormat() );

    for ( int y = 0; y < h2; ++y )
    {
        for ( int x = 0; x < w2; ++x )
        {
            uint32_t c = img.getPixel( x + crop.l, y + crop.t );
            dst.setPixel( x, y, c );
        }
    }

    return dst;
}

// static
BumpMapUtil::BBox
BumpMapUtil::computeBBox( const Image & img )
{
    if (img.empty()) { DE_ERROR("Empty image") return {}; }

    if (img.pixelFormat() == PixelFormat::R32F)
    {
        float rMin = std::numeric_limits<float>::max();
        float rMax = std::numeric_limits<float>::min();
        const float* pSrc = img.readPtr<float>();
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            const float r = *pSrc++;
            rMin = std::min(rMin, r);
            rMax = std::max(rMax, r);
        }

        BBox bbox;
        bbox.minEdge = V3(rMin,0,0);
        bbox.maxEdge = V3(rMax,0,0);
        return bbox;
    }
    else if (img.pixelFormat() == PixelFormat::RGB32F)
    {
        float rMin = std::numeric_limits<float>::max();
        float rMax = std::numeric_limits<float>::min();
        float gMin = std::numeric_limits<float>::max();
        float gMax = std::numeric_limits<float>::min();
        float bMin = std::numeric_limits<float>::max();
        float bMax = std::numeric_limits<float>::min();
        const float* pSrc = img.readPtr<float>();
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            const float r = *pSrc++;
            rMin = std::min(rMin, r);
            rMax = std::max(rMax, r);
            const float g = *pSrc++;
            gMin = std::min(gMin, g);
            gMax = std::max(gMax, g);
            const float b = *pSrc++;
            bMin = std::min(bMin, b);
            bMax = std::max(bMax, b);
        }

        BBox bbox;
        bbox.minEdge = V3(rMin,gMin,bMin);
        bbox.maxEdge = V3(rMax,gMax,bMax);
        return bbox;
    }
    else if (img.pixelFormat() == PixelFormat::RGBA32F)
    {
        float rMin = std::numeric_limits<float>::max();
        float rMax = std::numeric_limits<float>::min();
        float gMin = std::numeric_limits<float>::max();
        float gMax = std::numeric_limits<float>::min();
        float bMin = std::numeric_limits<float>::max();
        float bMax = std::numeric_limits<float>::min();
        const float* pSrc = img.readPtr<float>();
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            const float r = *pSrc++;
            rMin = std::min(rMin, r);
            rMax = std::max(rMax, r);
            const float g = *pSrc++;
            gMin = std::min(gMin, g);
            gMax = std::max(gMax, g);
            const float b = *pSrc++;
            bMin = std::min(bMin, b);
            bMax = std::max(bMax, b);
            pSrc++;
        }

        BBox bbox;
        bbox.minEdge = V3(rMin,gMin,bMin);
        bbox.maxEdge = V3(rMax,gMax,bMax);
        return bbox;
    }
    else if (img.pixelFormat() == PixelFormat::RGB_24)
    {
        float rMin = std::numeric_limits<float>::max();
        float rMax = std::numeric_limits<float>::min();
        float gMin = std::numeric_limits<float>::max();
        float gMax = std::numeric_limits<float>::min();
        float bMin = std::numeric_limits<float>::max();
        float bMax = std::numeric_limits<float>::min();
        const uint8_t* pSrc = img.readPtr<uint8_t>();
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            const float r = *pSrc++;
            rMin = std::min(rMin, r);
            rMax = std::max(rMax, r);
            const float g = *pSrc++;
            gMin = std::min(gMin, g);
            gMax = std::max(gMax, g);
            const float b = *pSrc++;
            bMin = std::min(bMin, b);
            bMax = std::max(bMax, b);
        }

        BBox bbox;
        bbox.minEdge = V3(rMin,gMin,bMin);
        bbox.maxEdge = V3(rMax,gMax,bMax);
        return bbox;
    }
    else
    {
        DE_ERROR("Unsupported pixelFormat ", img.pixelFormat().str())
        return {};
    }
}

// static
void
BumpMapUtil::translateHeightmap( Image & img, float offset )
{
    if (img.empty()) { DE_ERROR("Empty image") return; }

    if (img.pixelFormat() == PixelFormat::R32F)
    {
        float* pDst = img.writePtr<float>();
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            *pDst++ += offset;
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGB32F)
    {
        float* pDst = img.writePtr<float>();
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            *pDst++ += offset;
            *pDst++ += offset;
            *pDst++ += offset;
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGBA32F)
    {
        float* pDst = img.writePtr<float>();
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            *pDst++ += offset;
            *pDst++ += offset;
            *pDst++ += offset;
            *pDst++ += offset;
        }
    }
    else
    {
        DE_ERROR("Unsupported pixelFormat ", img.pixelFormat().str())
        return;
    }

}
// static
void
BumpMapUtil::scaleHeightmap( Image & img, float scale )
{
    if (img.empty()) { DE_ERROR("Empty image") return; }

    if (img.pixelFormat() == PixelFormat::R32F)
    {
        float* pDst = img.writePtr<float>();
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            *pDst++ *= scale;
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGB32F)
    {
        float* pDst = img.writePtr<float>();
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            *pDst++ *= scale;
            *pDst++ *= scale;
            *pDst++ *= scale;
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGBA32F)
    {
        float* pDst = img.writePtr<float>();
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            *pDst++ *= scale;
            *pDst++ *= scale;
            *pDst++ *= scale;
            *pDst++ *= scale;
        }
    }
    else
    {
        DE_ERROR("Unsupported pixelFormat ", img.pixelFormat().str())
        return;
    }

}


// static
void
BumpMapUtil::saturateHeightmap( Image & img )
{
    if (img.empty()) { DE_ERROR("Empty image") return; }

    if (img.pixelFormat() == PixelFormat::R32F)
    {
        // **Compute boundingBox**
        const float* pSrc = img.readPtr<float>();
        float f = *pSrc++;
        float yMin = f;
        float yMax = f;
        for ( size_t i = 1; i < img.pixelCount(); ++i )
        {
            f = *pSrc++;
            yMin = std::min(yMin, f);
            yMax = std::max(yMax, f);
        }

        // **Normalize heights**
        float* pDst = img.writePtr<float>();
        const float yScale = 255.0f / (yMax - yMin);
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            const float h = *pDst;
            *pDst++ = yScale * (h - yMin);
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGB_24)
    {
        // **Compute boundingBox**
        uint8_t rMin = 255;
        uint8_t rMax = 0;
        auto pSrc = img.readPtr<uint8_t>();
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            uint8_t r = *pSrc;
            pSrc += 3;
            rMin = std::min(rMin, r);
            rMax = std::max(rMax, r);
        }
        if (rMin == 0 && rMax == 255)
        {
            DE_WARN("Nothing to saturate")
            return; // Nothing todo
        }

        if (rMax - rMin == 0)
        {
            DE_WARN("Delta is 0, abort to prevent divByZero.")
            return; // Nothing todo
        }

        DE_OK("Saturate rMin(",int(rMin),"), rMax(",int(rMax),")")

        // **Normalize heights**
        auto pDst = img.writePtr<uint8_t>();
        const float rScale = 255.0f / (rMax - rMin);
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            uint8_t r1 = *pDst;
            uint8_t r2 = static_cast<uint8_t>( std::clamp(
                std::lround(rScale * (r1 - rMin)), 0l, 255l));
            *pDst++ = r2;
            *pDst++ = r2;
            *pDst++ = r2;
        }
    }
    else if (img.pixelFormat() == PixelFormat::RGBA_32)
    {
        // **Compute boundingBox**
        auto pSrc = img.readPtr<uint8_t>();
        uint8_t rMin = 255;
        uint8_t rMax = 0;
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            uint8_t r = *pSrc;
            pSrc += 4;
            rMin = std::min(rMin, r);
            rMax = std::max(rMax, r);
        }
        if (rMin == 0 && rMax == 255)
        {
            return; // Nothing todo
        }

        if (rMax - rMin == 0)
        {
            DE_ERROR("Delta is 0, abort to prevent divByZero.")
            return; // Nothing todo
        }

        // **Normalize heights**
        auto pDst = img.writePtr<uint8_t>();
        const float rScale = 255.0f / (rMax - rMin);
        for ( size_t i = 0; i < img.pixelCount(); ++i )
        {
            uint8_t r1 = *pDst;
            uint8_t r2 = static_cast<uint8_t>( std::clamp(
                std::lround(rScale * (r1 - rMin)), 0l, 255l));
            *pDst++ = r2;
            *pDst++ = r2;
            *pDst++ = r2;
            *pDst++ = r2;
        }
    }
    else
    {
        DE_ERROR("Unsupported pixelFormat ", img.pixelFormat().str())
        return;
    }
}

// static
BumpMapUtil::BBox
BumpMapUtil::computeBBox( const Triangles& triangles )
{
    if (triangles.empty())
    {
        return {};
    }

    // **Compute boundingBox**
    BBox bbox = triangles[0].getBoundingBox();

    // **Compute boundingBox**
    for ( size_t i = 1; i < triangles.size(); ++i )
    {
        const BBox bbox2 = triangles[i].getBoundingBox();
        bbox.minEdge = glm::min(bbox.minEdge, bbox2.minEdge);
        bbox.maxEdge = glm::max(bbox.maxEdge, bbox2.maxEdge);
    }

    return bbox;
}

// static
void
BumpMapUtil::centerTrianglesXZ(Triangles& triangles)
{
    if (triangles.empty())
    {
        return;
    }

    // **Compute boundingBox**
    BBox bbox = computeBBox( triangles );

    // **Center triangles XZ**
    const float dx = bbox.maxEdge.x - bbox.minEdge.x;
    const float dz = bbox.maxEdge.z - bbox.minEdge.z;
    const float cx = bbox.minEdge.x + 0.5f*dx;
    const float cz = bbox.minEdge.z + 0.5f*dz;

    const V3 center(cx,0,cz);
    for ( Triangle & t : triangles )
    {
        t.A.pos -= center;
        t.B.pos -= center;
        t.C.pos -= center;
    }
}

// static
glm::vec3
BumpMapUtil::computeNormal( const Triangle & o )
{
    return Math::getNormal3D( o.A.pos, o.B.pos, o.C.pos );
}

// static
void
BumpMapUtil::repairWinding( Triangles & o )
{
    if (o.empty())
    {
        return;
    }

    for ( Triangle & t : o )
    {
        auto n = computeNormal( t );
        if (n.y < 0.0f)
        {
            std::swap( t.B, t.C );
            n = computeNormal( t );
        }
        t.A.normal = n;
        t.B.normal = n;
        t.C.normal = n;
    }
}

// static
void
BumpMapUtil::makeVerticesUnique( SMeshBuffer & o )
{
    /*
    const auto & v0 = o.vertices;
    const auto & i0 = o.indices;

    if ( i0.empty() )
    {
        DE_ERROR("No indices")
        return;
    }

    auto compareV = [] ( const S3DVertex& a, const S3DVertex& b)
    {
        const auto e_pos = 1.0e-5f;
        const auto e_tex = 1.0e-5f;

        if ( glm::epsilonEqual(a.pos.x, b.pos.x, e_pos)
          && glm::epsilonEqual(a.pos.y, b.pos.y, e_pos)
          && glm::epsilonEqual(a.pos.z, b.pos.z, e_pos))
        {
            // if (glm::epsilonEqual(a.tex.x, b.tex.x, e_tex))
            // {
            //     DE_WARN
            // }
            return true;
        }
        else
        {
            return false;
        }
    };

    // Compares
    std::vector< S3DVertex > v1;
    std::vector< uint32_t > i1;

    for ( size_t k = 0; k < v0.size(); ++k )
    {
        const S3DVertex & v = v0[ k ];
        std::find_if
    }
*/
}

/*
// static
BumpMapUtil::V2
BumpMapUtil::getNormalXZ( const V3& normal )
{
    return V2(normal.x,normal.z);
}

// static
BumpMapUtil::V2
BumpMapUtil::getOrtho_CCW( const V2& normal )
{
    return V2(-normal.y,normal.x);
}

// static
BumpMapUtil::V2
BumpMapUtil::getOrtho_CW( const V2& normal )
{
    return V2(normal.y,-normal.x);
}
// static
BumpMapUtil::V3
// Function to compute a valid point on the plane using the normal and offset
BumpMapUtil::getPointOnPlane( const V4& plane )
{
    const float EPSILON = 1e-6f; // Small threshold for floating-point comparisons

    glm::vec3 normal = glm::vec3(plane.x, plane.y, plane.z); // Extract normal vector
    float D = plane.w; // Plane offset (distance from the origin)

    glm::vec3 p(0.0f); // Initialize point to (0,0,0) explicitly

    // Ensure that the normal is valid (not a zero vector)
    if (glm::epsilonEqual(glm::length(normal), 0.0f, EPSILON)) {
        DE_ERROR("Invalid normal: Cannot determine a valid plane.")
        return glm::vec3(0.0f); // Return zero vector if invalid
    }

    // Case 1: Plane is parallel to XY-plane (normal = (0,0,1))
    // - The equation simplifies to z = D
    // - x and y are arbitrary because they do not affect the equation
    if (glm::epsilonEqual(normal.x, 0.0f, EPSILON) &&
        glm::epsilonEqual(normal.y, 0.0f, EPSILON)) {
        p.z = D; // Assign z directly based on the plane equation
    }
    // Case 2: Plane is parallel to XZ-plane (normal = (0,1,0))
    // - The equation simplifies to y = D
    // - x and z are arbitrary
    else if (glm::epsilonEqual(normal.x, 0.0f, EPSILON) &&
             glm::epsilonEqual(normal.z, 0.0f, EPSILON)) {
        p.y = D; // Assign y directly based on the equation
    }
    // Case 3: Plane is parallel to YZ-plane (normal = (1,0,0))
    // - The equation simplifies to x = D
    // - y and z are arbitrary
    else if (glm::epsilonEqual(normal.y, 0.0f, EPSILON) &&
             glm::epsilonEqual(normal.z, 0.0f, EPSILON)) {
        p.x = D; // Assign x directly based on the equation
    }
    // General Case: Solve for x given arbitrary y and z
    // - We choose y = 0 and z = 0 to simplify the equation
    else {
        p.x = D / normal.x; // Solve for x using the full equation
    }

    return p; // Return computed point
}

// static
bool
BumpMapUtil::getPerpendicularVectors1(V3 normal, V3 & tangent, V3 & bitangent )
{
    normal = glm::normalize(normal); // Ensure normal is unit-length

    // Select an arbitrary reference vector that is not parallel to normal
    V3 reference = (glm::abs(normal.y) < 0.99f) ? V3(0.0f, 1.0f, 0.0f) : V3(1.0f, 0.0f, 0.0f);

    // Compute first perpendicular vector
    tangent = glm::normalize(glm::cross(normal, reference));

    // Compute second perpendicular vector
    bitangent = glm::normalize(glm::cross(normal, tangent));

    return true;
}

// static
void
BumpMapUtil::addPlaneXZ( SMeshBuffer & mesh, const V4& plane, float size, V3 p1, V3 p2)
{
    // Compute point on plane
    V3 n(plane);
    float d = plane.w;
    V3 P = -n * d;
    float s = size * 0.5f;

    V3 Q = getPointOnPlane( plane );
    // A.x * n.x + A.z * n.z = 0;
    // A.y = 0
    // A.x = -A.z * n.z / n.x
    //Q.z = 100;
    //Q.x = -Q.z * n.z / n.x;

    V3 r1 = glm::normalize(Q - P);
    V3 r2 = glm::cross(n,r1);

    DE_OK("n = ", n)
    DE_OK("d = ", d)
    DE_OK("P = ", P)
    DE_OK("Q = ", Q)
    DE_OK("r1 = ", r1)
    DE_OK("r2 = ", r2)

    V3 A = P - (r1 * s) - (r2 * s);
    V3 B = P - (r1 * s) + (r2 * s);
    V3 C = P + (r1 * s) + (r2 * s);
    V3 D = P + (r1 * s) - (r2 * s);
    DE_OK("A = ", A)
    DE_OK("B = ", B)
    DE_OK("C = ", C)
    DE_OK("D = ", D)

    float dotP1 = glm::dot(n,p1 - P);
    float dotP2 = glm::dot(n,p2 - P);
    DE_OK("dotP1 = ", dotP1)
    DE_OK("dotP2 = ", dotP2)

    auto v = mesh.vertices.size();
    mesh.vertices.push_back( S3DVertex(A, n, 0xFF0000FF, V2(0,1)) );
    mesh.vertices.push_back( S3DVertex(B, n, 0xFF00FF00, V2(0,0)) );
    mesh.vertices.push_back( S3DVertex(C, n, 0xFFFF0000, V2(1,0)) );
    mesh.vertices.push_back( S3DVertex(D, n, 0xFF00FFFF, V2(1,1)) );
    mesh.indices.push_back(v + 0);
    mesh.indices.push_back(v + 2);
    mesh.indices.push_back(v + 1);
    mesh.indices.push_back(v + 0);
    mesh.indices.push_back(v + 3);
    mesh.indices.push_back(v + 2);
}
// static
BumpMapUtil::V4
BumpMapUtil::createPlaneFromPoints(const V3& a, const V3& b)
{
    auto n2 = getOrtho_CW(getNormalXZ(b - a));
    auto n = glm::normalize( V3(n2.x, 0, n2.y) );
    float d = std::abs(glm::dot(n,a));
    return V4(n.x,n.y,n.z,d);
}
*/

// static
BumpMapUtil::V4
BumpMapUtil::computePlaneFromXZ(const V3& a, const V3& b)
{
    // Compute direction vector
    V3 ab = b - a;

    // Compute normal pointing toward the origin
    V3 normal = glm::normalize(V3(-ab.z, 0.0f, ab.x));

    // Compute plane distance using P1
    float d = -glm::dot(normal, a);

    return V4(normal, d); // Plane equation (Nx, Ny, Nz, d)
}

// Function to check if two vectors are collinear
// static
bool
BumpMapUtil::areCollinear(const V3& v1, const V3& v2)
{
    constexpr float EPSILON = 1e-6f;
    V3 crossProduct = glm::cross(v1, v2);
    return glm::epsilonEqual(glm::length(crossProduct), 0.0f, EPSILON);
}

// Function to compute two perpendicular vectors for any 3D normal
// static
bool
BumpMapUtil::getPerpendicularVectors(V3 normal, V3 & tangent, V3 & bitangent )
{
    constexpr float EPSILON = 1e-6f; // Small threshold for floating-point checks

    normal = glm::normalize(normal); // Ensure unit normal

    V3 helper;
    int iteration = 0;

    bool bCollinear = true;
    // Loop until we find a non-collinear helper vector
    do
    {
        helper = glm::sphericalRand(1.0f); // Random unit vector
        bCollinear = areCollinear(normal, helper);
        iteration++;
    }
    while (bCollinear && iteration < 3);

    if (!bCollinear)
    {
        // Compute perpendicular vectors
        tangent = glm::normalize(glm::cross(normal, helper));
        bitangent = glm::normalize(glm::cross(normal, tangent));
    }

    return bCollinear;
}

// static
void
BumpMapUtil::addPlaneXZ( SMeshBuffer & mesh, const V4& plane, float size )
{
    V3 n(plane);
    V3 b;
    V3 t;
    getPerpendicularVectors(n, t, b );

    // Compute point on plane
    float d = plane.w;
    V3 P = -n * d;

    const float s = size * 0.5f;
    V3 A = P - (t * s) - (b * s);
    V3 B = P - (t * s) + (b * s);
    V3 C = P + (t * s) + (b * s);
    V3 D = P + (t * s) - (b * s);

    const auto v = mesh.vertices.size();
    mesh.vertices.push_back( S3DVertex(A, n, 0x8F8080FF, V2(0,1)) );
    mesh.vertices.push_back( S3DVertex(B, n, 0x8F80FF80, V2(0,0)) );
    mesh.vertices.push_back( S3DVertex(C, n, 0x8FFF8080, V2(1,0)) );
    mesh.vertices.push_back( S3DVertex(D, n, 0x8F80FFFF, V2(1,1)) );
    mesh.indices.push_back(v + 0);
    mesh.indices.push_back(v + 1);
    mesh.indices.push_back(v + 2);
    mesh.indices.push_back(v + 0);
    mesh.indices.push_back(v + 2);
    mesh.indices.push_back(v + 3);

    // float dotP1 = glm::dot(n,p1 - P);
    // float dotP2 = glm::dot(n,p2 - P);
    //DE_OK("n = ", n)
    //DE_OK("t = ", t)
    //DE_OK("b = ", b)
    //DE_OK("d = ", d)
    //DE_OK("P = ", P)
    //DE_OK("A = ", A)
    //DE_OK("B = ", B)
    //DE_OK("C = ", C)
    //DE_OK("D = ", D)
    //DE_OK("dotP1 = ", dotP1)
    //DE_OK("dotP2 = ", dotP2)
}

// **Berechnung des Schnittpunkts einer Kante mit einer Clipping-Ebene**
// static
BumpMapUtil::V
BumpMapUtil::computeIntersection(const V& a, const V& b, const V4& plane)
{
    V3 plane_normal = V3(plane);
    float d1 = glm::dot(plane_normal, a.pos) + plane.w;
    float d2 = glm::dot(plane_normal, b.pos) + plane.w;

    float t = d1 / (d1 - d2); // **Interpolation entlang der Kante**

    V out;
    out.pos = a.pos + t * (b.pos - a.pos); // lerp
    out.tex = a.tex + t * (b.tex - a.tex); // lerp
    out.color = lerpColor(a.color, b.color, t);
    out.normal = a.normal;
    return out;
}

// **Clipping eines Dreiecks gegen eine einzelne Clipping-Ebene**
// static
BumpMapUtil::Triangles
BumpMapUtil::clipTriangle(const Triangle& tri, const V4& plane)
{
    std::vector<V> vInside;
    std::vector<V> vOutside;

    //DE_BENNI("Triangle A(",tri.A.pos,"),
    //                  "B(",tri.B.pos,"), "
    //                  "C(",tri.C.pos,")")

    // **Jede Ecke des Dreiecks testen**
    for (const auto& vtx : {tri.A, tri.B, tri.C})
    {
        float d = glm::dot(V3(plane), vtx.pos) + plane.w;
        (d <= 0) ? vInside.push_back(vtx)
                 : vOutside.push_back(vtx);
    }

    if (vInside.size() == 3)
    {
        //DE_OK("Got triangle inside")
        return { tri }; // **Dreieck bleibt unverändert**
    }
    if (vInside.empty())
    {
        //DE_WARN("Got triangle outside")
        return {}; // **Komplett entfernt**
    }

    Triangles clippedTriangles;

    if (vInside.size() == 1 && vOutside.size() == 2)
    {
        //DE_WARN("Got 2 vertices outside")
        // **Fall: Eine Ecke innen, zwei außen → Neues kleineres Dreieck**
        V v1 = vInside[0];
        V i1 = computeIntersection(v1, vOutside[0], plane);
        V i2 = computeIntersection(v1, vOutside[1], plane);
        clippedTriangles.push_back(Triangle(v1, i1, i2));
    }
    else if (vInside.size() == 2 && vOutside.size() == 1)
    {
        //DE_WARN("Got 1 vertices outside")
        // **Fall: Zwei Ecken innen, eine außen → Zwei neue Dreiecke**
        V v1 = vInside[0];
        V v2 = vInside[1];
        V i = computeIntersection(v1, vOutside[0], plane);
        clippedTriangles.push_back(Triangle(v1, v2, i)); // **Erstes neues Dreieck**
        clippedTriangles.push_back(Triangle(v2, i, computeIntersection(v2, vOutside[0], plane))); // **Zweites neues Dreieck**
    }
    else
    {
        DE_ERROR("Got error inside(",vInside.size(),"), outside(",vOutside.size(),")" )
    }

    /*
    DE_BENNI("clippedTriangles.size() = ", clippedTriangles.size())
    for (size_t i = 0; i < clippedTriangles.size(); ++i)
    {
        const auto & t = clippedTriangles[i];
        DE_BENNI("clippedTriangle[",i,"] A(",t.A.pos,"), B(",t.B.pos,"), C(",t.C.pos,")")
    }
    */
    return clippedTriangles;
}

// **Rekursives Clipping eines Dreiecks gegen alle sechs Ebenen**
// static
BumpMapUtil::Triangles
BumpMapUtil::clipTriangles(const Triangles& in_triangles, const V4& plane)
{
    Triangles tris;

    for (const auto& tri : in_triangles)
    {
        Triangles clipped = clipTriangle(tri, plane);
        tris.insert(tris.end(), clipped.begin(), clipped.end());
    }

    return tris;
}

// **Rekursives Clipping eines Dreiecks gegen alle sechs Ebenen**
//static
BumpMapUtil::Triangles
BumpMapUtil::clipTriangles(const Triangles& in_triangles, const std::vector<V4>& planes)
{
    Triangles tris = in_triangles;

    for (const auto& plane : planes)
    {
        tris = clipTriangles(tris, plane);
    }

    return tris;
}

// static
void
BumpMapUtil::clipTrianglesHexagon(const Triangles& in_triangles,
                               Triangles& out_triangles, const V3& tileSize )
{
    const V3 n(0,1,0);
    const uint32_t color = 0xFFFFFFFF;
    const float w = tileSize.x;
    const float h = tileSize.z;

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
    const S3DVertex A( V3(     0,0,-.5f*h ), n, color, V2(.5f, 1 ) );
    const S3DVertex B( V3(-.5f*w,0,-.25f*h), n, color, V2(0, .75f ));
    const S3DVertex C( V3(-.5f*w,0, .25f*h), n, color, V2(0, .25f ));
    const S3DVertex D( V3(     0,0, .5f*h ), n, color, V2(.5f, 0 ) );
    const S3DVertex E( V3(0.5f*w,0, .25f*h), n, color, V2(1, .25f ));
    const S3DVertex F( V3(0.5f*w,0,-.25f*h), n, color, V2(1, .75f ));

    // **Sechs Clipping-Ebenen mit nach innen gerichteten Normalen**
    const std::vector<V4> clip_planes =
    {
        computePlaneFromXZ(A.pos,B.pos),
        computePlaneFromXZ(B.pos,C.pos),
        computePlaneFromXZ(C.pos,D.pos),
        computePlaneFromXZ(D.pos,E.pos),
        computePlaneFromXZ(E.pos,F.pos),
        computePlaneFromXZ(F.pos,A.pos)
    };

    out_triangles = clipTriangles( in_triangles, clip_planes );
}

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

void
BumpMapUtil::drawHexagon( Image & img, Recti pos, uint32_t color, bool blend )
{
    const int w = pos.w;
    const int h = pos.h;

    if ( w % 2 != 0 )
    {
        DE_WARN("w is odd, untested")
    }

    if ( h % 2 != 0 )
    {
        DE_WARN("h is odd, untested")
    }

    int x1 = pos.x;
    int x2 = pos.x + w/2;
    int x3 = pos.x + w - 1;

    int y1 = pos.y;
    int y2 = pos.y + h/4;
    int y3 = pos.y + (3*h)/4;
    int y4 = pos.y + h - 1;


    ImagePainter::drawLine( img, x1, y3-1, x2-1, y4, color, blend ); // AB
    ImagePainter::drawLine( img, x1, y2, x1, y3-2, color, blend ); // BC
    ImagePainter::drawLine( img, x1, y2, x2-1, y1, color, blend ); // CD
    ImagePainter::drawLine( img, x2, y1, x3, y2, color, blend ); // DE
    ImagePainter::drawLine( img, x3, y2+1, x3, y3-2, color, blend ); // EF
    ImagePainter::drawLine( img, x3, y3-1, x2, y4, color, blend ); // FA

    // ImagePainter::drawLine( img, x1, y3-1, x2-1, y4, dbRGBA(255,255,255), blend ); // AB
    // ImagePainter::drawLine( img, x1, y2, x1, y3-2, dbRGBA(255,0,255), blend ); // BC
    // ImagePainter::drawLine( img, x1, y2, x2-1, y1, dbRGBA(255,255,0), blend ); // CD
    // ImagePainter::drawLine( img, x2, y1, x3, y2, dbRGBA(255,0,0), blend ); // DE
    // ImagePainter::drawLine( img, x3, y2+1, x3, y3-2, dbRGBA(100,200,0), blend ); // EF
    // ImagePainter::drawLine( img, x3, y3-1, x2, y4, dbRGBA(100,0,200), blend ); // FA

    const int cx = pos.x + pos.w/2;
    const int cy = pos.y + pos.h/2;

    img.floodFill( cx,cy, color, img.getPixel(cx,cy) );
}

/*
void
BumpMapUtil::drawHexagon( Image & img, Recti pos, uint32_t color, bool blend )
{
    const int w = pos.w;
    const int h = pos.h;

    int x1 = pos.x;
    int x2 = pos.x + w/2;
    int x3 = pos.x + w - 1;

    int y1 = pos.y;
    int y2 = pos.y + h/4;
    int y3 = pos.y + (3*h)/4;
    int y4 = pos.y + h - 1;

    ImagePainter::drawLine( img, x1, y3, x2, y4, color ); // AB
    ImagePainter::drawLine( img, x1, y2, x1, y3, color ); // BC
    ImagePainter::drawLine( img, x1, y2, x2, y1, color ); // CD
    ImagePainter::drawLine( img, x2, y1, x3, y2, color ); // DE
    ImagePainter::drawLine( img, x3, y2, x3, y3, color ); // EF
    ImagePainter::drawLine( img, x3, y3, x2, y4, color ); // FA

    const int cx = pos.x + pos.w/2;
    const int cy = pos.y + pos.h/2;

    img.floodFill( cx,cy, color, img.getPixel(cx,cy) );
}
*/

Image
BumpMapUtil::createHexagonFilter( int w )
{
    Image img( w, w, PixelFormat::RGBA_32);
    img.fill( dbRGBA(0,0,0) );
    int d = w / 33;
    drawHexagon( img, Recti(d, d, w-2*d, w-2*d), 0xFFFFFFFF );
    return img;
}

/*
 *
Image
BumpMapUtil::createHexagonFilter( int w )
{
    Image img( w, w, PixelFormat::RGBA_32);
    img.fill( dbRGBA(0,0,0) );
    int d = w / 6;
    drawHexagon( img, Recti(d, d, w-2*d, w-2*d), 0xFFFFFFFF );
    return img;
}
Image
BumpMapUtil::createHexagonFilter( int w )
{
    Image img( w, w, PixelFormat::RGBA_32);
    img.fill( dbRGBA(0,0,0) );

    int pc = w / 5;

    float fStep = 255.0f / float(pc - 1);

    for ( int i = 0; i < pc; i++ )
    {
        uint8_t r = std::clamp( std::lround(fStep * i), 0l, 255l);
        uint32_t color = dbRGBA(r,r,r);

        auto rect = Recti(i+1, i+1, w-2 - (2*i), w-2 - (2*i));
        drawHexagon( img, rect, color );
    }

    return img;
}
*/

Image
BumpMapUtil::createHexagonFilterf( int w )
{
    Image img( w, w, PixelFormat::R32F);
    img.fill( 1.0f );

    auto drawLinef = [&]( int x1, int y1, int x2, int y2, float v )
    {
        auto setPixelf = [&] ( int32_t x, int32_t y ) { img.setPixel4f( x,y, glm::vec4{ v,v,v,v }, false ); };
        Bresenham::drawLine( x1,y1,x2,y2, setPixelf );
    };

    int pc = w / 6;

    float fStep = 1.0f / float(pc - 1);

    for ( int i = 0; i < pc; i++ )
    {
        int x1 = i;
        int x2 = w/2;
        int x3 = w - 1 - i;

        int y1 = i;
        int y2 = w/4 + i;
        int y3 = (3*w)/4 - i;
        int y4 = w - 1 - i;

        float color = fStep * i;

        drawLinef( x1, y3, x2, y4, color ); // AB
        drawLinef( x1, y2, x1, y3, color ); // BC
        drawLinef( x1, y2, x2, y1, color ); // CD
        drawLinef( x2, y1, x3, y2, color ); // DE
        drawLinef( x3, y2, x3, y3, color ); // EF
        drawLinef( x3, y3, x2, y4, color ); // FA
    }

    return img;
}

void
BumpMapUtil::filterGrey( Image & img, const Image & mask )
{
    if (img.w() != mask.w())
    {
        DE_ERROR("img.w() != mask.w()")
    }

    if (img.h() != mask.h())
    {
        DE_ERROR("img.h() != mask.h()")
    }

    for ( int y = 0; y < img.h(); y++ )
    {
        for ( int x = 0; x < img.w(); x++ )
        {
            uint32_t c_mask = mask.getPixel(x,y);
            float fR = float(dbRGBA_R(c_mask)) / 255.0f;

            uint32_t c_img = img.getPixel(x,y);
            uint8_t r0 = dbRGBA_R(c_img);
            uint8_t r = std::clamp( std::lround( fR * r0 ), 0l, 255l );
            // uint8_t g0 = dbRGBA_G(c_img);
            // uint8_t g1 = std::clamp( std::lround( f_mask * g0 ), 0l, 255l );
            // uint8_t b0 = dbRGBA_B(c_img);
            // uint8_t b1 = std::clamp( std::lround( f_mask * b0 ), 0l, 255l );

            img.setPixel( x,y, dbRGBA(r,r,r) );
        }
    }
}

// static
void BumpMapUtil::windowVertices( SMeshBuffer & o )
{
    auto bx = o.boundingBox.size().x * 0.5f;
    auto by = o.boundingBox.size().y;
    auto bz = o.boundingBox.size().z * 0.5f;

    auto cx = o.boundingBox.center().x;
    auto cz = o.boundingBox.center().z;

    auto r_max = 0.60f * sqrt(bx*bx + bz*bz);

    for ( size_t i = 0; i < o.vertices.size(); ++i )
    {
        auto & v = o.vertices[i];
        auto dx = v.pos.x - cx;
        auto dz = v.pos.z - cz;
        auto r = sqrt(dx*dx + dz*dz);

        auto f = (r >= r_max) ? 0.0f : sqrt(1-((r/r_max)*(r/r_max)));
        v.pos.y *= f;
    }
}

// static
void BumpMapUtil::smoothNormals( SMeshBuffer & o )
{
    if ( o.getPrimitiveType() != PrimitiveType::Triangles )
    {
        DE_ERROR("No triangles ", o.name)
        return;
    }

    if ( o.vertices.empty() )
    {
        DE_ERROR("No vertices ", o.name)
        return;
    }

    if ( o.indices.empty() )
    {
        DE_ERROR("No indices ", o.name)
        return;
    }

    std::vector< glm::vec3 > normals;
    normals.reserve( o.vertices.size() );

    std::vector< Triangle3f > triangles;
    triangles.reserve( 16 );

    for ( size_t i = 0; i < o.vertices.size(); ++i )
    {
        // Collect all triangles that use o.vertices[i]:
        triangles.clear();

        for ( size_t k = 0; k < o.indices.size()/3; ++k )
        {
            uint32_t iA = o.indices[ 3*i + 0 ];
            uint32_t iB = o.indices[ 3*i + 1 ];
            uint32_t iC = o.indices[ 3*i + 2 ];

            if (iA == i || iB == i || iC == i)
            {
                auto const & a = o.vertices[ iA ].pos;
                auto const & b = o.vertices[ iB ].pos;
                auto const & c = o.vertices[ iC ].pos;
                triangles.emplace_back( a, b, c );
            }
        }

        if (triangles.empty())
        {
            normals[i] = glm::vec3(0,1,0);
        }
        else
        {
            glm::vec3 normal = glm::vec3(0,1,0);

            for ( size_t k = 0; k < triangles.size(); ++k )
            {
                glm::vec3 n2 = triangles[k].computeNormal();
                normal += n2;
            }

            normals[i] = glm::normalize( normal );
        }
    }

    // Give vertices the smoothed normals:
    for ( size_t i = 0; i < o.vertices.size(); ++i )
    {
        o.vertices[i].normal = normals[i];
    }
}

/*

// testNevada( m_meshTileDesert, m_driver, glm::vec3(100.0, 10.0, 100.0 ) );
// testMountWilder( m_meshTileA, m_driver, glm::vec3(100.0, 10.0, 100.0 ) );
// testLehm( m_meshTileB, m_driver, glm::vec3(100.0, 10.0, 100.0 ) );
// testThuringen( m_meshTileC, m_driver, glm::vec3(100.0, 10.0, 100.0 ) );
// testWeizen( m_meshTileD, m_driver, glm::vec3(100.0, 10.0, 100.0 ) );
// testAlpen( m_meshTileE, m_driver, glm::vec3(100.0, 10.0, 100.0 ) );

// static
void
BumpMapUtil::test1_4x4( SMeshBuffer & o, VideoDriver* driver, V3 tileSize )
{
    Image imgDiffuseMap( 4, 4, PixelFormat::RGBA_32 );
    imgDiffuseMap.setPixel(0,0,dbRGBA(255,0,0));
    imgDiffuseMap.setPixel(1,0,dbRGBA(0,255,0));
    imgDiffuseMap.setPixel(2,0,dbRGBA(0,0,255));
    imgDiffuseMap.setPixel(3,0,dbRGBA(255,255,0));

    imgDiffuseMap.setPixel(0,1,dbRGBA(155,55,255));
    imgDiffuseMap.setPixel(1,1,dbRGBA(0,200,0));
    imgDiffuseMap.setPixel(2,1,dbRGBA(255,0,255));
    imgDiffuseMap.setPixel(3,1,dbRGBA(255,155,0));

    imgDiffuseMap.setPixel(0,2,dbRGBA(0,0,0));
    imgDiffuseMap.setPixel(1,2,dbRGBA(0,0,100));
    imgDiffuseMap.setPixel(2,2,dbRGBA(0,0,255));
    imgDiffuseMap.setPixel(3,2,dbRGBA(255,255,0));

    imgDiffuseMap.setPixel(0,3,dbRGBA(255,255,255));
    imgDiffuseMap.setPixel(1,3,dbRGBA(0,200,0));
    imgDiffuseMap.setPixel(2,3,dbRGBA(55,0,255));
    imgDiffuseMap.setPixel(3,3,dbRGBA(255,155,0));

    Triangles in_triangles;
    createTriangles( in_triangles, imgDiffuseMap, tileSize );
    centerTrianglesXZ( in_triangles );



    Triangles out_triangles = in_triangles;
    //clipPioneerHexagon( in_triangles, out_triangles, tileSize );

    addTriangles( o, out_triangles );

    //auto tex = driver->createTexture2D("img4x4", imgDiffuseMap);
    //o.setTexture( 0, tex );
    o.material.Lighting = 0;
    o.material.FogEnable = false;
    o.material.state.blend = Blend::alphaBlend();
    o.material.state.culling = Culling::disabled();
    o.upload();
}

// static
void
BumpMapUtil::test2_4x4( SMeshBuffer & o, VideoDriver* driver, V3 tileSize )
{
    Image imgDiffuseMap( 4, 4, PixelFormat::RGBA_32 );
    imgDiffuseMap.setPixel(0,0,dbRGBA(255,0,0));
    imgDiffuseMap.setPixel(1,0,dbRGBA(0,255,0));
    imgDiffuseMap.setPixel(2,0,dbRGBA(0,0,255));
    imgDiffuseMap.setPixel(3,0,dbRGBA(255,255,0));

    imgDiffuseMap.setPixel(0,1,dbRGBA(155,55,255));
    imgDiffuseMap.setPixel(1,1,dbRGBA(0,200,0));
    imgDiffuseMap.setPixel(2,1,dbRGBA(255,0,255));
    imgDiffuseMap.setPixel(3,1,dbRGBA(255,155,0));

    imgDiffuseMap.setPixel(0,2,dbRGBA(0,0,0));
    imgDiffuseMap.setPixel(1,2,dbRGBA(0,0,100));
    imgDiffuseMap.setPixel(2,2,dbRGBA(0,0,255));
    imgDiffuseMap.setPixel(3,2,dbRGBA(255,255,0));

    imgDiffuseMap.setPixel(0,3,dbRGBA(255,255,255));
    imgDiffuseMap.setPixel(1,3,dbRGBA(0,200,0));
    imgDiffuseMap.setPixel(2,3,dbRGBA(55,0,255));
    imgDiffuseMap.setPixel(3,3,dbRGBA(255,155,0));

    Triangles in_triangles;
    createTriangles( in_triangles, imgDiffuseMap, tileSize );
    centerTrianglesXZ( in_triangles );

    BBox bbox = computeBBox( in_triangles );
    DE_BENNI("BBox Min(",bbox.minEdge,"), Max(",bbox.maxEdge,")")

    // Triangles out_triangles;
    //clipPioneerHexagon( in_triangles, out_triangles, tileSize );

    Triangles out_triangles;
    clipTrianglesHexagon( in_triangles, out_triangles, tileSize );

    addTriangles( o, out_triangles );

    // addPlaneXZ( o, planeAB, 110.0f );
    // addPlaneXZ( o, planeBC, 110.0f );
    // addPlaneXZ( o, planeCD, 110.0f );
    // addPlaneXZ( o, planeDE, 110.0f );
    // addPlaneXZ( o, planeEF, 110.0f );
    // addPlaneXZ( o, planeFA, 110.0f );

    //auto tex = driver->createTexture2D("img4x4", imgDiffuseMap);
    //o.setTexture( 0, tex );
    o.material.Lighting = 0;
    o.material.FogEnable = false;
    o.material.state.blend = Blend::alphaBlend();
    o.material.state.culling = Culling::disabled();
    o.upload();
}

// static
void
BumpMapUtil::testMask()
{
    Image img( 17, 17, PixelFormat::RGBA_32 );
    img.fill( dbRGBA(255,120,32) );
    auto r0 = img.rect();
    drawHexagon(img, r0, dbRGBA(0,90,0) );
    DE_DEBUG("r0 = ", r0.str())
    //auto r1 = Recti(r0.x + 1, r0.y + 1, r0.w-2, r0.h - 2);
    //drawHexagon(img, r1, dbRGBA(255,255,0,128), true );
    dbSaveImage( img, "a_testMask.png" );
}

// Desert,A,B,C,D,E
// static
void
BumpMapUtil::testImpl( SMeshBuffer & o, VideoDriver* driver,
    V3 tileSize, std::string uriBumpMap, std::string uriDiffuseMap )
{
    // =============== DiffuseMap: ===========================================
    Image diffMap;
    if (!dbLoadImage(diffMap, uriDiffuseMap))
    {
        DE_ERROR("No diffMap ", uriDiffuseMap)
    }
    DE_BENNI("Loaded diffMap = ", dbStrBytes(diffMap.computeMemoryConsumption()) )

    BBox bbD1 = computeBBox( diffMap );
    DE_BENNI("BBox1 Min(",bbD1.minEdge,"), Max(",bbD1.maxEdge,")")

    // =============== BumpMap: ===========================================

    Image bumpMap;
    if (!dbLoadImage(bumpMap, uriBumpMap))
    {
        DE_ERROR("No bumpMap ", uriBumpMap)
    }
    DE_BENNI("Loaded bumpMap = ", dbStrBytes(bumpMap.computeMemoryConsumption()) )

    //dbSaveImage(bumpMap, "a_MountWilder_bumpMap.webp");
    //dbSaveImage(diffMap, "a_MountWilder_diffMap.webp");

#if 0
    auto crop = BumpMapUtil::computeSquaredCenter(bumpMap);
    DE_BENNI("Crop = ", crop.str() )

    bumpMap = BumpMapUtil::crop( bumpMap, crop );
    DE_BENNI("Cropped bumpMap = ", bumpMap.str() )

    diffMap = BumpMapUtil::crop( diffMap, crop );
    DE_BENNI("Cropped diffMap = ", diffMap.str() )
#endif

    saturateHeightmap( bumpMap );
    DE_BENNI("Saturated bumpMap = ", bumpMap.str() )

#if 0
// Export resources for embedding into *.exe
    std::string uriBumpMap2 = FileSystem::fileBase( uriBumpMap ) + ".webp";
    dbSaveImage(bumpMap, uriBumpMap2 );

    std::string uriDiffMap2 = FileSystem::fileBase( uriDiffuseMap ) + ".webp";
    dbSaveImage(diffMap, uriDiffMap2 );
#endif

    bumpMap = ImageScaler::resizeBilinear( bumpMap, 118,118 );
    DE_BENNI("Resized bumpMap = ", bumpMap.str() )

    //Image filterMap = createHexagonFilter( bumpMap.w() );
    //filterGrey( bumpMap, filterMap );

    //std::string uriSave1 = FileSystem::fileBase( uriBumpMap ) + ".png";
    //dbSaveImage(bumpMap, uriSave1 );

    // =============== Mesh Creation & Clipping: =============================

    Triangles in_triangles;
    createTriangles( in_triangles, bumpMap, tileSize );
    centerTrianglesXZ( in_triangles );

    BBox bbox = computeBBox( in_triangles );
    DE_BENNI("BBox Min(",bbox.minEdge,"), Max(",bbox.maxEdge,")")

    Triangles out_triangles;
    clipTrianglesHexagon( in_triangles, out_triangles, tileSize );

    repairWinding( out_triangles );
    DE_OK("Before SMeshBuffer has ", out_triangles.size() * 3, " vertices")
    DE_OK("Before SMeshBuffer has ", out_triangles.size() * 3, " indices")

    SO so;
    so.setAF( 1.0f );
    so.setMag( SO::Magnify::Linear );
    so.setMin( SO::Minify::LinearMipmapLinear );
    so.setWrapR( SO::Wrap::ClampToEdge );
    so.setWrapS( SO::Wrap::ClampToEdge );
    so.setWrapT( SO::Wrap::ClampToEdge );
    auto tex = driver->createTexture2D(uriDiffuseMap, diffMap, so);
    o.setTexture( 0, tex );

    //auto tex = driver->createTexture2D("img4x4", imgDiffuseMap);
    //o.setTexture( 0, tex );
    o.name = FileSystem::fileBase(uriBumpMap);
    o.material.Lighting = 0;
    o.material.FogEnable = false;
    o.material.state.blend = Blend::alphaBlend();
    o.material.state.culling = Culling::disabled();
    addTriangles( o, out_triangles );

    DE_OK("After SMeshBuffer has ", o.vertices.size(), " vertices")
    DE_OK("After SMeshBuffer has ", o.indices.size(), " indices")

    windowVertices( o );

    SMeshBufferTool::computeNormals( o );

    //o.upload();

    DE_OK( o.str() )
}

// Desert
// static
void
BumpMapUtil::testNevada( SMeshBuffer & o, VideoDriver* driver )
{
    DE_PERF_MARKER
    auto tileSize = V3(100.0, 10.0, 100.0 );
    std::string mediaDir = "media/H3/gfx/tiles2/";
    std::string uriBumpMap = mediaDir + "Desert_h.webp";
    std::string uriDiffMap = mediaDir + "Desert_d.webp";
    testImpl( o, driver, tileSize, uriBumpMap, uriDiffMap );
}

// A
// static
void
BumpMapUtil::testMountWilder( SMeshBuffer & o, VideoDriver* driver )
{
    DE_PERF_MARKER
    auto tileSize = V3(100.0, 10.0, 100.0 );
    std::string mediaDir = "media/H3/gfx/tiles2/";
    std::string uriBumpMap = mediaDir + "A_h.webp";
    std::string uriDiffMap = mediaDir + "A_d.webp";
    testImpl( o, driver, tileSize, uriBumpMap, uriDiffMap );
}

// B
// static
void
BumpMapUtil::testLehm( SMeshBuffer & o, VideoDriver* driver )
{
    DE_PERF_MARKER
    auto tileSize = V3(100.0, 10.0, 100.0 );
    std::string mediaDir = "media/H3/gfx/tiles2/";
    std::string uriBumpMap = mediaDir + "B_h.webp";
    std::string uriDiffMap = mediaDir + "B_d.webp";
    testImpl( o, driver, tileSize, uriBumpMap, uriDiffMap );
}

// C
// static
void
BumpMapUtil::testThuringen( SMeshBuffer & o, VideoDriver* driver )
{
    DE_PERF_MARKER
    auto tileSize = V3(100.0, 10.0, 100.0 );
    std::string mediaDir = "media/H3/gfx/tiles2/";
    std::string uriBumpMap = mediaDir + "C_h.webp";
    std::string uriDiffMap = mediaDir + "C_d.webp";
    testImpl( o, driver, tileSize, uriBumpMap, uriDiffMap );
}

// D
// static
void
BumpMapUtil::testWeizen( SMeshBuffer & o, VideoDriver* driver )
{
    DE_PERF_MARKER
    auto tileSize = V3(100.0, 10.0, 100.0 );
    std::string mediaDir = "media/H3/gfx/tiles2/";
    std::string uriBumpMap = mediaDir + "D_h.webp";
    std::string uriDiffMap = mediaDir + "D_d.webp";
    testImpl( o, driver, tileSize, uriBumpMap, uriDiffMap );
}

// E
// static
void
BumpMapUtil::testAlpen( SMeshBuffer & o, VideoDriver* driver )
{
    DE_PERF_MARKER
    auto tileSize = V3(100.0, 10.0, 100.0 );
    std::string mediaDir = "media/H3/gfx/tiles2/";
    std::string uriBumpMap = mediaDir + "E_h.webp";
    std::string uriDiffMap = mediaDir + "E_d.webp";
    testImpl( o, driver, tileSize, uriBumpMap, uriDiffMap );
}

void
Hillplane::addTriangles( SMeshBuffer & o, Image const & heightMap, glm::vec3 siz )
{
    o.setPrimitiveType( PrimitiveType::Triangles );
    o.setLighting( 2 );

    int32_t w = heightMap.w();
    int32_t h = heightMap.h();
    if ( w < 2 ) { DE_ERROR("Not enough heightmap width") return; }
    if ( h < 2 ) { DE_ERROR("Not enough heightmap height") return; }

    if ( heightMap.pixelFormat() == PixelFormat::R32F )
    {
        DE_ERROR("[Benni] Got R32F GeoTIFF heightmap")
    }

    //====================
    // Temporary vertices
    //====================
    std::vector< S3DVertex > vertices;
    vertices.reserve( size_t( w ) * size_t( h ) );

    float cell_x = siz.x / float( w - 1 );
    float cell_y = siz.y / 255.0f;
    float cell_z = siz.z / float( h - 1 );
    for ( int32_t j = 0; j < h; ++j )
    {
        for ( int32_t i = 0; i < w; ++i )
        {
            uint8_t r = dbRGBA_R( heightMap.getPixel( i, j ) );
            float x = cell_x * i;
            float y = cell_y * float( r );
            float z = cell_z * j;
            float u = float( i ) / float( w - 1 );
            float v = float( j ) / float( h - 1 );
            uint32_t color = 0xFFFFFFFF; // RGBA( r,r,r );
            vertices.emplace_back( x,y,z, 0,1,0, color, u,v );
        }
    }

    //=====================
    // Create vertex quads
    //=====================
    o.vertices.clear();
    o.vertices.reserve( 6 * size_t( w-1 ) * size_t( h-1 ) );

    for ( int32_t j = 0; j < h - 1; ++j )
    {
        for ( int32_t i = 0; i < w - 1; ++i )
        {
            size_t iA = size_t( i ) + size_t( w ) * size_t( j );
            size_t iB = iA + size_t( w );
            size_t iC = iB + 1;
            size_t iD = iA + 1;

            o.vertices.emplace_back( vertices[iA] );
            o.vertices.emplace_back( vertices[iB] );
            o.vertices.emplace_back( vertices[iC] );

            o.vertices.emplace_back( vertices[iA] );
            o.vertices.emplace_back( vertices[iC] );
            o.vertices.emplace_back( vertices[iD] );
        }
    }

    o.recalculateBoundingBox();

    SMeshBufferTool::computeNormals( o );
}

void
Hillplane::addIndexedTriangles( SMeshBuffer & o, Image const & heightMap, glm::vec3 siz )
{
    o.setPrimitiveType( PrimitiveType::Triangles );
    o.setLighting( 2 );
    //o.setCulling( false );
    //o.setDepth( Depth( Depth::Always, true ) );
    //o.setTexture( 0, m_Texture0 );

    int32_t w = heightMap.w();
    int32_t h = heightMap.h();
    if ( w < 2 ) { DE_ERROR("Not enough heightmap width") return; }
    if ( h < 2 ) { DE_ERROR("Not enough heightmap height") return; }

    if ( heightMap.pixelFormat() == PixelFormat::R32F )
    {
        DE_ERROR("[Benni] Got R32F GeoTIFF heightmap")
    }

    // createTerrainVertices();
    o.vertices.clear();
    o.vertices.reserve( size_t( w ) * size_t( h ) );

    float cell_x = siz.x / float( w - 1 );
    float cell_y = siz.y / 255.0f;
    float cell_z = siz.z / float( h - 1 );
    for ( int32_t j = 0; j < h; ++j )
    {
        for ( int32_t i = 0; i < w; ++i )
        {
            uint8_t r = dbRGBA_R( heightMap.getPixel( i, j ) );
            float x = cell_x * i;
            float y = cell_y * float( r );
            float z = cell_z * j;
            float u = float( i ) / float( w - 1 );
            float v = float( j ) / float( h - 1 );
            uint32_t color = 0xFFFFFFFF; // RGBA( r,r,r );
            o.addVertex( S3DVertex( x,y,z, 0,1,0, color, u,v ));
        }
    }

    o.recalculateBoundingBox();

    SMeshBufferTool::computeNormals( o );

    //createTerrainIndices();
    o.indices.clear();
    o.indices.reserve( 6 * size_t( w-1 ) * size_t( h-1 ) );

    for ( int32_t j = 0; j < h-1; ++j )
    {
        for ( int32_t i = 0; i < w-1; ++i )
        {
            size_t iA = size_t( i ) + size_t( w ) * size_t( j );
            size_t iB = iA + size_t( w );
            size_t iC = iB + 1;
            size_t iD = iA + 1;
            o.addIndexedQuad( iA, iB, iC, iD );
        }
    }
}

*/

} // end namespace gpu.
} // end namespace de.

#endif
