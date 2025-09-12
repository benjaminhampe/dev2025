#ifndef DE_OS_RAM_HPP
#define DE_OS_RAM_HPP

#include <cstdint>
#include <sstream>

namespace de
{

// ----------------------------------------------------------------
// Define: RAMInfo
// ----------------------------------------------------------------

struct RAMInfo
{
   uint64_t totalBytes;
   uint64_t availBytes;
   uint64_t usedBytes;

   RAMInfo() : totalBytes( 0 ), availBytes( 0 ), usedBytes( 0 )
   {}


   std::string toString() const {
      std::stringstream s;
      s << "totalMB = " << (totalBytes >> 20) << ", ";
      s << "availMB = " << (availBytes >> 20) << ", ";
      s << "usedMB = " << (usedBytes >> 20);
      return s.str();
   }
};

} // end namespace de

// ----------------------------------------------------------------
// Get/Query: RAMInfo
// ----------------------------------------------------------------
de::RAMInfo dbRAMInfo();




#endif
