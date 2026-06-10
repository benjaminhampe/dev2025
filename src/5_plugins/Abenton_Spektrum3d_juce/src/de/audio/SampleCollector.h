#pragma once
//#include <de/audio/dsp/IDspChainElement.h>
//#include <de/audio/fft/approx_math.h>
#include <de/fft/WindowFunction.h>
#include <de/fft/DE_FFT_pffft.h>

namespace de {
namespace audio {

// ===================================================================
class SampleCollector
// ===================================================================
{
public:
    u32 m_blockSize;
    u32 m_blockCount; // Blocks we collect before a new fft calc. 1 = fft every block
    u32 m_blockIndex;
    u32 m_sampleRate;
    u32 m_fftSize;
    u32 m_cols;
    u32 m_rows;
    bool m_bBypassed;
    bool m_bCollectAccumMatrix;

    AlignedFloatVector m_L;
    AlignedFloatVector m_R;

    // L+R are summed up for FFT
    AlignedFloatVector m_sum;

    // blockSize FFT (potentially not a power of 2)
    // AlignedFloatShiftVector m_raw;
    // AlignedFloatVector m_raw_fft_in;
    // AlignedFloatVector m_raw_fft_out;
    // AlignedFloatShiftMatrix m_raw_fft_matrix;

    // shiftVector FFT
    WindowFunction m_accum_win;
    //AlignedFloatShiftVector m_accum;
    AlignedFloatVector m_accum_ori;
    AlignedFloatVector m_accum_tmp;
    AlignedFloatVector m_accum_vec_in;
    AlignedFloatVector m_accum_vec_out;
    AlignedFloatShiftMatrix m_accum_mat;

    DE_FFT_pffft m_accum_fft;
public:
    const AlignedFloatVector& getL() const { return m_L; }
    const AlignedFloatVector& getR() const { return m_R; }

    const AlignedFloatVector& getAccumVecIn() const { return m_accum_vec_in; }
    const AlignedFloatVector& getAccumVecOut() const { return m_accum_vec_out; }
    const AlignedFloatShiftMatrix& getAccumMat() const { return m_accum_mat; }

    SampleCollector();
    ~SampleCollector();

    uint32_t blockSize() const { return m_blockSize; }
    uint32_t fftSize() const { return m_fftSize; }
    uint32_t cols() const { return m_cols; }
    uint32_t rows() const { return m_rows; }
    WindowFunction::eFunc windowFunc() const { return m_accum_win.function(); }

    void setBypassed( bool bBypassed )
    {
        m_bBypassed = bBypassed;
    }

    void setCollectAccumMatrix( bool bBypassed )
    {
        m_bCollectAccumMatrix = bBypassed;
    }

    void setWindowFunc( int windowFunc )
    {
        m_accum_win.setFunction((WindowFunction::eFunc)windowFunc);
    }

    void setFftSize( uint32_t fftSize )
    {
        m_fftSize = fftSize;
        m_accum_fft.setFftSize(fftSize);
    }

    void setColumnCount( u32 columns )
    {
        m_cols = std::max(64u,columns);
    }

    void setRowCount( u32 rows )
    {
        m_rows = std::max(8u,rows);
    }

    void dsp_init( u64 frames, u32 channels, u32 sampleRate );

    void dsp_push(const de::TAlignedVector<float>& samples);

};

} // end namespace audio.
} // end namespace de.
