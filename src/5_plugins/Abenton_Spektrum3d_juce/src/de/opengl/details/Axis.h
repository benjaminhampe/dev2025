#pragma once
#include <DarkImage.h>

// ==========================================================
struct Axis
// ==========================================================
{
    uint32_t m_sampleRate;
    uint32_t m_fftSize;
    uint32_t m_scaleMode; // 0=linear, 1=logarithm
    float m_scaleFactor;
    float m_fMin;
    float m_fMax;

    Axis()
        : m_sampleRate{ 48000 }
        , m_fftSize{ 2048 }
        , m_scaleMode{ 1 } // 0=linear, 1=logarithm
        , m_scaleFactor{ 1.0f }
        , m_fMin{ 0 }
        , m_fMax{ 256 }
    {

    }

    double x( double freq )
    {
        if ( m_scaleMode == 1 )
            return de::freq2log(freq, m_fMin, m_fMax, m_sampleRate );
        else
            return de::freq2lin(freq, m_fMin, m_fMax, m_sampleRate );
    }
/*
    // / (sampleRate_over_fftSize * colCount);
    const float sampleRate = 48000.0f;
    const float fftSize = cols;

    const float sampleRate_over_fftSize = sampleRate / fftSize;
    const float f = sampleRate_over_fftSize; //  / log10f( float(cols) );

    for ( size_t col = 0; col < cols; col++ )
    {
    //  - 1.5f -1 = shift by 10^-1
        m_matrix_fft_xmap[ col ] = f * log10f( float(col+1) );
    }
*/
};