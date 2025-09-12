// Copyright (C) 2002-2012 Benjamin Hampe
#include "de_ImageXPM.h"

#if defined(DE_IMAGE_READER_XPM_ENABLED) || defined(DE_IMAGE_WRITER_XPM_ENABLED)

namespace de {
namespace image {

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

} // end namespace image.
} // end namespace de.

#endif
