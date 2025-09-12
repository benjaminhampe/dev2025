#pragma once
#include <cstdint>
#include <sstream>

namespace de {
namespace midi {
namespace file {

// =======================================================================
template< typename T > struct Range
// =======================================================================
{
   T min = std::numeric_limits< T >::max();
   T max = std::numeric_limits< T >::lowest();

   T getRange() const { return max - min; }

   std::string
   str() const
   {
      std::ostringstream o;
      o << min << "," << max;
      return o.str();
   }
};

} // end namespace file
} // end namespace midi
} // end namespace de