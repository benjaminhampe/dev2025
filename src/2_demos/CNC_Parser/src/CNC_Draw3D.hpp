#pragma once
#include <CNC_Register.hpp>
#include <CNC_Block.hpp>

namespace de {
namespace cnc {


#pragma pack( push )
#pragma pack( 1 )

// remarkably a line has same data as boundingbox
struct BoundingBoxf
{
   glm::vec3 Min;
   glm::vec3 Max;

   BoundingBoxf()
      : Min(), Max()
   {}

   BoundingBoxf( glm::vec3 min, glm::vec3 max )
      : Min(min), Max(max)
   {}
};

struct CNC_ColorVertex
{
   DE_CREATE_LOGGER("de.cnc.ColorVertex")

   glm::vec3 pos;
   u32 color;

   CNC_ColorVertex()
      : pos(), color(0x02FFFFFF)
   {}

   CNC_ColorVertex( glm::vec3 const & pos_, u32 color_ = 0x02FFFFFF)
      : pos(pos_), color(color_)
   {}

   // compute BoundingBox of std::vector< CNC_Draw3DLine >
   // remarkably a line has same data as boundingbox
   static BoundingBoxf
   computeBoundingBox( std::vector< CNC_ColorVertex > const & vertices )
   {
      PerformanceTimer perf;
      perf.start();

      constexpr auto fMin = std::numeric_limits< float >::lowest();
      constexpr auto fMax = std::numeric_limits< float >::max();

      BoundingBoxf bb;
      bb.Min = glm::vec3(fMax,fMax,fMax);
      bb.Max = glm::vec3(fMin,fMin,fMin);

      for ( size_t i = 0; i < vertices.size(); ++i )
      {
         auto const & vertex = vertices[ i ];

         bb.Min = glm::min( bb.Min, vertex.pos );
         bb.Max = glm::max( bb.Max, vertex.pos );
      }

      perf.stop();
      DE_DEBUG("needed(",perf.ms(), "ms), "
               "bbSize(",bb.Max - bb.Min,"), "
               "bbMin(",bb.Min,"), "
               "bbMax(",bb.Max,")")

      return bb;
   }
};

#pragma pack( pop )

struct CNC_Draw3D
{
   DE_CREATE_LOGGER("de.cnc.Draw3D")
   static glm::ivec4
   transformLine( glm::vec3 A, glm::vec3 B, glm::mat4 const & mvp, int w, int h );

   static glm::ivec4
   clipLine( glm::ivec4 const & line, int w, int h );

   static glm::ivec4
   clipLine( glm::ivec4 const & line, glm::ivec4 const & clipRect );

//   static glm::ivec4
//   clipLine( glm::ivec4 const & line, glm::ivec4 const & clipRect );

   static void
   drawLineBox( Image & img, glm::vec3 minCorner, glm::vec3 maxCorner,
                u32 color, glm::mat4 const & mvp, bool clip );

   static void
   drawLine( Image & img, glm::vec3 A, glm::vec3 B,
             u32 color, glm::mat4 const & mvp, bool clip );

   static void
   drawLineStrip( Image & img, std::vector< CNC_ColorVertex > const & points,
                  glm::mat4 const & mvp, bool clip );

   static Image
   createWallpaperImage( int w, int h, int n = 1000 );

   static Image
   createPreviewImage( int w, int h, std::vector< CNC_ColorVertex > const & vertices );

   static Image
   testClipping( int w, int h );

};



} // end namespace cnc
} // end namespace de


/*
void addListener( IParserListener* listener )
{
   if ( !listener ) return;
   m_listeners.emplace_back( listener );
}

void emit_begin( std::string const & uri )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_begin( uri ); }
}

void emit_end( std::string const & uri )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_end( uri ); }
}

void emit_newLine( std::string const & line )
{
   for ( auto listener : m_listeners )
   {
      if ( listener ) listener->on_newLine( line );
   }
}

void emit_commandList( std::vector< std::string > const & commands, std::string const & comment )
{
   for ( auto listener : m_listeners )
   {
      if ( listener ) listener->on_commandList( commands, comment );
   }
}

void emitG00( f32 x, f32 y, f32 z )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_G00( x,y,z ); }
}

void emitG01( f32 x, f32 y, f32 z )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_G01( x,y,z ); }
}

void emitG02( f32 x, f32 y, f32 z )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_G02( x,y,z ); }
}

void emitG03( f32 x, f32 y, f32 z )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_G03( x,y,z ); }
}

void emitG04( f32 x, f32 y, f32 z )
{
   for ( auto listener : m_listeners ) { if ( listener ) listener->on_G04( x,y,z ); }
}
*/
