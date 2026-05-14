#include <de/audio/file/SoundSource.h>

#if 0
#include <de/audio/file/FilePlayer_Plugin.h>

#include <App.h>
namespace de {
namespace audio {

// ============================================================================
struct FilePlayer_Plugin_Impl
// ============================================================================
{
    u32 m_pluginId = 0;
    bool m_bIsPluginOpen = false;
    bool m_bIsBypassed = false;
    Track* m_track = nullptr;
    PluginEditorWindow* m_editor = nullptr;
    IDspChainElement* m_inputSignal = nullptr;
    u32 m_numInputs = 0;
    u32 m_numOutputs = 0;
    u32 m_sampleRate = 0;
    u32 m_blockSize = 0;
    std::atomic< u64 > m_framePos = 0;

    std::string m_uri;                 // FilePlayer_Plugin file name
    std::string m_pluginName;
    std::string m_pluginVendor;
    std::string m_pluginVersion;
    double m_pluginRuntime;

    NormalizedSumComputer m_normalizedSumComputer;
    PluginTimer m_perfTimer;
    PluginClock m_midiClock;

    AlignedFloatVector m_L;
    AlignedFloatVector m_R;
    AlignedFloatVector m_I;

    FilePlayer_Plugin_Impl()
        : m_pluginId{ 0 }
        , m_bIsPluginOpen{ false }
        , m_bIsBypassed{ false }
        , m_track{ nullptr }
        , m_editor{ nullptr }
        , m_inputSignal{ nullptr }
        , m_numInputs{ 0 }
        , m_numOutputs{ 0 }
        , m_sampleRate{ 0 }
        , m_blockSize{ 0 }
        , m_framePos{ 0 }
    {
        DE_DEBUG("")
        dsp_init( 1024, 2, 48000 );
    }

    ~FilePlayer_Plugin_Impl()
    {
        DE_DEBUG("")
        if (m_bIsPluginOpen)
        {
            DE_ERROR("No closePlugin() called.")
            closePlugin();
        }
    }

    void closePlugin()
    {
        if ( !m_bIsPluginOpen )
        {
            DE_TRACE("Plugin already closed")
            return;
        }

        DE_WARN("Close ",m_uri)

        m_bIsPluginOpen = false;  // Set this first, so the audio callback does bypass this dsp element.

        //   if ( isSynth() )
        //   {
        //      emit removedSynth( this ); // Unregister synth from MIDI keyboards
        //   }

        DE_WARN("Stop plugin")

        if (m_editor)
        {
            DE_TRACE("Close editor")
            m_editor->enableClosing();
            m_editor->close();
            m_editor->deleteLater();
            //delete m_editor;
            m_editor = nullptr;
        }

        m_framePos = 0;
    }

    void openPlugin( std::string uri )
    {
        if (m_bIsPluginOpen)
        {
            DE_WARN("Plugin already open")
            return;
        }

        //setBypassed( true );

        if ( uri.empty() )
        {
            DE_WARN("empty filename")
            return;
        }

        m_uri = uri;
        m_pluginName = dbFileBase(uri);
        m_pluginVendor = "";
        m_pluginVersion = "";
        m_pluginRuntime = 0.0;

        DE_TRACE("uri = ",m_uri)
        m_numInputs = 0;
        m_numOutputs = 2;

        dsp_init(256, 2, 48000);

        // DE_DEBUG("VST2 plugin File = ", dbFileBase(m_uri))
        // DE_DEBUG("VST2 plugin Name = ", m_pluginName)
        // DE_DEBUG("VST2 plugin Vendor = ", m_pluginVendor)
        // DE_DEBUG("VST2 plugin Version = ", m_pluginVersion)
        // DE_TRACE("VST2 plugin Inputs = ",m_numInputs)
        // DE_TRACE("VST2 plugin Outputs = ",m_numOutputs)

        //dumpPrograms();
        //dumpParams();
        m_bIsBypassed = false;
        m_bIsPluginOpen = true;
    }

    void dsp_init(u64 frames, u32 channels, u32 sampleRate)
    {
        bool bNeedResize = false;
        if ( m_blockSize != frames )
        {
            m_blockSize = frames;
            bNeedResize = true;
        }

        if ( m_sampleRate != sampleRate )
        {
            m_sampleRate = sampleRate;
            bNeedResize = true;
        }

        if ( bNeedResize )
        {
            DE_WARN("frames(",frames,", channels(",channels,"), sampleRate(",sampleRate,")")
        }
    }

    void dsp_read(f64 pts,
                  u32 frames,
                  u32 sampleRate,
                  f32* __restrict__ outL,
                  f32* __restrict__ outR)
    {
        const double timeStart = m_perfTimer.now();

        if ( !outL || !outR )
        {
            throw std::runtime_error("No dst audio dsp buffer in FilePlayer_Plugin::readSamples()!");
        }

        //===============================
        // VST2 processing is inactive:
        //===============================

        if (!m_bIsPluginOpen || m_bIsBypassed)
        {
            if ( m_inputSignal )
            {
                m_inputSignal->dsp_read( pts, frames, sampleRate, outL, outR );
            }
            else
            {
                std::fill(outL,outL+frames,0.0f);
                std::fill(outR,outR+frames,0.0f);
            }

            // if (m_bIsBypassed)
            // {
            //     DE_WARN("IsBypassed ", m_uri)
            // }
            return; // We relayed samples or filled output with zeroes
        }

        //===============================
        // VST2 processing is active:
        //===============================

        dsp_init(frames,2,sampleRate);

        //========================================================
        // Get L+R sample data from previous signal, or zeroes.
        //========================================================
        if ( m_inputSignal )
        {
            m_inputSignal->dsp_read( pts, frames, sampleRate,
                                    m_L.data(),
                                    m_R.data() );
        }
        else
        {
            std::fill(m_L.begin(), m_L.begin() + frames, 0.0f);
            std::fill(m_R.begin(), m_R.begin() + frames, 0.0f);
        }

        //========================================================
        // Copy L+R sample data to VST input.
        //========================================================

        const auto bytesPerChannel = u64(frames) * sizeof(float);

        // // Fill input[0] with L data:
        // std::memcpy(m_iBuffers.at(0).data(),  // dst L
        //             m_L.data(),               // src L
        //             bytesPerChannel);

        // // Fill input[1] with R data:
        // std::memcpy(m_iBuffers.at(1).data(),  // dst R
        //             m_R.data(),               // src R
        //             bytesPerChannel);

        // // Fill input[2...N-1] with zeroes:
        // for (int i = 2; i < int(m_numInputs); ++i)
        // {
        //     std::memset(m_iBuffers.at(i).data(), 0, bytesPerChannel);
        // }

        // ======================================================
        // We support legacy (worst-case) "in-place" processing
        // by copying numInputs to output buffers.
        // ======================================================

        // // TODO: Maybe move to m_vst->process() case only.
        // // Copy available input to output channels:
        // for (int i = 0; i < m_numInputs; ++i)
        // {
        //     std::memcpy(m_oBuffers.at(i).data(),
        //                 m_iBuffers.at(i).data(),
        //                 bytesPerChannel);
        // }

        // // Fill remaining output channels with silence (0.0f).
        // for (int i = m_numInputs; i < m_maxChannels; ++i)
        // {
        //     std::memset(m_oBuffers.at(i).data(),
        //                 0, bytesPerChannel);
        // }

        // ======================================================
        // Process Audio samples:
        // ======================================================


        // ======================================================
        // Write (L+R) VST audio output back to DspChain.
        // ======================================================

        m_framePos += frames; // atomic.

        // TODO: Maybe add all output buffers in the hopes to catch some missing data.

        // if (m_bIsSynth)
        // {
        //     DSP_ADD(outL, frames, m_buffers.m_oBuffers.at(0).data(), m_buffers.m_L.data());
        //     DSP_ADD(outR, frames, m_buffers.m_oBuffers.at(1).data(), m_buffers.m_R.data());
        // }
        // else
        // {
        //     // Copy [L]eft channel:
        //     std::memcpy(outL, m_buffers.m_oBuffers.at(0).data(), bytesPerChannel);

        //     // Copy [R]ight channel:
        //     std::memcpy(outR, m_buffers.m_oBuffers.at(1).data(), bytesPerChannel);
        // }

        // For audio-level-meter
        m_normalizedSumComputer.calc(outL, outR, frames);

        // Thank you for participating in our DspChain dear plugin.
        const double timeEnd = m_perfTimer.now();

        m_pluginRuntime = timeEnd - timeStart;
    }

    void onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
    {
        DE_WARN("Not implemented, ", msg.str())
    }

    void onMidiMessage(f64 pts, const midi::MidiMessage& msg)
    {
        DE_WARN("Not implemented, ", msg.size())
    }

};

// ============================================================================
FilePlayer_Plugin::FilePlayer_Plugin()
    // ============================================================================
    : _d( new FilePlayer_Plugin_Impl )
{
    DE_TRACE("")
}

FilePlayer_Plugin::~FilePlayer_Plugin()
{
    DE_TRACE("")
    App::instance()->getMidiCentral().deregisterListener(this);
    delete _d;
}

// ===================================================

void FilePlayer_Plugin::dsp_init(u64 frames, u32 channels, u32 sampleRate)
{
    _d->dsp_init(frames, channels, sampleRate);
}

void FilePlayer_Plugin::dsp_read(f64 pts,
                           u32 frames,
                           u32 sampleRate,
                           f32* __restrict__ L,
                           f32* __restrict__ R)
{
    _d->dsp_read(pts, frames, sampleRate, L, R);
}

u32 FilePlayer_Plugin::dsp_getInputSignalCount() const
{
    return 1;
}

IDspChainElement* FilePlayer_Plugin::dsp_getInputSignal(int i)
{
    return _d->m_inputSignal;
}

void FilePlayer_Plugin::dsp_setInputSignal(IDspChainElement* pSignal, int i)
{
    _d->m_inputSignal = pSignal;
}

void FilePlayer_Plugin::dsp_clearInputSignals()
{
    _d->m_inputSignal = nullptr;
}

bool FilePlayer_Plugin::isBypassed() const
{
    return _d->m_bIsBypassed;
}

void FilePlayer_Plugin::setBypassed( bool bBypassed )
{
    _d->m_bIsBypassed = bBypassed;
}

// ===================================================

const Track* FilePlayer_Plugin::getTrack() const { return _d->m_track; }

Track* FilePlayer_Plugin::getTrack() { return _d->m_track; }

void FilePlayer_Plugin::setTrack(Track* track) { _d->m_track = track; }

// ===================================================

u32 FilePlayer_Plugin::getPluginId() const { return _d->m_pluginId; }

void FilePlayer_Plugin::setPluginId( u32 pluginId ) { _d->m_pluginId = pluginId; }

// ===================================================

std::string FilePlayer_Plugin::getUri() const { return _d->m_uri; }

std::string FilePlayer_Plugin::getName() const { return _d->m_pluginName; }

std::string FilePlayer_Plugin::getVendor() const { return _d->m_pluginVendor; }

std::string FilePlayer_Plugin::getVersion() const { return _d->m_pluginVersion; }

double FilePlayer_Plugin::getRuntime() const { return _d->m_pluginRuntime; }

// ===================================================

void FilePlayer_Plugin::openPlugin( std::string uri )
{
    _d->openPlugin( uri );
}

void FilePlayer_Plugin::closePlugin()
{
    _d->closePlugin();
}

bool FilePlayer_Plugin::isPluginOpen() const
{
    return _d->m_bIsPluginOpen;
}

bool FilePlayer_Plugin::isSynth() const
{
    return false;
}

PluginEditorWindow* FilePlayer_Plugin::getEditor()
{
    return _d->m_editor;
}

// ===================================================

void FilePlayer_Plugin::onMidiMessage(f64 pts, const midi::MidiMessage& msg)
{
    _d->onMidiMessage(pts, msg);
}

void FilePlayer_Plugin::onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
{
    _d->onShortMidiMessage(pts, msg);
}

// ===================================================

u32 FilePlayer_Plugin::getProgramCount() const
{
    return 1;
}

std::string FilePlayer_Plugin::getProgramName( int i ) const
{
    return "Default";
}

int FilePlayer_Plugin::getProgram() const
{
    return 0;
}

void FilePlayer_Plugin::setProgram( int i )
{
}

// ===================================================

u32 FilePlayer_Plugin::getParameterCount() const
{
    return 0;
}

f32 FilePlayer_Plugin::getParameter(int i) const
{
    return 0.0f;
}

std::string FilePlayer_Plugin::getParameterName(int i) const
{
    return "NotImpl";
}

void FilePlayer_Plugin::setParameter(int i, f32 value)
{

}

float FilePlayer_Plugin::getSpecialValue( eSpecialValue type ) const
{
    switch (type)
    {
    case IPlugin::eSV_NormalizedSumL: return _d->m_normalizedSumComputer.m_sumL;
    case IPlugin::eSV_NormalizedSumR: return _d->m_normalizedSumComputer.m_sumR;
    default: return 0.0f;
    }
}


} // end namespace audio.
} // end namespace de.

#endif