#pragma once
#include <de_core/de_StringUtil.h>

namespace de {

// ===========================================================================
struct SO
// ===========================================================================
{
   DE_CREATE_LOGGER("SamplerOptions")

   enum class TexMin
   {
      Nearest = 0, // = GL_NEAREST,
      Linear, // GL_LINEAR,
      NearestMipmapNearest, //  = GL_NEAREST_MIPMAP_NEAREST,
      NearestMipmapLinear, //  = GL_NEAREST_MIPMAP_LINEAR,
      LinearMipmapNearest, //  = GL_LINEAR_MIPMAP_NEAREST,
      LinearMipmapLinear, //  = GL_LINEAR_MIPMAP_LINEAR,
      MaxCount,
      Default = TexMin::Linear
   };

   enum class TexMag
   {
      Nearest = 0, // = GL_NEAREST,
      Linear, // = GL_LINEAR,
      MaxCount,
      Default = TexMag::Linear
   };

   enum class TexWrap
   {
      Repeat = 0, //  = GL_REPEAT,
      RepeatMirrored, //  = GL_MIRRORED_REPEAT
      ClampToEdge, //  = GL_CLAMP_TO_EDGE,
      ClampToBorder, //  = GL_CLAMP_TO_BORDER,
      MaxCount,
      Default = TexWrap::Repeat
   };

   static std::string toStr( TexMin const texMin )
   {
      switch( texMin )
      {
         case TexMin::Nearest: return "Nearest";
         case TexMin::Linear: return "Linear";
         case TexMin::NearestMipmapNearest: return "NearestMipmapNearest";
         case TexMin::NearestMipmapLinear: return "NearestMipmapLinear";
         case TexMin::LinearMipmapNearest: return "LinearMipmapNearest";
         case TexMin::LinearMipmapLinear: return "LinearMipmapLinear";
         default: return "Invalid";
      }
   }

   static std::string toStr( TexMag const texMag )
   {
      switch( texMag )
      {
         case TexMag::Nearest: return "Nearest";
         case TexMag::Linear: return "Linear";
         default: return "Invalid";
      }
   }

   static std::string toStr( TexWrap const texWrap )
   {
      switch( texWrap )
      {
         case TexWrap::Repeat: return "Repeat";
         case TexWrap::RepeatMirrored: return "RepeatMirrored";
         case TexWrap::ClampToEdge: return "ClampToEdge";
         case TexWrap::ClampToBorder: return "ClampToBorder";
         default: return "Invalid";
      }
   }

   std::string toString() const
   {
      std::ostringstream o;
      if ( anisotropicFilter > 0.5f )                 o << "AF";
      else                                            o << "0";
      o << ",";
           if ( min == TexMin::Linear )               o << "L";
      else if ( min == TexMin::NearestMipmapNearest ) o << "NMN";
      else if ( min == TexMin::NearestMipmapNearest ) o << "NML";
      else if ( min == TexMin::LinearMipmapNearest )  o << "LMN";
      else if ( min == TexMin::LinearMipmapLinear )   o << "LML";
      else                                            o << "N";
      o << ",";
           if ( mag == TexMag::Linear )               o << "L";
      else                                            o << "N";
      o << ",";
           if ( wrapS == TexWrap::RepeatMirrored ) o << "RM";
      else if ( wrapS == TexWrap::ClampToEdge )    o << "CTE";
      else if ( wrapS == TexWrap::ClampToBorder )  o << "CTB";
      else                                         o << "R";
      o << ",";
           if ( wrapS == TexWrap::RepeatMirrored ) o << "RM";
      else if ( wrapS == TexWrap::ClampToEdge )    o << "CTE";
      else if ( wrapS == TexWrap::ClampToBorder )  o << "CTB";
      else                                         o << "R";
      return o.str();
   }

   static SO
   parseString( std::string csv )
   {
      SO so;
      so.anisotropicFilter = 0.0f;
      so.min = TexMin::Nearest;
      so.mag = TexMag::Nearest;
      so.wrapS = TexWrap::ClampToEdge;
      so.wrapT = TexWrap::ClampToEdge;
      so.wrapR = TexWrap::ClampToEdge;

      auto words = StringUtil::split( csv, ',' );

      // Parse AF ( Anisotropic Filtering 0,1 ) -> auto increased to max. AF when >0.5.
      if ( words.size() > 0 )
      {
         std::string const & word = words[0];
         if ( word.size() > 0 )
         {
            if ( word == "AF" )
            {
               so.anisotropicFilter = 1.0f;
            }
         }
      }
      // Parse Minify filter Nearest|Linear|NearestMipmapNearest|LinearMipmapLinear|etc...
      if ( words.size() > 1 )
      {
         std::string const & word = words[1];
         if ( word.size() > 1 )
         {
            if ( word == "L" )
            {
               so.min = TexMin::Linear;
            }
            else if ( word == "NMN" )
            {
               so.min = TexMin::NearestMipmapNearest;
            }
            else if ( word == "NML" )
            {
               so.min = TexMin::NearestMipmapLinear;
            }
            else if ( word == "LMN" )
            {
               so.min = TexMin::LinearMipmapNearest;
            }
            else if ( word == "LML" )
            {
               so.min = TexMin::LinearMipmapLinear;
            }
         }
      }
      // Parse Magnify filter Nearest|Linear
      if ( words.size() > 2 )
      {
         std::string const & word = words[2];
         if ( word == "L" )
         {
            so.mag = TexMag::Linear;
         }
      }
      // Parse WrapS
      if ( words.size() > 3 )
      {
         std::string const & word = words[3];
         if ( word == "R" )
         {
            so.wrapS = TexWrap::Repeat;
         }
         else if ( word == "RM" )
         {
            so.wrapS = TexWrap::RepeatMirrored;
         }
         else if ( word == "CTE" )
         {
            so.wrapS = TexWrap::ClampToEdge;
         }
         else if ( word == "CTB" )
         {
            so.wrapS = TexWrap::ClampToBorder;
         }
      }
      // Parse WrapT
      if ( words.size() > 4 )
      {
         std::string const & word = words[4];
         if ( word == "R" )
         {
            so.wrapT = TexWrap::Repeat;
         }
         else if ( word == "RM" )
         {
            so.wrapT = TexWrap::RepeatMirrored;
         }
         else if ( word == "CTE" )
         {
            so.wrapT = TexWrap::ClampToEdge;
         }
         else if ( word == "CTB" )
         {
            so.wrapT = TexWrap::ClampToBorder;
         }
      }

      return so;
   }

   TexMin min;
   TexMag mag;
   TexWrap wrapS;
   TexWrap wrapT;
   TexWrap wrapR; // 3D
   float   anisotropicFilter; // anisotropicFilter

   SO()
      : min(TexMin::Default)
      , mag(TexMag::Default)
      , wrapS(TexWrap::Default)
      , wrapT(TexWrap::Default)
      , wrapR(TexWrap::Default)
      , anisotropicFilter(1.0f) // enabled, auto increased to max level.
   {

   }

   // >= 1 means, enabled, will auto increase level to max supported 16x.
   SO( float anisotropicFilterLevel,
       TexMin minify = TexMin::Default,
       TexMag magnify = TexMag::Default,
       TexWrap wrapmodeS = TexWrap::Default,
       TexWrap wrapmodeT = TexWrap::Default,
       TexWrap wrapmodeR = TexWrap::Default )
      : min(minify)
      , mag(magnify)
      , wrapS(wrapmodeS)
      , wrapT(wrapmodeT)
      , wrapR(wrapmodeR)
      , anisotropicFilter(anisotropicFilterLevel)
   {

   }

   void setMin( TexMin minify ) { min = minify; }
   void setMag( TexMag magnify ) { mag = magnify; }
   void setWrapS( TexWrap wrap ) { wrapS = wrap; }
   void setWrapT( TexWrap wrap ) { wrapT = wrap; }
   void setWrapR( TexWrap wrap ) { wrapR = wrap; }
   void setAF( float anisotropicLevel ) { anisotropicFilter = anisotropicLevel; }
/*
   std::string toString() const
   {
      std::ostringstream s;
      s << "min(" << toStr(min) << "), ";
      s << "mag(" << toStr(mag) << "), ";
      s << "wrapS(" << toStr(wrapS) << "), ";
      s << "wrapT(" << toStr(wrapT) << "), ";
      s << "wrapR(" << toStr(wrapR) << "), ";
      s << "AF(" << anisotropicFilter << ")";
      return s.str();
   }
*/
   bool hasMipmaps() const
   {
      if ( (min == TexMin::NearestMipmapNearest)
        || (min == TexMin::NearestMipmapLinear)
        || (min == TexMin::LinearMipmapNearest)
        || (min == TexMin::LinearMipmapLinear) )
      {
         return true;
      }
      else
      {
         return false;
      }
   }
};

} // end namespace GL.



/*

struct SO
{
   DE_CREATE_LOGGER("SamplerOptions")

   enum class TexMin
   {
      Nearest = 0, // = GL_NEAREST,
      Linear, // GL_LINEAR,
      NearestMipmapNearest, //  = GL_NEAREST_MIPMAP_NEAREST,
      NearestMipmapLinear, //  = GL_NEAREST_MIPMAP_LINEAR,
      LinearMipmapNearest, //  = GL_LINEAR_MIPMAP_NEAREST,
      LinearMipmapLinear, //  = GL_LINEAR_MIPMAP_LINEAR,
      MaxCount,
      Default = TexMin::Linear
   };

   enum class TexMag
   {
      Nearest = 0, // = GL_NEAREST,
      Linear, // = GL_LINEAR,
      MaxCount,
      Default = TexMag::Linear
   };

   enum class TexWrap
   {
      Repeat = 0, //  = GL_REPEAT,
      RepeatMirrored, //  = GL_MIRRORED_REPEAT
      ClampToEdge, //  = GL_CLAMP_TO_EDGE,
      ClampToBorder, //  = GL_CLAMP_TO_BORDER,
      MaxCount,
      Default = TexWrap::Repeat
   };

   static std::string toStr( TexMin const texMin )
   {
      switch( texMin )
      {
         case TexMin::Nearest: return "Nearest";
         case TexMin::Linear: return "Linear";
         case TexMin::NearestMipmapNearest: return "NearestMipmapNearest";
         case TexMin::NearestMipmapLinear: return "NearestMipmapLinear";
         case TexMin::LinearMipmapNearest: return "LinearMipmapNearest";
         case TexMin::LinearMipmapLinear: return "LinearMipmapLinear";
         default: return "Invalid";
      }
   }

   static std::string toStr( TexMag const texMag )
   {
      switch( texMag )
      {
         case TexMag::Nearest: return "Nearest";
         case TexMag::Linear: return "Linear";
         default: return "Invalid";
      }
   }

   static std::string toStr( TexWrap const texWrap )
   {
      switch( texWrap )
      {
         case TexWrap::Repeat: return "Repeat";
         case TexWrap::RepeatMirrored: return "RepeatMirrored";
         case TexWrap::ClampToEdge: return "ClampToEdge";
         case TexWrap::ClampToBorder: return "ClampToBorder";
         default: return "Invalid";
      }
   }

   TexMin min;
   TexMag mag;
   TexWrap wrapS;
   TexWrap wrapT;
   TexWrap wrapR; // 3D
   float   anisotropicFilter; // anisotropicFilter

   SO()
      : min(TexMin::Default)
      , mag(TexMag::Default)
      , wrapS(TexWrap::Default)
      , wrapT(TexWrap::Default)
      , wrapR(TexWrap::Default)
      , anisotropicFilter(1.0f) // enabled, auto increased to max level.
   {

   }

   // >= 1 means, enabled, will auto increase level to max supported 16x.
   SO( float anisotropicFilterLevel,
       TexMin minify = TexMin::Default,
       TexMag magnify = TexMag::Default,
       TexWrap wrapmodeS = TexWrap::Default,
       TexWrap wrapmodeT = TexWrap::Default,
       TexWrap wrapmodeR = TexWrap::Default )
      : min(minify)
      , mag(magnify)
      , wrapS(wrapmodeS)
      , wrapT(wrapmodeT)
      , wrapR(wrapmodeR)
      , anisotropicFilter(anisotropicFilterLevel)
   {

   }

   void setMin( TexMin minify ) { min = minify; }
   void setMag( TexMag magnify ) { mag = magnify; }
   void setWrapS( TexWrap wrap ) { wrapS = wrap; }
   void setWrapT( TexWrap wrap ) { wrapT = wrap; }
   void setWrapR( TexWrap wrap ) { wrapR = wrap; }
   void setAF( float anisotropicLevel ) { anisotropicFilter = anisotropicLevel; }

   std::string toString() const
   {
      std::ostringstream s;
      s << "min(" << toStr(min) << "), ";
      s << "mag(" << toStr(mag) << "), ";
      s << "wrapS(" << toStr(wrapS) << "), ";
      s << "wrapT(" << toStr(wrapT) << "), ";
      s << "wrapR(" << toStr(wrapR) << "), ";
      s << "AF(" << anisotropicFilter << ")";
      return s.str();
   }

   bool hasMipmaps() const
   {
      if ( (min == TexMin::NearestMipmapNearest)
        || (min == TexMin::NearestMipmapLinear)
        || (min == TexMin::LinearMipmapNearest)
        || (min == TexMin::LinearMipmapLinear) )
      {
         return true;
      }
      else
      {
         return false;
      }
   }

   void query()
   {
      min = queryMinify();
      mag = queryMagnify();
      wrapS = queryWrapS();
      wrapT = queryWrapT();
      wrapR = queryWrapR();
      anisotropicFilter = queryAF();
   }

   void apply() const
   {
      applyMinify( min );
      applyMagnify( mag );
      applyWrapS( wrapS );
      applyWrapT( wrapT );
      //applyWrapR( wrapR );
      applyAF( anisotropicFilter );
      if ( hasMipmaps() )
      {
         glGenerateMipmap( GL_TEXTURE_2D );
      }
   }

   static TexMin queryMinify()
   {
      GLint val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &val );
      switch ( val )
      {
         case GL_NEAREST: return TexMin::Nearest;
         case GL_LINEAR: return TexMin::Linear;
         case GL_NEAREST_MIPMAP_NEAREST: return TexMin::NearestMipmapNearest;
         case GL_NEAREST_MIPMAP_LINEAR: return TexMin::NearestMipmapLinear;
         case GL_LINEAR_MIPMAP_NEAREST: return TexMin::LinearMipmapNearest;
         case GL_LINEAR_MIPMAP_LINEAR: return TexMin::LinearMipmapLinear;
         default: return TexMin::Default;
      }
   }

   static TexMag queryMagnify()
   {
      GLint val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &val );
      switch ( val )
      {
         case GL_NEAREST: return TexMag::Nearest;
         case GL_LINEAR: return TexMag::Linear;
         default: return TexMag::Default;
      }
   }

   static TexWrap queryWrapS()
   {
      GLint val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &val );
      switch ( val )
      {
         case GL_REPEAT: return TexWrap::Repeat;
         case GL_MIRRORED_REPEAT: return TexWrap::RepeatMirrored;
         case GL_CLAMP_TO_EDGE: return TexWrap::ClampToEdge;
         case GL_CLAMP_TO_BORDER: return TexWrap::ClampToBorder;
         default: return TexWrap::Default;
      }
   }

   static TexWrap queryWrapT()
   {
      GLint val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &val );
      switch ( val )
      {
         case GL_REPEAT: return TexWrap::Repeat;
         case GL_MIRRORED_REPEAT: return TexWrap::RepeatMirrored;
         case GL_CLAMP_TO_EDGE: return TexWrap::ClampToEdge;
         case GL_CLAMP_TO_BORDER: return TexWrap::ClampToBorder;
         default: return TexWrap::Default;
      }
   }

   static TexWrap queryWrapR()
   {
      GLint val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &val );
      switch ( val )
      {
         case GL_REPEAT: return TexWrap::Repeat;
         case GL_MIRRORED_REPEAT: return TexWrap::RepeatMirrored;
         case GL_CLAMP_TO_EDGE: return TexWrap::ClampToEdge;
         case GL_CLAMP_TO_BORDER: return TexWrap::ClampToBorder;
         default: return TexWrap::Default;
      }
   }

   static void applyMinify( TexMin texMin )
   {
      GLint minMode = GL_NEAREST;// Low quality is default.
      switch ( texMin )
      {
         //case TexMin::Nearest: minMode = GL_NEAREST; break;
         case TexMin::Linear: minMode = GL_LINEAR; break;
         case TexMin::NearestMipmapNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
         case TexMin::NearestMipmapLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
         case TexMin::LinearMipmapNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
         case TexMin::LinearMipmapLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
         default: minMode = GL_NEAREST; break;
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode );
   }

   static void applyMagnify( TexMag texMag )
   {
      GLint magMode = GL_NEAREST; // Low quality is default.
      if ( texMag == TexMag::Linear ) { magMode = GL_LINEAR; }// High quality on demand.
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode );
   }

   static void applyWrapS( TexWrap texWrap )
   {
      GLint mode = GL_REPEAT;
      switch ( texWrap )
      {
         case TexWrap::RepeatMirrored: mode = GL_MIRRORED_REPEAT; break;
         case TexWrap::ClampToEdge: mode = GL_CLAMP_TO_EDGE; break;
         case TexWrap::ClampToBorder: mode = GL_CLAMP_TO_BORDER; break;
         default: break;
      }
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode );
   }

   static void applyWrapT( TexWrap texWrap )
   {
      GLint mode = GL_REPEAT;
      switch ( texWrap )
      {
         case TexWrap::RepeatMirrored: mode = GL_MIRRORED_REPEAT; break;
         case TexWrap::ClampToEdge: mode = GL_CLAMP_TO_EDGE; break;
         case TexWrap::ClampToBorder: mode = GL_CLAMP_TO_BORDER; break;
         default: break;
      }
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode );
   }

   static void applyWrapR( TexWrap texWrap )
   {
      GLint mode = GL_REPEAT;
      switch ( texWrap )
      {
         case TexWrap::RepeatMirrored: mode = GL_MIRRORED_REPEAT; break;
         case TexWrap::ClampToEdge: mode = GL_CLAMP_TO_EDGE; break;
         case TexWrap::ClampToBorder: mode = GL_CLAMP_TO_BORDER; break;
         default: break;
      }
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, mode );
   }

   static float queryAF()
   {
      // GL_EXT_texture_filter_anisotropic
      GLfloat maxAF = 0.0f;
      glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
      return maxAF;
   }

   static void applyAF( float af )
   {
      // [Anisotropic Filtering] AF | needs GL_EXT_texture_filter_anisotropic ( >= 3.0 ) so its there.
      if ( af > 0 )
      {
         float maxAF = queryAF();
         glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );
      }
   }
};

*/
