// Copyright (C) 2002-2012 Benjamin Hampe
#include "ImageXPM_Reader.hpp"

#ifdef DE_IMAGE_READER_XPM_ENABLED

namespace de {
namespace image {

bool
ImageReaderXPM::load( Image & img, std::string const & uri )
{
   if ( uri.empty() )
   {
      DE_ERROR("Empty uri")
      return false;
   }

   //std::vector< std::string > lines = FileSystem::loadTextLines( uri );

   std::string fileContent = FileSystem::loadText( uri );
   fileContent = StringUtil::replace( fileContent, "\n\r", "\n");
   fileContent = StringUtil::replace( fileContent, "\r\n", "\n");
   fileContent = StringUtil::replace( fileContent, "\r", "\n");
   fileContent = StringUtil::replace( fileContent, "\n\n", "\n");
   std::vector< std::string > lines = StringUtil::split( fileContent, '\n' );

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
         DE_DEBUG("Comment line = ", line)
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

   bool ok = loadImpl( img, xpm_data );
   if ( ok )
   {
      img.setUri( uri );
   }
   return ok;
}

bool
ImageReaderXPM::loadImpl( Image & img, std::vector< std::string > const & xpm_data )
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
         std::vector< std::string > header = StringUtil::split( line, ' ');
         if ( header.size() < 4 )
         {
            DE_ERROR("Malformed xpm header, "
                     "line(",line,"), header(",header.size(),")")
            for ( size_t k = 0; k < header.size(); ++k )
            {
               DE_ERROR("header[",k,"] ", header[ k ])
            }
            return false;
         }

         w = atoi( header[ 0 ].c_str() );
         h = atoi( header[ 1 ].c_str() );
         colorCount = atoi( header[ 2 ].c_str() );
         digitCount = atoi( header[ 3 ].c_str() );

         DE_DEBUG("Parsed XPM header w(",w,"), h(",h,"), "
                  "colorCount(",colorCount,"), digitCount(",digitCount,")")

         symbolToColorMap.reserve( colorCount );
         img.setFormat( ColorFormat::RGBA8888 );
         img.resize( w, h );
         hadHeaderLine = true;
      }
      else
      {
         if ( parsedColors < colorCount )
         {
            // example: ".@ c #C4BBAC"
            std::vector< std::string > dat = StringUtil::split( line, ' ');

            if ( dat.size() < 3 || dat[1] != "c")
            {
               DE_ERROR("Malformed xpm color table entry ", line)
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

/*


bool
ImageReaderXPM::loadImpl( Image & img, std::vector< std::string > const & lines, bool usesQuotes )
{
   DE_DEBUG("lines = ", lines.size())

   bool hadHeaderLine = false;
   uint32_t w = 0;
   uint32_t h = 0;
   uint32_t colorCount = 0;
   uint32_t digitCount = 0;

   size_t parsedColors = 0;
   size_t parsedRows = 0;

   std::unordered_map< std::string, uint32_t > symbolToColorMap;

   auto getValidLineData = [] ( std::string const & line)
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

   size_t i = 0;
   while ( i < lines.size() )
   {
      std::string const & line = lines[ i ];

      std::pair< int,int > validRange = getValidLineData( line );
      if ( validRange.first < 1 ||
           validRange.second < 1 )
      {
         DE_DEBUG("Comment line = ", line)
      }
      else
      {
         if ( !hadHeaderLine )
         {
            // Trim "", split by spaces -> get 4 numbers
            std::string lineData = line.substr( validRange.first, validRange.second );
            std::vector< std::string > header = dbStrSplit( lineData, ' ');
            if ( header.size() < 4 )
            {
               DE_ERROR("Malformed xpm header, "
                        "lineData(",lineData,"), header(",header.size(),")")
               for ( size_t k = 0; k < header.size(); ++k )
               {
                  DE_ERROR("header[",k,"] ", header[ k ])
               }
               return false;
            }

            w = atoi( header[ 0 ].c_str() );
            h = atoi( header[ 1 ].c_str() );
            colorCount = atoi( header[ 2 ].c_str() );
            digitCount = atoi( header[ 3 ].c_str() );

            DE_DEBUG("Parsed XPM header w(",w,"), h(",h,"), "
                     "colorCount(",colorCount,"), digitCount(",digitCount,")")

            symbolToColorMap.reserve( colorCount );
            img.setColorFormat( ColorFormat::RGBA8888 );
            img.resize( w, h );
            hadHeaderLine = true;
         }
         else
         {
            if ( parsedColors < colorCount )
            {
               // example: ".@ c #C4BBAC"
               std::string lineData = line.substr( validRange.first,
                                                   validRange.second );
               std::vector< std::string > dat = dbStrSplit( lineData, ' ');

               if ( dat.size() < 3 || dat[1] != "c")
               {
                  DE_ERROR("Malformed xpm color table entry ", lineData)
                  return false;
               }

               std::string symbol = dat[ 0 ];
               uint32_t color = parseColor( dat[ 2 ] );
               symbolToColorMap[ symbol ] = color;

               DE_DEBUG("ParsedColor[",parsedColors+1," of ",colorCount,"] = ", color, ", symbol(",symbol,")")
               ++parsedColors;
            }
            else if ( parsedRows < h )
            {
               // DE_DEBUG("ParsedRow[",parsedRows+1," of ",h,"]:")

               for ( size_t x = 0; x < w; ++x )
               {
                  size_t charIndex = size_t( validRange.first ) + digitCount * x;

                  std::string symbol = line.substr( charIndex, digitCount );
                  uint32_t color = symbolToColorMap[ symbol ];

                  DE_DEBUG("Pixel(",x,",",parsedRows,") = ",color, " at index(",charIndex,"), "
                            "symbol(",symbol,")")

                  img.setPixel( x, parsedRows, color );
               }

               ++parsedRows;
            }
         }
      }

      i++;
   }

   DE_DEBUG("img ",img.toString())
   return true;

}

*/

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_XPM_ENABLED














#if 0
   // get file size in bytes
   const u32 fileSize = file->getSize();

   // abort
   if (fileSize == 0)
   {
      os::Printer::log("Invalid file-size.", ELL_ERROR);
      return 0;
   }

   // create memory buffer for file
   c8* bytes = new c8[fileSize];
   // abort
   if (!bytes)
   {
      os::Printer::log("Could not allocate buffer.", ELL_ERROR);
      return 0;
   }

   // just to make sure
   file->seek( 0 );

   // begin reading
   const u32 charCount = file->read(bytes, fileSize);

   if (charCount != fileSize)
   {
      os::Printer::log("Could not read all bytes, may result in corrupted image.", ELL_WARNING);
   }

   // copy buffer to string, delete buffer
   core::stringc irrStr = bytes;
   delete [] bytes;

   // split string on line-end characters '\r' and '\n', skip empty lines
   core::array< core::stringc > lines;
   irrStr.split( lines, "\r\n", 2, true, false);

   // os::Printer::log( core::sprintf("File has %i lines.", lines.size()).c_str(), (lines.size()<2)?ELL_ERROR:ELL_NONE);
#endif

#if READ_FILE_TO_VECTOR_OF_NOTEMPTY_TRIMMED_LINES
   // purge all unwanted white-space chars at beginning and end of lines
   for (u32 line = 0; line<lines.size(); line++)
   {
      lines[line].trim();
   }
#endif

#if READ_FILE_TO_VECTOR_OF_NOTEMPTY_TRIMMED_UNCOMMENTED_LINES
   // remove all C-Style comments the quick way //, /* /**, ///, //! -> just erase all lines starting with '/'
   u32 line = 0;
   while (line < lines.size())
   {
      if (lines[line].size()>0)
      {
         if (lines[line][0]=='/')
         {
            lines.erase(line,1);
            line--;
         }
      }
      else
      {
         lines.erase(line,1);
         line--;
      }
      line++;
   }
#endif
   // --> now the data is most likely pure XPM-image data

#if REACHED_2020
   // infos to extract from data
   Image* result = 0;
   // locals
   std::vector< XPM_Color > xpmColors; // final array containing all unique colors and their ascii combination
   std::string xpmName, xpmColorKey, xpmColorType, xpmColorValue;
   char xpmNameBuf[1024];
   memset(xpmNameBuf, 0, 1024);

   uint32_t xpmWidth, xpmHeight, xpmColorCount, xpmBytesPerColor;

   uint32_t x=0, y=0;	// current pixel position
   int32_t i=0, p=0;	 // i=current-line-index, p = position of a char within current line
   uint8_t CurrentState = 0;

   while ( i < int32_t( lines.size() ) )
   {
      std::string s = lines[ i ]; // copy current line in work-string

      // extract name
      if (i==0)
      {
         ::sscanf( s.c_str(), "static const char *const %s[] = {", xpmNameBuf );
      }
      // extract header
      else if (i==1)
      {
         ::sscanf(s.c_str(), "\"%i %i %i %i\",", &xpmWidth, &xpmHeight, &xpmColorCount, &xpmBytesPerColor );
      }
      // extract colors & pixels
      else
      {
         // extract colors
         if (CurrentState == 0)
         {
            auto pos = s.find_first_of( '\"' );
            p = ( pos == std::string::npos ) ? -1 : int32_t( pos );
            if ( p == -1 )
               p = 0;

            xpmColorKey = s.substr( p, xpmBytesPerColor );
            p += xpmBytesPerColor;
            s = s.substr( p, s.size()-p-1 );

            char xpmColorTypeBuf[ 8 ];
            char xpmColorValueBuf[ 8 ];
            ::sscanf( s.c_str(), " %s %s\",", xpmColorTypeBuf, xpmColorValueBuf );

            xpmColorType = xpmColorTypeBuf;
            xpmColorValue = xpmColorValueBuf;
#if IMPLEMENTED
            xpmColorType.trim();
            xpmColorValue.trim();
            xpmColorType.make_lower();
            xpmColorValue.make_lower();
#endif
//				if (xpmColorType != core::stringc("c"))
//					os::Printer::log( core::sprintf("Could not determine color-type %s.", xpmColorType.c_str()).c_str(), ELL_WARNING);

            if ( xpmColorValue == "none" )
            {
               xpmColors.emplace_back( xpmColorKey, 0x00000000 );
            }
            else
            {
               if ( xpmColorValue.size() >= 7 )
               {
                  uint32_t r(0), g(0), b(0);
                  ::sscanf( xpmColorValue.c_str(), "#%02x%02x%02x", &r, &g, &b );
                  xpmColors.emplace_back( xpmColorKey, ARGB( r, g, b ) );
               }
               else
               {
                  // os::Printer::log( core::sprintf("Could not read color-value %s.", xpmColorValue.c_str()).c_str(), ELL_WARNING);
                  // os::Printer::log( "Could not read color-value.", ELL_WARNING);
               }
            }

            if ( ( i - 2 >= int32_t( xpmColorCount ) ) || ( xpmColors.size() >= xpmColorCount ) )
            {
               result = new CImage(ECF_A8R8G8B8, core::dimension2du(xpmWidth, xpmHeight));
               if (!result)
                  break;
               result->fill(0);
               CurrentState++;
            }

         }
         // extract pixels
         else if (CurrentState == 1)
         {
            if ( y >= result->getDimension().Height)
               break;

            p = s.findFirst('\"');
            if (p==-1)
               p=0;
            else
               p++;
            x=0;

            while (x < result->getDimension().Width)
            {
               core::stringc key = s.subString(p, xpmBytesPerColor, false);

               bool found = false;
               u32 index=0;
               while (index<xpmColors.size())
               {
                  if (key == xpmColors[index].key)
                  {
                     found = true;
                     break;
                  }
                  index++;
               }

               if (found)
               {
                  result->setPixel(x,y,xpmColors[index].value);
               }

               p += xpmBytesPerColor;
               x++;
            }
            y++;
         }
         else
         {
            break;
         }
      }
      i++;
   }
   return result;
#endif
