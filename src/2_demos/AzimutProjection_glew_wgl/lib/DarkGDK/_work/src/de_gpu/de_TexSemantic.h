#pragma once
#include <cstdint>
#include <sstream>

namespace de {

// ============================================================================
struct TexSemantic
// ============================================================================
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

} // end namespace GL.
