#pragma once
#include <de_core/de_StringUtil.h>

namespace de {

// ===========================================================================
struct TexOptions
// ===========================================================================
{
   DE_CREATE_LOGGER("SamplerOptions")

   enum class Minify
   {
      Nearest = 0, // = GL_NEAREST,
      Linear, // GL_LINEAR,
      NearestMipmapNearest, //  = GL_NEAREST_MIPMAP_NEAREST,
      NearestMipmapLinear, //  = GL_NEAREST_MIPMAP_LINEAR,
      LinearMipmapNearest, //  = GL_LINEAR_MIPMAP_NEAREST,
      LinearMipmapLinear, //  = GL_LINEAR_MIPMAP_LINEAR,
      MaxCount,
      Default = Minify::Linear
   };

   enum class Magnify
   {
      Nearest = 0, // = GL_NEAREST,
      Linear, // = GL_LINEAR,
      MaxCount,
      Default = Magnify::Linear
   };

   enum class Wrap
   {
      Repeat = 0, //  = GL_REPEAT,
      RepeatMirrored, //  = GL_MIRRORED_REPEAT
      ClampToEdge, //  = GL_CLAMP_TO_EDGE,
      ClampToBorder, //  = GL_CLAMP_TO_BORDER,
      MaxCount,
      Default = Wrap::Repeat
   };

   static std::string toStr( Minify const texMin )
   {
      switch( texMin )
      {
         case Minify::Nearest: return "Nearest";
         case Minify::Linear: return "Linear";
         case Minify::NearestMipmapNearest: return "NearestMipmapNearest";
         case Minify::NearestMipmapLinear: return "NearestMipmapLinear";
         case Minify::LinearMipmapNearest: return "LinearMipmapNearest";
         case Minify::LinearMipmapLinear: return "LinearMipmapLinear";
         default: return "Invalid";
      }
   }

   static std::string toStr( Magnify const texMag )
   {
      switch( texMag )
      {
         case Magnify::Nearest: return "Nearest";
         case Magnify::Linear: return "Linear";
         default: return "Invalid";
      }
   }

   static std::string toStr( Wrap const texWrap )
   {
      switch( texWrap )
      {
         case Wrap::Repeat: return "Repeat";
         case Wrap::RepeatMirrored: return "RepeatMirrored";
         case Wrap::ClampToEdge: return "ClampToEdge";
         case Wrap::ClampToBorder: return "ClampToBorder";
         default: return "Invalid";
      }
   }

   std::string toString() const
   {
      std::ostringstream o;
      if ( anisotropicFilter > 0.5f )                 o << "AF";
      else                                            o << "0";
      o << ",";
           if ( min == Minify::Linear )               o << "L";
      else if ( min == Minify::NearestMipmapNearest ) o << "NMN";
      else if ( min == Minify::NearestMipmapNearest ) o << "NML";
      else if ( min == Minify::LinearMipmapNearest )  o << "LMN";
      else if ( min == Minify::LinearMipmapLinear )   o << "LML";
      else                                            o << "N";
      o << ",";
           if ( mag == Magnify::Linear )               o << "L";
      else                                            o << "N";
      o << ",";
           if ( wrapS == Wrap::RepeatMirrored ) o << "RM";
      else if ( wrapS == Wrap::ClampToEdge )    o << "CTE";
      else if ( wrapS == Wrap::ClampToBorder )  o << "CTB";
      else                                         o << "R";
      o << ",";
           if ( wrapS == Wrap::RepeatMirrored ) o << "RM";
      else if ( wrapS == Wrap::ClampToEdge )    o << "CTE";
      else if ( wrapS == Wrap::ClampToBorder )  o << "CTB";
      else                                         o << "R";
      return o.str();
   }

   static TexOptions
   parseString( std::string csv )
   {
      TexOptions so;
      so.anisotropicFilter = 0.0f;
      so.min = Minify::Nearest;
      so.mag = Magnify::Nearest;
      so.wrapS = Wrap::ClampToEdge;
      so.wrapT = Wrap::ClampToEdge;
      so.wrapR = Wrap::ClampToEdge;

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
               so.min = Minify::Linear;
            }
            else if ( word == "NMN" )
            {
               so.min = Minify::NearestMipmapNearest;
            }
            else if ( word == "NML" )
            {
               so.min = Minify::NearestMipmapLinear;
            }
            else if ( word == "LMN" )
            {
               so.min = Minify::LinearMipmapNearest;
            }
            else if ( word == "LML" )
            {
               so.min = Minify::LinearMipmapLinear;
            }
         }
      }
      // Parse Magnify filter Nearest|Linear
      if ( words.size() > 2 )
      {
         std::string const & word = words[2];
         if ( word == "L" )
         {
            so.mag = Magnify::Linear;
         }
      }
      // Parse WrapS
      if ( words.size() > 3 )
      {
         std::string const & word = words[3];
         if ( word == "R" )
         {
            so.wrapS = Wrap::Repeat;
         }
         else if ( word == "RM" )
         {
            so.wrapS = Wrap::RepeatMirrored;
         }
         else if ( word == "CTE" )
         {
            so.wrapS = Wrap::ClampToEdge;
         }
         else if ( word == "CTB" )
         {
            so.wrapS = Wrap::ClampToBorder;
         }
      }
      // Parse WrapT
      if ( words.size() > 4 )
      {
         std::string const & word = words[4];
         if ( word == "R" )
         {
            so.wrapT = Wrap::Repeat;
         }
         else if ( word == "RM" )
         {
            so.wrapT = Wrap::RepeatMirrored;
         }
         else if ( word == "CTE" )
         {
            so.wrapT = Wrap::ClampToEdge;
         }
         else if ( word == "CTB" )
         {
            so.wrapT = Wrap::ClampToBorder;
         }
      }

      return so;
   }

   Minify min;
   Magnify mag;
   Wrap wrapS;
   Wrap wrapT;
   Wrap wrapR; // 3D
   float   anisotropicFilter; // anisotropicFilter

   TexOptions()
      : min(Minify::Default)
      , mag(Magnify::Default)
      , wrapS(Wrap::Default)
      , wrapT(Wrap::Default)
      , wrapR(Wrap::Default)
      , anisotropicFilter(1.0f) // enabled, auto increased to max level.
   {

   }

   // >= 1 means, enabled, will auto increase level to max supported 16x.
   TexOptions( float anisotropicFilterLevel,
       Minify minify = Minify::Default,
       Magnify magnify = Magnify::Default,
       Wrap wrapmodeS = Wrap::Default,
       Wrap wrapmodeT = Wrap::Default,
       Wrap wrapmodeR = Wrap::Default )
      : min(minify)
      , mag(magnify)
      , wrapS(wrapmodeS)
      , wrapT(wrapmodeT)
      , wrapR(wrapmodeR)
      , anisotropicFilter(anisotropicFilterLevel)
   {

   }

   void setMin( Minify minify ) { min = minify; }
   void setMag( Magnify magnify ) { mag = magnify; }
   void setWrapS( Wrap wrap ) { wrapS = wrap; }
   void setWrapT( Wrap wrap ) { wrapT = wrap; }
   void setWrapR( Wrap wrap ) { wrapR = wrap; }
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
      if ( (min == Minify::NearestMipmapNearest)
        || (min == Minify::NearestMipmapLinear)
        || (min == Minify::LinearMipmapNearest)
        || (min == Minify::LinearMipmapLinear) )
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

struct TexOptions
{
   DE_CREATE_LOGGER("SamplerOptions")

   enum class Minify
   {
      Nearest = 0, // = GL_NEAREST,
      Linear, // GL_LINEAR,
      NearestMipmapNearest, //  = GL_NEAREST_MIPMAP_NEAREST,
      NearestMipmapLinear, //  = GL_NEAREST_MIPMAP_LINEAR,
      LinearMipmapNearest, //  = GL_LINEAR_MIPMAP_NEAREST,
      LinearMipmapLinear, //  = GL_LINEAR_MIPMAP_LINEAR,
      MaxCount,
      Default = Minify::Linear
   };

   enum class Magnify
   {
      Nearest = 0, // = GL_NEAREST,
      Linear, // = GL_LINEAR,
      MaxCount,
      Default = Magnify::Linear
   };

   enum class Wrap
   {
      Repeat = 0, //  = GL_REPEAT,
      RepeatMirrored, //  = GL_MIRRORED_REPEAT
      ClampToEdge, //  = GL_CLAMP_TO_EDGE,
      ClampToBorder, //  = GL_CLAMP_TO_BORDER,
      MaxCount,
      Default = Wrap::Repeat
   };

   static std::string toStr( Minify const texMin )
   {
      switch( texMin )
      {
         case Minify::Nearest: return "Nearest";
         case Minify::Linear: return "Linear";
         case Minify::NearestMipmapNearest: return "NearestMipmapNearest";
         case Minify::NearestMipmapLinear: return "NearestMipmapLinear";
         case Minify::LinearMipmapNearest: return "LinearMipmapNearest";
         case Minify::LinearMipmapLinear: return "LinearMipmapLinear";
         default: return "Invalid";
      }
   }

   static std::string toStr( Magnify const texMag )
   {
      switch( texMag )
      {
         case Magnify::Nearest: return "Nearest";
         case Magnify::Linear: return "Linear";
         default: return "Invalid";
      }
   }

   static std::string toStr( Wrap const texWrap )
   {
      switch( texWrap )
      {
         case Wrap::Repeat: return "Repeat";
         case Wrap::RepeatMirrored: return "RepeatMirrored";
         case Wrap::ClampToEdge: return "ClampToEdge";
         case Wrap::ClampToBorder: return "ClampToBorder";
         default: return "Invalid";
      }
   }

   Minify min;
   Magnify mag;
   Wrap wrapS;
   Wrap wrapT;
   Wrap wrapR; // 3D
   float   anisotropicFilter; // anisotropicFilter

   TexOptions()
      : min(Minify::Default)
      , mag(Magnify::Default)
      , wrapS(Wrap::Default)
      , wrapT(Wrap::Default)
      , wrapR(Wrap::Default)
      , anisotropicFilter(1.0f) // enabled, auto increased to max level.
   {

   }

   // >= 1 means, enabled, will auto increase level to max supported 16x.
   TexOptions( float anisotropicFilterLevel,
       Minify minify = Minify::Default,
       Magnify magnify = Magnify::Default,
       Wrap wrapmodeS = Wrap::Default,
       Wrap wrapmodeT = Wrap::Default,
       Wrap wrapmodeR = Wrap::Default )
      : min(minify)
      , mag(magnify)
      , wrapS(wrapmodeS)
      , wrapT(wrapmodeT)
      , wrapR(wrapmodeR)
      , anisotropicFilter(anisotropicFilterLevel)
   {

   }

   void setMin( Minify minify ) { min = minify; }
   void setMag( Magnify magnify ) { mag = magnify; }
   void setWrapS( Wrap wrap ) { wrapS = wrap; }
   void setWrapT( Wrap wrap ) { wrapT = wrap; }
   void setWrapR( Wrap wrap ) { wrapR = wrap; }
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
      if ( (min == Minify::NearestMipmapNearest)
        || (min == Minify::NearestMipmapLinear)
        || (min == Minify::LinearMipmapNearest)
        || (min == Minify::LinearMipmapLinear) )
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

   static Minify queryMinify()
   {
      GLint val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &val );
      switch ( val )
      {
         case GL_NEAREST: return Minify::Nearest;
         case GL_LINEAR: return Minify::Linear;
         case GL_NEAREST_MIPMAP_NEAREST: return Minify::NearestMipmapNearest;
         case GL_NEAREST_MIPMAP_LINEAR: return Minify::NearestMipmapLinear;
         case GL_LINEAR_MIPMAP_NEAREST: return Minify::LinearMipmapNearest;
         case GL_LINEAR_MIPMAP_LINEAR: return Minify::LinearMipmapLinear;
         default: return Minify::Default;
      }
   }

   static Magnify queryMagnify()
   {
      GLint val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &val );
      switch ( val )
      {
         case GL_NEAREST: return Magnify::Nearest;
         case GL_LINEAR: return Magnify::Linear;
         default: return Magnify::Default;
      }
   }

   static Wrap queryWrapS()
   {
      GLint val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &val );
      switch ( val )
      {
         case GL_REPEAT: return Wrap::Repeat;
         case GL_MIRRORED_REPEAT: return Wrap::RepeatMirrored;
         case GL_CLAMP_TO_EDGE: return Wrap::ClampToEdge;
         case GL_CLAMP_TO_BORDER: return Wrap::ClampToBorder;
         default: return Wrap::Default;
      }
   }

   static Wrap queryWrapT()
   {
      GLint val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &val );
      switch ( val )
      {
         case GL_REPEAT: return Wrap::Repeat;
         case GL_MIRRORED_REPEAT: return Wrap::RepeatMirrored;
         case GL_CLAMP_TO_EDGE: return Wrap::ClampToEdge;
         case GL_CLAMP_TO_BORDER: return Wrap::ClampToBorder;
         default: return Wrap::Default;
      }
   }

   static Wrap queryWrapR()
   {
      GLint val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &val );
      switch ( val )
      {
         case GL_REPEAT: return Wrap::Repeat;
         case GL_MIRRORED_REPEAT: return Wrap::RepeatMirrored;
         case GL_CLAMP_TO_EDGE: return Wrap::ClampToEdge;
         case GL_CLAMP_TO_BORDER: return Wrap::ClampToBorder;
         default: return Wrap::Default;
      }
   }

   static void applyMinify( Minify texMin )
   {
      GLint minMode = GL_NEAREST;// Low quality is default.
      switch ( texMin )
      {
         //case Minify::Nearest: minMode = GL_NEAREST; break;
         case Minify::Linear: minMode = GL_LINEAR; break;
         case Minify::NearestMipmapNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
         case Minify::NearestMipmapLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
         case Minify::LinearMipmapNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
         case Minify::LinearMipmapLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
         default: minMode = GL_NEAREST; break;
      }
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode );
   }

   static void applyMagnify( Magnify texMag )
   {
      GLint magMode = GL_NEAREST; // Low quality is default.
      if ( texMag == Magnify::Linear ) { magMode = GL_LINEAR; }// High quality on demand.
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode );
   }

   static void applyWrapS( Wrap texWrap )
   {
      GLint mode = GL_REPEAT;
      switch ( texWrap )
      {
         case Wrap::RepeatMirrored: mode = GL_MIRRORED_REPEAT; break;
         case Wrap::ClampToEdge: mode = GL_CLAMP_TO_EDGE; break;
         case Wrap::ClampToBorder: mode = GL_CLAMP_TO_BORDER; break;
         default: break;
      }
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode );
   }

   static void applyWrapT( Wrap texWrap )
   {
      GLint mode = GL_REPEAT;
      switch ( texWrap )
      {
         case Wrap::RepeatMirrored: mode = GL_MIRRORED_REPEAT; break;
         case Wrap::ClampToEdge: mode = GL_CLAMP_TO_EDGE; break;
         case Wrap::ClampToBorder: mode = GL_CLAMP_TO_BORDER; break;
         default: break;
      }
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode );
   }

   static void applyWrapR( Wrap texWrap )
   {
      GLint mode = GL_REPEAT;
      switch ( texWrap )
      {
         case Wrap::RepeatMirrored: mode = GL_MIRRORED_REPEAT; break;
         case Wrap::ClampToEdge: mode = GL_CLAMP_TO_EDGE; break;
         case Wrap::ClampToBorder: mode = GL_CLAMP_TO_BORDER; break;
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
