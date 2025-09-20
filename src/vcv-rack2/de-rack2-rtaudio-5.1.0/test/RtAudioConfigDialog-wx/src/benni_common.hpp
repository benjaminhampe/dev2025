#pragma once
#include <cstdint>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream> // debug
#include <functional>
#include <algorithm>
#include <chrono>
#include <thread>
//#include <cstdarg>
#include <cstring> // strlen()
#include <cmath>
#include <codecvt> // convert wstring ( unicode ) <-> string ( multibyte utf8 )
#include <cwchar>
#include <iomanip> // string_converter
#include <memory>
#include <wx/image.h>
#include <wx/imagxpm.h>
#include <wx/wx.h>
#include <wx/dcbuffer.h>
// #include <wx/combobox.h>
// #include <wx/dialog.h>
// #include <wx/filectrl.h>
// #include <wx/filepicker.h>
// #include <wx/filedlg.h>
// #include <wx/image.h>
// #include <wx/panel.h>
// #include <wx/stattext.h>
// #include <wx/textctrl.h>
#include <RtAudio/RtAudio.hpp>
#include <tinyxml2/tinyxml2.h>
#include <de_fontawesome.hpp>
#include <de/audio/dsp/IDspChainElement.hpp>

inline void
dbSleepMs( int ms )
{
   std::this_thread::sleep_for( std::chrono::milliseconds( ms ) );
}

inline int64_t
dbNanoseconds()
{
   typedef std::chrono::steady_clock Clock_t; // high_resolution_clock Clock_t;
   auto dur = Clock_t::now() - Clock_t::time_point(); // now - epoch = dur
   return std::chrono::duration_cast< std::chrono::nanoseconds >( dur ).count();
}

inline int64_t
dbMicroseconds()
{
   return dbNanoseconds() / 1000;
}

inline int32_t
dbMilliseconds()
{
   return uint64_t( std::abs( dbNanoseconds() ) ) / 1000000ul;
}

inline double
dbSeconds()
{
   return double( dbNanoseconds() ) * 1e-9;
}

inline std::string
dbLoadText( std::string const & uri )
{
   std::ifstream fin( uri.c_str() );
   std::ostringstream s;
   if ( fin.is_open() )
   {
      s << fin.rdbuf();
   }
   else
   {
      s << "Not a file " << uri;
   }

   return s.str();
}


inline bool
dbLoadImageXPM( wxImage & img, std::string const & xpm_data )
{
   //std::string xpm_data = dbLoadText( uri );
   img = wxImage( xpm_data.c_str() );
   return true;
}

inline double
computeEnergySum( float const* pSamples, size_t nSamples )
{
   double energy = 0.0;
   if ( nSamples > 0 )
   {
      for ( size_t i = 0; i < nSamples; ++i )
      {
         double const A = *pSamples++;
         energy += A * A;
      }
   }
   return energy;
}

inline RtAudio::DeviceInfo
getRtDeviceInfo( RtAudio* dac, int i )
{
   if (!dac) return {};
   int n = dac->getDeviceCount();
   if ( i >= 0 && i < n )
   {
      RtAudio::DeviceInfo di = dac->getDeviceInfo( i );
      std::cout << "i(" << i << "), n(" << n << "), name(" << di.name << ")\n";
      return di;
   }
   else
   {
      std::cout << "Invalid i(" << i << "), n(" << n << ")\n";
      return {};
   }
}

inline RtAudio::DeviceInfo
getRtDeviceInfo( RtAudio* dac, std::string const & name )
{
   for ( size_t i = 0; i < dac->getDeviceCount(); ++i )
   {
      RtAudio::DeviceInfo const & info = dac->getDeviceInfo( i );
      if ( info.name == name )
      {
         return info;
      }
   }
   return {};
}

inline int
getRtDeviceIndex( RtAudio* dac, std::string const & name )
{
   for ( size_t i = 0; i < dac->getDeviceCount(); ++i )
   {
      auto const & dml = dac->getDeviceInfo( i );
      if ( dml.name == name )
      {
         return i;
      }
   }
   return -1;
}

inline int getRtDefaultOutputDeviceIndex( RtAudio* dac )
{
   for ( size_t i = 0; i < dac->getDeviceCount(); ++i )
   {
      auto const & d = dac->getDeviceInfo( i );
      if ( d.outputChannels > 0 && d.isDefaultOutput )
      {
         return i;
      }
   }
   return -1;
}

inline int getRtDefaultInputDeviceIndex( RtAudio* dac )
{
   for ( size_t i = 0; i < dac->getDeviceCount(); ++i )
   {
      auto const & d = dac->getDeviceInfo( i );
      if ( d.inputChannels > 0 && d.isDefaultInput )
      {
         return i;
      }
   }
   return -1;
}

inline int getRtDefaultOutputDeviceChannelCount( RtAudio* dac )
{
   int found = getRtDefaultOutputDeviceIndex( dac );
   if ( found > -1 )
   {
      return dac->getDeviceInfo( found ).outputChannels;
   }
   return 0;
}

inline int getRtDefaultInputDeviceChannelCount( RtAudio* dac )
{
   int found = getRtDefaultInputDeviceIndex( dac );
   if ( found > -1 )
   {
      return dac->getDeviceInfo( found ).inputChannels;
   }
   return 0;
}

/*

inline std::vector< std::string >
dbStrSplit( std::string const & txt, char searchChar )
{
   std::vector< std::string > lines;
   std::string::size_type pos1 = 0;
   std::string::size_type pos2 = txt.find( searchChar, pos1 );

   while ( pos2 != std::string::npos )
   {
      std::string line = txt.substr( pos1, pos2-pos1 );
      if ( !line.empty() )
      {
         lines.emplace_back( std::move( line ) );
      }

      pos1 = pos2+1;
      pos2 = txt.find( searchChar, pos1 );
   }

   std::string line = txt.substr( pos1 );
   if ( !line.empty() )
   {
      lines.emplace_back( std::move( line ) );
   }
   return lines;
}

inline uint32_t
dbStrReplace( std::string & s, std::string const & from, std::string const & to )
{
   uint32_t replaceActionsDone = 0;

   if ( s.empty() )
   {
      return replaceActionsDone;
   }

   if ( to.empty() )
   {
      size_t pos = s.find( from );
      if ( pos == std::string::npos )
      {
         //std::cout << "No expected from(" << from << ") found" << std::endl;
      }
      else
      {
         while( pos != std::string::npos )
         {
            s.erase( pos, pos + from.size() );
            replaceActionsDone++;
            pos = s.find( from, pos );
         }
      }
   }
   else
   {
      size_t pos = s.find( from );
      if ( pos == std::string::npos )
      {
         //std::cout << "No expected from(" << from << ") found" << std::endl;
      }
      else
      {
         while( pos != std::string::npos )
         {
            s.replace( pos, from.size(), to ); // there is something to replace
            pos += to.size(); // handles bad cases where 'to' is a substring of 'from'
            replaceActionsDone++;   // advance action counter
            pos = s.find( from, pos ); // advance to next token, if any
         }
      }
   }
   return replaceActionsDone;
}



inline std::vector< std::string >
dbLoadTextLn( std::string const & uri )
{
   std::string content = dbLoadText( uri );
   dbStrReplace( content, "\r\n", "\n" );
   dbStrReplace( content, "\n\r", "\n" );
   std::vector< std::string > lines = dbStrSplit( content, '\n' );
   return lines;
}

bool
dbLoadImageXPMImpl( wxImage & img, std::vector< std::string > const & xpm_data )
{
   //DE_DEBUG("xpm_data = ", xpm_data.size())

   bool hadHeaderLine = false;
   uint32_t w = 0;
   uint32_t h = 0;
   uint32_t colorCount = 0;
   uint32_t digitCount = 0;
   size_t parsedColors = 0;
   size_t parsedRows = 0;

   std::unordered_map< std::string, uint32_t > symbolToColorMap;

   for ( size_t i = 0; i < xpm_data.size(); ++i )
   {
      std::string const & line = xpm_data[ i ];

      if ( !hadHeaderLine )
      {
         std::vector< std::string > header = dbStrSplit( line, ' ');
         if ( header.size() < 4 )
         {
            //DE_ERROR("Malformed xpm header, "
            //         "line(",line,"), header(",header.size(),")")
            for ( size_t k = 0; k < header.size(); ++k )
            {
               //DE_ERROR("header[",k,"] ", header[ k ])
            }
            return false;
         }

         w = atoi( header[ 0 ].c_str() );
         h = atoi( header[ 1 ].c_str() );
         colorCount = atoi( header[ 2 ].c_str() );
         digitCount = atoi( header[ 3 ].c_str() );

         //DE_DEBUG("Parsed XPM header w(",w,"), h(",h,"), "
         //         "colorCount(",colorCount,"), digitCount(",digitCount,")")

         symbolToColorMap.reserve( colorCount );
         // ::PNG setFormat( ColorFormat::RGBA8888
         //::PNG setFormat( ColorFormat::RGBA8888
         img.SetType( wxBITMAP_TYPE_PNG );
         img.resize( w, h );
         hadHeaderLine = true;
      }
      else
      {
         if ( parsedColors < colorCount )
         {
            // example: ".@ c #C4BBAC"
            std::vector< std::string > dat = dbStrSplit( line, ' ');

            if ( dat.size() < 3 || dat[1] != "c")
            {
               //DE_ERROR("Malformed xpm color table entry ", line)
               return false;
            }

            std::string symbol = dat[ 0 ];
            uint32_t color = parseColor( dat[ 2 ] );
            symbolToColorMap[ symbol ] = color;

            //DE_DEBUG("ParsedColor[",parsedColors+1," of ",colorCount,"] = ", color, ", symbol(",symbol,")")
            ++parsedColors;
         }
         else if ( parsedRows < h )
         {
            // DE_DEBUG("ParsedRow[",parsedRows+1," of ",h,"]:")

            for ( size_t x = 0; x < w; ++x )
            {
               size_t charIndex = digitCount * x;

               std::string symbol = line.substr( charIndex, digitCount );
               uint32_t color = symbolToColorMap[ symbol ];

              // DE_DEBUG("Pixel(",x,",",parsedRows,") = ",color, " at index(",charIndex,"), "
              //           "symbol(",symbol,")")

               img.setPixel( x, parsedRows, color );
            }

            ++parsedRows;
         }
      }
   }

   //DE_DEBUG("img ",img.toString())
   return true;

}


inline bool
dbLoadImageXPM( wxImage & img, std::string const & uri )
{
   if ( uri.empty() )
   {
      //DE_ERROR("Empty uri")
      return false;
   }

   std::vector< std::string > lines = dbLoadTextLn( uri );

   auto findQuotes = [] ( std::string const & line)
   {
      int start = -1;
      int ende = -1;

      std::string::size_type pos1 = line.find('"');
      if ( pos1 != std::string::npos )
      {
         start = int( pos1 + 1 );
         std::string::size_type pos2 = line.find('"', start );

         if ( pos2 != std::string::npos )
         {
            ende = int( pos2 );
            return std::pair< int,int >( start,ende-start );
         }
      }

      return std::pair< int,int >( -1,0 );
   };

   std::vector< std::string > xpm_data;
   xpm_data.reserve( lines.size() );

   for ( size_t i = 0; i < lines.size(); ++i )
   {
      std::string const & line = lines[ i ];

      std::pair< int,int > validRange = findQuotes( line );
      if ( validRange.first < 1 || validRange.second < 1 )
      {
         //DE_DEBUG("Comment line = ", line)
      }
      else
      {
         // Get only string part between quotes ""
         // Dismiss all enclosing chars - trim.
         std::string xpm_line = line.substr( validRange.first, validRange.second );
         xpm_data.emplace_back( std::move( xpm_line ) );
      }
   }

   //DE_DEBUG("uri = ",uri)
   //DE_DEBUG("lines = ",lines.size())

   bool ok = dbLoadImageXPMImpl( img, xpm_data );
   if ( ok )
   {
      img.setUri( uri );
   }
   return ok;
}

*/

namespace glm {

// =======================================================================
struct ivec2
// =======================================================================
{
   int x;
   int y;

   ivec2() : x(0), y(0) {}
   ivec2( int x_, int y_ ) : x(x_), y(y_) {}
};

} // end namespace glm

namespace de {

// =======================================================================
struct AspectRatio
// =======================================================================
{
   // Aligns a text around its anchor point inside screen(scr_w,scr_h) rect.
   // textRect.xy = given anchor point
   // textRect.wh = given text size(w,h) in (pixels).
   static glm::ivec2
   fitAspectPreserving( int src_w, int src_h, int max_w, int max_h )
   {
      if ( src_w < 1 ) return { 0,0 };
      if ( src_h < 1 ) return { 0,0 };
      if ( max_w < 1 ) return { 0,0 };
      if ( max_h < 1 ) return { 0,0 };

      float aspect_src = float( src_w ) / float( src_h );
      float aspect_max = float( max_w ) / float( max_h );

      int dst_w = max_w;
      int dst_h = max_h;

      // +----max_w----+
      // |             |
      // |+-----------+|
      // ||           ||max_h       dst_w     src_w     max_w
      // |+-----------+|           ------- = ------- > -------
      // |             |            dst_h     src_h     max_h
      // +-------------+
      if ( aspect_src >= aspect_max )
      {
         dst_w = max_w;
         dst_h = int32_t( std::round( float( dst_w ) / aspect_src ) );
      }
      // +-------------+
      // |   +-----+   |
      // |   |     |   |
      // |   |     |   |max_h
      // |   |     |   |
      // |   +-----+   |
      // +-------------+
      else
      {
         dst_h = max_h;
         dst_w = int32_t( std::round( float( dst_h ) * aspect_src ) );
      }

      return { dst_w, dst_h };
   }

};

// =======================================================================
struct Align
// =======================================================================
{
   enum EAlign
   {
      NoAlign = 0,
      Left = 1,
      Center = 2,
      Right = 4,
      Top = 8,
      Middle = 16,
      Bottom = 32,

      TopLeft = Left | Top,
      TopCenter = Center | Top,
      TopRight = Right | Top,
      MiddleLeft = Left | Middle,
      MiddleCenter = Center | Middle,
      MiddleRight = Right | Middle,
      BottomLeft = Left | Bottom,
      BottomCenter = Center | Bottom,
      BottomRight = Right | Bottom,

      LeftTop = TopLeft,
      CenterTop = TopCenter,
      RightTop = TopRight,
      LeftMiddle = MiddleLeft,
      Centered = Center | Middle,
      RightMiddle = MiddleRight,
      LeftBottom = BottomLeft,
      CenterBottom = BottomCenter,
      RightBottom = BottomRight,

      Default = TopLeft
   };
};

} // end namespace de







inline wxColour randomWxColor()
{
   int r = rand() % 255;
   int g = rand() % 255;
   int b = rand() % 255;
   return wxColour(r,g,b);
}






#ifdef USE_DARKIMAGE
// #include <DarkImage.hpp>

inline wxImage
toWxImage( de::Image const & src )
{
   int w = src.getWidth();
   int h = src.getHeight();
   wxImage dst( wxSize( w,h ) );

   for ( int y = 0; y < h; ++y )
   {
      for ( int x = 0; x < w; ++x )
      {
         uint32_t color = src.getPixel( x,y );
         uint8_t r = de::RGBA_R( color );
         uint8_t g = de::RGBA_G( color );
         uint8_t b = de::RGBA_B( color );
         uint8_t a = de::RGBA_A( color );
         dst.SetRGB( x,y,r,g,b );
         dst.SetAlpha( x,y,a );
      }
   }

   return dst;
}

inline de::Image
fromWxImage( wxImage const & src )
{
   int w = src.GetWidth();
   int h = src.GetHeight();
   de::Image dst( w,h );

   for ( int y = 0; y < h; ++y )
   {
      for ( int x = 0; x < w; ++x )
      {
         uint8_t r = src.GetRed( x,y );
         uint8_t g = src.GetGreen( x,y );
         uint8_t b = src.GetBlue( x,y );
         uint8_t a = src.GetAlpha( x,y );
         dst.setPixel( x,y,de::RGBA(r,g,b,a) );
      }
   }

   return dst;
}


inline wxImage
createWxImageFromColor( int w, int h, uint32_t color )
{
   wxImage dst( wxSize( w,h ) );

   uint8_t r = de::RGBA_R( color );
   uint8_t g = de::RGBA_G( color );
   uint8_t b = de::RGBA_B( color );
   uint8_t a = de::RGBA_A( color );

   for ( int y = 0; y < h; ++y )
   {
      for ( int x = 0; x < w; ++x )
      {
         dst.SetRGB( x,y,r,g,b );
         dst.SetAlpha( x,y,a );
      }
   }

   return dst;
}
#endif


