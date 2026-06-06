#pragma once
#include <DarkImage.h>

namespace de {
namespace audio {

// Kaiser window β (beta)

// Rule of thumb: pick β from desired sidelobe attenuation A (in dB):
// β={0,A<210.5842(A−21)0.4+0.07886(A−21),21≤A≤500.1102(A−8.7),A>50

// Typical choices:
// Use case	Target attenuation A	Approx. β
// Very rough, high resolution	20–30 dB	2–4
// General spectral analysis	40–60 dB	5–8
// Strong leakage suppression	70–90 dB	9–12

// If you just want something that “usually works” for FFT spectra:

//     β ≈ 5 → decent leakage control, still okay resolution

//     β ≈ 8 → strong leakage suppression, noticeably wider main lobe

// Tukey window α (alpha)

// For Tukey:

//     α=0 → rectangular

//     α=1 → Hann

// So α is just “how much taper” you want:
// α value	Behavior	When to use
// 0.1–0.25	Mostly flat, light taper	Max resolution, modest leakage
// 0.3–0.5	Balanced taper vs. resolution	Good general‑purpose choice
// 0.75–1	Very tapered, like Hann	Minimize leakage, accept blur

// A very common default is:

//     α ≈ 0.5 for general FFT work.
// ==================================================================
struct WindowFunction
// ==================================================================
{
    enum eFunc
    {
        // Fenster        Leakage       Freq.-auflösung Ampl.-genau Typische Anwendung
        Rect = 0,       // schlecht     sehr gut        schlecht    periodische Signale
        Hann,           // gut          gut             gut         Standard‑FFT
        Hamming,        // mittel       gut             besser      Audio, Vibrationsanalyse
        Blackman,       // sehr gut     mittel          gut         Spektren mit hohem Dynamikbereich
        BlackmanHarris, // exzellent	schlecht        gut         Audio, EMV
        FlatTop,        // schlecht     sehr schlecht   beste       Pegelmessungen
        Bartlet,        // mittel       mittel          mittel      einfache Analysen
        Kaiser,         // variabel     variabel        variabel    DSP‑Design
        Tukey,          // variabel     gut             gut         gemischte Signale
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
            case BlackmanHarris: return "Blackman-Harris";
            case FlatTop: return "FlatTop";
            case Bartlet: return "Bartlet";
            case Kaiser: return "Kaiser";
            case Tukey: return "Tukey";
            default: return "Unknown";
        }
    }

    WindowFunction();

    void setFunction(eFunc func);

    eFunc function() const { return m_func; }

    void resize(uint32_t n);

    void apply(const AlignedFloatVector& v_in, AlignedFloatVector& v_out);

    void apply(const float* __restrict__ pIn, uint32_t nIn,
                     float* __restrict__ pOut, uint32_t nOut);


    static void drawImage( eFunc func, Image & img, uint32_t penColor );

    static std::string createSVG( eFunc func, int w, int h, int nPoints );

    static void test();
    static void testImg();
    static void testSvg();

protected:
    void updateCoefficients();

private:
    eFunc m_func;
    AlignedFloatVector m_lut; // LookupTable
};

} // end namespace audio.
} // end namespace de.

