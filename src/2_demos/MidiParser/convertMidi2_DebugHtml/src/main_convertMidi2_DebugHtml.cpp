//////////////////////////////////////////////////////////////////////////////
/// @file main_MidiParser.cpp
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////

#include "DebugHtml.hpp"
#include <de_commdlg_openfile.hpp>
#include <de_commdlg_savefile.hpp>
#include <de/PerformanceTimer.hpp>

#include <de/midi/duration_computer/DurationComputer.hpp>

#include <de/midi/file/Listener.hpp>

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

   // (c) 2022 by <benjaminhampe@gmx.de>
   // If no command 3rd command line arg for saveUri was found it
   // chooses to save mid.html along the .mid file in same directory.
   std::string saveUri;
   if ( argc > 2 )
   {
      saveUri = argv[ 2 ];
   }
   saveUri = loadUri + ".html";

   // (c) 2022 by <benjaminhampe@gmx.de>
   // Validate dialog result.
   // if ( !de::midi::convertMidi2Html::convert( loadUri, saveUri ) )
   // {
      // std::cout << "Cant convert midi("<<loadUri<<") to html("<<saveUri<<"), program exits now."<<std::endl;
      // return 0;
   // }

   if ( !de::FileSystem::existFile( loadUri ) )
   {
      DG_ERROR("No loadUri")
      return 0;
   }

   if ( saveUri.empty() )
   {
      DG_ERROR("No saveUri")
      saveUri = loadUri + ".html";
      // return false;
   }

   DG_DEBUG("MidiParser loadUri = ", loadUri )
   DG_DEBUG("HtmlWriter saveUri = ", saveUri )

   // Parser:
   de::midi::Parser parser;

   // Add listener 0: DurationComputer
   //de::DurationComputer durationComputer;
   //parser.addListener( &durationComputer );

   // Add listener 1: Html writer
   de::midi::DebugHtml html;
   parser.addListener( &html );

   // Add listener 2: Standard Midi file listener
   de::midi::file::File file;
   de::midi::file::Listener fileListener;
   fileListener.setFile( &file );
   parser.addListener( &fileListener );

   if ( !parser.parse( loadUri ) )
   {
      DG_ERROR("Parser had error, ", loadUri)
   }

   DG_DEBUG( "SMF:" )
   DG_DEBUG( file.toString() )

   // Write html ( most time intensive part )
   html.save( saveUri );

   // Open html in system default browser
   std::ostringstream o;
   o << "\"" << saveUri << "\"";  // Add Quotation marks for uris with spaces.
   std::string s = o.str();
   DG_DEBUG("Run command ",s)
   system( s.c_str() );

   return 0;
}
