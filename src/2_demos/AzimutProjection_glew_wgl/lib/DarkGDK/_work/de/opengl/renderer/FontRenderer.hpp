#pragma once
#include <de/gpu/Material.hpp>

namespace de {
namespace gpu {

struct VideoDriver;
struct Shader;

// ===========================================================================
struct FontRenderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.FontRenderer")
   VideoDriver* m_Driver;
   Shader* m_Shader;

   FontRenderer( VideoDriver* driver = nullptr );
   ~FontRenderer();

   TextSize
   getTextSize( std::wstring const & msg, Font const & font = Font() );

   void
   draw2DText( int x, int y, std::wstring const & msg, uint32_t color = 0xFF000000,
               Align align = Align::Default, Font const & font = Font() );

   void
   draw2DTextDebug( int x, int y, std::wstring const & msg,
               uint32_t color = 0xFF000000,
               Align align = Align::Default, Font const & font = Font() );

   void
   draw2DText( Text const & text )
   {
      draw2DText( text.x, text.y, text.msg, text.brush.color, text.align, text.font );
   }

   void clear();
};

#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct FontVertex
// ===========================================================================
{
   glm::vec3 pos;
   glm::vec2 tex;    // 12 + 12 + 4 + 8 = 36 Bytes if packed
   //uint32_t color;

   static FVF const & getFVF() {
      static FVF const vertexFormat( sizeof( FontVertex ),
         {  VertexAttrib( VertexAttribType::F32, 3, VertexAttribSemantic::Pos ),
            VertexAttrib( VertexAttribType::F32, 2, VertexAttribSemantic::Tex )
         // VertexAttrib( VertexAttribType::U8, 4, VertexAttribSemantic::Color )
         }
      );
      return vertexFormat;
   }

   FontVertex() : pos(), tex() //, color( 0xFFFFFFFF )
   {}

   FontVertex( float x, float y, float z, float u, float v ) : pos( x,y,z ), tex( u, v ) //, color( color )
   {}

   glm::vec3 const & getPos() const       { return pos; }
   glm::vec3 &       getPos()             { return pos; }
   glm::vec2 const & getTexCoord() const  { return tex; }
   //uint32_t const & getColor() const    { return color; }

   void setPos( glm::vec3 const & position ) { pos = position; }
   void setTexCoord( glm::vec2 const & texCoord ) { tex = texCoord; }
   //void setColor( uint32_t const & color ) { color = color; }

};

#pragma pack( pop )

} // end namespace gpu.
} // end namespace de.
