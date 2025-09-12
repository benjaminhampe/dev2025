#pragma once
#include <cstdint>
#include <sstream>

namespace de {

// ===========================================================================
struct TexType
// ===========================================================================
{
   enum EType : uint8_t
   {
      Unknown = 0,
      Tex2D,
      TexCube,
      Tex2DArray,
      Tex3D
   };

   static std::string
   getString( TexType const texType )
   {
      switch ( texType )
      {
         case Tex2D: return "Tex2D";
         case TexCube: return "TexCube";
         case Tex2DArray: return "Tex2DArray";
         case Tex3D: return "Tex3D";
         default: return "Unknown";
      }
   }

   EType m_type;
   TexType() : m_type(Unknown) {}
   TexType( EType type ) : m_type(type) {}
   TexType( TexType const & other ) : m_type(other.m_type) {}
   operator uint8_t() const { return m_type; }
   std::string toString() const { return getString( *this ); }
};

} // end namespace de
