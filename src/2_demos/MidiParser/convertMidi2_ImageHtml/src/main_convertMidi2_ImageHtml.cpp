//////////////////////////////////////////////////////////////////////////////
/// @file main_MidiParser.cpp
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#include "Midi2ImageHtmlConverter.hpp"
#include <de_commdlg_openfile.hpp>
#include <de_commdlg_savefile.hpp>
#include <de/midi/Parser.hpp>
#include <de/midi/file/Listener.hpp>

namespace de {
namespace midi {

// =======================================================================
struct Converter
// =======================================================================
{
   DE_CREATE_LOGGER("de.midi.Converter")

   static void
   test()
   {
      convert( "../../media/midi/But not tonight - Depeche Mode_4_14.mid" );
      convert( "../../media/midi/porcelain_moby_davebulow_4_07.mid" ); //
      convert( "../../media/midi/hotelcal_6_37.mid" ); //
      convert( "../../media/midi/gigidagostino_illflywithyou_joerock_5_03.mid" ); //
      convert( "../../media/midi/thank_you_50_12.mid" ); //
      //convert( "../../media/midi/lost_3_34.mid" );
      //convert( "../../media/midi/Bitter Sweet Symphony_4_16.mid" ); //
      //convert( "../../media/midi/mmm_3_43.mid" ); //
   }

   static bool
   convert( std::string loadUri, std::string saveUri = "" )
   {
      if ( saveUri.empty() )
      {
         saveUri = FileSystem::fileName(loadUri) + ".html";
      }

      file::File file;
      file::Listener listener;
      listener.setFile( &file );

      Parser parser;
      parser.addListener( &listener );
      parser.parse( loadUri );

//      size_t expect = bv.size();
//      size_t result = parser.parse( bv.data(), bv.data() + expect, loadUri );
//      if ( expect != result )
//      {
//         std::cout << "Parser had error expect(" << expect << ") != result("<<result<<")" <<std::endl;
//      }

      return ImageHtml::convert( saveUri, file );
   }
};

} // end namespace midi
} // end namespace de

//========================================================================
int main( int argc, char** argv )
//========================================================================
{
   DG_DEBUG( argv[ 0 ], " by (c) 2022 by <benjaminhampe@gmx.de>" )

   std::string loadUri;
   if ( argc > 1 )
   {
      loadUri = argv[1];
   }

   // (c) 2022 by <benjaminhampe@gmx.de>
   // Choose midi file...
   if ( !de::FileSystem::existFile( loadUri ) )
   {
      loadUri = dbOpenFileA(
         "MidiParser expects a midi file (.mid, .midi, .smf ) | (c) 2022 by <benjaminhampe@gmx.de>");
   }

   std::string saveUri = de::FileSystem::fileName(loadUri) + ".html";

   de::midi::Converter::convert( loadUri, saveUri );

   // Open your local html browser
   std::ostringstream o;
   o << "\"" << saveUri << "\"";  // Add Quotation marks for uris with spaces.
   std::string s = o.str();
   DG_DEBUG("Run command ",s)
   system( s.c_str() );
   return 0;
}
