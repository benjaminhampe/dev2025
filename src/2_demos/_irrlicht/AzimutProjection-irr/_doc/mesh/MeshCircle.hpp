#pragma once
#include <de/gpu/mesh/MeshPolyLine.hpp>

namespace de {
namespace gpu {

inline void addCircle( SMeshBuffer & o, Recti const & pos,
      uint32_t outerColor, uint32_t innerColor, TexRef const & ref, uint32_t tess = 12 )
{
   float rx = 0.5f * pos.getWidth();
   float ry = 0.5f * pos.getWidth();

   o.setPrimitiveType( PrimitiveType::Triangles );
   o.setBlend( Blend::alphaBlend() );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }

   // Add center point
   uint32_t v = o.getVertexCount();
   o.addVertex( S3DVertex( 0,0,0, 0,0,-1, innerColor, 0.5f, 0.5f ) );

   float const angleStep = float( ( 2.0 * M_PI ) / double( tess ) );
//      float const outwardX = float( radius.x ) / float( tessOutwards );
//      float const outwardY = float( radius.y ) / float( tessOutwards );

   for ( uint32_t i = 0; i <= tess; ++i )
   {
      float const phi = angleStep * float( i );
      float const s = ::sinf( phi );
      float const c = ::cosf( phi );
//   for ( uint32_t j = 0; j <= tessOutwards; ++j )
//   {
      float x = rx * c; // * float( j ) * outwardX);
      float y = ry * s; // * float( j ) * outwardY);
      float u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
      float v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
      o.addVertex( S3DVertex( x,y,0, 0,0,-1, outerColor, u, v ) );
//         }
   }

   for ( uint32_t i = 0; i < tess; ++i )
   {
//   for ( uint32_t j = 0; j < tessOutwards; ++j )
//   {
      uint32_t M = v;
      uint32_t A = v + 1 + i;
      uint32_t B = v + 2 + i;
      o.addIndexedTriangle( M, A, B );
//   }
   }
}

inline void addCircleOutline( SMeshBuffer & o, Recti const & pos,
      uint32_t color, TexRef const & ref, int borderWidth, uint32_t tess = 12 )
{
   float rx = 0.5f * pos.getWidth();
   float ry = 0.5f * pos.getWidth();

   std::vector< glm::vec2 > points;
   float const angleStep = float( ( 2.0 * M_PI ) / double( tess ) );
   for ( uint32_t i = 0; i <= tess; ++i )
   {
      float const phi = angleStep * float( i );
      float const x = rx * ::sinf( phi );
      float const y = ry * ::cosf( phi );
      points.emplace_back( x, y );
   }
   points.emplace_back( points.front() );

   SMeshPolyLine::addTriangleStrip( o, points, color, borderWidth, LineCap::Square, LineJoin::Bevel );
   o.setBlend( Blend::alphaBlend() );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }

}

// =========================================================================
struct SMeshCircle
// =========================================================================
{
   DE_CREATE_LOGGER("de.gpu.SMeshCircle")

   static void
   addXY(
         SMeshBuffer & o,
         glm::vec2 const & siz,
         uint32_t innerColor = 0xFFFFFFFF,
         uint32_t outerColor = 0xFFFFFFFF,
         uint32_t tessRadial = 33 ) // bool forceIndices = false
   {
      glm::vec2 radius = siz * 0.5f;

      //bool useIndices = o.getIndexCount() > 0 || forceIndices;

      // Add center point
      uint32_t v = o.getVertexCount();
      o.addVertex( S3DVertex( 0,0,0, 0,0,-1, innerColor, 0.5f, 0.5f ) );

      float const angleStep = float( ( 2.0 * M_PI ) / double( tessRadial ) );
//      float const outwardX = float( radius.x ) / float( tessOutwards );
//      float const outwardY = float( radius.y ) / float( tessOutwards );

      for ( uint32_t i = 0; i <= tessRadial; ++i )
      {
         float const phi = angleStep * float( i );
         float const s = ::sinf( phi );
         float const c = ::cosf( phi );

//         for ( uint32_t j = 0; j <= tessOutwards; ++j )
//         {
         float x = (radius.x * c); // * float( j ) * outwardX);
         float y = (radius.y * s); // * float( j ) * outwardY);
         float u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
         float v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
         o.addVertex( S3DVertex( x,y,0, 0,0,-1, outerColor, u, v ) );
//         }
      }

      for ( uint32_t i = 0; i < tessRadial; ++i )
      {
//         for ( uint32_t j = 0; j < tessOutwards; ++j )
//         {
            uint32_t M = v;
            uint32_t A = v + 1 + i;
            uint32_t B = v + 1 + i + 1;
            o.addIndexedTriangle( M, A, B );
//         }
      }
   }

   static void
   addXY(   SMeshBuffer & o,
            glm::vec2 const & siz,
            uint32_t color,
            uint32_t tessRadial = 33 ) // bool forceIndices = false
   {
      addXY( o, siz, color, color, tessRadial );
   }

   static void
   addXZ(   SMeshBuffer & o,
            glm::vec2 const & siz,
            uint32_t innerColor = 0xFFFFFFFF,
            uint32_t outerColor = 0xFFFFFFFF,
            uint32_t tessRadial = 33 ) // bool forceIndices = false
   {
      glm::vec2 radius = siz * 0.5f;

      //bool useIndices = o.getIndexCount() > 0 || forceIndices;

      // Add center point
      uint32_t v = o.getVertexCount();
      o.addVertex( S3DVertex( 0,0,0, 0,1,0, innerColor, 0.5f, 0.5f ) );

      float const angleStep = float( ( 2.0 * M_PI ) / double( tessRadial ) );
//      float const outwardX = float( radius.x ) / float( tessOutwards );
//      float const outwardY = float( radius.y ) / float( tessOutwards );

      for ( uint32_t i = 0; i <= tessRadial; ++i )
      {
         float const phi = angleStep * float( i );
         float const s = ::sinf( phi );
         float const c = ::cosf( phi );

//         for ( uint32_t j = 0; j <= tessOutwards; ++j )
//         {
         float x = (radius.x * c); // * float( j ) * outwardX);
         float z = (radius.y * s); // * float( j ) * outwardY);
         float u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
         float v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
         o.addVertex( S3DVertex( x,0,z, 0,1,0, outerColor, u, v ) );
//         }
      }

      for ( uint32_t i = 0; i < tessRadial; ++i )
      {
//         for ( uint32_t j = 0; j < tessOutwards; ++j )
//         {
            uint32_t M = v;
            uint32_t A = v + 1 + i;
            uint32_t B = v + 1 + i + 1;
            o.addIndexedTriangle( M, A, B );
//         }
      }
   }

   static void
   addXZ(
         SMeshBuffer & o,
         glm::vec2 const & siz,
         uint32_t color,
         uint32_t tessRadial = 33 ) // bool forceIndices = false
   {
      addXZ( o, siz, color, color, tessRadial );
   }




};















} // end namespace gpu.
} // end namespace de.

