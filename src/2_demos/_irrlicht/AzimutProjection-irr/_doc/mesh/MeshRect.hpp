#pragma once
#include <de/gpu/Mesh.hpp>
#include <de/gpu/mesh/MeshPolyLine.hpp>

namespace de {
namespace gpu {

inline void
addLine2D( SMeshBuffer & o, int ax, int ay, int bx, int by, uint32_t colorA, uint32_t colorB )
{
   o.setPrimitiveType( PrimitiveType::Lines );
   o.Vertices.reserve( 2 );
   o.Vertices.push_back( S3DVertex( ax, ay, -1, 0,0,0, colorA, 0,0 ) ); // A
   o.Vertices.push_back( S3DVertex( bx, by, -1, 0,0,0, colorB, 0,0 ) ); // B
}

//
//  B +-----+ C --> +x, +u
//    |   / |
//    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
//  A +-----+ D
//    |           Normal -z shows towards viewer
//   +v = -y      Pos.z is always -1, so its at near plane.
inline void
addRect( SMeshBuffer & o, Recti const & pos,
              uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD,
              TexRef const & ref )
{
   int w = pos.w() > 0 ? pos.w() : ref.getWidth();
   int h = pos.h() > 0 ? pos.h() : ref.getHeight();
   if ( w < 1 || w > 10000 ) { return; }
   if ( h < 1 || h > 10000 ) { return; }

   o.setPrimitiveType( PrimitiveType::Triangles );
   o.setBlend( Blend::alphaBlend() );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }

   int x = pos.x();
   int y = pos.y();
   Rectf const & coords = ref.coords; // Is identity(0,0,1,1) if ref.tex is empty/nullptr.
   auto u1 = coords.u1();
   auto v1 = coords.v1();
   auto u2 = coords.u2();
   auto v2 = coords.v2();
   S3DVertex A( x,   y-1,   1.f, 0,0,0, colorA, u1,v1 );
   S3DVertex B( x,   y-1+h, 1.f, 0,0,0, colorB, u1,v2 );
   S3DVertex C( x+w, y-1+h, 1.f, 0,0,0, colorC, u2,v2 );
   S3DVertex D( x+w, y-1,   1.f, 0,0,0, colorD, u2,v1 );
   o.addTriangle( A, B, C );
   o.addTriangle( A, C, D );
}

//
//  B +-----+ C --> +x, +u
//    |   / |
//    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
//  A +-----+ D
//    |           Normal -z shows towards viewer
//   +v = -y      Pos.z is always -1, so its at near plane.
//
inline void
addRectOutline( SMeshBuffer & o, Recti const & pos,
                uint32_t color, TexRef const & ref, int borderWidth )
{
   int w = pos.w() > 0 ? pos.w() : ref.getWidth();
   int h = pos.h() > 0 ? pos.h() : ref.getHeight();
   if ( w < 1 || w > 10000 ) { return; }
   if ( h < 1 || h > 10000 ) { return; }

   o.setPrimitiveType( PrimitiveType::TriangleStrip );
   o.setLighting( 0 );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }
   o.setBlend( Blend::alphaBlend() );

   int x = pos.x();
   int y = pos.y();
//   Rectf const & coords = ref.coords;
//   auto const & u1 = coords.u1();
//   auto const & v1 = coords.v1();
//   auto const & u2 = coords.u2();
//   auto const & v2 = coords.v2();

   std::vector< glm::vec2 > points;
   points.reserve( 5 );
   points.emplace_back( x, y );
   points.emplace_back( x, y+h );
   points.emplace_back( x+w, y+h );
   points.emplace_back( x+w, y );
   points.emplace_back( x, y );
   SMeshPolyLine::addTriangleStrip( o, points, color, borderWidth, LineCap::None, LineJoin::Bevel );
}

inline void
addRectOutline( SMeshBuffer & o, Recti const & pos,
                uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD,
                TexRef const & ref, int borderWidth )
{
   int w = pos.w() > 0 ? pos.w() : ref.getWidth();
   int h = pos.h() > 0 ? pos.h() : ref.getHeight();
   if ( w < 1 || w > 10000 ) { return; }
   if ( h < 1 || h > 10000 ) { return; }

   o.setPrimitiveType( PrimitiveType::Lines );
   o.setLighting( 0 );
   if ( ref.tex )
   {
      o.setTexture( 0, ref );
   }
   o.setBlend( Blend::alphaBlend() );

   int x = pos.x();
   int y = pos.y();
   Rectf const & coords = ref.coords;
   auto const & u1 = coords.u1();
   auto const & v1 = coords.v1();
   auto const & u2 = coords.u2();
   auto const & v2 = coords.v2();
   //
   //  B +-----+ C --> +x, +u
   //    |   / |
   //    | /   |     FrontQuad ABCD (cw) = ABC (cw) + ACD (cw)
   //  A +-----+ D
   //    |           Normal -z shows towards viewer
   //   +v = -y      Pos.z is always -1, so its at near plane.
   //
//      int x1 = x + 1;
//      int y1 = y - 1;
//      int x2 = x + w;
//      int y2 = y + h;

   S3DVertex A1( x+1, y-1+h,  -1, 0,0,-1, colorA, u1,v2 );
   S3DVertex B1( x+1, y-1,    -1, 0,0,-1, colorB, u1,v1 );
   o.addLine( A1, B1 );

   S3DVertex B2( x,   y-1,    -1, 0,0,-1, colorB, u1,v1 );
   S3DVertex C2( x+w, y-1,    -1, 0,0,-1, colorC, u2,v1 );
   o.addLine( B2, C2 );

   S3DVertex C3( x+w, y-1,    -1, 0,0,-1, colorC, u2,v1 );
   S3DVertex D3( x+w, y-1+h,  -1, 0,0,-1, colorD, u2,v2 );
   o.addLine( C3, D3 );

   S3DVertex D4( x+w, y-2+h,  -1, 0,0,-1, colorD, u2,v2 );
   S3DVertex A4( x,   y-2+h,  -1, 0,0,-1, colorA, u1,v2 );
   o.addLine( D4, A4 );
}

} // end namespace gpu.
} // end namespace de.
