#ifndef DE_OS_RAM_HPP
#define DE_OS_RAM_HPP

#include <cstdint>
#include <sstream>

namespace de
{

// ----------------------------------------------------------------
// Define: RAMInfo
// ----------------------------------------------------------------

struct tRAM
{
    uint64_t bytesTotal;
    uint64_t bytesFree;
    uint64_t bytesUsed;
    tRAM() : bytesTotal( 0 ), bytesFree( 0 ), bytesUsed( 0 ) {}

    double totalMB() const { return double(bytesTotal)/double(1024*1024); }
    double availMB() const { return double(bytesFree)/double(1024*1024); }
    double usedMB() const { return double(bytesUsed)/double(1024*1024); }
    
    std::string 
    toString() const 
    {
        std::ostringstream o; o <<
        "RAM_avail = " << availMB() << ", "
        "RAM_used = " << usedMB() << ", "
        "RAM_total = " << totalMB();
        return s.str();
    }
};

} // end namespace de

// ----------------------------------------------------------------
// Get/Query: RAMInfo
// ----------------------------------------------------------------
de::RAMInfo dbRAMInfo();




#endif
