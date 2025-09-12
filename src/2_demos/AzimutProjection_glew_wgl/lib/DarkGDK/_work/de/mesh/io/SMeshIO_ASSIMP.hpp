#pragma once
#include "SMeshIO.hpp"

namespace de {
namespace gpu {

struct VideoDriver;

// ===========================================================================
struct SMeshLoadASSIMP
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.SMeshLoadASSIMP")

   static bool
   load( SMesh & out, 
         std::string const & uri, 
         VideoDriver* driver, 
         SMeshOptions const & options = SMeshOptions() );
};

} // end namespace gpu.
} // end namespace de.

