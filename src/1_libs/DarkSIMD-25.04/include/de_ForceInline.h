#pragma once

#if defined(_MSC_VER)
    #ifndef DE_FORCEINLINE
    #define DE_FORCEINLINE __forceinline
    #endif
#else
    #ifndef DE_FORCEINLINE
    #define DE_FORCEINLINE __attribute__((always_inline)) inline
    #endif
#endif