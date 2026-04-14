#include <de/audio/plugin/details/CLAP_Plugin.h>

#ifdef BENNI_USE_CLAP

#include <de/audio/plugin/details/CLAP_Editor.h>
#include <de/audio/plugin/details/BasePluginUtils.h>
#include <App.h>

#include <clap/clap.h>
#include <vector>
#include <string>

namespace de {
namespace audio {

struct CLAP_OutputStream
{
    clap_ostream_t stream;
    std::vector<uint8_t> data;

    static int64_t
    write(const clap_ostream_t *s, const void *buffer, uint64_t size)
    {
        auto self = (CLAP_OutputStream*)s->ctx;
        const uint8_t* bytes = (const uint8_t*)buffer;
        self->data.insert(self->data.end(), bytes, bytes + size);
        return size;
    }

};

// u32 dumpSampleRate(const clap_plugin* m_plugin)
// {
//     auto cfg = (const clap_plugin_audio_ports_config_t*)
//         m_plugin->get_extension(m_plugin, CLAP_EXT_AUDIO_PORTS_CONFIG);

//     if (cfg) {
//         uint32_t count = cfg->count(plugin);
//         for (uint32_t i = 0; i < count; i++) {
//             clap_audio_ports_config_t info;
//             if (cfg->get(plugin, i, &info)) {
//                 double sample_rate = info.sample_rate;
//                 uint32_t block_size = info.max_block_size;
//             }
//         }
//     }
// }

u32 dumpInputs(const clap_plugin* m_plugin)
{
    auto audioPorts = (const clap_plugin_audio_ports_t*)
        m_plugin->get_extension(m_plugin, CLAP_EXT_AUDIO_PORTS);

    if (!audioPorts)
    {
        DE_ERROR("No CLAP_EXT_AUDIO_PORTS")
        return 0;
    }

    const u32 n = audioPorts->count(m_plugin, 1);
    DE_DEBUG("Inputs.Count = ",n)

    for (uint32_t i = 0; i < n; i++)
    {
        clap_audio_port_info_t info;
        if (audioPorts->get(m_plugin, i, 1, &info))
        {
            DE_WARN("Input[",i,"].ID = ",info.id)
            DE_WARN("Input[",i,"].Name = ",info.name)
            DE_WARN("Input[",i,"].Channels = ", info.channel_count)
            // ---
            // This port is the main audio input or output.
            // There can be only one main input and main output.
            // Main port must be at index 0.
            // CLAP_AUDIO_PORT_IS_MAIN = 1 << 0,
            // ---
            // This port can be used with 64 bits audio
            // CLAP_AUDIO_PORT_SUPPORTS_64BITS = 1 << 1,
            // ---
            // 64 bits audio is preferred with this port
            // CLAP_AUDIO_PORT_PREFERS_64BITS = 1 << 2,
            // ---
            // This port must be used with the same sample size as all the other ports which have this flag.
            // In other words if all ports have this flag then the plugin may either be used entirely with
            // 64 bits audio or 32 bits audio, but it can't be mixed.
            // CLAP_AUDIO_PORT_REQUIRES_COMMON_SAMPLE_SIZE = 1 << 3,
            DE_WARN("Input[",i,"].Flags = ",dbHex(info.flags))
            // CLAP_PORT_MONO
            // CLAP_PORT_STEREO
            // CLAP_PORT_SURROUND (defined in the surround extension)
            // CLAP_PORT_AMBISONIC (defined in the ambisonic extension)
            DE_WARN("Input[",i,"].Type = ", info.port_type ? info.port_type : "unknown");
            DE_WARN("Input[",i,"].InPlacePairID = ", info.in_place_pair)
        }
    }
    return n;
}

u32 dumpOutputs(const clap_plugin* m_plugin)
{
    auto audioPorts = (const clap_plugin_audio_ports_t*)
        m_plugin->get_extension(m_plugin, CLAP_EXT_AUDIO_PORTS);

    if (!audioPorts)
    {
        DE_ERROR("No CLAP_EXT_AUDIO_PORTS")
        return 0;
    }

    const u32 n = audioPorts->count(m_plugin, 0);
    DE_DEBUG("Outputs.Count = ",n)

    for (uint32_t i = 0; i < n; i++)
    {
        clap_audio_port_info_t info;
        if (audioPorts->get(m_plugin, i, 0, &info))
        {
            DE_WARN("Output[",i,"].ID = ",info.id)
            DE_WARN("Output[",i,"].Name = ",info.name)
            DE_WARN("Output[",i,"].Channels = ", info.channel_count)
            // ---
            // This port is the main audio input or output.
            // There can be only one main input and main output.
            // Main port must be at index 0.
            // CLAP_AUDIO_PORT_IS_MAIN = 1 << 0,
            // ---
            // This port can be used with 64 bits audio
            // CLAP_AUDIO_PORT_SUPPORTS_64BITS = 1 << 1,
            // ---
            // 64 bits audio is preferred with this port
            // CLAP_AUDIO_PORT_PREFERS_64BITS = 1 << 2,
            // ---
            // This port must be used with the same sample size as all the other ports which have this flag.
            // In other words if all ports have this flag then the plugin may either be used entirely with
            // 64 bits audio or 32 bits audio, but it can't be mixed.
            // CLAP_AUDIO_PORT_REQUIRES_COMMON_SAMPLE_SIZE = 1 << 3,
            DE_WARN("Output[",i,"].Flags = ",dbHex(info.flags))
            // CLAP_PORT_MONO
            // CLAP_PORT_STEREO
            // CLAP_PORT_SURROUND (defined in the surround extension)
            // CLAP_PORT_AMBISONIC (defined in the ambisonic extension)
            DE_WARN("Output[",i,"].Type = ", info.port_type ? info.port_type : "unknown");
            DE_WARN("Output[",i,"].InPlacePairID = ", info.in_place_pair)
        }
    }
    return n;
}

u32 dumpParams(const clap_plugin* m_plugin)
{
    auto params = (const clap_plugin_params_t*)
        m_plugin->get_extension(m_plugin, CLAP_EXT_PARAMS);

    if (!params)
    {
        DE_ERROR("No CLAP_EXT_PARAMS")
        return 0;
    }

    const u32 n = params->count(m_plugin);
    DE_DEBUG("Params.Count = ",n)

    for (uint32_t i = 0; i < n; i++)
    {
        clap_param_info_t info;
        if (!params->get_info(m_plugin, i, &info))
        {
            DE_ERROR("Param[",i,"] NOT_EXIST")
            continue;
        }

        DE_TRACE("Param[",i,"].ID = ", info.id)
        DE_TRACE("Param[",i,"].Name = ", info.name)
        DE_TRACE("Param[",i,"].Module = ", info.module)
        DE_TRACE("Param[",i,"].Min = ", info.min_value)
        DE_TRACE("Param[",i,"].Max = ", info.max_value)
        DE_TRACE("Param[",i,"].Default = ", info.default_value)
        DE_TRACE("Param[",i,"].Flags = ", dbHex(info.flags))

        double value;
        if (!params->get_value(m_plugin, info.id, &value))
        {
            DE_ERROR("Param[",i,"].Value = NOT_EXIST")
        }
        else
        {
            DE_TRACE("Param[",i,"].Value = ",value)
        }

        DE_TRACE("")
    }

    return n;
}

//===============================
struct CLAP_Plugin_Impl
//===============================
{
    u32 m_pluginId = 0;
    bool m_bIsPluginOpen = false;
    bool m_bIsDsoInit = false; // if (true) then call clap->uninit();
    bool m_bNeedSetup = true;
    bool m_bIsSynth = false;
    bool m_bIsBypassed = false;

    ITrack* m_track = nullptr;
    PluginEditorWindow* m_editor = nullptr;
    IDspChainElement* m_inputSignal = nullptr;

    int64_t m_steadyTime = 0;
    u32 m_sampleRate = 0;
    u32 m_blockSize = 480;
    u32 m_minBlock = 64;
    u32 m_maxBlock = 2048;
    u32 m_numInputs = 2;
    u32 m_numOutputs = 2;
    u32 m_numPrograms = 0;
    u32 m_numParams = 0;

    std::atomic< u64 > m_framePos = 0;

    std::string m_uri;
    std::string m_directoryMultiByte;
    std::string m_pluginName;
    std::string m_pluginVendor;

    SymbolLoader m_symLoader;
    PluginClock m_midiClock;

    const clap_plugin_factory* m_factory = nullptr;
    const clap_plugin* m_plugin = nullptr;

    std::vector<clap_event_header_t*> m_inEventList;
    std::vector<clap_event_header_t*> m_outEventList;

/*
    AEffect* m_vst = nullptr;
    VstTimeInfo m_timeInfo;

    VST2_SampleBuffers m_sampleBuffers;
    // VST seems to work channelwise / planar, not interleaved audio.
    // std::vector< f32 > m_outBuffer;
    // std::vector< f32*> m_outBufferHeads;
    // std::vector< f32 > m_inBuffer;
    // std::vector< f32*> m_inBufferHeads;

    // VST midi event handling
    std::vector< VstMidiEvent > m_vstMidiEvents;
    std::vector< char > m_vstEventBuffer;

    struct MyVstMidi
    {
        std::unique_lock< std::mutex >
        lock() const { return std::unique_lock<std::mutex>(m_mutex); }
        std::vector< VstMidiEvent > events;
    private:
        std::mutex mutable m_mutex;
    } m_vstMidi;

    // PluginEditorWindow* m_editorWindow = nullptr; // PluginEditorWindow HWND

    bool getFlags( int32_t m ) const
    {
        return m_vst ? ((m_vst->flags & m) == m) : 0;
    }
*/

    // ============================================================================
    CLAP_Plugin_Impl()
    // ============================================================================
    {
        DE_DEBUG("")
    }

    ~CLAP_Plugin_Impl()
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

        DE_WARN("Stop vst plugin")
        if (m_plugin)
        {
            m_plugin->destroy(m_plugin);
            m_plugin = nullptr;
        }

        m_symLoader.close();

        m_framePos = 0;
    }

    // ---------------------------------------------------------
    // Host Descriptor
    // ---------------------------------------------------------
    static const clap_host* getHost()
    {
        static clap_host host {
            .clap_version = CLAP_VERSION,
            .host_data = nullptr,
            .name = "QtClapHost",
            .vendor = "Qt CLAP Host",
            .url = "https://github.com/benjaminhampe",
            .version = "1.0",
            .get_extension = nullptr,
            .request_restart = nullptr,
            .request_process = nullptr,
            .request_callback = nullptr
        };
        return &host;
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
        m_directoryMultiByte = dbFileDir(uri);
        m_pluginName = dbFileBase(uri);
        m_pluginVendor = "";

        DE_TRACE("uri = ",m_uri)
        DE_TRACE("dir = ",m_directoryMultiByte)

        m_symLoader.open(uri);
        if ( !m_symLoader.is_open() )
        {
            DE_ERROR("No library ",uri)
            return;
        }

        typedef const clap_plugin_entry_t*(__cdecl *ClapEntryProc)(void);

        auto proc = (ClapEntryProc)m_symLoader.getSymbol("clap_entry");
        if (!proc)
        {
            DE_ERROR("No 'clap_entry' proc found. ",uri)
            return;
        }

        DE_TRACE("Got proc")

        const clap_plugin_entry_t* entry = proc();
        if (!entry)
        {
            DE_ERROR("No entry. ",uri)
            return;
        }

        DE_TRACE("Got entry")

        m_bIsDsoInit = entry->init( uri.c_str());
        if (!m_bIsDsoInit)
        {
            DE_ERROR("No init. ",uri)
            return;
        }

        DE_TRACE("Got init")

        m_factory = (const clap_plugin_factory*)entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
        if (!m_factory)
        {
            DE_ERROR("No factory. ",uri)
            return;
        }

        DE_TRACE("Got factory")

        u32 numPlugins = m_factory->get_plugin_count(m_factory);
        if (numPlugins == 0)
        {
            DE_ERROR("No plugins counted. ",uri)
            return;
        }

        DE_OK("Got (",numPlugins,") plugins.")

        const clap_plugin_descriptor* desc = m_factory->get_plugin_descriptor(m_factory, 0);
        if (!desc)
        {
            DE_ERROR("No plugin[0] descriptor. ",uri)
            return;
        }

        DE_TRACE("Got desc")

        m_plugin = m_factory->create_plugin(m_factory, getHost(), desc->id);
        if (!m_plugin)
        {
            DE_ERROR("No plugin[0] created. ",uri)
            return;
        }

        DE_TRACE("Got plugin")

        if (!m_plugin->init(m_plugin))
        {
            DE_ERROR("No plugin[0] initialized. ",uri)
            m_plugin->destroy(m_plugin);
            m_plugin = nullptr;
            return;
        }

        DE_TRACE("Got plugin init")

        m_numInputs = dumpInputs(m_plugin);
        m_numOutputs = dumpOutputs(m_plugin);
        m_numParams = dumpParams(m_plugin);

        DE_TRACE("m_numInputs = ",m_numInputs)
        DE_TRACE("m_numOutputs = ",m_numOutputs)
        DE_TRACE("m_numParams = ",m_numParams)
/*
        auto state = (const clap_plugin_state_t*)
            m_plugin->get_extension(m_plugin, CLAP_EXT_STATE);

        if (state)
        {
            CLAP_OutputStream out;
            out.stream.ctx = &out;
            out.stream.write = &CLAP_OutputStream::write;

            // Save
            state->save(m_plugin, &out.stream);

            // Load
            //state->load(m_plugin, stream);
        }


        auto preset = (const clap_plugin_preset_load_t*)
            m_plugin->get_extension(m_plugin, CLAP_EXT_PRESET_LOAD);

        if (preset)
        {
            preset->from_location(load_from_file(m_plugin, "/path/to/preset");
        }
*/

        dsp_init( 4*512, 2, 48000 );

        // reactivate();

        if (!m_plugin->activate)
        {
            DE_ERROR("No activate")
            return;
        }

        if (!m_plugin->activate(m_plugin,
            m_sampleRate,
            m_blockSize,
            m_blockSize))
        {
            DE_ERROR("")
        }
/*
        DE_DEBUG("VST plugin = ", dbFileBase(m_uri))
        DE_DEBUG("VST plugin dir = ", m_directoryMultiByte)
        DE_TRACE("VST plugin isSynth = ",m_bIsSynth)
        DE_TRACE("VST plugin hasEditor = ",bHasEditor)
        DE_TRACE("VST plugin programCount = ",m_numPrograms)
        DE_TRACE("VST plugin parameterCount = ",m_numParams)
        DE_TRACE("VST plugin inputCount = ",m_numInputs)
        DE_TRACE("VST plugin outputCount = ",m_numOutputs)
        DE_TRACE("VST plugin can float replacing = ",getFlags( effFlagsCanReplacing ))
        DE_TRACE("VST plugin can double replacing = ",getFlags( effFlagsCanDoubleReplacing ))
        DE_TRACE("VST plugin has program chunks = ",getFlags( effFlagsProgramChunks ))

        //connect( m_editorWindow, SIGNAL(closed()),
        //       this,           SLOT(on_editorClosed()), Qt::QueuedConnection );

        if (bHasEditor)
        {
            m_editor = new VST2_Editor(m_vst, nullptr );
        }
*/

        // setBypassed( isBypassed() );
        m_bIsBypassed = false;
        m_bIsPluginOpen = true;
    }

    PluginEditorWindow* getEditor()
    {
        return m_editor;
    }

    void setInputSignal( IDspChainElement* input, int i = 0 )
    {
        m_inputSignal = input;
    }

    void clearInputSignals()
    {
        m_inputSignal = nullptr;
    }

    void dsp_init(u64 frames, u32 channels, u32 sampleRate)
    {
        if ( m_blockSize != frames )
        {
            m_blockSize = frames;
            m_bNeedSetup = true;
        }

        if ( m_sampleRate != sampleRate )
        {
            m_sampleRate = sampleRate;
            m_bNeedSetup = true;
        }
#if 0
        if ( m_vst && m_bNeedSetup )
        {
            m_bNeedSetup = false;

            dispatcher(effStopProcess);
            dispatcher(effMainsChanged, 0, 0);

            // Prepare input buffer + input channel heads ( planar = non-interleaved )
            // Prepare output buffer + output channel heads ( planar = non-interleaved )
            m_sampleBuffers.setup(m_numInputs,m_numOutputs, m_blockSize);

            // Setup VST plugin
            dispatcher(effSetSampleRate, 0, 0, 0, float( m_sampleRate ) );
            dispatcher(effSetBlockSize, 0, m_blockSize);
            dispatcher(effSetProcessPrecision, 0, kVstProcessPrecision32);
            dispatcher(effMainsChanged, 0, 1);
            dispatcher(effStartProcess);
            //dispatcher(effSetProgram, 0, 0, 0);
        }
#endif
    }

    void dsp_read(f64 pts,
                  u32 frames,
                  u32 sampleRate,
                  f32* __restrict__ outL,
                  f32* __restrict__ outR)
    {
        if ( !outL || !outR )
        {
            throw std::runtime_error("No dst audio dsp buffer in VST2_Plugin::readSamples()!");
        }
#if 0
        //===============================
        // VST2 processing is inactive:
        //===============================

        if ( !m_bIsPluginOpen || m_bIsBypassed)
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

            return; // We relayed samples or filled output with zeroes
        }

        //===============================
        // VST2 processing is active:
        //===============================

        dsp_init(frames,2,sampleRate);

        if ( m_inputSignal )
        {
            m_inputSignal->dsp_read( pts, frames, sampleRate,
                m_sampleBuffers.m_iBuffers.at(0).data(),
                m_sampleBuffers.m_iBuffers.at(1).data() );

            for (int i = 2; i < int(m_numInputs); ++i)
            {
                m_sampleBuffers.zeroInput(i);
            }
        }
        else
        {
            m_sampleBuffers.zeroInputs();
        }

        // ======================================================
        // We support legacy (worst-case) "in-place" processing
        // by copying numInputs to output buffers.
        // ======================================================

        const auto bytesPerChannel = u64(frames) * sizeof(float);

        // TODO: Maybe move to m_vst->process() case only.
        // Copy available input to output channels:
        for (int i = 0; i < m_numInputs; ++i)
        {
            std::memcpy(m_sampleBuffers.m_oBuffers.at(i).data(),
                        m_sampleBuffers.m_iBuffers.at(i).data(),
                        bytesPerChannel);
        }

        // Fill remaining output channels with silence (0.0f).
        for (int i = m_numInputs; i < m_sampleBuffers.m_maxChannels; ++i)
        {
            std::memset(m_sampleBuffers.m_oBuffers.at(i).data(),
                        0, bytesPerChannel);
        }

        // ======================================================
        // Process MIDI messages:
        // ======================================================
        processVstMidiEvents();

        // ======================================================
        // Process Audio samples:
        // ======================================================
        if (m_vst->processReplacing)
        {
            m_vst->processReplacing(
                m_vst,
                m_sampleBuffers.m_iHeads.data(),
                m_sampleBuffers.m_oHeads.data(),
                frames );
        }
        else
        {
            if (m_vst->process)
            {
                m_vst->process(
                    m_vst,
                    m_sampleBuffers.m_iHeads.data(),
                    m_sampleBuffers.m_oHeads.data(),
                    frames );
            }
            else
            {
                DE_ERROR("Either only double processing or no processing at all!")
            }
        }

        // ======================================================
        // Write (L+R) VST audio output back to DspChain.
        // ======================================================

        m_framePos += frames; // atomic.

        // Copy [L]eft channel:
        std::memcpy(outL,
                    m_sampleBuffers.m_oBuffers.at(0).data(),
                    bytesPerChannel);

        // Copy [R]ight channel:
        std::memcpy(outR,
                    m_sampleBuffers.m_oBuffers.at(1).data(),
                    bytesPerChannel);

#endif
        // Thank you for participating in our DspChain dear plugin.
    }

    // This function is called from refillCallback() which is running in audio thread.
    void
    processVstMidiEvents()
    {
        m_midiClock.restart();

#if 0
        m_vstMidiEvents.clear();
        if ( auto l = m_vstMidi.lock() )
        {
            std::swap( m_vstMidiEvents, m_vstMidi.events );
            //m_vstMidi.events.clear();
        }

        auto const n = m_vstMidiEvents.size();
        if ( n > 0 )
        {
            auto const m = sizeof( VstEvents ) +
                           sizeof( VstEvent* ) * n;
            m_vstEventBuffer.resize( m );
            auto vstEvents = reinterpret_cast< VstEvents* >( m_vstEventBuffer.data() );
            memset( vstEvents, 0, sizeof( VstEvents ) );

            vstEvents->numEvents = n;
            vstEvents->reserved = 0;
            for ( size_t i = 0; i < n; ++i )
            {
                vstEvents->events[ i ] = reinterpret_cast< VstEvent* >( &m_vstMidiEvents[ i ] );
            }
            //DE_ERROR("Dispatch MIDI n = ",n)
            dispatcher( effProcessEvents, 0, 0, vstEvents );
        }
#endif
    }

    void onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
    {
        if ( !m_bIsSynth )
        {
            return;
        }
#if 0
        VstMidiEvent e;
        e.type        = kVstMidiType;
        e.byteSize    = sizeof( VstMidiEvent );
        e.flags       = kVstMidiEventIsRealtime;
        e.midiData[0] = static_cast<char>( msg.status);
        e.midiData[1] = static_cast<char>( msg.data1 );
        e.midiData[2] = static_cast<char>( msg.data2 );

        // HOPEFULLY that fixes missing NoteOff events:
        // Pianos work ok without that, but monophonic synth are
        // beasts on a higher level...
        double dt = m_midiClock.now(); // Clock is restarted every callback call.
        int deltaFrames = std::clamp(
                            int(dt * m_sampleRate),
                            int(0),
                            int(m_blockSize) - 10);

        // if (deltaFrames < 0)
        // {
        //     //DE_WARN("deltaFrames(",deltaFrames,") < 0")
        //     deltaFrames = 0;
        // }
        // if (deltaFrames > m_blockSize - 10)
        // {
        //     //DE_WARN("deltaFrames(",deltaFrames,") >= blockSize(",m_blockSize,")")
        //     deltaFrames = m_blockSize - 10;
        // }
        e.deltaFrames = deltaFrames; // <- Yay relative to start of audio callback

        // Special event: All Notes Off (Bn 7B 00):
        if (((msg.status & 0xF0) == 0xB0) &&
             (msg.data1 == 0x7B) &&
             (msg.data2 == 0x00) )
        {
            if ( auto l = m_vstMidi.lock() )
            {
                m_vstMidi.events.clear();
            }
            return;
        }

        //size_t n = 0;
        if ( auto l = m_vstMidi.lock() )
        {
            m_vstMidi.events.push_back( e );
            //n = m_vstMidi.events.size();
        }
#endif
        // DE_DEBUG("events(",n,"), byte1(",dbHex(byte1),"), data1(",dbHex(data1),"), data2(",dbHex(data2),")")
    }

    void onMidiMessage(f64 pts, const midi::MidiMessage& msg)
    {
        DE_WARN("Not implemented, ", msg.size())
    }

};

// ============================================================================
CLAP_Plugin::CLAP_Plugin()
// ============================================================================
   : _d( new CLAP_Plugin_Impl )
{
    DE_TRACE("")
}

CLAP_Plugin::~CLAP_Plugin()
{
    DE_TRACE("")
    App::instance()->getMidiCentral().deregisterListener(this);
    delete _d;
}

// ===================================================

void CLAP_Plugin::dsp_init(u64 frames, u32 channels, u32 sampleRate)
{
    _d->dsp_init(frames, channels, sampleRate);
}

void CLAP_Plugin::dsp_read(f64 pts,
                          u32 frames,
                          u32 sampleRate,
                          f32* __restrict__ L,
                          f32* __restrict__ R)
{
    _d->dsp_read(pts, frames, sampleRate, L, R);
}

u32 CLAP_Plugin::dsp_getInputSignalCount() const
{
    return 1;
}

IDspChainElement* CLAP_Plugin::dsp_getInputSignal(int i)
{
    return _d->m_inputSignal;
}

void CLAP_Plugin::dsp_setInputSignal(IDspChainElement* inSignal, int i)
{
    _d->setInputSignal(inSignal, i);
}

void CLAP_Plugin::dsp_clearInputSignals()
{
    _d->clearInputSignals();
}

bool CLAP_Plugin::isBypassed() const
{
    return _d->m_bIsBypassed;
}

void CLAP_Plugin::setBypassed( bool bBypassed )
{
    _d->m_bIsBypassed = bBypassed;
}

// ===================================================

const ITrack* CLAP_Plugin::getTrack() const { return _d->m_track; }

ITrack* CLAP_Plugin::getTrack() { return _d->m_track; }

void CLAP_Plugin::setTrack(ITrack* track) { _d->m_track = track; }

// ===================================================

u32 CLAP_Plugin::getPluginId() const { return _d->m_pluginId; }

void CLAP_Plugin::setPluginId( u32 pluginId ) { _d->m_pluginId = pluginId; }

// ===================================================

std::string CLAP_Plugin::getUri() const { return _d->m_uri; }

std::string CLAP_Plugin::getName() const { return _d->m_pluginName; }

std::string CLAP_Plugin::getVendor() const { return _d->m_pluginVendor; }

// ===================================================

void CLAP_Plugin::openPlugin( std::string uri )
{
    _d->openPlugin( uri );
}

void CLAP_Plugin::closePlugin()
{
    _d->closePlugin();
}

bool CLAP_Plugin::isPluginOpen() const
{
    return _d->m_bIsPluginOpen;
}

bool CLAP_Plugin::isSynth() const
{
    return _d->m_bIsSynth;
}

PluginEditorWindow* CLAP_Plugin::getEditor()
{
    return _d->getEditor();
}

// ===================================================

void CLAP_Plugin::onMidiMessage(f64 pts, const midi::MidiMessage& msg)
{
    _d->onMidiMessage(pts, msg);
}

void CLAP_Plugin::onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
{
    _d->onShortMidiMessage(pts, msg);
}

// ===================================================

u32 CLAP_Plugin::getProgramCount() const
{
    return 0;
}

int CLAP_Plugin::getProgram() const
{
    return 0;
}

void CLAP_Plugin::setProgram( int i )
{

}

// ===================================================

u32 CLAP_Plugin::getParameterCount() const
{
    return 0;
}

f32 CLAP_Plugin::getParameter(int i) const
{
    return 0.0f;
}

void CLAP_Plugin::setParameter(int i, f32 value)
{

}

} // end namespace audio.
} // end namespace de.

#endif // BENNI_USE_CLAP

#if 0

#pragma once
#include <Windows.h>
#include <QString>
#include <QWidget>

#include <clap/clap.h>
#include <vector>
#include <string>

class ClapHostWinQt {
public:
    ClapHostWinQt(const QString& pluginPath)
        : m_pluginPath(pluginPath.toStdString())
    {
        loadLibrary();
        loadFactory();
        createPlugin();
    }

    ~ClapHostWinQt() {
        if (m_plugin) {
            m_plugin->destroy(m_plugin);
            m_plugin = nullptr;
        }
        unloadLibrary();
    }

    // ---------------------------------------------------------
    // Audio Configuration
    // ---------------------------------------------------------
    void setSampleRate(double sr) {
        m_sampleRate = sr;
        reactivate();
    }

    void setBlockSize(uint32_t bs) {
        m_blockSize = bs;
        m_minBlock = m_maxBlock = bs;
        reactivate();
    }

    void setChannels(uint32_t inCh, uint32_t outCh) {
        m_inChannels = inCh;
        m_outChannels = outCh;
    }

    // ---------------------------------------------------------
    // GUI (Qt → Win32 HWND)
    // ---------------------------------------------------------
    bool openEditor(QWidget* parentWidget) {
        if (!m_plugin) return false;

        const auto* gui = (const clap_plugin_gui*)
            m_plugin->get_extension(m_plugin, CLAP_EXT_GUI);
        if (!gui) return false;

        if (!gui->is_api_supported(m_plugin, CLAP_WINDOW_API_WIN32, true))
            return false;

        if (!gui->create(m_plugin, CLAP_WINDOW_API_WIN32, 0))
            return false;

        HWND hwnd = (HWND)parentWidget->winId();

        clap_window win {};
        win.api = CLAP_WINDOW_API_WIN32;
        win.ptr = hwnd;

        if (!gui->set_parent(m_plugin, &win))
            return false;

        gui->show(m_plugin);
        return true;
    }

    void closeEditor() {
        const auto* gui = (const clap_plugin_gui*)
            m_plugin->get_extension(m_plugin, CLAP_EXT_GUI);
        if (!gui) return;
        gui->hide(m_plugin);
        gui->destroy(m_plugin);
    }

    // ---------------------------------------------------------
    // Audio Processing
    // ---------------------------------------------------------
    void process(float** inputs, float** outputs, uint32_t frames) {
        if (!m_plugin) return;

        clap_audio_buffer inBuf {};
        clap_audio_buffer outBuf {};

        inBuf.data32 = inputs;
        inBuf.channel_count = m_inChannels;

        outBuf.data32 = outputs;
        outBuf.channel_count = m_outChannels;

        clap_process p {};
        p.frames_count = frames;
        p.steady_time = m_steadyTime;
        p.audio_inputs = &inBuf;
        p.audio_outputs = &outBuf;
        p.audio_inputs_count = 1;
        p.audio_outputs_count = 1;
        p.in_events = &m_inEvents;
        p.out_events = &m_outEvents;

        m_plugin->process(m_plugin, &p);

        m_steadyTime += frames;
        m_inEventList.clear();
    }

    // ---------------------------------------------------------
    // MIDI Sending
    // ---------------------------------------------------------
    void sendNoteOn(int ch, int note, int vel, uint32_t frame = 0) {
        clap_event_midi ev {};
        ev.header.size = sizeof(ev);
        ev.header.time = frame;
        ev.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
        ev.header.type = CLAP_EVENT_MIDI;
        ev.port_index = 0;
        ev.data[0] = 0x90 | (ch & 0x0F);
        ev.data[1] = note;
        ev.data[2] = vel;
        pushEvent(ev.header);
    }

    void sendNoteOff(int ch, int note, int vel, uint32_t frame = 0) {
        clap_event_midi ev {};
        ev.header.size = sizeof(ev);
        ev.header.time = frame;
        ev.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
        ev.header.type = CLAP_EVENT_MIDI;
        ev.port_index = 0;
        ev.data[0] = 0x80 | (ch & 0x0F);
        ev.data[1] = note;
        ev.data[2] = vel;
        pushEvent(ev.header);
    }

private:
    // ---------------------------------------------------------
    // Members (m_ prefix)
    // ---------------------------------------------------------
    std::string m_pluginPath;
    HMODULE m_library = nullptr;
    const clap_plugin_factory* m_factory = nullptr;
    const clap_plugin* m_plugin = nullptr;

    double m_sampleRate = 44100.0;
    uint32_t m_blockSize = 512;
    uint32_t m_minBlock = 512;
    uint32_t m_maxBlock = 512;
    uint32_t m_inChannels = 2;
    uint32_t m_outChannels = 2;

    int64_t m_steadyTime = 0;

    // ---------------------------------------------------------
    // Event Queues
    // ---------------------------------------------------------
    std::vector<clap_event_header_t*> m_inEventList;
    std::vector<clap_event_header_t*> m_outEventList;

    static uint32_t eventsSize(const clap_input_events* list) {
        auto self = (std::vector<clap_event_header_t*>*)list->ctx;
        return (uint32_t)self->size();
    }

    static const clap_event_header_t* eventsGet(const clap_input_events* list, uint32_t index) {
        auto self = (std::vector<clap_event_header_t*>*)list->ctx;
        if (index >= self->size()) return nullptr;
        return (*self)[index];
    }

    static void eventsPush(const clap_output_events* list, const clap_event_header_t* event) {
        auto self = (std::vector<clap_event_header_t*>*)list->ctx;
        self->push_back((clap_event_header_t*)event);
    }

    clap_input_events m_inEvents {
        &m_inEventList,
        &eventsSize,
        &eventsGet
    };

    clap_output_events m_outEvents {
        &m_outEventList,
        &eventsPush
    };

    void pushEvent(const clap_event_header_t& ev) {
        // In real host: allocate/copy event
        m_inEventList.push_back((clap_event_header_t*)&ev);
    }

    // ---------------------------------------------------------
    // Plugin Loading
    // ---------------------------------------------------------
    void loadLibrary() {
        m_library = LoadLibraryA(m_pluginPath.c_str());
    }

    void unloadLibrary() {
        if (m_library)
            FreeLibrary(m_library);
        m_library = nullptr;
    }

    void loadFactory() {
        if (!m_library) return;

        auto entry = (const clap_plugin_entry_t*(*)())GetProcAddress(m_library, "clap_entry");
        if (!entry) return;

        const clap_plugin_entry_t* e = entry();
        e->init(m_pluginPath.c_str());

        m_factory = (const clap_plugin_factory*)e->get_factory(CLAP_PLUGIN_FACTORY_ID);
    }

    void createPlugin() {
        if (!m_factory) return;

        uint32_t count = m_factory->get_plugin_count(m_factory);
        if (count == 0) return;

        const clap_plugin_descriptor* desc =
            m_factory->get_plugin_descriptor(m_factory, 0);

        m_plugin = m_factory->create_plugin(m_factory, getHost(), desc->id);

        if (!m_plugin) return;

        if (!m_plugin->init(m_plugin)) {
            m_plugin->destroy(m_plugin);
            m_plugin = nullptr;
            return;
        }

        reactivate();
    }

    // ---------------------------------------------------------
    // Host Descriptor
    // ---------------------------------------------------------
    static const clap_host* getHost() {
        static clap_host host {
            CLAP_VERSION,
            "QtClapHost",
            "Qt CLAP Host",
            "https://example.com",
            "1.0",
            nullptr, nullptr, nullptr, nullptr
        };
        return &host;
    }

    void reactivate() {
        if (m_plugin && m_plugin->activate)
            m_plugin->activate(m_plugin, m_sampleRate, m_minBlock, m_maxBlock);
    }
};

#endif