#pragma once
#include <de_core/de_Logger.h>
#include <cstdint>
#include <vector>

#include <de_image/de_ColorRGBA.h>
#include <de_gpu/de_VideoDriver.h>
#include <de_gpu/de_Geometry2d.h>
#include <de_gpu/de_FVF.h>

namespace de {

#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct ScreenRendererVertex
// ===========================================================================
{
   glm::vec2 m_pos;
   glm::vec2 m_tex;
   glm::vec4 m_color;

   ScreenRendererVertex()
      : m_pos(0,0)
      , m_tex(0,0)
      , m_color(1,1,1,1)
   {}
   ScreenRendererVertex( float x, float y, float u, float v, glm::vec4 color = glm::vec4(1,1,1,1) )
      : m_pos(x,y)
      , m_tex(u,v)
      , m_color(color)
   {}
   ScreenRendererVertex( glm::vec2 pos, glm::vec2 tex, glm::vec4 color = glm::vec4(1,1,1,1) )
      : m_pos(pos)
      , m_tex(tex)
      , m_color(color)
   {}

   std::string
   toString() const
   {
      std::ostringstream s;
      s << "pos(" << m_pos << "), tex(" << m_tex << "), color(" << m_color << ")";
      return s.str();
   }
};

#pragma pack( pop )

// ===========================================================================
struct ScreenRendererMeshBuffer
// ===========================================================================
{
   DE_CREATE_LOGGER("ScreenRendererMeshBuffer")
   PrimitiveType m_primType;
   bool m_shouldUpload;
   bool m_shouldBlend;
   uint8_t m_dummy;
   uint32_t m_vao;
   uint32_t m_vbo;
   uint32_t m_ibo;
   std::vector< uint32_t > m_indices;
   std::vector< ScreenRendererVertex > m_vertices;
   // Material
   uint32_t m_texDiffuse;
   glm::vec4 m_texTransform;
   uint32_t m_color;
   //float m_zIndex;

   ScreenRendererMeshBuffer();
   ~ScreenRendererMeshBuffer();
   void destroy();
   void render();

   std::string toString() const
   {
      std::stringstream s;
      s << "VertexCount = " << m_vertices.size() << "\n";
      for ( size_t i = 0; i < m_vertices.size(); ++i )
      {
         s << "Vertex[" << i << "] " << m_vertices[ i ].toString() << "\n";
      }
      s << "IndexCount = " << m_indices.size() << "\n";
      for ( size_t i = 0; i < m_indices.size(); ++i )
      {
         s << m_indices[ i ] << " ";
      }
      s << "\n";
      return s.str();
   }

   //==========================================================
   /// @brief Create custom geometry
   //==========================================================
   uint32_t getVertexCount() const { return static_cast< uint32_t >(m_vertices.size()); }
   uint32_t getIndexCount() const { return static_cast< uint32_t >(m_indices.size()); }

   //==========================================================
   /// @brief Index
   //==========================================================
   //
   //       /
   //    --i--
   //     /
   //
   void addIndex( uint32_t i )
   {
      m_indices.emplace_back( i );
      m_shouldUpload = true;
   }


   //==========================================================
   /// @brief Vertex
   //==========================================================
   //
   //       /
   //    --v--  POSITION_XY + DIFFUSE_TEX_UV + DIFFUSE_COLOR_RGBA4F
   //     /
   //
   void addVertex( glm::vec2 pos, glm::vec2 uv, uint32_t color = 0xFFFFFFFF )
   {
//      auto primStr = PrimitiveType::fromGL(m_primType).str();
//      std::cout << primStr << ".addVertex(" << pos << ", " << uv <<
//                   ", " << dbStrHex(color) << ")" << std::endl;

      if ( de::RGBA_A(color) < 0xFF )
      {
//         std::cout << "m_shouldBlend color " << dbStrHex(color) << std::endl;
         m_shouldBlend = true;
      }

      m_vertices.emplace_back( pos, uv, de::RGBAf(color) ); // Convert 32 bit color -> 128bit color.
      m_shouldUpload = true;
   }

   //==========================================================
   /// @brief Line
   //==========================================================
   //
   //        B
   //       /
   //      /
   //     /
   //    A
   //
   void
   addLine( glm::vec2 A, glm::vec2 B, uint32_t colorA, uint32_t colorB )
   {
      m_primType = PrimitiveType::Lines;
      //m_zIndex = zIndex;

      //size_t v0 = m_vertices.size();
      addVertex( A, glm::vec2(0,0), colorA );
      addVertex( B, glm::vec2(1,0), colorB );
      //   addIndex( v0 );
      //   addIndex( v0 + 1 );
   }

   inline void
   addLine( glm::vec2 A, glm::vec2 B, uint32_t color )
   {
      addLine( A,B,color,color );
   }

   //==========================================================
   /// @brief Triangle
   //==========================================================
   //
   //    B---C  cw - clock-wise, math negative
   //    |  /
   //    | /    ABC -> BCA ( B in center )
   //    |/
   //    A
   //
   void
   addTriangle( glm::vec2 A, glm::vec2 B, glm::vec2 C,
                uint32_t colorA,
                uint32_t colorB,
                uint32_t colorC,
                glm::vec2 uvA = glm::vec2(0.0f, 1.0f),
                glm::vec2 uvB = glm::vec2(0.0f, 0.0f),
                glm::vec2 uvC = glm::vec2(1.0f, 0.0f) )
   {
      m_primType = PrimitiveType::Triangles;
      //m_zIndex = zIndex;
      //m_color = color;
         //size_t v0 = m_vertices.size();
      addVertex( A, uvA, colorA );
      addVertex( B, uvB, colorB );
      addVertex( C, uvC, colorC );
      //   addIndex( v0 );
      //   addIndex( v0 + 1 );
      //   addIndex( v0 + 2 );
   }

   //==========================================================
   /// @brief Triangle
   //==========================================================
   //
   //    B---C  cw - clock-wise, math negative
   //    |  /
   //    | /    ABC -> BCA ( B in center )
   //    |/
   //    A
   //
   void
   addTriangle( glm::vec2 A, glm::vec2 B, glm::vec2 C,
         glm::vec2 uvA = glm::vec2(0.0f, 1.0f), glm::vec2 uvB = glm::vec2(0.0f, 0.0f),
         glm::vec2 uvC = glm::vec2(1.0f, 0.0f), uint32_t color = 0xFFFFFFFF )
   {
      m_primType = PrimitiveType::Triangles;
      //m_zIndex = zIndex;
      //m_color = color;
         //size_t v0 = m_vertices.size();
      addVertex( A, uvA, color );
      addVertex( B, uvB, color );
      addVertex( C, uvC, color );
      //   addIndex( v0 );
      //   addIndex( v0 + 1 );
      //   addIndex( v0 + 2 );
   }

   //==========================================================
   /// @brief Quad
   //==========================================================
   //
   //    B---C
   //    |  /|
   //    | / |    ABC -> BCA ( B in center )
   //    |/  |    ACD -> DAC ( D in center )
   //    A---D
   //
   void addRect(
         glm::vec2 pos, glm::vec2 size, uint32_t color = 0xFFFFFFFF,
         uint32_t texDiffuse = 0, bool flipV = false )
   {
      //m_zIndex = zIndex;
      //m_color = color;
      m_texDiffuse = texDiffuse;

      glm::vec2 A = glm::vec2( pos.x, pos.y + size.y);
      glm::vec2 B = glm::vec2( pos.x, pos.y);
      glm::vec2 C = glm::vec2( pos.x + size.x, pos.y);
      glm::vec2 D = glm::vec2( pos.x + size.x, pos.y + size.y);

      glm::vec2 uvA = glm::vec2(0.0f, 1.0f);
      glm::vec2 uvB = glm::vec2(0.0f, 0.0f);
      glm::vec2 uvC = glm::vec2(1.0f, 0.0f);
      glm::vec2 uvD = glm::vec2(1.0f, 1.0f);
      if ( flipV )
      {
         uvA = glm::vec2(0.0f, 0.0f);
         uvB = glm::vec2(0.0f, 1.0f);
         uvC = glm::vec2(1.0f, 1.0f);
         uvD = glm::vec2(1.0f, 0.0f);
      }

      addTriangle( A,B,C, uvA, uvB, uvC, color );
      addTriangle( C,D,A, uvC, uvD, uvA, color );
   }

   inline void
   addRectBorder( glm::vec2 pos, int border, glm::vec2 size, uint32_t color )
   {
      float const w = size.x;
      float const h = size.y;

      addRect( pos + glm::vec2(0,0), glm::vec2(w,border), color );
      addRect( pos + glm::vec2(0,border), glm::vec2(border,h-1-border), color );
      addRect( pos + glm::vec2(w-1-border,border), glm::vec2(border,h-1-border), color );
      addRect( pos + glm::vec2(0,h-1-border), glm::vec2(w,border), color );
   }

   //==========================================================
   /// @brief LineRect
   //==========================================================
   //
   //    B---C
   //    |  /|
   //    | / |    ABC -> BCA ( B in center )
   //    |/  |    ACD -> DAC ( D in center )
   //    A---D
   //

   inline void
   addLineRect( glm::vec2 pos, glm::vec2 size, uint32_t color )
   {
      float const w = size.x;
      float const h = size.y;
      addLine( pos + glm::vec2(0,0), pos + glm::vec2(w,0), color );
      addLine( pos + glm::vec2(0,1), pos + glm::vec2(0,h-1), color );
      addLine( pos + glm::vec2(w-1,1), pos + glm::vec2(w-1,h-1), color );
      addLine( pos + glm::vec2(0,h-1), pos + glm::vec2(w,h-1), color );
   }

   //===============================w=====================
   /// @brief RoundRect
   //==========================================================
   /**
      y0--  /G---H\      innerColor for ABCD
           / | / | \     outerColor for all other
      y1- F--B---C--I
          | /|  /| /|    radius and tess affect circular corners.
          |/ | / |/ |
          |  |/  |  |
      y2- E--A---D--J
           \ | / | /
      y3--  \L---K/

          |  |   |  |
          x0 x1  x2 x3
   */
   void addRoundRect(
         glm::vec2 pos, glm::vec2 size,
         glm::vec2 radius, int32_t tess = 12,
         uint32_t color = 0xFFFFFFFF, uint32_t texDiffuse = 0, bool flipV = false )
   {
      m_primType = PrimitiveType::Triangles;
      m_texDiffuse = texDiffuse;

      auto rr_getVertexCount = [&] () { return m_vertices.size(); };
      auto rr_addIndex = [&] ( uint32_t index ) { addIndex( index ); };
      auto rr_addVertex = [&] ( float px, float py, uint32_t color )
      {
         float x = pos.x + size.x*0.5f + px;
         float y = pos.y + size.y*0.5f + py;
         float u = 0.5f + px / size.x;
         float v = 0.5f + py / size.y;
         addVertex( glm::vec2( x,y ), glm::vec2(u,v), color );
      };

      RoundRect::addTriangles(
               size.x, size.y,
               radius.x, radius.y, uint32_t(tess),
               color, color,
               rr_getVertexCount, rr_addVertex, rr_addIndex );
   }
   //==========================================================
   /// @brief LineRoundRect
   //==========================================================
   //       x0  x1  x2  x3
   // ry    |   |   |   |
   //       |
   // y0 ---+   G---H
   //        (         )     A,B,C,D are not used for the outline
   //    ry (           )
   //       (           )    E=0, F=1, G=2, H=3, I=4, J=5, K=6, L=7, n = 8
   // y1 -  F   B   C   I
   //       |           |     Lines: EF, GH, IJ, KL
   //       |     M     |            + lineFG[tess] + lineHI[tess] + lineJK[tess] + lineLE[tess]
   //       |           |
   // y2 -  E   A   D   J        (CW) top-left = FGB
   //       (           )        (CW) top-right = HIC
   //        (         )         (CW) bottom-right = JKD
   //         (       )          (CW) bottom-left = LEA
   // y3 ---+   L---K
   void addLineRoundRect( glm::vec2 pos, glm::vec2 size, glm::vec2 radius, int32_t tess = 12,
         uint32_t color = 0xFFFFFFFF, uint32_t texDiffuse = 0, bool flipV = false )
   {
      m_primType = PrimitiveType::Lines;
      m_texDiffuse = texDiffuse;

      auto lrr_getVertexCount = [&] () { return m_vertices.size(); };
      auto lrr_addIndex = [&] ( uint32_t index ) { addIndex( index ); };
      auto lrr_addVertex = [&] ( float px, float py, uint32_t color )
      {
         float x = pos.x + size.x*0.5f + px;
         float y = pos.y + size.y*0.5f + py;
         addVertex( glm::vec2( x,y ), glm::vec2(0,0), color );
      };

      LineRoundRect::addLines(
               size.x, size.y, radius.x, radius.y, uint32_t(tess), color,
               lrr_getVertexCount, lrr_addVertex, lrr_addIndex );
   }
   //==========================================================
   /// @brief SolidCircle
   //==========================================================
   void addCircle(
         glm::vec2 pos, glm::vec2 size, int32_t tess = 12,
         uint32_t color = 0xFFFFFFFF, uint32_t texDiffuse = 0, bool flipV = false )
   {
      m_primType = PrimitiveType::Triangles;
      m_texDiffuse = texDiffuse;

      auto sc_getVertexCount = [&] () { return m_vertices.size(); };
      auto sc_addIndex = [&] ( uint32_t index ) { addIndex( index ); };
      auto sc_addVertex = [&] ( float px, float py, uint32_t color, float u, float v )
      {
         float x = pos.x + size.x*0.5f + px;
         float y = pos.y + size.y*0.5f + py;
         addVertex( glm::vec2( x,y ), glm::vec2(u,v), color );
      };

      SolidCircle::addIndexedTriangles(
               size.x, size.y, uint32_t(tess),
               color, color,
               sc_getVertexCount, sc_addVertex, sc_addIndex );
   }
   //==========================================================
   /// @brief LineCircle
   //==========================================================
   void addLineCircle(
         glm::vec2 pos, glm::vec2 size, int32_t tess = 12,
         uint32_t color = 0xFFFFFFFF, uint32_t texDiffuse = 0, bool flipV = false )
   {
      m_primType = PrimitiveType::Lines;
      m_texDiffuse = texDiffuse;

      auto sc_getVertexCount = [&] () { return m_vertices.size(); };
      auto sc_addIndex = [&] ( uint32_t index ) { addIndex( index ); };
      auto sc_addVertex = [&] ( float px, float py, uint32_t color, float u, float v )
      {
         float x = pos.x + size.x*0.5f + px;
         float y = pos.y + size.y*0.5f + py;
         addVertex( glm::vec2( x,y ), glm::vec2(u,v), color );
      };

      SolidCircle::addIndexedTriangles(
               size.x, size.y, uint32_t(tess),
               color, color,
               sc_getVertexCount, sc_addVertex, sc_addIndex );
   }
};

// ===========================================================================
struct ScreenRendererMesh
// ===========================================================================
{
   std::vector< ScreenRendererMeshBuffer > m_buffers;

   ScreenRendererMesh()
   {}

   ~ScreenRendererMesh()
   {
      if ( m_buffers.size() > 0 )
      {
         std::cout << "Forgot to call ScreenRendererMesh.destroy()" << std::endl;
      }
   }

   void
   destroy()
   {
      for ( auto & buffer : m_buffers )
      {
         buffer.destroy();
      }
      m_buffers.clear();
   }

   std::vector< ScreenRendererMeshBuffer > const &
   getMeshBuffers() const { return m_buffers; }

   std::vector< ScreenRendererMeshBuffer > &
   getMeshBuffers() { return m_buffers; }

   void
   addMeshBuffer( ScreenRendererMeshBuffer && buffer )
   {
      m_buffers.emplace_back( std::move( buffer ) ); // Convert 32 bit color -> 128bit color.
   }

   std::string
   toString() const
   {
      std::stringstream s;
      s << "BufferCount = " << m_buffers.size() << "\n";
      for ( size_t i = 0; i < m_buffers.size(); ++i )
      {
         s << "Buffer[" << i << "] " << m_buffers[ i ].toString() << "\n";
      }
      return s.str();
   }

};

// ===========================================================================
struct ScreenRenderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.opengl.ScreenRenderer")
   VideoDriver* m_driver;
   float m_screenW;
   float m_screenH;
   Shader* m_shader[2];  // colored or textured

   //std::vector< ScreenRendererMeshBuffer > m_mesh;

//   ScreenRendererMeshBuffer m_unitLineRect;
//   ScreenRendererMeshBuffer m_unitSolidRect;

//   ScreenRendererMeshBuffer m_unitLineRoundRect2;
//   ScreenRendererMeshBuffer m_unitSolidRoundRect2;

   ScreenRenderer();
   ~ScreenRenderer();

   void init( int w, int h, VideoDriver* driver );
   void destroy();

   // VIP function: main setup function
   void setScreenSize( float w, float h ) { m_screenW = w; m_screenH = h; }

   // VIP getter: Get current screen width the renderer uses.
   float getScreenWidth() const { return m_screenW; }
   // VIP getter: Get current screen width the renderer uses.
   float getScreenHeight() const { return m_screenH; }

   void
   draw( ScreenRendererMeshBuffer & meshBuffer,
         glm::vec4 color = glm::vec4(1,1,1,1),
         float zIndex = 0.0f,
         glm::vec4 pos_transform = glm::vec4(0,0,1,1) );

   inline void
   draw( ScreenRendererMesh & mesh,
         glm::vec4 color = glm::vec4(1,1,1,1),
         float zIndex = 0.0f,
         glm::vec4 pos_transform = glm::vec4(0,0,1,1) )
   {
      for ( auto & buffer : mesh.getMeshBuffers() )
      {
         draw( buffer, color, zIndex, pos_transform );
      }
   }



   // QuickAndExpensive [SolidRect]:

   void
   drawRect( glm::vec2 pos, glm::vec2 size,
      uint32_t color = 0xFFFFFFFF, uint32_t texDiffuse = 0, bool flipV = false)
   {
      //m_unitSolidRect.m_texDiffuse = texDiffuse;
      //draw( m_unitSolidRect, RGBAf(color), 0.0f, glm::vec4(pos,size) ); // RGBAf(color)
      ScreenRendererMeshBuffer meshBuffer;
      meshBuffer.m_texDiffuse = texDiffuse;
      meshBuffer.addRect( pos, size, color, texDiffuse, flipV );
      draw( meshBuffer );
      meshBuffer.destroy();
      // std::cout << "ScreenRenderer.drawRect() :: " << meshBuffer.toString() << "\n";
   }

   // QuickAndExpensive [SolidRoundRect]:
   void
   drawRoundRect( glm::vec2 pos, glm::vec2 size, glm::vec2 radius, int32_t tess = 13,
      uint32_t color = 0xFFFFFFFF, uint32_t texDiffuse = 0, bool flipV = false)
   {
      //m_unitSolidRoundRect2.m_texDiffuse = texDiffuse;
      //draw( m_unitSolidRoundRect2, RGBAf(color), 0.0f, glm::vec4(pos,size) ); // glm::vec4(1,1,1,1)
      ScreenRendererMeshBuffer meshBuffer;
      meshBuffer.m_texDiffuse = texDiffuse;
      meshBuffer.addRoundRect( pos, size, radius, tess, color, texDiffuse, flipV );
      draw( meshBuffer );
      meshBuffer.destroy();
      // std::cout << "ScreenRenderer::drawRoundRect() :: " << meshBuffer.toString() << "\n";
   }

   // QuickAndExpensive [SolidCircle]:
   void
   drawCircle( glm::vec2 pos, glm::vec2 size, int32_t tess = 43,
      uint32_t color = 0xFFFFFFFF, uint32_t texDiffuse = 0, bool flipV = false)
   {
      ScreenRendererMeshBuffer meshBuffer;
      meshBuffer.addCircle( pos, size, tess, color, texDiffuse, flipV );
      draw( meshBuffer );
      meshBuffer.destroy();
      //std::cout << "ScreenRenderer.drawCircle() :: " << meshBuffer.toString() << "\n";
   }

   // QuickAndExpensive [Line]:
   void
   drawLine( glm::vec2 A, glm::vec2 B, uint32_t colorA, uint32_t colorB )
   {
      ScreenRendererMeshBuffer meshBuffer;
      meshBuffer.addLine( A, B, colorA, colorB );
      draw( meshBuffer );
      meshBuffer.destroy();
      //std::cout << "ScreenRenderer.drawLine() :: " << meshBuffer.toString() << "\n";
   }

   // QuickAndExpensive [Line]:
   inline void
   drawLine( glm::vec2 A, glm::vec2 B, uint32_t color = 0xFFFFFFFF )
   {
      drawLine( A, B, color, color );
   }

   // QuickAndExpensive [LineRect]:
   void
   drawLineRect( glm::vec2 pos, glm::vec2 size, uint32_t color = 0xFFFFFFFF )
   {
      //m_unitLineRect.m_texDiffuse = texDiffuse;
      //draw( m_unitLineRect, RGBAf(color), 0.0f, glm::vec4(pos,size) );
      ScreenRendererMeshBuffer meshBuffer;
      meshBuffer.addLineRect( pos, size, color );
      draw( meshBuffer );
      meshBuffer.destroy();
      //std::cout << "ScreenRenderer.drawLineRect() :: " << meshBuffer.toString() << "\n";
   }

   // QuickAndExpensive [LineRoundRect]:
   void
   drawLineRoundRect( glm::vec2 pos, glm::vec2 size, glm::vec2 radius, int32_t tess = 13,
                  uint32_t color = 0xFFFFFFFF, uint32_t texDiffuse = 0, bool flipV = false)
   {
      //draw( m_unitLineRoundRect2, RGBAf(color), 0.0f, glm::vec4(pos,size) );
      ScreenRendererMeshBuffer meshBuffer;
      meshBuffer.addLineRoundRect( pos, size, radius, tess, color, texDiffuse, flipV );
      draw( meshBuffer );
      meshBuffer.destroy();
      //std::cout << "ScreenRenderer.drawLineRoundRect() :: " << meshBuffer.toString() << "\n";
   }

   // QuickAndExpensive [LineCircle]:
   void
   drawLineCircle( glm::vec2 pos, glm::vec2 size, int32_t tess = 43,
               uint32_t color = 0xFFFFFFFF, uint32_t texDiffuse = 0, bool flipV = false)
   {
      ScreenRendererMeshBuffer meshBuffer;
      meshBuffer.addLineCircle( pos, size, tess, color, texDiffuse, flipV );
      draw( meshBuffer );
      meshBuffer.destroy();
      //std::cout << "ScreenRenderer.drawLineCircle() :: " << meshBuffer.toString() << "\n";
   }
};

} // end namespace GL.

