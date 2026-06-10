#include "DspSoundTouch.h"

namespace de {
namespace audio {

// ============================================================================
DspSoundTouch::DspSoundTouch()
   : m_isBypassed( false )
   //, m_inputSignal( nullptr )
   , m_channelCount(0)
   , m_blockSize(0)
   , m_fifo(2048)
   // , m_srcRate( 44100 )
   // , m_dstRate( 48000 )
   // , m_srcFrames( 1024 )
   // , m_dstFrames( 1024 )
   // , m_callCounter( 0 )
   // , m_putFrames( 0 )
   // , m_popFrames( 0 )
{
   DE_DEBUG("DEFAULT_AA_FILTER_LENGTH = ", m_soundTouch.getSetting(SETTING_AA_FILTER_LENGTH ) )
   DE_DEBUG("DEFAULT_SEQUENCE_MS = ", m_soundTouch.getSetting(SETTING_SEQUENCE_MS ) )
   DE_DEBUG("DEFAULT_SEEKWINDOW_MS = ", m_soundTouch.getSetting(SETTING_SEEKWINDOW_MS ) )
   DE_DEBUG("DEFAULT_OVERLAP_MS = ", m_soundTouch.getSetting(SETTING_OVERLAP_MS ) )

   m_soundTouch.setSampleRate( 44100 );
   m_soundTouch.setChannels( 2 );
   m_soundTouch.setRate( 1.0f );
   m_soundTouch.setSetting(SETTING_USE_QUICKSEEK, 0 );
   m_soundTouch.setSetting(SETTING_USE_AA_FILTER, 1 );
   m_soundTouch.setSetting(SETTING_AA_FILTER_LENGTH,128 );
   m_soundTouch.setSetting(SETTING_SEQUENCE_MS,28 );
   m_soundTouch.setSetting(SETTING_SEEKWINDOW_MS, 28);
   m_soundTouch.setSetting(SETTING_OVERLAP_MS,80 );

   bool speechConfig = false;
   if (speechConfig)
   {
      // use settings for speech processing
      m_soundTouch.setSetting(SETTING_SEQUENCE_MS, 40);
      m_soundTouch.setSetting(SETTING_SEEKWINDOW_MS, 15);
      m_soundTouch.setSetting(SETTING_OVERLAP_MS, 8);
      DE_DEBUG("Tune processing parameters for speech processing.")
   }


   DE_DEBUG("Final Setting:" )
   DE_DEBUG("SETTING_AA_FILTER_LENGTH = ", m_soundTouch.getSetting(SETTING_AA_FILTER_LENGTH ) )
   DE_DEBUG("SETTING_SEQUENCE_MS = ", m_soundTouch.getSetting(SETTING_SEQUENCE_MS ) )
   DE_DEBUG("SETTING_SEEKWINDOW_MS = ", m_soundTouch.getSetting(SETTING_SEEKWINDOW_MS ) )
   DE_DEBUG("SETTING_OVERLAP_MS = ", m_soundTouch.getSetting(SETTING_OVERLAP_MS ) )

   // Read in Params first time.
   m_realTimeParams.quickSeek = (m_soundTouch.getSetting(SETTING_USE_QUICKSEEK ) != 0);
   m_realTimeParams.sequenceMs = m_soundTouch.getSetting(SETTING_SEQUENCE_MS);
   m_realTimeParams.seekWinMs = m_soundTouch.getSetting(SETTING_SEEKWINDOW_MS);
   m_realTimeParams.overlapMs = m_soundTouch.getSetting(SETTING_OVERLAP_MS);
   m_realTimeParams.aaFilterLen = m_soundTouch.getSetting(SETTING_AA_FILTER_LENGTH);
   if ( m_soundTouch.getSetting(SETTING_USE_AA_FILTER) == 0 )
   {
      m_realTimeParams.aaFilterLen = 0;
   }

   // m_inputBuffer.resize( 8192, 0.0f );
   // m_outputBuffer.resize( 8192, 0.0f );
}

DspSoundTouch::~DspSoundTouch()
{
}

// void
// SoundTouch::clearInputSignals()
// {
//    m_inputSignal = nullptr;
// }
// void
// SoundTouch::setInputSignal( int, IDspChainElement* input )
// {
//    m_inputSignal = input;
// }
void
DspSoundTouch::setInputSampleRate( uint32_t rate )
{
   m_soundTouch.setSampleRate( rate );
}
void
DspSoundTouch::setBypassed( bool bypassed )
{
   DE_DEBUG("bypassed = ", bypassed)
   m_isBypassed = bypassed;
}
bool
DspSoundTouch::isBypassed() const
{
   return m_isBypassed;
}

void
DspSoundTouch::setup(uint32_t frames, uint32_t channels, uint32_t rate )
{
    m_blockSize = frames;
    m_channelCount = channels;
    m_soundTouch.setSampleRate( rate );
    m_popBuffer.resize( frames * channels );
}

void DspSoundTouch::updateParams()
{
    m_soundTouch.setRate( m_realTimeParams.virtualRate );
    m_soundTouch.setTempo( m_realTimeParams.virtualTempo );
    m_soundTouch.setPitch( m_realTimeParams.virtualPitch );

    if ( m_realTimeParams.aaFilterLen > 0 )
    {
        auto enabled = (m_soundTouch.getSetting( SETTING_USE_AA_FILTER ) != 0);
        if ( !enabled )
        {
            m_soundTouch.setSetting( SETTING_USE_AA_FILTER, 1 );
        }

        auto old = m_soundTouch.getSetting( SETTING_AA_FILTER_LENGTH );
        if ( old != m_realTimeParams.aaFilterLen )
        {
            m_soundTouch.setSetting( SETTING_AA_FILTER_LENGTH, m_realTimeParams.aaFilterLen );
        }
    }
    else
    {
        auto enabled = (m_soundTouch.getSetting( SETTING_USE_AA_FILTER ) != 0);
        if ( enabled )
        {
            m_soundTouch.setSetting( SETTING_USE_AA_FILTER, 0 );
        }
    }

    auto sequenceMs = m_soundTouch.getSetting( SETTING_SEQUENCE_MS );
    if ( sequenceMs != m_realTimeParams.sequenceMs )
    {
        m_soundTouch.setSetting( SETTING_SEQUENCE_MS, m_realTimeParams.sequenceMs );
    }

    auto seekWinMs = m_soundTouch.getSetting( SETTING_SEEKWINDOW_MS );
    if ( seekWinMs != m_realTimeParams.seekWinMs )
    {
        m_soundTouch.setSetting( SETTING_SEEKWINDOW_MS, m_realTimeParams.seekWinMs );
    }

    auto overlapMs = m_soundTouch.getSetting( SETTING_OVERLAP_MS );
    if ( overlapMs != m_realTimeParams.overlapMs )
    {
        m_soundTouch.setSetting( SETTING_OVERLAP_MS, m_realTimeParams.overlapMs );
    }
}

void
DspSoundTouch::process( const float* __restrict__ pSrc, float* __restrict__ pL, float* __restrict__ pR )
{
    updateParams();

    m_soundTouch.putSamples( pSrc, m_blockSize );

    uint64_t popFrames = m_soundTouch.receiveSamples(m_popBuffer.data(), m_blockSize);

    m_fifo.push(m_popBuffer.data(), popFrames * m_channelCount);

    if ( m_fifo.available() < m_blockSize * m_channelCount )
    {
        std::memset(pL, 0, m_blockSize * sizeof(float));
        std::memset(pR, 0, m_blockSize * sizeof(float));
    }
    else
    {
        m_fifo.pop(m_popBuffer.data(), m_blockSize * m_channelCount);

        const float volume = 0.0001f * float( m_realTimeParams.volume * m_realTimeParams.volume );

        // === DE-INTERLEAVE-L ===
        {
            const float* __restrict__ src = m_popBuffer.data();
                  float* __restrict__ L = pL;

            for (size_t i = 0; i < m_blockSize; ++i)
            {
                L[i] = src[i] * volume;
                src += m_channelCount;
            }
        }

        // === DE-INTERLEAVE-R ===
        {
            const float* __restrict__ src = m_popBuffer.data() + 1;
                  float* __restrict__ R = pR;

            for (size_t i = 0; i < m_blockSize; ++i)
            {
                R[i] = src[i] * volume;
                src += m_channelCount;
            }
        }
    }
}

} // end namespace audio
} // end namespace de
