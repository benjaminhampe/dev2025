#pragma once
#include <de_image/de_Font5x8.h>
#include <de_gpu/de_VideoDriver.h>

namespace de {

#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct Font5x8Renderer2D_Vertex
// ===========================================================================
{
   float m_x;
   float m_y;
   float m_z;
   uint32_t m_color;

   Font5x8Renderer2D_Vertex()
      : m_x( .0f ), m_y( .0f ), m_z( .0f ), m_color(0xFFFFFFFF)
   {}
   Font5x8Renderer2D_Vertex( float x, float y, float z, uint32_t color = 0xFFFFFFFF, float u = 0.0f, float v = 0.0f )
      : m_x( x ), m_y( y ), m_z( z ), m_color( color )
   {}
};

#pragma pack( pop )

// ===========================================================================
struct Font5x8Renderer2D
// ===========================================================================
{
   VideoDriver* m_driver;
   Shader* m_shader;
   uint32_t m_vao;
   uint32_t m_vbo;
   bool m_shouldUpload;
   float m_screenWidth;
   float m_screenHeight;
   int32_t m_screenWidthLoc;
   int32_t m_screenHeightLoc;

   std::vector< Font5x8Renderer2D_Vertex > m_vertices;

   Font5x8Renderer2D();
   //~Font5x8Renderer();

   void
   init( int w, int h, VideoDriver* driver );

   void
   destroy();

   void
   setScreenSize( float w, float h );

   void
   draw2DText( int x, int y, std::string const & msg,
               uint32_t color = 0xFFFFFFFF,
               de::Align align = de::Align::Default,
               de::Font5x8 const & font = de::Font5x8() );
   void
   add2DText( int x, int y, std::string const & msg,
              uint32_t color, de::Align align, de::Font5x8 const & font );

   static de::Font5x8::TextSize
   getTextSize( std::string const & msg, de::Font5x8 const & font = de::Font5x8() )
   {
      return de::Font5x8::getTextSize( msg, font );
   }

protected:
   static void
   addGeometry2DText( std::vector< Font5x8Renderer2D_Vertex > & vertices,
                      int x, int y, std::string const & msg,
                      uint32_t color, de::Align align, de::Font5x8 const & font );
};

} // end namespace GL.
