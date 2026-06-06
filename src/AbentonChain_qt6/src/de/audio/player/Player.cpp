#include <de/audio/player/Player.h>
#include <de/audio/plugin/details/BasePluginUtils.h>
#include <App.h>
#include <de/audio/file/SoundFactory.h>
#include <de/audio/dsp/DspResampler-1.8.h>

namespace de {
namespace audio {

// ============================================================================
struct Player_Impl
// ============================================================================
{
    u32 m_pluginId = 0;
    bool m_bIsPluginOpen = false;
    bool m_bIsBypassed = false;
    Track* m_track = nullptr;
    PluginEditorWindow* m_editor = nullptr;
    IDspChainElement* m_inputSignal = nullptr;

    //u32 m_numInputs = 0;
    //u32 m_numOutputs = 0;
    //u32 m_sampleRate = 0;
    //u32 m_blockSize = 0;
    //std::atomic< u64 > m_framePos = 0;

    std::string m_uri;
    // std::string m_pluginName;
    // std::string m_pluginVendor;
    // std::string m_pluginVersion;
    double m_pluginRuntime;

    // NormalizedSumComputer m_normalizedSumComputer;
    // PluginTimer m_perfTimer;
    // PluginClock m_midiClock;

    // AlignedFloatVector m_L;
    // AlignedFloatVector m_R;
    // AlignedFloatVector m_I;

    Sound m_sound;
    DspResampler m_resampler;

    Programs m_programsList;
    Parameters m_paramList;


    Player_Impl()
        : m_pluginId{ 0 }
        , m_bIsPluginOpen{ false }
        , m_bIsBypassed{ false }
        , m_track{ nullptr }
        , m_editor{ nullptr }
        , m_inputSignal{ nullptr }
        // , m_numInputs{ 0 }
        // , m_numOutputs{ 0 }
        // , m_sampleRate{ 0 }
        // , m_blockSize{ 0 }
        // , m_framePos{ 0 }
    {
        DE_DEBUG("")
        dsp_init( 1024, 2, 48000 );
    }

    ~Player_Impl()
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

        DE_WARN("Stop plugin")

        m_bIsPluginOpen = false;  // Set this first, so the audio callback does bypass this dsp element.

        if (m_editor)
        {
            DE_TRACE("Close editor")
            m_editor->enableClosing();
            m_editor->close();
            m_editor->deleteLater();
            //delete m_editor;
            m_editor = nullptr;
        }

        // m_framePos = 0;
    }

    void openPlugin( std::string uri )
    {
        if (m_bIsPluginOpen)
        {
            DE_WARN("Plugin already open")
            return;
        }

        //setBypassed( true );

        bool ok = SoundFactory::load(m_sound, uri);
        if ( !ok )
        {
            DE_ERROR("No sound")
            return;
        }

        m_uri = de::FileSystem::makeAbsolute(uri);
        m_pluginRuntime = 0.0;

        Parameter p0;
        p0.m_id = 0;
        p0.m_name = "Volume";
        p0.m_disp = "Vol.";
        p0.m_flags = Parameter::kCanAutomate;
        p0.m_nowValue = 0.5;
        p0.m_defValue = 0.5;
        p0.m_minValue = 0.0;
        p0.m_maxValue = 1.0;

        Parameter p1;
        p1.m_id = 1;
        p1.m_name = "Gain";
        p1.m_disp = "Gain";
        p1.m_flags = Parameter::kCanAutomate;
        p1.m_nowValue = 1.0;
        p1.m_defValue = 1.0;
        p1.m_minValue = 0.0;
        p1.m_maxValue = 1.0;

        Parameter p2;
        p2.m_id = 2;
        p2.m_name = "Pan";
        p2.m_disp = "Pan";
        p2.m_flags = Parameter::kCanAutomate;
        p2.m_nowValue = 0.0;
        p2.m_defValue = 0.0;
        p2.m_minValue = -1.0;
        p2.m_maxValue = 1.0;

        m_paramList.emplace_back(p0);
        m_paramList.emplace_back(p1);
        m_paramList.emplace_back(p2);

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
        m_resampler.setSampleRateIn(m_sound.m_fileInfo.sampleRate);
        m_resampler.dsp_setInputSignal(&m_sound);
        m_sound.dsp_setInputSignal(m_inputSignal);
        m_sound.dsp_init( frames, channels, sampleRate );
        m_resampler.dsp_init( frames, channels, sampleRate );
    }

    void dsp_read(f64 pts,
                  u32 frames,
                  u32 sampleRate,
                  f32* __restrict__ L,
                  f32* __restrict__ R)
    {
        // const double timeStart = m_perfTimer.now();

        // m_sound.dsp_read(pts,frames,sampleRate,L,R);
        m_resampler.dsp_read(pts,frames,sampleRate,L,R);


        float volume = m_paramList[0].m_nowValue;
        for (size_t i = 0; i < frames; ++i)
        {
            L[i] *= volume;
            R[i] *= volume;
        }
        // For audio-level-meter
        // m_normalizedSumComputer.calc(outL, outR, frames);

        // // Thank you for participating in our DspChain dear plugin.
        // const double timeEnd = m_perfTimer.now();

        // m_pluginRuntime = timeEnd - timeStart;
    }

};

// ============================================================================
Player::Player()
    // ============================================================================
    : _d( new Player_Impl )
{
    DE_TRACE("")
}

Player::~Player()
{
    DE_TRACE("")
    App::instance()->getMidiCentral().deregisterListener(this);
    delete _d;
}

// ===================================================

void Player::dsp_init(u64 frames, u32 channels, u32 sampleRate)
{
    _d->dsp_init(frames, channels, sampleRate);
}

void Player::dsp_read(f64 pts,
                           u32 frames,
                           u32 sampleRate,
                           f32* __restrict__ L,
                           f32* __restrict__ R)
{
    _d->dsp_read(pts, frames, sampleRate, L, R);
}

u32 Player::dsp_getInputSignalCount() const
{
    return 1;
}

IDspChainElement* Player::dsp_getInputSignal(int i)
{
    return _d->m_inputSignal;
}

void Player::dsp_setInputSignal(IDspChainElement* pSignal, int i)
{
    _d->m_inputSignal = pSignal;
}

void Player::dsp_clearInputSignals()
{
    _d->m_inputSignal = nullptr;
}

bool Player::isBypassed() const
{
    return _d->m_bIsBypassed;
}

void Player::setBypassed( bool bBypassed )
{
    _d->m_bIsBypassed = bBypassed;
}

// ===================================================

const Track* Player::getTrack() const { return _d->m_track; }

Track* Player::getTrack() { return _d->m_track; }

void Player::setTrack(Track* track) { _d->m_track = track; }

// ===================================================

u32 Player::getPluginId() const { return _d->m_pluginId; }

void Player::setPluginId( u32 pluginId ) { _d->m_pluginId = pluginId; }

// ===================================================

std::string Player::getUri() const { return _d->m_uri; }

std::string Player::getName() const { return "Player"; }

std::string Player::getVendor() const { return "Abenton"; }

std::string Player::getVersion() const { return "1.0.0"; }

double Player::getRuntime() const { return _d->m_pluginRuntime; }

// ===================================================

void Player::openPlugin( std::string uri )
{
    _d->openPlugin( uri );
}

void Player::closePlugin()
{
    _d->closePlugin();
}

bool Player::isPluginOpen() const
{
    return _d->m_bIsPluginOpen;
}

bool Player::isSynth() const
{
    return false;
}

PluginEditorWindow* Player::getEditor()
{
    return _d->m_editor;
}

// ===================================================

void Player::onMidiMessage(f64 pts, const midi::MidiMessage& msg)
{

}

void Player::onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
{

}

// ===================================================

int Player::getProgram() const
{
    return 0;
}

void Player::setProgram( int i )
{
}


const Programs& Player::getPrograms() const
{
    return _d->m_programsList;
}

const Parameters& Player::getParameters() const
{
    return _d->m_paramList;
}

f64 Player::getParameterValue(uint32_t id) const
{
    switch (id)
    {
        case 0: return _d->m_paramList[0].m_nowValue;
        case 1: return _d->m_paramList[1].m_nowValue;
        case 2: return _d->m_paramList[2].m_nowValue;
        default: return 0.0;
    }
}

void Player::setParameterValue(uint32_t id, f64 value, int64_t framePos)
{
    switch (id)
    {
        case 0:
        {
            double v_min = _d->m_paramList[0].m_minValue;
            double v_max = _d->m_paramList[0].m_maxValue;
            double v = ::de::clampd(value,v_min,v_max);
            _d->m_paramList[0].m_nowValue = v;
            break;
        }
        default: break;
    }
}


float Player::getSpecialValue( eSpecialValue type ) const
{
    switch (type)
    {
    // case IPlugin::eSV_NormalizedSumL: return _d->m_normalizedSumComputer.m_sumL;
    // case IPlugin::eSV_NormalizedSumR: return _d->m_normalizedSumComputer.m_sumR;
    default: return 0.0f;
    }
}


} // end namespace audio.
} // end namespace de.
