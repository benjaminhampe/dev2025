//! author: BenjaminHampe@gmx.de
#pragma once
#include <de_core/de_ForceInline.h>
#include <cstdint>
#include <cstdlib>

DE_FORCE_INLINE void* de_aligned_malloc( size_t n, size_t alignBytes )
{
   return _aligned_malloc( n, alignBytes );
}

DE_FORCE_INLINE void de_aligned_free( void* ptr )
{
   if (!ptr) return;
   _aligned_free( ptr );
}

DE_FORCE_INLINE void* de_aligned_realloc( void* ptr, size_t n, size_t alignBytes )
{
   return _aligned_realloc( ptr, n, alignBytes );
}
