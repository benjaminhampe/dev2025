#pragma once

// STL headers
#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// GLM headers
#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#ifndef GLM_FORCE_AVX2
#define GLM_FORCE_AVX2
#endif

#ifndef GLM_FORCE_INLINE
#define GLM_FORCE_INLINE
#endif

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <glm/gtc/matrix_transform.hpp> // ortho, lookAt
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

// OpenGL headers
#include <glad/glad.h>   // my DesktopOpenGL version supports up to 4.2, d.k. if enough for compute shader.
#include <GLFW/glfw3.h>  // uses DesktopOpenGL glad itself

// QuickDev headers
#include <learnopengl/irrstyle_types.h>

namespace learnopengl {

struct SO
{
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


   static bool hasMipmaps( TexMin const filter )
   {
      if ( (filter == TexMin::NearestMipmapNearest)
        || (filter == TexMin::NearestMipmapLinear)
        || (filter == TexMin::LinearMipmapNearest)
        || (filter == TexMin::LinearMipmapLinear) )
      {
         return true;
      }
      else
      {
         return false;
      }
   }

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


   static GLint toGL( TexMag const filter )
   {
      GLint mode = GL_NEAREST; // Low quality is default.
      if ( filter == TexMag::Linear ) { mode = GL_LINEAR; }// High quality on demand.
      return mode;
   }

   static GLint toGL( TexMin const filter )
   {
      switch ( filter )
      {
         case TexMin::Nearest: return GL_NEAREST;
         case TexMin::Linear: return GL_LINEAR;
         case TexMin::NearestMipmapNearest: return GL_NEAREST_MIPMAP_NEAREST;
         case TexMin::NearestMipmapLinear: return GL_NEAREST_MIPMAP_LINEAR;
         case TexMin::LinearMipmapNearest: return GL_LINEAR_MIPMAP_NEAREST;
         case TexMin::LinearMipmapLinear: return GL_LINEAR_MIPMAP_LINEAR;
         default: return GL_NEAREST;
      }
   }

   static GLint toGL( TexWrap const wrapMode )
   {
      switch ( wrapMode )
      {
         case TexWrap::Repeat: return GL_REPEAT;
         case TexWrap::RepeatMirrored: return GL_MIRRORED_REPEAT;
         case TexWrap::ClampToEdge: return GL_CLAMP_TO_EDGE;
         case TexWrap::ClampToBorder: return GL_CLAMP_TO_BORDER;
         default: return GL_REPEAT;
      }
   }

   static TexMin fromGLMinFilter( GLint mode )
   {
      switch ( mode )
      {
         case GL_NEAREST: return TexMin::Nearest;
         case GL_LINEAR: return TexMin::Linear;
         case GL_NEAREST_MIPMAP_NEAREST: return TexMin::NearestMipmapNearest;
         case GL_NEAREST_MIPMAP_LINEAR: return TexMin::NearestMipmapLinear;
         case GL_LINEAR_MIPMAP_NEAREST: return TexMin::LinearMipmapNearest;
         case GL_LINEAR_MIPMAP_LINEAR: return TexMin::LinearMipmapLinear;
         default: return TexMin::Nearest;
      }
   }

   static TexMag fromGLMagFilter( GLint mode )
   {
      switch ( mode )
      {
         case GL_NEAREST: return TexMag::Nearest;
         case GL_LINEAR: return TexMag::Linear;
         default: return TexMag::Nearest;
      }
   }

   static TexWrap fromGLWrapMode( GLint mode )
   {
      switch ( mode )
      {
         case GL_REPEAT: return TexWrap::Repeat;
         case GL_MIRRORED_REPEAT: return TexWrap::RepeatMirrored;
         case GL_CLAMP_TO_EDGE: return TexWrap::ClampToEdge;
         case GL_CLAMP_TO_BORDER: return TexWrap::ClampToBorder;
         default: return TexWrap::Repeat;
      }
   }

   TexMin min;
   TexMag mag;
   TexWrap wrapS;
   TexWrap wrapT;
   TexWrap wrapR; // 3D
   float anisotropicFilter; // anisotropicFilter

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

   bool hasMipmaps() const { return hasMipmaps(min); }

   void query()
   {
      GLint val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &val );
      min = fromGLMinFilter( val );
      val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &val );
      mag = fromGLMagFilter( val );
      val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &val );
      wrapS = fromGLWrapMode( val );
      val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &val );
      wrapT = fromGLWrapMode( val );
      val = 0;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &val );
      wrapR = fromGLWrapMode( val );

      GLfloat maxAF = 0.0f;
      glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
      anisotropicFilter = maxAF;
   }

   void apply() const
   {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGL(min) );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGL(mag) );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGL(wrapS) );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGL(wrapT) );
      //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, toGL(wrapR) );
      if ( hasMipmaps() )
      {
         glGenerateMipmap( GL_TEXTURE_2D );
      }

      // [Anisotropic Filtering] AF | needs GL_EXT_texture_filter_anisotropic ( >= 3.0 ) so its there.
      if ( anisotropicFilter > 0.5f )
      {
         GLfloat maxAF = 0.0f;
         glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );
      }
   }
};


} // end namespace learnopengl.
