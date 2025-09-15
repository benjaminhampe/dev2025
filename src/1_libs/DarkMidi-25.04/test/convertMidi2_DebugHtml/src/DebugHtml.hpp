#pragma once
#include <de/midi/Parser.h>
#include <DarkImage.h>

namespace de {
namespace midi {

inline uint32_t randomHtmlColor()
{
   return dbRGBA( rand()%200, rand()%200, rand()%200 );
}

// =======================================================================
struct DebugHtmlToken
// =======================================================================
{
   uint64_t m_address;
   uint32_t m_fg;
   uint32_t m_bg;
   std::string m_comment;
   std::vector< uint8_t > m_original;
   std::vector< uint8_t > m_parsed;

   DebugHtmlToken()
         : m_address( 0 )
         , m_fg( 0xFF000000 )
         , m_bg( 0xFFFFFFFF )
   {}
};

// =======================================================================
struct DebugHtml : public IParserListener
// =======================================================================
{
   DE_CREATE_LOGGER("de.midi.DebugHtml")

   std::string m_fileName;

   uint8_t const* m_startAddress; // Start of file
   uint8_t const* m_endAddress;   // End of file
   uint8_t const* m_currAddress;  // Current offset to file start

   std::vector< DebugHtmlToken > m_tokens;

   DebugHtml()
      : m_startAddress( nullptr )
      , m_endAddress( nullptr )
   {
      //DE_DEBUG("Constructor")
   }

   ~DebugHtml() override
   {
      //DE_DEBUG("Destructor")
   }

   void
   mpStart( uint8_t const* beg, uint8_t const* end, std::string const & uri ) override
   {
      m_fileName = uri;
      m_startAddress = beg;
      m_endAddress = end;
      m_currAddress = beg;
      m_tokens.clear();
   }

   void
   mpToken( uint8_t const* beg, uint8_t const* end, std::string const & comment ) override
   {
      auto fileOffset = uint64_t( beg - m_startAddress );
      DebugHtmlToken token;
      token.m_address = fileOffset;
      token.m_fg = randomHtmlColor();
      token.m_bg = 0xFFFFFFFF;
      token.m_comment = comment;

      token.m_parsed.assign( beg, end );
      token.m_original.assign( m_currAddress, m_currAddress + token.m_parsed.size() );

      m_currAddress += token.m_parsed.size();

      m_tokens.emplace_back( std::move( token ) );
   }

   bool
   save( std::string saveUri ) const
   {
      // Write html ( most time intensive part )
      PerformanceTimer perf;
      perf.start();

      std::ostringstream o;

      o << "<html>\n"
      "<head>\n"
      "<title>Midi2Html: " << m_fileName << "</title>\n"
      "<style type=\"text/css\">\n"
      "*    { margin:0; border:0; padding:0; }\n"
      "body { width:100%; height:100%; color:rgb(0,0,0); background-color:rgb(255,255,255); font-family:monospace; font-size:16px; }\n"
      //"caption { font-weight:bold; font-size:24px; line-height:50px; vertical-align:middle;}\n"
      "content { background-color:white; color:black; position:absolute; top:50px; padding-top:20px; white-space:nowrap; z-index:1;}\n"
      // "img { border:5px; border-color:black; padding:5px; margin:5px;}\n"
      // "div12 { width:100%; height:12px; line-height:12px; display-style:inline; }\n"
      // "h1 { color:white; background-color:rgb(0   ,0   ,0  ); font-size:64px; vertical-align:middle; font-weight:900; }\n"
      // "h2 { color:white; background-color:rgb(127 ,192 ,255); font-size:32px; vertical-align:middle; font-weight:750; }\n"
      // "h3 { color:white; background-color:rgb(192 ,200 ,255); font-size:24px; vertical-align:middle; font-weight:600; }\n"
      // "h4 { color:white; background-color:rgb(200 ,200 ,255); font-size:24px; vertical-align:middle; font-weight:600; }\n"
      // "h5 { color:black; background-color:rgb(255 ,255 ,255); font-size:16px; vertical-align:middle; font-weight:400; }\n"
      // "h6 { color:white; background-color:rgb(64  ,64  ,192); font-size:12px; vertical-align:middle; font-weight:400; }\n"
      // "hr { color:black; background-color:rgb(255 ,255 ,0); height:10px; line-height:5px; vertical-align:middle; }\n"
      // "date { color:#FF00FF; font-weight:bold; margin-left:20px;}\n"
      // "line { color:#207F20; font-weight:bold; margin-left:20px;}\n"
      // "time { color:#4040DF; font-weight:bold; margin-left:20px;}\n"
      // "trace {color:white; background-color:#00AF00; margin-left:20px;}\n"
      // "debug {color:white; background-color:#FF00FF; margin-left:20px;}\n"
      // "info { margin-left:20px; }\n"
      // "warn {color:black; background-color:yellow; margin-left:20px;}\n"
      // "error {color:white; background-color:red; margin-left:20px;}\n"
      "</style>\n"
      "</head>\n"
      "<body>\n"

      // ---------------------- title (=loadUri) ----------------------------------
      "<panel style=\"background-color:blue; position:fixed; top:0px; left:0px; width:100%; height:50px; z-index:2;\">\n"
      "<table width=\"100%\" height=\"100%\">\n"
      "<tr valign=\"middle\">"
      "<td align=\"center\">"
      "<caption style=\"color:white; font-weight:bold; font-size:16px; line-height:50px; vertical-align:middle;\">" << m_fileName << "</caption></td>\n"
      "</tr>\n"
      "</table>\n"
      "</panel>\n";

      // ---------------------- content start ----------------------------------
      o << "<content>\n";

   // ---------------------------- print colored midiTokens ----------------------------------

      o <<
      "<table width=\"90%\" cellspacing=\"5\" cellpadding=\"5\">\n"
      "<tr>\n"
         "<th width=\"10%\">Index</th>\n"
         "<th width=\"20%\">Address</th>\n"
         "<th width=\"30%\">Original Data</th>\n"
         "<th width=\"30%\">Parsed Data</th>\n"
         "<th width=\"40%\">Comments</th>\n"
      "</tr>\n";

      for ( size_t i = 0; i < m_tokens.size(); ++i )
      {
         auto const & token = m_tokens[ i ];

         int r = dbRGBA_R( token.m_fg );
         int g = dbRGBA_G( token.m_fg );
         int b = dbRGBA_B( token.m_fg );
         auto css = StringUtil::join( "color:rgb(",r,",",g,",",b,");" );

         if ( token.m_original != token.m_parsed )
         {
            css += StringUtil::join( " background-color:rgb(255,150,150); " );
         }

         uint8_t const* orig_beg = token.m_original.data();
         uint8_t const* orig_end = token.m_original.data() + token.m_original.size();

         uint8_t const* pars_beg = token.m_parsed.data();
         uint8_t const* pars_end = token.m_parsed.data() + token.m_parsed.size();

         o <<
         "<tr style=\"" << css << "\">"
         "<td><pre>" << i << "</pre></td>"
         "<td><pre>(0x" << StringUtil::hex( uint32_t(token.m_address) ) << ")</pre></td>"
         "<td><pre>" << StringUtil::hex( orig_beg, orig_end, 4 ) << "</pre></td>"
         "<td><pre>" << StringUtil::hex( pars_beg, pars_end, 4 ) << "</pre></td>"
         "<td><pre>" << token.m_comment << "</pre></td>"
         "</tr>\n";
      }

      // ---------------------------- end content ----------------------------------

      o <<
      "</table>\n"

      "<br />"
      "<br />"
      "<br />"
      "<br />"
      "<br />"
      "</content>\n"

      // ---------------------------- footer ----------------------------------

      "<footer style=\"background-color:blue; color:white; position:fixed; bottom:0px; left:0px; width:100%; height:50px; line-height:50px; z-index:3;\">\n"
      "<table width=\"100%\" height=\"100%\">\n"
      "<tr valign=\"middle\"><td align=\"center\"><h3 style=\"color:white;\">File produced by class DebugHtml.hpp</h3></td></tr>\n"
      "<tr valign=\"middle\"><td align=\"center\"><h6 style=\"color:white;\">(c) 2023 by BenjaminHampe@gmx.de</h6></td></tr>\n"
      "</table>\n"
      "</footer>\n"
      "</body>\n"
      "</html>\n";

      std::string htmlStr = o.str();

      bool const ok = FileSystem::saveStr( saveUri, htmlStr );
      perf.stop();

      if ( ok )
      {
         DE_DEBUG("Needed ", perf.ms(), " ms, saved HTML ", saveUri, " with ", htmlStr.size(), " bytes.")
      }
      else
      {
         DE_ERROR("Needed ", perf.ms(), " ms, can't save HTML ", saveUri, " with ", htmlStr.size(), " bytes.")
      }

      return ok;
   }
};

} // end namespace midi
} // end namespace de

