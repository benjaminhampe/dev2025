#include <de/image/Image_XPM.h>
#include <de/Core.h>
#if defined(DE_IMAGE_READER_XPM_ENABLED) || defined(DE_IMAGE_WRITER_XPM_ENABLED)
#include <fstream>
#include <sstream>
#endif

namespace de {
namespace image {

#ifdef DE_IMAGE_READER_XPM_ENABLED

/*
bool
ImageReaderXPM::load( Image & img, std::string const & uri )
{
    if ( uri.empty() )
    {
        DE_ERROR("Empty uri")
        return false;
    }

    std::string fileContent = FileSystem::loadStr( uri );

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
*/

bool
ImageReaderXPM::load( Image & img, const uint8_t* p, size_t n, const std::string& uri )
{
    if (!p || n < 12) return false;

    // Convert raw memory buffer to an std::string
    std::string fileContent(reinterpret_cast<const char*>(p), n);
    //fileContent = StringUtil::replace( fileContent, "\n\r", "\n");
    //fileContent = StringUtil::replace( fileContent, "\r\n", "\n");
    //fileContent = StringUtil::replace( fileContent, "\r", "\n");
    //fileContent = StringUtil::replace( fileContent, "\n\n", "\n");
    //std::vector< std::string > lines = StringUtil::split( fileContent, '\n' );

    if ( fileContent.empty() )
    {
        DE_ERROR("Empty data.")
        return false;
    }

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

    // Create an istringstream from the string
    std::istringstream iss(fileContent);

    bool hadHeaderLine = false;
    uint32_t w = 0;
    uint32_t h = 0;
    uint32_t colorCount = 0;
    uint32_t digitCount = 0;
    size_t parsedColors = 0;
    size_t parsedRows = 0;
    std::unordered_map< std::string, uint32_t > symbolToColorMap;

    // Read from stream (example: parsing text)
    std::string line;
    while (std::getline(iss, line))
    {
        // DE_DEBUG("Parsed line: ", line)

        if ( !hadHeaderLine )
        {
            std::vector< std::string > header = StringUtil::split( line, ' ');
            if ( header.size() < 4 )
            {
                DE_ERROR("Malformed xpm header, line(",line,"), header(",header.size(),")")
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

            DE_DEBUG("Parsed XPM header w(",w,"), h(",h,"), colorCount(",colorCount,"), digitCount(",digitCount,")")

            symbolToColorMap.reserve( colorCount );
            img.resize( w, h, PixelFormat::R8G8B8A8 );
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

#endif // DE_IMAGE_READER_XPM_ENABLED



#ifdef DE_IMAGE_WRITER_XPM_ENABLED

bool
ImageWriterXPM::save( const Image& img, const std::string& uri, uint32_t param )
{
   if ( uri.empty() ) { DE_ERROR("Empty uri(", uri, ")") return false; }
   if ( img.empty() ) { DE_ERROR("Cant save empty image uri(", uri, ")") return false; }
   int32_t w = img.w();
   int32_t h = img.h();
   if ( w < 1 || h < 1 ) { DE_ERROR("Got empty image(", uri, ")") return false; }

   // calculate needed bytes to store one color
   std::string const valid_chars = "1234567890"
                                   "abcdefghijklmnopqrstuvwxyz"
                                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "{}[|]$<^>:~(;%-)=@_!.+`#'&*,/";

   // Allocate worst case ( each pixel has a different color )
   std::vector< uint32_t > colors;
   colors.reserve( img.pixelCount() );

   // loop image and find all unique colors

   // std::ostringstream s;
   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t const color = img.getPixel( x, y ); // get color
         //color |= 0xFF000000; // set alpha const to 255

         // add_unique
         auto itCached = std::find_if( colors.begin(), colors.end(),
            [&] ( uint32_t cached )
            { return cached == color; } );
         if ( itCached == colors.end() )
         {
            colors.emplace_back( color );
         }

//         bool found = false;
//         for ( size_t i = 0; i < colors.size(); ++i )
//         {
//            if ( colors[ i ] == color )
//            {
//               found = true;
//               break;
//            }
//         }

//         // add_unique
//         if ( !found )
//         {
//            colors.emplace_back( color );
//         }
      }
   }

   uint32_t digitCount = 1;
   double f = double( colors.size() );
   double e = double( valid_chars.size() );
   while ( f >= e )
   {
      f /= e;
      ++digitCount;
   }

   //DE_DEBUG("digitCount(",digitCount,"), img(", img.toString(),")")

   std::vector< std::string > names;
   names.reserve( colors.size() );

   // Unique color -> Unique name
   for ( size_t i = 0; i < colors.size(); ++i )
   {
      names.push_back( xpm::Utils::computeColorName(i,valid_chars,digitCount) );
   }

   // XPM-Header
   // static char const * const xpm_test1[] = {
   // "/* XPM */",
   // "16 16 6 1",
   // " 	c None",
   // ".	c #FFFFFF",
   // "+	c #000000",
   // "@	c #9A9A9A",
   // "#	c #FEFEFE",
   // "$	c #F7F7F7",
   // "................",

   std::string baseName = FileSystem::fileBase( uri );
   StringUtil::replace( baseName, ".", "_");
   StringUtil::replace( baseName, ",", "_");
   StringUtil::replace( baseName, "-", "_");
   StringUtil::replace( baseName, "+", "_");
   StringUtil::replace( baseName, "*", "_");
   StringUtil::replace( baseName, "/", "_");
   StringUtil::replace( baseName, "\\","_");
   StringUtil::replace( baseName, " ", "_");
   StringUtil::replace( baseName, "#", "_");
   StringUtil::replace( baseName, "<", "_");
   StringUtil::replace( baseName, ">", "_");
   StringUtil::replace( baseName, "$", "_");
   StringUtil::replace( baseName, "!", "_");
   StringUtil::replace( baseName, "?", "_");
   StringUtil::replace( baseName, "(", "_");
   StringUtil::replace( baseName, ")", "_");
   StringUtil::replace( baseName, "{", "_");
   StringUtil::replace( baseName, "}", "_");
   StringUtil::replace( baseName, "[", "_");
   StringUtil::replace( baseName, "]", "_");
   StringUtil::replace( baseName, "%", "_");
   StringUtil::replace( baseName, "'", "_");
   StringUtil::replace( baseName, "~", "_");
   StringUtil::replace( baseName, "|", "_");
   StringUtil::replace( baseName, "'", "_");
   StringUtil::replace( baseName, "^", "_");
   StringUtil::replace( baseName, "°", "_");
   StringUtil::replace( baseName, "&", "_");
   StringUtil::replace( baseName, "€", "_");

   std::ostringstream o;
   o <<
   "/* XPM-Header */\n"
   "/* URI = " << uri << " */\n"
   "/* ValidChars = " << valid_chars.size() << "*/\n"
   "/* Width in [px] = " << w << "*/\n"
   "/* Height in [px] = " << h << "*/\n"
   "/* NumColors = " << colors.size() << "*/\n"
   "/* CharsPerColor = " << digitCount << "*/\n"
   "/* Encoded by ImageWriterXPM.cpp from libDarkImage */\n"
   "/* Author: Benjamin Hampe <benjaminhampe@gmx.de> */\n"
   "/* Whats special about these: Decoding and Encoding are much faster than IrfanView! */\n"
   //"static std::vector< std::string > const " << baseName << "_xpm {\n"
   "static const char * const " << baseName << "_xpm[] = {\n"

   "\""<<w<<" "<<h<<" "<<
#if 0
   colorCount
#else
   colors.size()
#endif
   <<" "<<digitCount<<"\",\n";

   // ====================================
   // Write color-table to xpm
   // ====================================
   for ( size_t i = 0; i < colors.size(); ++i )
   {
      // format color #RRGGBB
      uint32_t color = colors[ i ];
//      std::string name = ;
//      char buf[ 8 ];
//      sprintf( buf, "#%02x%02x%02x",
//                     RGBA_R( color ),
//                     RGBA_G( color ),
//                     RGBA_B( color ) );
//      std::string tmp = buf;
//      dbStrUpperCase( tmp );
      o << "\"" << names[ i ] << " c " << xpm::Utils::toRGBA( color ) << "\",\n";
   }

   // ====================================
   // Write pixels to xpm
   // ====================================
   std::string line;
   for ( int32_t y = 0; y < h; ++y )
   {
      line = "\"";
      for ( int32_t x = 0; x < w; ++x )
      {
         // current pixel
         uint32_t color = img.getPixel( x, y );
         //color.setAlpha(255);

         // search color in color-table
//         if ( color != 0 )
//         {
//            color |= 0xFF000000; // Set alpha to opaque ( speed up compare )
//         }

         auto cit = std::find_if( colors.begin(), colors.end(),
                        [&] ( uint32_t cached ) { return color == cached; }
         );
         if ( cit != colors.end() )
         {
            // write found color-table color
            line += names[ std::distance( colors.begin(), cit ) ];
         }

/*
         size_t found = 0;
         for ( size_t i = 0; i < colors.size(); ++i )
         {
            if ( colors[ i ] == color )
            {
               found = i;
               break;
            }
         }

         // write found color-table color
         line += names[ found ];
*/
      }

      line += "\"";
      if ( y < h-1 )
         line+=",";
      else
         line+="};";

      o << line << "\n";
   }

   return dbSaveTextA(o.str(), uri);
}


#endif // DE_IMAGE_WRITER_XPM_ENABLED


} // end namespace image.
} // end namespace de.



















































#if 0

// XPM 1
static char const * const xpm_test1[] = {
   "/* XPM */",
   "16 16 6 1",
   " 	c None",
   ".	c #FFFFFF",
   "+	c #000000",
   "@	c #9A9A9A",
   "#	c #FEFEFE",
   "$	c #F7F7F7",
   "................",
   "................",
   "................",
   "................",
   "..++............",
   "..++............",
   "..++..++++.++++.",
   "..++..++@#.++@#.",
   "..++..++$..++$..",
   "..++..++...++...",
   "..++..++...++...",
   "..++..++...++...",
   "................",
   "................",
   "................",
   "................"
};

// XPM 2
static const char* const write_test_xpm[] = {
   "/* XPM */",
   "32 32 407 2",
   "  	c None",
   ". 	c #7373C1",
   "+ 	c #6E6EBF",
   "@ 	c #6B6BBF",
   "# 	c #6868BF",
   "$ 	c #6464BF",
   "Z+	c #FF4A4A",
   "`+	c #FF4545",
   " @	c #E73535",
   "`@	c #B8B819",
   " #	c #B7B717",
   "0#	c #C50000",
   "                                                                ",
   "                                                                ",
   "                                                                ",
   "                                                                ",
   "                      . + @ # $ % & * =                         ",
   "                  - ; > , ' ) ! ~ { ] ^ / (                     ",
   "                _ : < [ } | 1 2 3 4 5 6 7 8 9                   ",
   "                0 a b b c 2 3 4 5 6 d e f g h                   ",
   "                i j k l 2 4 5 6 d e f m n o p                   ",
   "                q r s t u 6 d e f m n v w x y                   ",
   "                z A B | C D f m E F G H I J K                   ",
   "                L M 1 N O m n P Q H R S T U V W X Y Z `  ...+.  ",
   "                p @.#.$.%.&.*.=.-.;.>.,.'.).!.~.{.].^./.(._.:.<.",
   "  [.}.|.1.2.3.4.5.6.6 7.&.8.I ;.9.0.a.b.c.d.e.f.g.h.i.j.k.l.m.n.",
   "}.o.p.q.r.s.t.u.v.w.O x.y.;.9.0.z.A.B.C.D.E.F.h.i.G.k.l.H.I.J.K.",
   "L.M.N.O.P.Q.R.S.T.U.V.e W.0.z.A.X.Y.Z.`.d. +.+G.k.l.H.I.J.++@+#+",
   "$+%+&+*+R.S.=+-+;+>+,+'+)+A.X.Y.!+~+{+]+^+/+(+_+H.I.J.:+<+[+}+|+",
   "1+2+3+4+=+-+5+6+7+8+9+0+a+b+c+d+e+f+g+h+i+j+k+I.l+m+n+o+p+q+r+s+",
   "t+u+v+w+5+6+7+x+y+z+A+B+C+D+E+F+G+H+I+J+K+h.L+++M+N+p+O+P+Q+R+S+",
   "T+U+2+V+7+x+y+W+X+Y+Z+`+ @.@        +@@@#@$@%@&@*@O+P+Q+R+=@-@;@",
   ">@,@'@)@y+!@~@{@Z+`+]@^@/@(@        _@:@<@[@}@O+P+Q+R+=@-@|@1@2@",
   "3@4@5@7+6@7@8@9@0@a@b@c@d@e@        f@g@h@i@j@Q+R+=@-@|@1@k@l@m@",
   "n@y+o@p@{@9@0@a@b@c@q@r@s@t@        u@v@i@w@Q+=@-@|@1@k@l@x@y@z@",
   "A@p@-+B@0@a@b@c@q@r@C@D@E@F@        G@H@I@J@K@L@M@N@O@P@Q@R@S@T@",
   "U@7@4@V@b@c@q@r@C@D@W@X@Y@Z@          `@ #.#+#@###$#%#&#*#=#=#  ",
   "-#;#>#,#q@r@C@D@W@X@'#)#!#~#                                    ",
   "{#(@]#^#/#(#_#:#<#[#}#|#1#                                      ",
   "    2#3#4#5#6#7#8#9#1#0#                                        ",
   "                                                                ",
   "                                                                ",
   "                                                                ",
   "                                                                "
};

std::string
ImageWriterXPM::colorIndexToChars( uint32_t index, uint32_t bytesPerColor ) const
{
   std::string result;

   uint32_t i = index;
   c8* buf = new c8[bytesPerColor];
   uint32_t byteIndex = 0;

   while (byteIndex < bytesPerColor)
   {
      i = index / core::Math::pow( valid_chars.size(), bytesPerColor - byteIndex );
      buf[ bytesPerColor - byteIndex - 1 ] = valid_chars[ i%valid_chars.size() ];
      byteIndex++;
   }

   result = buf;
   delete [] buf;
   return result;
}

bool
ImageWriterXPM::save( Image const & img, std::string const & uri, uint32_t param )
{
   if ( img.isEmpty() ) return false;
#if IMPLEMENTED_2020
   // abort
   if (!file)
      return false;

   // size of source-image
    core::dimension2du Size = image->getDimension();

    // abort
    if (Size == core::dimension2du(0,0))
        return false;

   // create array to hold all unique RGB colors
    core::array< XPM_Color > colors(Size.Width*Size.Height);
    colors.set_used(0);

   // loop image and find all unique colors
    for (uint32_t y=0; y<Size.Height; y++)
    {
        for (uint32_t x=0; x<Size.Width; x++)
        {
         // get color
            SColor color = image->getPixel(x,y);

            // set alpha const to 255
            color.setAlpha(255);

            // loop color array to see if color already stored
            bool found = false;
            for (uint32_t i=0; i<colors.size(); i++)
            {
                if (colors[i].value == color)
                {
                    found = true;
                    break;
                }
            }

            // if not found in array, color is unique --> store
            if (!found)
            {
               XPM_Color data(color,"");
                colors.push_back(data);
            }
        }
    }

   // calculate needed bytes to store one color
   uint32_t bytesPerColor = 1;
   uint32_t k = colors.size();
   while (k>=valid_chars.size())
   {
      k/=valid_chars.size();
      bytesPerColor++;
   }

   // now give every unique color a corresponding byte-combination out of valid_chars
   uint32_t c=0;
   while ( c < colors.size() )
   {
      colors[c].key = colorIndexToChars(c, bytesPerColor);
      c++;
   }

   // uri
   std::string uri = file->getFileName();
   uri.make_lower();
   uri.replace('\\','/');

   // fileBaseName erase path
   std::string fileBaseName = uri;
   s32 pos = fileBaseName.findLast('/');
   if (pos != -1)
   {
      fileBaseName = fileBaseName.subString(pos+1, fileBaseName.size()-(pos+1));
   }

   // fileBaseName erase extension
   pos = fileBaseName.findLast('.');
   if (pos != -1)
   {
      fileBaseName = fileBaseName.subString(0,pos);
   }

   // XPM-Header
   // static const char *const wxwin32x32_xpm[] = {
   // "32 32 407 2",
   // "  	c None",

   // /* XPM */
   file->write("/* XPM */",9);
   endLine(file);

   // static const char *const wxwin32x32_xpm[] = {
   std::string s;
   s="static const char *const "; s+=fileBaseName; s+="_xpm[] = {";
   file->write(s.c_str(),s.size() );
   endLine(file);

   // "32 32 407 2"
   // Breite des Bildes (in Pixel)
   // Höhe des Bildes (in Pixel)
   // Anzahl der Farben im Bild
   // Anzahl der Zeichen pro Pixelwert
   // (X-Position des „Hotspots") --> MausCursor
   // (Y-Position des „Hotspots") --> MausCursor
   s="\""; s+=image->getDimension().Width;
   s+=" "; s+=image->getDimension().Height;
   s+=" "; s+=colors.size();
   s+=" "; s+=bytesPerColor; s+="\",";
   file->write(s.c_str(),s.size() );
   endLine(file);

   // write color-table
   // "  	c None",
   // ". 	c #7373C1",
   // "+ 	c #6E6EBF",
   // "@ 	c #6B6BBF",
   // "# 	c #6868BF",
   // "$ 	c #6464BF",
   // "Z+	c #FF4A4A",
   // "`+	c #FF4545",
   // " @	c #E73535",
   // "`@	c #B8B819",
   // " #	c #B7B717",
   // "0#	c #C50000",

   for (uint32_t i=0; i<colors.size(); i++)
   {
      file->write("\"",1);
      file->write( colors[i].key.c_str(), colors[i].key.size() );
      file->write(" c ",3);

      // format color #RRGGBB
      c8 buf[7];
      const SColor& c = colors[i].value;
      sprintf(buf,"#%02x%02x%02x", c.getRed(), c.getGreen(), c.getBlue());
      std::string tmp = buf;
      tmp.make_upper();
      file->write(tmp.c_str(), tmp.size());

      file->write("\",",2);
      endLine(file);
   }

   // write pixels
   //	"$+%+&+*+R.S.=+-+;+>+,+'+)+A.X.Y.!+~+{+]+^+/+(+_+H.I.J.:+<+[+}+|+",

   // buffer for current line
   std::string line;

   // loop
   for (uint32_t y=0; y<Size.Height; y++)
   {
      line = "\"";
      for (uint32_t x=0; x<Size.Width; x++)
      {
         // current pixel
         SColor colorNow = image->getPixel(x,y);
         colorNow.setAlpha(255);

         // search color in color-table
         uint32_t i=0;
         while (i<colors.size())
         {
            if (colors[i].value == colorNow)
            {
               break;
            }
            i++;
         }

         // write found color-table color
         line+=colors[i].key;
      }
      line+="\"";
      if (y < Size.Height-1)
         line+=",";
      else
         line+="};";

      file->write(line.c_str(),line.size() );
      endLine(file);
   }
#endif
   return true;
}

#endif
