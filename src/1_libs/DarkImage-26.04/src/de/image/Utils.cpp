#include <de/image/Utils.h>

namespace de {

void
cropImage( Image & img, u32 left, u32 top, u32 right, u32 bottom )
{
    int w = img.w();
    int h = img.h();

    int crop_w = left + right;
    int crop_h = top + bottom;

    if (crop_w < 1 && crop_h < 1)
    {
        return; // Nothing todo.
    }

    if (crop_w > w || crop_h > h)
    {
        DE_ERROR("crop_w > w || crop_h > h")
        return;
    }

    int w2 = w - crop_w;
    int h2 = h - crop_h;
    Image tmp( w2,h2, PixelFormat::R8G8B8A8 );

    for ( int y = 0; y < h2; ++y )
    {
        for ( int x = 0; x < w2; ++x )
        {
            const u32 color = img.getPixel(x + int(left), y + int(top));
            tmp.setPixel( x,y, color );
        }
    }

    img = tmp;
}

void
replaceColor( Image & img, u32 srcColor, u32 dstColor )
{
    int w = img.w();
    int h = img.h();

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            u32 color = img.getPixel(x,y);
            if (color == srcColor)
            {
                img.setPixel( x,y, dstColor );
            }
        }
    }
}

//
void
addAlphaChannel( Image & img )
{
    if ( img.a() > 0 ) { return; } // Nothing todo

    int w = img.w();
    int h = img.h();
    Image tmp( w,h, PixelFormat::R8G8B8A8 );

    for ( int y = 0; y < h; ++y )
    {
        for ( int x = 0; x < w; ++x )
        {
            const u32 color = img.getPixel(x,y);
            const uint8_t r = dbRGBA_R(color);
            const uint8_t g = dbRGBA_G(color);
            const uint8_t b = dbRGBA_B(color);
            tmp.setPixel( x,y, dbRGBA(r,g,b,255) );
        }
    }

    img = tmp;
}

Image
genHeightMap( Image const & src )
{
   Image heightMap = src;
   saturateImage( heightMap );
   //dbSaveImage( heightMap, "genHeightMap_saturate.png");
   convertToGrayscale( heightMap );
   //dbSaveImage( heightMap, "genHeightMap_greyscale.png");
   saturateRedChannel( heightMap );
   //dbSaveImage( heightMap, "genHeightMap_greyscale_saturate.png");
   return heightMap;
}

Image
genNormalMap( Image const & heightMap )
{
   Image normalMap = heightMap;

   int w = normalMap.w();
   int h = normalMap.h();

   for ( int32_t y = 0; y < h-1; ++y )
   {
      for ( int32_t x = 0; x < w-1; ++x )
      {
         int iA = dbRGBA_R(heightMap.getPixel( x,y ));
         int iB = dbRGBA_R(heightMap.getPixel( x,y+1 ));
         //int iC = dbRGBA_R(heightMap.getPixel( x+1,y+1 ));
         int iD = dbRGBA_R(heightMap.getPixel( x+1,y ));
         float dx = float(iD - iA)/255.0f;
         float dy = float(iB - iA)/255.0f;
         float dz = 1.0f;
         glm::vec3 n = 0.5f * glm::normalize( glm::vec3(dx,dy,dz) ) + 0.5f;
         uint8_t nx = uint8_t(de_clampi( 255.0f * n.x, 0, 255 ));
         uint8_t ny = uint8_t(de_clampi( 255.0f * n.y, 0, 255 ));
         uint8_t nz = uint8_t(de_clampi( 255.0f * n.z, 0, 255 ));
         normalMap.setPixel( x,y, dbRGBA(nx,ny,nz,255) );
      }
   }

   //dbSaveImage( normalMap, "genNormalMap.png");

   return normalMap;
}

void
convertToGrayscale( Image & img )
{
   int w = img.w();
   int h = img.h();

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         int r = dbRGBA_R(color);
         int g = dbRGBA_G(color);
         int b = dbRGBA_B(color);
         uint8_t grey = uint8_t( (r + 2*g + b)/4 );
         img.setPixel( x,y, dbRGBA(grey,grey,grey,255) );
      }
   }
}

void
computeMinMaxColor( Image const & img, uint32_t & minColor, uint32_t & maxColor )
{
   int w = img.w();
   int h = img.h();
   uint8_t minR = 255;
   uint8_t maxR = 0;
   uint8_t minG = 255;
   uint8_t maxG = 0;
   uint8_t minB = 255;
   uint8_t maxB = 0;
   uint8_t minA = 255;
   uint8_t maxA = 0;

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         uint8_t r = dbRGBA_R(color);
         uint8_t g = dbRGBA_G(color);
         uint8_t b = dbRGBA_B(color);
         uint8_t a = dbRGBA_A(color);
         minR = std::min(minR,r);
         maxR = std::max(maxR,r);
         minG = std::min(minG,g);
         maxG = std::max(maxG,g);
         minB = std::min(minB,b);
         maxB = std::max(maxB,b);
         minA = std::min(minA,a);
         maxA = std::max(maxA,a);
      }
   }
   minColor = dbRGBA(minR,minG,minB,minA);
   maxColor = dbRGBA(maxR,maxG,maxB,maxA);
}


// If your normalMap is green and red then its 2d and lacks 3d z blue.
// Adds z=255 as (B)lue component to (RG) normalMap xy,
// normals are not normalized in length.
// Correction could simply be done in shader. This is for visual debug the world.
void
saturateImage( Image & img )
{
   int w = img.w();
   int h = img.h();

   uint32_t minColor = 0;
   uint32_t maxColor = 0;
   computeMinMaxColor( img, minColor, maxColor );
   int const minR = dbRGBA_R(minColor);
   int const minG = dbRGBA_G(minColor);
   int const minB = dbRGBA_B(minColor);
   int const maxR = dbRGBA_R(maxColor);
   int const maxG = dbRGBA_G(maxColor);
   int const maxB = dbRGBA_B(maxColor);
   //int const minA = dbRGBA_A(minColor);
   //int const maxA = dbRGBA_A(maxColor);

   int const dR = maxR - minR;
   int const dG = maxG - minG;
   int const dB = maxB - minB;

   float const fR = 255.0f / float(dR);
   float const fG = 255.0f / float(dG);
   float const fB = 255.0f / float(dB);

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         int r = dbRGBA_R(color);
         int g = dbRGBA_G(color);
         int b = dbRGBA_B(color);
         int a = dbRGBA_A(color);

         uint8_t finalR = uint8_t(de_clampi( float(r - minR) * fR, 0, 255));
         uint8_t finalG = uint8_t(de_clampi( float(g - minG) * fG, 0, 255));
         uint8_t finalB = uint8_t(de_clampi( float(b - minB) * fB, 0, 255));
         //uint8_t finalA = uint8_t(de_clampi( de_fma(mA,a,nA), 0, 255));
         img.setPixel( x,y, dbRGBA(finalR,finalG,finalB,a) );
      }
   }
}

void
saturateRedChannel( Image & img )
{
   int w = img.w();
   int h = img.h();

   int minR = 255;
   int maxR = 0;

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         int r = dbRGBA_R(color);
         minR = std::min(minR,r);
         maxR = std::max(maxR,r);
      }
   }

   float fR = 255.0f / float( int(maxR) - int(minR) );

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         uint8_t r = dbRGBA_R(color);
         uint8_t finalR = uint8_t(std::clamp( int(float(r - minR) * fR), 0, 255));
         img.setPixel( x,y, dbRGBA( finalR,finalR,finalR,255 ) );
      }
   }
}


void
saturateAlphaChannel( Image & img )
{
   int w = img.w();
   int h = img.h();

   uint8_t minValue = 255;
   uint8_t maxValue = 0;

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         uint8_t a = dbRGBA_A(color);
         minValue = std::min(minValue,a);
         maxValue = std::max(maxValue,a);
      }
   }

   float n = float(minValue);
   float m = 255.0f / float( int(maxValue) - int(minValue) );

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         uint8_t a = dbRGBA_A(color);
         uint8_t b = std::clamp( int(m * float(a) + n), 0, 255);
         dbRGBA_setA( color, b );
         img.setPixel( x,y, color );
      }
   }
}

//
void
scaleAlphaChannel( Image & img, float scale )
{
//   if ( img.getAlphaBits() < 1 )
//   {
//      return; // Nothing todo
//   }
   int w = img.w();
   int h = img.h();

   for ( int j = 0; j < h; j++ )
   {
      for ( int i = 0; i < w; i++ )
      {
         uint32_t color = img.getPixel(i,j);
         uint8_t r = uint8_t(std::clamp( int( float(dbRGBA_R(color)) * scale ), 0, 255));
         uint8_t g = uint8_t(std::clamp( int( float(dbRGBA_G(color)) * scale ), 0, 255));
         uint8_t b = uint8_t(std::clamp( int( float(dbRGBA_B(color)) * scale ), 0, 255));
         uint8_t a = uint8_t(std::clamp( int( float(dbRGBA_A(color)) * scale ), 0, 255));
         img.setPixel( i,j, dbRGBA(r,g,b,a) );
      }
   }
}

//
void
removeAlphaChannel( Image & img )
{
   if ( img.a() < 1 )
   {
      return; // Nothing todo
   }

   int w = img.w();
   int h = img.h();
   Image tmp( w,h, PixelFormat::R8G8B8 );

   for ( int j = 0; j < h; j++ )
   {
      for ( int i = 0; i < w; i++ )
      {
         tmp.setPixel( i,j, img.getPixel(i,j) );
      }
   }

   img = tmp;
}



// If your normalMap is green and red then its 2d and lacks 3d z blue.
// Adds z=255 as (B)lue component to (RG) normalMap xy,
// normals are not normalized in length.
// Correction could simply be done in shader. This is for visual debug the world.
void
correct2DNormals( Image & img, float amplitude )
{
   int w = img.w();
   int h = img.h();

//	float vh = float(h) / float(w);
//	float hh = float(w) / float(h);

//	int32_t *p = (int32_t*)img.data();
//	int32_t* in = new int32_t[ w * h ];
//	memcpy(in, p, w * h * 4);

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.getPixel( x,y );
         uint8_t r = dbRGBA_R(color);
         uint8_t g = dbRGBA_G(color);
         img.setPixel( x,y, dbRGBA(r,g,255,255) );
      }
   }
}

// Expects one image with RGB encoded normals and second image with R(GB) heights.
// This is what the modified parallax shader expects
Image
fuseImages_NormalMap_plus_BumpMap( Image const & normals, Image const & heights )
{
   int w = std::min( normals.w(), heights.w() );
   int h = std::min( normals.h(), heights.h() );
   Image img( w,h );

   for ( int j = 0; j < h; j++ )
   {
      for ( int i = 0; i < w; i++ )
      {
         uint32_t normal = normals.getPixel(i,j);
         uint8_t r = dbRGBA_R(normal);
         uint8_t g = dbRGBA_G(normal);
         uint8_t b = dbRGBA_B(normal);
         uint8_t a = dbRGBA_R(heights.getPixel(i,j));
         img.setPixel( i,j, dbRGBA(r,g,b,a) );
      }
   }
   return img;
}

/*
// ===========================================================================
void SMeshHillplane::add( SMeshBuffer & o, std::string uri, glm::vec3 siz )
{
    Image heightMap;
    dbLoadImage( heightMap, uri );
    add( o, heightMap, siz );
}


void SMeshHillplane::add( SMeshBuffer & o, Image const & heightMap, glm::vec3 siz )
{
    o.setPrimitiveType( PrimitiveType::Triangles );
    o.setLighting( 2 );
    //o.setCulling( false );
    //o.setDepth( Depth( Depth::Always, true ) );
    //o.setTexture( 0, m_Texture0 );

    int32_t w = heightMap.getWidth();
    int32_t h = heightMap.getHeight();
    if ( w < 2 ) { DE_ERROR("Not enough heightmap width") return; }
    if ( h < 2 ) { DE_ERROR("Not enough heightmap height") return; }

    if ( heightMap.getFormat() != PixelFormat::R32F )
    {
        DE_ERROR("[Benni] No R32F EXR/GeoTIFF heightMap")
        return;
    }

    // createTerrainVertices();
    o.m_vertices.clear();
    o.m_vertices.reserve( size_t( w ) * size_t( h ) );

    float cell_x = siz.x / float( w - 1 );
    float cell_y = siz.y / 255.0f;
    float cell_z = siz.z / float( h - 1 );
    for ( int32_t j = 0; j < h; ++j )
    {
        for ( int32_t i = 0; i < w; ++i )
        {
            uint8_t r = RGBA_R( heightMap.getPixel( i, j ) );
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
    o.m_indices.clear();
    o.m_indices.reserve( 6 * size_t( w-1 ) * size_t( h-1 ) );

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

void SMeshHillplane::addLines( SMeshBuffer & o, Image const & heightMap, glm::vec3 siz )
{
    o.setPrimitiveType( PrimitiveType::Lines );
    o.setLighting( 0 );
    //o.setCulling( false );
    //o.setDepth( Depth( Depth::Always, true ) );
    //o.setTexture( 0, m_Texture0 );

    int32_t w = heightMap.getWidth();
    int32_t h = heightMap.getHeight();
    if ( w < 2 ) { DE_ERROR("Not enough heightmap width") return; }
    if ( h < 2 ) { DE_ERROR("Not enough heightmap height") return; }

    if ( heightMap.getFormat() != PixelFormat::R32F )
    {
        DE_ERROR("[Benni] No R32F EXR/GeoTIFF heightMap")
        return;
    }

    // createTerrainVertices();
    o.m_vertices.clear();
    o.m_vertices.reserve( size_t( w ) * size_t( h ) );

    float fMin = std::numeric_limits<float>::max();
    float fMax = std::numeric_limits<float>::lowest();

    float cell_x = siz.x / float( w - 1 );
    float cell_y = siz.y;
    float cell_z = siz.z / float( h - 1 );
    for ( int32_t j = 0; j < h; ++j )
    {
        for ( int32_t i = 0; i < w; ++i )
        {
            //uint8_t r = RGBA_R( heightMap.getPixel( i, j ) );
            float r = heightMap.getPixelf( i, j );
            fMin = std::min(fMin,r);
            fMax = std::max(fMax,r);
            float x = cell_x * i;
            float y = cell_y * float( r );
            float z = cell_z * j;
            float u = float( i ) / float( w - 1 );
            float v = float( j ) / float( h - 1 );
            uint32_t color = 0xFFFFFFFF; // RGBA( r,r,r );
            o.addVertex( S3DVertex( x,y,z, 0,1,0, color, u,v ));
        }
    }

    DE_DEBUG("fMin = ",fMin)
    DE_DEBUG("fMax = ",fMax)

    o.recalculateBoundingBox();



    SMeshBufferTool::computeNormals( o );

    //createTerrainIndices();
    o.m_indices.clear();

    o.m_indices.reserve( 8 * size_t( w ) * size_t( h ) );

    // x - lines
    for ( int32_t j = 0; j < h; ++j )
    {
        for ( int32_t i = 0; i < w-1; ++i )
        {
            size_t iA = size_t( w ) * size_t( j ) + i;
            size_t iB = iA + 1;
            o.addIndexedLine( iA, iB );
        }
    }

    // y - lines
    for ( int32_t i = 0; i < w; ++i )
    {
        for ( int32_t j = 0; j < h-1; ++j )
        {
            size_t iA = size_t( w ) * size_t( j ) + i;
            size_t iB = size_t( w ) * size_t( j + 1 ) + i;
            o.addIndexedLine( iA, iB );
        }
    }
}
*/

} // end namespace de.

