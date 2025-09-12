#pragma once
#include <cstdint>
#include <sstream>

namespace de {

// ===========================================================================
// ===   ImageLoadOptions
// ===========================================================================
struct ImageLoadOptions
{
   uint32_t m_SearchColor;
   uint32_t m_ReplaceColor;

   bool m_ConvertToGrey121;
   bool m_ConvertToGrey111;
   bool m_AutoSaturate;

   float m_Brighten;
   float m_Gamma;
   float m_Contrast;

   std::string m_DefaultExportExt;

   ImageLoadOptions()
      : m_SearchColor( 0 )
      , m_ReplaceColor( 0 )
      , m_ConvertToGrey121( false )
      , m_ConvertToGrey111( false )
      , m_AutoSaturate( false )
      , m_Brighten( 1.0f )
      , m_Gamma( 1.0f )
      , m_Contrast( 1.0f )
      , m_DefaultExportExt( "raw" )
   {}
};

} // end namespace de.

