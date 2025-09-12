#pragma once
#include <learnopengl/so.h>

namespace learnopengl {

//float getMaxAnisotropicFilter();

u32 loadTexture( std::string const & uri, SO const & so = SO(), bool gammaCorrection = false );

/*
struct TexFormat
{
   enum EFormat : uint8_t
   {
      R8 = 0,
      R8G8B8,
      R8G8B8A8,
      R16,
      R32F,
      D24S8,
      D32F,
      D32FS8,
      INVALID
   };

   static std::string getString( EFormat const texFormat )
   {
      switch ( texFormat )
      {
         case R8 : return "R8";
         case R8G8B8 : return "R8G8B8";
         case R8G8B8A8 : return "R8G8B8A8";
         case R16 : return "R16";
         case R32F : return "R32F";
         case D24S8 : return "D24S8";
         case D32F : return "D32F";
         case D32FS8 : return "D32FS8";
         default: return "INVALID";
      }
   }

   EFormat m_format;
   TexFormat() : m_format(INVALID) {}
   TexFormat( EFormat format ) : m_format(format) {}
   TexFormat( TexFormat const & other ) : m_format(other.m_format) {}
   operator EFormat() const { return m_format; }
};

struct TexSemantic
{
   enum ESemantic : uint8_t
   {
      DiffuseMap,
      NormalMap,
      BumpMap,
      CombiNormalBumpMap,
      DetailMap,
      SpecularMap,
      INVALID
   };

   static std::string getString( ESemantic const texSemantic )
   {
      switch ( texSemantic )
      {
         case DiffuseMap : return "DiffuseMap";
         case NormalMap : return "NormalMap";
         case BumpMap : return "BumpMap";
         case CombiNormalBumpMap : return "CombiNormalBumpMap";
         case DetailMap : return "DetailMap";
         case SpecularMap : return "SpecularMap";
         default: return "INVALID";
      }
   }
   ESemantic m_semantic;
   TexSemantic() : m_semantic(INVALID) {}
   TexSemantic( ESemantic semantic ) : m_semantic(semantic) {}
   TexSemantic( TexSemantic const & other ) : m_semantic(other.m_semantic) {}
   operator ESemantic() const { return m_semantic; }
};

*/

} // end namespace learnopengl.
