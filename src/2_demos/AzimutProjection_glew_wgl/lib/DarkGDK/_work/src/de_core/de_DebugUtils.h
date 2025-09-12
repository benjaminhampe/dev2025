//! author: BenjaminHampe@gmx.de
#pragma once
#include <de_core/de_ForceInline.h>
#include <de_core/de_RuntimeError.h>
#include <de_core/de_Logger.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

struct DebugUtils
{
   DE_CREATE_LOGGER("DebugUtils")

   static void
   debug_memcpy(
      uint8_t* dst,
      uint8_t const* src,
      uint64_t const bytesToCopy,
      uint8_t const* const src_beg, uint8_t const* const src_end,
      uint8_t const* const dst_beg, uint8_t const* const dst_end )
   {
      if ( !dst ) { DE_ERROR("1. !dst") return; }
      if ( !src ) { DE_ERROR("2. !src") return; }
      if ( bytesToCopy < 1 ) { DE_ERROR("3. bytesToCopy < 1") return; }
      if ( !src_beg ) { DE_ERROR("4. !src_beg") return; }
      if ( !src_end ) { DE_ERROR("5. !src_end") return; }
      if ( !dst_beg ) { DE_ERROR("6. !dst_beg") return; }
      if ( !dst_end ) { DE_ERROR("7. !dst_end") return; }
      if ( src_beg > src_end ) { DE_ERROR("8. src_beg > src_end") return; }
      if ( dst_beg > dst_end ) { DE_ERROR("9. dst_beg > dst_end") return; }
      if ( src < src_beg ) { DE_ERROR("10. src < src_beg") return; }
      if ( src >= src_end ) { DE_ERROR("11. src >= src_end") return; }
      if ( dst < dst_beg ) { DE_ERROR("12. dst < dst_beg") return; }
      if ( dst >= dst_end ) { DE_ERROR("13. dst >= dst_end") return; }
      if ( src_beg + bytesToCopy > src_end )
      {
         DE_ERROR("14. src_beg(",(void*)src_beg,") + "
                  "bytesToCopy(",bytesToCopy,") >= "
                  "src_end(",(void*)src_end,"), "
                  "diff(",int64_t(src_end - src_beg),")") return;
      }
      if ( dst_beg + bytesToCopy > dst_end ) { DE_ERROR("15. dst_beg + bytesToCopy >= dst_end") return; }

      // ::memcpy( dst, src, bytesToCopy );
   }

};
