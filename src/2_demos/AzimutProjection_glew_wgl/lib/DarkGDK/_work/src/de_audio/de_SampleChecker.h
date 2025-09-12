#pragma once
#include <cstdint>
#include <vector>
#include <cmath>
#include <functional>
#include <de_core/de_Logger.h>
//#include <de_core/de_RuntimeError.h>

namespace de {

/////////////////////////////////////////////////////////////////////////////
struct SampleChecker
/////////////////////////////////////////////////////////////////////////////
{
   DE_CREATE_LOGGER("de.SampleChecker")

   static void check( float const* p, uint32_t n, std::string const & check_name = "" )
   {
      for ( uint_fast32_t i = 0; i < n; ++i )
      {
         float sample = *p; p++;
         if ( std::isnan(sample) || std::isinf(sample) || std::abs(sample) > 10.0f  )
         {
            DE_ERROR( check_name,"[",i,"] Invalid sample = ",sample)
         }
      }
   }
};

} // end namespace de
