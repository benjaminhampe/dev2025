#pragma once
#include <cstdint>
#include <sstream>

namespace de {
namespace midi {
namespace file {

// =======================================================================
struct KeySignatureEvent
// =======================================================================
{
   uint64_t m_tick;
   int m_cdur_offset;
   int m_minor;

   KeySignatureEvent()
      : m_tick(0)
      , m_cdur_offset(-127)
      , m_minor(-127)
   {}

   bool
   isValid() const
   {
      if (m_cdur_offset < -7) return false;
      if (m_cdur_offset >  7) return false;
      if (m_minor < -2) return false;
      if (m_minor >  2) return false;
      return true;
   }

   std::string
   toString() const
   {
      std::ostringstream o;
      o << "tick(" << m_tick << ")"
         ", C-Dur-offset(" << m_cdur_offset << ")"
         ", minor(" << m_minor << ")";
      return o.str();
   }

   size_t computeMemoryConsumption() const
   {
      size_t n = sizeof(*this);
      return n;
   }
};

} // end namespace file
} // end namespace midi
} // end namespace de

