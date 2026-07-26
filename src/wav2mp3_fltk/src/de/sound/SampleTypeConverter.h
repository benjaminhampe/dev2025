#pragma once
#include <de/sound/SampleType.h>

namespace de {

// ===================================================================
struct SampleTypeConverter
// ===================================================================
{
    /// @brief Converter functor
    /// @param src Source sample buffer
    /// @param dst Destination sample buffer
    /// @param nSamples Number of samples.
    typedef void (*Converter_t)( const void* __restrict__ /* src */, void* __restrict__ /* dst */, int64_t /* n-samples */ );

    /// @brief Get sampleType converter
    static Converter_t getConverter( const SampleType& src, const SampleType& dst );

    // Copy converters: Dummies to use API even when formats are same.
    static void convert_U8_to_U8( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );
    static void convert_S8_to_S8( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );
    static void convert_S16_to_S16( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );
    static void convert_S24_to_S24( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );
    static void convert_S32_to_S32( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );
    static void convert_F32_to_F32( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );
    static void convert_F64_to_F64( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );

    // Real converters:
    static void convert_F32_to_F64( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );
    static void convert_F64_to_F32( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );

    static void convert_F32_to_S16( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );
    static void convert_S16_to_F32( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );

    static void convert_F32_to_S24( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );
    static void convert_S24_to_F32( const void* __restrict__ src, void* __restrict__ dst, int64_t nSamples );

};

} // end namespace de.
