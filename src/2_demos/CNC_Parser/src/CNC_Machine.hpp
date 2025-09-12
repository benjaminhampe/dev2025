#pragma once
#include <CNC_Register.hpp>
#include <CNC_Block.hpp>
#include <CNC_Draw3D.hpp>

namespace de {
namespace cnc {

// Computer that understands CNC_Blocks
struct IParserListener
{
   virtual ~IParserListener() = default;
   virtual void parserStarted( std::string uri, std::string sourceText ) = 0;
   virtual void parserEnded() = 0;
   virtual void parseBlock( CNC_Block block ) = 0;
};

// Computer that understands CNC_Blocks
struct CNC_Machine : public IParserListener
{
   DE_CREATE_LOGGER("de.cnc.CNC_Machine")

   CNC_Register m_X;
   CNC_Register m_Y;
   CNC_Register m_Z;

   CNC_Register m_A;
   CNC_Register m_B;
   CNC_Register m_C;

   CNC_Register m_E;
   CNC_Register m_F;
   CNC_Register m_S;

   glm::dvec3 m_currPos;
   glm::dvec3 m_lastPos;

   // 0 = absolute (G90)
   // 1 = relative (G91)
   int m_relativeMode;
   // 0 = metric (G20),
   // 1 = imperial (G21)
   int m_imperialMode;
   // 0 = fast travel (G00) non cutting
   // 1 = linear (G01)  cutting
   // 2 = circle cw (G02)
   // 3 = circle ccw (G03)
   // 4 = dwell wait in seconds (G04F2.3) is a pause for 2.3 seconds
   // 5 = wait infinite for user intervention
   // 20 = inch units
   // 21 = metric units
   // 80 = canned cycle cancel
   // 81 = Drilling cycle
   // 82 = Drilling cycle with dwell
   int m_moveMode;
   int m_dummy1;
   double m_feedSpeed;

   // What the parser gave as input
   std::string m_uri;
   std::string m_sourceText;
   std::vector< CNC_Block > m_blocks;
   double m_travelDistance;
   double m_parseTimeTotal;
   double m_parseTimeStart;

   std::vector< CNC_ColorVertex > m_colorLineStrip;

   //   u64 m_nParsedLines = 0;
   //   u64 m_nProcessedLines = 0;
   //   u64 m_nCommentLines = 0;
   //   u64 m_nBrokenLines = 0;

   CNC_Machine();

   // We (state-machine) get this signal from the parser first. ( we are listener )
   void parserStarted( std::string uri, std::string sourceText ) override;

   // We (state-machine) get this signal from the parser at last. ( we are listener )
   void parserEnded() override;

   // We (state-machine) get this signal many times in between from parser. ( we are consumer )
   void parseBlock( CNC_Block givenBlock ) override;

   // Important util: Moves all unparsed items to skip list -> finalize block. ( unsupported block )
   static void skipBlock( CNC_Block & block );

   // Important util: Moves current item from unparsed to parsed list. ( supported word )
   static void markWord( CNC_Block & block, s32 i, u32 color );

   // Important util: Moves current item from unparsed to skip list. ( unsupported word )
   static void skipWord( CNC_Block & block, s32 i );

   // Important software preview image util: Does a simple gpu pipeline in software.
   static glm::ivec4
   transformLine( glm::vec3 A, glm::vec3 B, glm::mat4 const & mvp, int w, int h );

   // VIP software preview image util:  This prevents dead locks by quasi infinite runtime!
   // Any 2d line can become very large when outside 3d camera view. We have to clip them.
   // Even with integers we can end up rendering 4 billion pixels for one line!
   // instead of 20000 for 20MP, which is already much but ok.
   // With CNC code we have upto 1 mio. lines in 3-5 seconds to draw. Thatswhy we cant afford
   // runtime increase factors of 400000 to raster invisible lines.
   // The algo is still new, young, ugly and untested, but it prevents the infinite runtime issues i had.
   static glm::ivec4
   clipLine( glm::ivec4 const & ab, int w, int h );

   // Important software preview image util:
   // Combines transform world3d to screen2d and screen2d-clipping in one action/call.
   static void
   drawColorLine( Image & img,
             glm::vec3 A, glm::vec3 B, u32 colorA, u32 colorB, glm::mat4 const & mvp, bool blend = true );

   void
   saveMeshJSLines( std::string saveUri );

   void
   saveMeshJSLineStrip( std::string saveUri );

   std::string
   save( std::string saveUri );
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
