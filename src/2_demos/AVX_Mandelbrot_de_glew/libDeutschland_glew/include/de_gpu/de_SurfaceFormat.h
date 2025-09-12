#pragma once
#include <de_core/de_StringUtil.h>
#include <de_image/de_PixelFormat.h>

namespace de {

// ===========================================================================
struct SurfaceFormat
// ===========================================================================
{
   DE_CREATE_LOGGER("SurfaceFormat")
   constexpr static uint8_t s_FloatMask = 0x80;
   uint8_t redBits = 0;
   uint8_t greenBits = 0;
   uint8_t blueBits = 0;
   uint8_t alphaBits = 0;
   uint8_t depthBits = 0;
   uint8_t stencilBits = 0;

   SurfaceFormat( uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 0, uint8_t d = 0, uint8_t s = 0 )
      : redBits(r), greenBits(g), blueBits(b), alphaBits(a), depthBits(d), stencilBits(s)
   {}

   std::string
   toString() const
   {
      std::ostringstream o;
      if ( redBits > 0 )
      {
         o << "R";
         if ( redBits > 0x80 )   { o << int(redBits & 0x7F) << "F"; }
         else                    { o << int(redBits); }
      }
      if ( greenBits > 0 )
      {
         o << "G";
         if ( greenBits > 0x80 ) { o << int(greenBits & 0x7F) << "F"; }
         else                    { o << int(greenBits); }
      }
      if ( blueBits > 0 )
      {
         o << "B";
         if ( blueBits > 0x80 )  { o << int(blueBits & 0x7F) << "F"; }
         else                    { o << int(blueBits); }
      }
      if ( alphaBits > 0 )
      {
         o << "A";
         if ( alphaBits > 0x80 ) { o << int(alphaBits & 0x7F) << "F"; }
         else                    { o << int(alphaBits); }
      }
      if ( depthBits > 0 )
      {
         o << "D";
         if ( depthBits > 0x80 ) { o << int(depthBits & 0x7F) << "F"; }
         else                    { o << int(depthBits); }
      }
      if ( stencilBits > 0 )
      {
         o << "S";
         if ( stencilBits > 0x80 ){ o << int(stencilBits & 0x7F) << "F"; }
         else                     { o << int(stencilBits); }
      }
      return o.str();
   }


   static std::vector< std::string >
   splitStringInWords( std::string const & txt )
   {
      if ( txt.size() < 2 )
      {
         return std::vector< std::string >{ txt };
      }

      size_t pos1 = 0;
      size_t pos2 = 0;

      std::vector< std::string > words;

      for ( size_t i = 0; i < txt.size(); ++i )
      {
         pos2 = i;

         char ch = txt[ i ];
         
         // makeUpperCase for easier (non repeated) parsing
         if ( ch >= 'a' && ch <= 'z' )
         {
            ch += char('A') - char('a');
         }

         // parse
         if ( ch >= 'A' && ch <= 'Z' )
         {
            if ( pos2 > pos1 )
            {
               std::string command = txt.substr( pos1, pos2-pos1 );
               words.emplace_back( std::move( command ) );
            }

            pos1 = pos2;
         }

      }

      if ( pos2 > pos1 )
      {
         std::string command = txt.substr( pos1 );
         words.emplace_back( std::move( command ) );
      }

      return words;
   }

   static SurfaceFormat
   parseString( std::string s )
   {
      std::vector< std::string > words = splitStringInWords( s );
      if (words.empty()) return SurfaceFormat();
      uint8_t redBits = 0;
      uint8_t greenBits = 0;
      uint8_t blueBits = 0;
      uint8_t alphaBits = 0;
      uint8_t depthBits = 0;
      uint8_t stencilBits = 0;
      for ( size_t i = 0; i < words.size(); ++i )
      {
         std::string word = words[i];
         if ( word.size() < 2) {
            DE_ERROR("Word has too few chars")
            continue;
         }
         char key = word[0];
         bool isFloat = false;   // TODO: fully support
         uint8_t value = 0;

         std::string tmp = word.substr(1); // Cut off key char from value str

         if ( StringUtil::endsWith( tmp, 'f' ) || StringUtil::endsWith( tmp, 'F' ) )
         {
            isFloat = true;
            tmp = tmp.substr( 0, tmp.size() - 1); // Cut off 'F|f' char from value str
         }

         if ( tmp.size() > 0 )
         {
            value = uint8_t(atol( tmp.c_str() )); // Parse value str as integer
         }

         if ( key == 'R' ) { redBits = value; }
         if ( key == 'G' ) { greenBits = value; }
         if ( key == 'B' ) { blueBits = value; }
         if ( key == 'A' ) { alphaBits = value; }
         if ( key == 'D' ) { depthBits = value; }
         if ( key == 'S' ) { stencilBits = value; }
      }
      return SurfaceFormat(redBits,greenBits,blueBits,alphaBits,depthBits,stencilBits);
   }
   


   // isFloatMask = 0x80, value 0-127
   static PixelFormat
   toPixelFormat( SurfaceFormat const & fmt )
   {
      bool floatR = fmt.redBits > 128;
      bool floatG = fmt.greenBits > 128;
      bool floatB = fmt.blueBits > 128;
      bool floatA = fmt.alphaBits > 128;
      bool floatD = fmt.depthBits > 128;
      bool floatS = fmt.stencilBits > 128;

      int redBits = floatR ? fmt.redBits - 128 : fmt.redBits;
      int greenBits = floatG ? fmt.greenBits - 128 : fmt.greenBits;
      int blueBits = floatB ? fmt.blueBits - 128 : fmt.blueBits;
      int alphaBits = floatA ? fmt.alphaBits - 128 : fmt.alphaBits;
      int depthBits = floatD ? fmt.depthBits - 128 : fmt.depthBits;
      int stencilBits = floatS ? fmt.stencilBits - 128 : fmt.stencilBits;

      if ( redBits > 0 ) {
         if ( redBits == 8 ) {
            if ( greenBits == 8 ) {
               if ( blueBits == 8 ) {
                  if ( alphaBits == 8 )
                  {
                     return PixelFormat::R8G8B8A8;
                  }
                  else
                  {
                     return PixelFormat::R8G8B8;
                  }
               }
            }
            else
            {
               return PixelFormat::R8;
            }
         }
         else if ( redBits == 16 )
         {
            if ( !floatR )
            {
               return PixelFormat::R16;
            }
            else
            {
               //return PixelFormat::R16F;
            }

         }
         else if ( redBits == 32 )
         {
            if ( floatR )
            {
               return PixelFormat::R32F;
            }

         }
      }
      else
      {
         if ( depthBits == 32 )
         {
            if ( stencilBits > 0 )
            {
               return PixelFormat::D32FS8;
            }
            else
            {
               return PixelFormat::D32F;
            }
         }
         else if ( depthBits > 0 )
         {
            return PixelFormat::D24S8;
         }
      }
      return PixelFormat::Unknown;
   }

   // isFloatMask = 0x80, value 0-127
   static void
   test()
   {
      SurfaceFormat s1( 8,0,0,0,24,8 );
      PixelFormat t1 = toPixelFormat( s1 );
      DE_DEBUG("Test1 SurfaceFormat = ",s1.toString())
      DE_DEBUG("Test1 PixelFormat = ",t1.toString())

      SurfaceFormat s2( 8,8,8,0,24,8 );
      PixelFormat t2 = toPixelFormat( s2 );
      DE_DEBUG("Test2 SurfaceFormat = ",s2.toString())
      DE_DEBUG("Test2 PixelFormat = ",t2.toString())

      SurfaceFormat s3( 8,8,8,8,24,8 );
      PixelFormat t3 = toPixelFormat( s2 );
      DE_DEBUG("Test3 SurfaceFormat = ",s3.toString())
      DE_DEBUG("Test3 PixelFormat = ",t3.toString())

      SurfaceFormat s4( 0,0,0,0,24,8 );
      PixelFormat t4 = toPixelFormat( s4 );
      DE_DEBUG("Test4 SurfaceFormat = ",s4.toString())
      DE_DEBUG("Test4 PixelFormat = ",t4.toString())

      SurfaceFormat s5( 0,0,0,0,32+128,8 );
      PixelFormat t5 = toPixelFormat( s5 );
      DE_DEBUG("Test5 SurfaceFormat = ",s5.toString())
      DE_DEBUG("Test5 PixelFormat = ",t5.toString())
   }
   
};


} // end namespace de