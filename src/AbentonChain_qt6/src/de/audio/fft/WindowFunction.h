#pragma once
#include <de/de_aligned_memory.h>

namespace de {
namespace audio {
	
// Fenster        Leakage   Freq.-auflösung Ampl.-genau Typische Anwendung
// Rechteck       schlecht	sehr gut        schlecht    periodische Signale
// Hann           gut       gut             gut         Standard‑FFT
// Hamming        mittel	gut             besser      Audio, Vibrationsanalyse
// Blackman       sehr gut	mittel          gut         Spektren mit hohem Dynamikbereich
// Blackman‑Harris exzellent	schlecht        gut         Audio, EMV
// Flat‑Top        schlecht	sehr schlecht   beste       Pegelmessungen
// Kaiser         variabel	variabel        variabel    DSP‑Design
// Bartlett       mittel	mittel          mittel      einfache Analysen
// Tukey          variabel	gut             gut         gemischte Signale



struct WindowFunction
{
    enum eFunc
    {
        Rect = 0,
        Hann,
        Hamming,
        Blackman,
        eFuncMax
    };

    static std::string getString(eFunc func)
    {
        switch (func)
        {
            case Rect: return "Rect";
            case Hann: return "Hann";
            case Hamming: return "Hamming";
            case Blackman: return "Blackman";
            default: return "Unknown";
        }
    }

    eFunc m_func = Rect;
    AlignedFloatVector m_lut; // LookupTable

    void setFunction(eFunc func);

    eFunc function() const { return m_func; }

    void resize(uint32_t n);
	
    void apply(const AlignedFloatVector& v_in, AlignedFloatVector& v_out);
	
    void apply(const float* __restrict__ pIn, uint32_t nIn,
                     float* __restrict__ pOut, uint32_t nOut);

protected:
    void updateCoefficients();
};

} // end namespace audio.
} // end namespace de.

