#include <de/audio/plugin/details/VST2_Plugin.h>

#ifdef BENNI_USE_VST2

#include <de/audio/plugin/details/VST2_Editor.h>
#include <de/audio/plugin/details/BasePluginUtils.h>
#include <App.h>
namespace de {
namespace audio {
namespace {

constexpr u64 GUARD = 256; // 64 extra bytes for "out-of-bounds" bugs.

//===============================
struct VST2_SampleBuffers
//===============================
{
    u32 m_maxChannels = 0;

    std::vector<TAlignedVector<float>> m_iBuffers;
    std::vector<TAlignedVector<float>> m_oBuffers;

    std::vector<float*> m_iHeads;
    std::vector<float*> m_oHeads;

    void setup(int numInputs, int numOutputs, int blockSize)
    {
        // Input & Output side get same worst case amount
        // of channels to enable "in-place" legacy/old mode.
        const auto maxChannels = std::max(2, // Atleast stereo
                    std::max(numInputs, numOutputs));

        // Do work...
        m_maxChannels = maxChannels;
        m_iBuffers.resize(maxChannels);
        m_oBuffers.resize(maxChannels);
        m_iHeads.resize(maxChannels);
        m_oHeads.resize(maxChannels);

        for (auto & inputBuffer : m_iBuffers)
        {
            inputBuffer.resize(blockSize + GUARD);
        }
        for (auto & outputBuffer : m_oBuffers)
        {
            outputBuffer.resize(blockSize + GUARD);
        }

        for (int i = 0; i < maxChannels; ++i)
        {
            m_iHeads[i] = m_iBuffers[i].data();
        }

        for (int i = 0; i < maxChannels; ++i)
        {
            m_oHeads[i] = m_oBuffers[i].data();
        }
    }

    void zeroInput(int i)
    {
        if (i >= int(m_iBuffers.size())) return;
        std::fill(m_iBuffers.at(i).begin(),
                  m_iBuffers.at(i).end(), 0.0f);
    }

    void zeroOutput(int i)
    {
        if (i >= int(m_oBuffers.size())) return;
        std::fill(m_oBuffers.at(i).begin(),
                  m_oBuffers.at(i).end(), 0.0f);
    }

    void zeroInputs()
    {
        for (auto & buffer : m_iBuffers)
        {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
        }
    }

    void zeroOutputs()
    {
        for (auto & buffer : m_oBuffers)
        {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
        }
    }
};

} // end namespace

//===============================
struct VST2_Plugin_Impl
//===============================
{
    u32 m_pluginId = 0;
    bool m_bIsPluginOpen = false;
    bool m_bNeedSetup = true;
    bool m_bIsSynth = false;
    bool m_bIsBypassed = false;

    ITrack* m_track = nullptr;
    PluginEditorWindow* m_editor = nullptr;
    IDspChainElement* m_inputSignal = nullptr;
    AEffect* m_vst = nullptr;
    u32 m_numPrograms = 0;
    u32 m_numParams = 0;
    u32 m_numInputs = 0;
    u32 m_numOutputs = 0;
    u32 m_sampleRate = 0;
    u32 m_blockSize = 0;
    std::atomic< u64 > m_framePos = 0;

    SymbolLoader m_symLoader;
    VstTimeInfo m_timeInfo;
    std::string m_uri;                 // VST2_Plugin file name
    std::string m_directoryMultiByte;
    std::string m_pluginName;
    std::string m_pluginVendor;

    VST2_SampleBuffers m_sampleBuffers;
    NormalizedSumComputer m_normalizedSumComputer;
    // VST seems to work channelwise / planar, not interleaved audio.
    // std::vector< f32 > m_outBuffer;
    // std::vector< f32*> m_outBufferHeads;
    // std::vector< f32 > m_inBuffer;
    // std::vector< f32*> m_inBufferHeads;

    // VST midi event handling
    PluginClock m_midiClock;
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

    // ============================================================================
    VST2_Plugin_Impl()
    // ============================================================================
        : m_pluginId{ 0 }
        , m_bIsPluginOpen{ false }
        , m_bNeedSetup{ true }
        , m_bIsSynth{ false }
        , m_bIsBypassed{ false }
        , m_track{ nullptr }
        , m_editor{ nullptr }
        , m_inputSignal{ nullptr }
        , m_vst{ nullptr }
        , m_numPrograms{ 0 }
        , m_numParams{ 0 }
        , m_numInputs{ 0 }
        , m_numOutputs{ 0 }
        , m_sampleRate{ 0 }
        , m_blockSize{ 0 }
        , m_framePos{ 0 }
    {
        DE_DEBUG("")
        dsp_init( 64, 2, 48000 );
    }

    ~VST2_Plugin_Impl()
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

        dispatcher(effMainsChanged, 0, 0);  // Stop plugin
        dispatcher(effStopProcess);         // Stop plugin

        if (m_editor)
        {
            DE_TRACE("Close editor")
            dispatcher(effEditClose, 0, 0, nullptr, 0.0f);
            m_editor->enableClosing();
            m_editor->close();
            m_editor->deleteLater();
            //delete m_editor;
            m_editor = nullptr;
        }

        dispatcher(effClose);               // Stop plugin

        m_symLoader.close();
        // if ( m_dllHandle )                  // Close plugin
        // {
        //     HMODULE hModule = reinterpret_cast< HMODULE >( m_dllHandle );
        //     FreeLibrary(hModule);
        //     m_dllHandle = 0;
        // }

        m_framePos = 0;

        //m_loadButton->setIcon( QIcon() );
        //m_editorImage->hide();
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

        m_symLoader.open( uri );
        if ( !m_symLoader.is_open() )
        {
            DE_WARN("No library ",uri)
            return;
        }

        typedef AEffect* (VstEntryProc)(audioMasterCallback);

        auto proc = reinterpret_cast< VstEntryProc* >( m_symLoader.getSymbol("VSTPluginMain") );
        if ( !proc )
        {
            proc = reinterpret_cast< VstEntryProc* >( m_symLoader.getSymbol("main") );
        }
        if ( !proc )
        {
            DE_WARN("No VST entry point found, ",uri)
            return;
        }

        m_vst = proc( hostCallback_static );
        if ( !m_vst )
        {
            DE_WARN("Not a VST plugin (I.) ",uri)
            return;
        }

        if ( m_vst->magic != kEffectMagic )
        {
            DE_WARN("Not a VST plugin with kEffectMagic, ",uri)
            return;
        }

        m_vst->user = this;
        m_numPrograms = m_vst->numPrograms;
        m_numParams = m_vst->numParams;
        m_numInputs = m_vst->numInputs;
        m_numOutputs = m_vst->numOutputs;
        m_bIsSynth = getFlags( effFlagsIsSynth );
        bool bHasEditor = getFlags( effFlagsHasEditor );

        // Get plugin Name
        char pluginName[kVstMaxEffectNameLen] = {0};
        dispatcher(effGetEffectName, 0, 0, pluginName, 0);
        if (strlen(pluginName) > 0)
        {
            m_pluginName = pluginName;
        }

        // Get plugin Vendor
        char pluginVendor[kVstMaxVendorStrLen] = {0};
        dispatcher(effGetVendorString, 0, 0, pluginVendor, 0);
        if (strlen(pluginVendor) > 0)
        {
            m_pluginVendor = pluginVendor;
        }

        dispatcher(effOpen);

        m_sampleBuffers.setup(m_numInputs,m_numOutputs, m_blockSize);
        m_bNeedSetup = true;
        dsp_init(256, 2, 48000);

        DE_DEBUG("VST2 plugin File = ", dbFileBase(m_uri))
        DE_DEBUG("VST2 plugin Dir = ", m_directoryMultiByte)
        DE_DEBUG("VST2 plugin Name = ", m_pluginName)
        DE_DEBUG("VST2 plugin Vendor = ", m_pluginVendor)
        DE_TRACE("VST2 plugin Synth = ",m_bIsSynth)
        DE_TRACE("VST2 plugin Editor = ",bHasEditor)
        DE_TRACE("VST2 plugin Programs = ",m_numPrograms)
        DE_TRACE("VST2 plugin Parameters = ",m_numParams)
        DE_TRACE("VST2 plugin Inputs = ",m_numInputs)
        DE_TRACE("VST2 plugin Outputs = ",m_numOutputs)
        DE_TRACE("VST2 plugin CanFloat32 = ",getFlags(effFlagsCanReplacing ))
        DE_TRACE("VST2 plugin CanFloat64 = ",getFlags(effFlagsCanDoubleReplacing ))
        DE_TRACE("VST2 plugin CanProgramChunks = ",getFlags(effFlagsProgramChunks))

        if (bHasEditor)
        {
            m_editor = new VST2_Editor(m_vst, nullptr );
        }

        const int currentProgram = dispatcher(effGetProgram);
        std::vector< IPlugin::ProgramInfo > programs;
        programs.reserve(m_numPrograms);
        for (int i = 0; i < m_numPrograms; ++i)
        {
            IPlugin::ProgramInfo pi;
            pi.id = i;

            dispatcher(effSetProgram, 0, i);

            char name[kVstMaxProgNameLen+GUARD] = {0};
            dispatcher(effGetProgramName, i, 0, name, 0);
            pi.name = name;

            programs.emplace_back( std::move( pi ) );
        }

        dispatcher(effSetProgram, 0, currentProgram);

        DE_DEBUG("Program.Count = ",programs.size())
        for (auto & pi : programs)
        {
            DE_DEBUG("Program",pi.str())
        }

        std::vector< IPlugin::ParamInfo > params;
        params.reserve(m_numParams);
        for (int i = 0; i < m_numParams; ++i)
        {
            IPlugin::ParamInfo pi;
            pi.id = i;
            pi.nowValue = m_vst->getParameter(m_vst, i);

            // GUARD already solved a "out of bounds" bug in a plugin
            // where it overwrote pi.id, so totally worth it!!!
            char name[kVstMaxParamStrLen+GUARD] = {0};
            dispatcher(effGetParamName, i, 0, name, 0);
            pi.name = name;

            char label[kVstMaxParamStrLen+GUARD] = {0};
            dispatcher(effGetParamLabel, i, 0, label, 0);
            pi.unit = label;

            char display[kVstMaxParamStrLen+GUARD] = {0};
            dispatcher(effGetParamDisplay, i, 0, display, 0);
            pi.disp = display;

            params.emplace_back( std::move( pi ) );
        }

        DE_DEBUG("Param.Count = ",params.size())
        for (auto & pi : params)
        {
            DE_DEBUG("Param",pi.str())
        }

        m_bIsBypassed = false;
        m_bIsPluginOpen = true;
    }

    PluginEditorWindow* getEditor()
    {
        return m_editor;
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

        if ( m_vst && m_bNeedSetup )
        {
            DE_WARN("frames(",frames,", channels(",channels,"), sampleRate(",sampleRate,")")
            m_bNeedSetup = false;

            dispatcher(effStopProcess);
            dispatcher(effMainsChanged, 0, 0);

            // Prepare input buffer + input channel heads ( planar = non-interleaved )
            // Prepare output buffer + output channel heads ( planar = non-interleaved )

            // Setup VST plugin
            dispatcher(effSetSampleRate, 0, 0, 0, float( m_sampleRate ) );
            dispatcher(effSetBlockSize, 0, m_blockSize);
            dispatcher(effSetProcessPrecision, 0, kVstProcessPrecision32);
            dispatcher(effMainsChanged, 0, 1);
            dispatcher(effStartProcess);
            //dispatcher(effSetProgram, 0, 0, 0);
        }
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

        // For audio-level-meter
        m_normalizedSumComputer.calc(outL, outR, frames);

        // Thank you for participating in our DspChain dear plugin.
    }

    VstIntPtr
    hostCallback( VstInt32 opcode, VstInt32 index, VstIntPtr value, void* ptr, float opt )
    {
        switch(opcode)
        {
        default:                                break;
        case audioMasterVersion:                return kVstVersion;
        case audioMasterCurrentId:              return m_vst->uniqueID;
        case audioMasterGetSampleRate:          return m_sampleRate;
        case audioMasterGetBlockSize:           return m_blockSize;
        case audioMasterGetCurrentProcessLevel: return kVstProcessLevelUnknown;
        case audioMasterGetAutomationState:     return kVstAutomationOff;
        case audioMasterGetLanguage:            return kVstLangEnglish;
        case audioMasterGetVendorVersion:       return 1;
        case audioMasterGetVendorString:
            strcpy_s(static_cast<char*>(ptr), kVstMaxVendorStrLen, "Abenton Live64");
            return 1;
        case audioMasterGetProductString:
            strcpy_s(static_cast<char*>(ptr), kVstMaxProductStrLen, "Abenton Live64");
            return 1;
        case audioMasterGetTime:
            m_timeInfo.flags      = 0;
            m_timeInfo.samplePos  = m_framePos;
            m_timeInfo.sampleRate = m_sampleRate;
            //DE_DEBUG("audioMasterGetTime(",m_timeInfo.samplePos,")")
            return reinterpret_cast< VstIntPtr >( &m_timeInfo );
        case audioMasterGetDirectory:
            return reinterpret_cast< VstIntPtr >( m_directoryMultiByte.c_str() );
        case audioMasterIdle:
            if ( m_editor ) { dispatcher(effEditIdle); }
            break;
        case audioMasterSizeWindow:
            if ( m_editor )
            {
                //RECT rc {};
                //GetWindowRect(m_editorWinHandle, &rc);
                //rc.right = rc.left + static_cast<int>(index);
                //rc.bottom = rc.top + static_cast<int>(value);
                //resizeEditor(rc);
                int w = int( index );
                int h = int( value );
                int x = m_editor->x();
                int y = m_editor->y();
                //DE_DEBUG("audioMasterSizeWindow(",w,",",h,"), pos(",x,",",y,")")
                //;
                //setMaximumSize( w, h );
                m_editor->setMinimumSize( w, h );
                m_editor->move(x,y);
            }
            break;
        case audioMasterCanDo:
            for ( const char** pp = getCapabilities(); *pp; ++pp )
            {
                if ( strcmp(*pp, static_cast<const char*>(ptr)) == 0 )
                {
                    return 1;
                }
            }
            return 0;
        }
        return 0;
    }

    static VstIntPtr
    hostCallback_static( AEffect* effect, VstInt32 opcode, VstInt32 index, VstIntPtr value, void *ptr, float opt )
    {
        if ( effect && effect->user )
        {
            auto me = static_cast< VST2_Plugin_Impl* >( effect->user );
            return me->hostCallback( opcode, index, value, ptr, opt );
        }

        switch( opcode )
        {
        case audioMasterVersion:    return kVstVersion;
        default:                    return 0;
        }
    }

    intptr_t
    dispatcher( int32_t opcode, int32_t index = 0, intptr_t value = 0, void *ptr = nullptr, float opt = 0.0f ) const
    {
        if ( !m_vst )
        {
            DE_ERROR("No plugin, bad")
            return 0;
        }
        return m_vst->dispatcher( m_vst, opcode, index, value, ptr, opt );
    }

    // This function is called from refillCallback() which is running in audio thread.
    void
    processVstMidiEvents()
    {
        m_midiClock.restart();

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
    }

    void onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
    {
        if ( !m_bIsSynth )
        {
            return;
        }

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

        // DE_DEBUG("events(",n,"), byte1(",dbHex(byte1),"), data1(",dbHex(data1),"), data2(",dbHex(data2),")")
    }

    void onMidiMessage(f64 pts, const midi::MidiMessage& msg)
    {
        DE_WARN("Not implemented, ", msg.size())
    }

    const char**
    getCapabilities() const
    {
        static const char* hostCapabilities[] =
        {
            "sendVstEvents",
            "sendVstMidiEvents",
            "sizeWindow",
            "startStopProcess",
            "sendVstMidiEventFlagIsRealtime",
            nullptr
        };
        return hostCapabilities;
    }

};


// ============================================================================
VST2_Plugin::VST2_Plugin()
// ============================================================================
   : _d( new VST2_Plugin_Impl )
{
    DE_TRACE("")
}

VST2_Plugin::~VST2_Plugin()
{
    DE_TRACE("")
    App::instance()->getMidiCentral().deregisterListener(this);
    delete _d;
}

// ===================================================

void VST2_Plugin::dsp_init(u64 frames, u32 channels, u32 sampleRate)
{
    _d->dsp_init(frames, channels, sampleRate);
}

void VST2_Plugin::dsp_read(f64 pts,
                          u32 frames,
                          u32 sampleRate,
                          f32* __restrict__ L,
                          f32* __restrict__ R)
{
    _d->dsp_read(pts, frames, sampleRate, L, R);
}

u32 VST2_Plugin::dsp_getInputSignalCount() const
{
    return 1;
}

IDspChainElement* VST2_Plugin::dsp_getInputSignal(int i)
{
    return _d->m_inputSignal;
}

void VST2_Plugin::dsp_setInputSignal(IDspChainElement* pSignal, int i)
{
    _d->m_inputSignal = pSignal;
}

void VST2_Plugin::dsp_clearInputSignals()
{
    _d->m_inputSignal = nullptr;
}

bool VST2_Plugin::isBypassed() const
{
    return _d->m_bIsBypassed;
}

void VST2_Plugin::setBypassed( bool bBypassed )
{
    _d->m_bIsBypassed = bBypassed;
}

// ===================================================

const ITrack* VST2_Plugin::getTrack() const { return _d->m_track; }

ITrack* VST2_Plugin::getTrack() { return _d->m_track; }

void VST2_Plugin::setTrack(ITrack* track) { _d->m_track = track; }

// ===================================================

u32 VST2_Plugin::getPluginId() const { return _d->m_pluginId; }

void VST2_Plugin::setPluginId( u32 pluginId ) { _d->m_pluginId = pluginId; }

// ===================================================

std::string VST2_Plugin::getUri() const { return _d->m_uri; }

std::string VST2_Plugin::getName() const { return _d->m_pluginName; }

std::string VST2_Plugin::getVendor() const { return _d->m_pluginVendor; }

// ===================================================

void VST2_Plugin::openPlugin( std::string uri )
{
    _d->openPlugin( uri );
}

void VST2_Plugin::closePlugin()
{
    _d->closePlugin();
}

bool VST2_Plugin::isPluginOpen() const
{
    return _d->m_bIsPluginOpen;
}

bool VST2_Plugin::isSynth() const
{
    return _d->m_bIsSynth;
}

PluginEditorWindow* VST2_Plugin::getEditor()
{
    return _d->getEditor();
}

// ===================================================

void VST2_Plugin::onMidiMessage(f64 pts, const midi::MidiMessage& msg)
{
    _d->onMidiMessage(pts, msg);
}

void VST2_Plugin::onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
{
    _d->onShortMidiMessage(pts, msg);
}

// ===================================================

u32 VST2_Plugin::getProgramCount() const
{
    return _d->m_numPrograms;
}

int VST2_Plugin::getProgram() const
{
    return _d->dispatcher(effGetProgram);
}

void VST2_Plugin::setProgram( int i )
{
    if (i < 0 || i >= _d->m_numPrograms)
    {
        DE_ERROR("Invalid index ",i," of ",_d->m_numPrograms)
        return;
    }
    _d->dispatcher(effSetProgram, 0, i);
}

// ===================================================

u32 VST2_Plugin::getParameterCount() const
{
    return _d->m_numParams;
}

f32 VST2_Plugin::getParameter(int i) const
{
    if (!_d->m_vst)
    {
        DE_ERROR("No vst")
        return 0.0f;
    }
    return _d->m_vst->getParameter(_d->m_vst, i);
}

void VST2_Plugin::setParameter(int i, f32 value)
{
    if (!_d->m_vst)
    {
        DE_ERROR("No vst")
        return;
    }

    _d->m_vst->setParameter(_d->m_vst, i, value);
}


float VST2_Plugin::getSpecialValue( eSpecialValue type ) const
{
    switch (type)
    {
        case IPlugin::eSV_NormalizedSumL: return _d->m_normalizedSumComputer.m_sumL;
        case IPlugin::eSV_NormalizedSumR: return _d->m_normalizedSumComputer.m_sumR;
        default: return 0.0f;
    }
}

// 🎯 1. Get plugin name
// cpp

// char name[kVstMaxProductStrLen] = {0};
// plugin->dispatcher(effGetProductString, 0, 0, name, 0);

// Alternative (older plugins):
// cpp

// plugin->dispatcher(effGetEffectName, 0, 0, name, 0);

// 🎯 2. Get vendor name
// cpp

// char vendor[kVstMaxVendorStrLen] = {0};
// plugin->dispatcher(effGetVendorString, 0, 0, vendor, 0);

// 🎯 3. Get vendor version
// cpp

// int version = plugin->dispatcher(effGetVendorVersion, 0, 0, nullptr, 0);

// 🎯 4. Get number of programs (presets)

// This is stored directly in the AEffect struct:
// cpp

// int numPrograms = plugin->numPrograms;

// 🎯 5. Get current program index
// cpp

// int currentProgram = plugin->dispatcher(effGetProgram, 0, 0, nullptr, 0);

// 🎯 6. Set current program
// cpp

// plugin->dispatcher(effSetProgram, 0, programIndex, nullptr, 0);

// 🎯 7. Get program name
// cpp

// char programName[kVstMaxProgNameLen] = {0};
// plugin->dispatcher(effGetProgramName, 0, 0, programName, 0);

// 🎯 8. Get number of parameters

// Also stored in AEffect:
// cpp

// int numParams = plugin->numParams;

// 🎯 9. Get parameter value
// cpp

// float value = plugin->getParameter(plugin, paramIndex);

// 🎯 10. Set parameter value
// cpp

// plugin->setParameter(plugin, paramIndex, value);

// 🎯 11. Get parameter name
// cpp

// char name[kVstMaxParamStrLen] = {0};
// plugin->dispatcher(effGetParamName, paramIndex, 0, name, 0);

// 🎯 12. Get parameter label (units)
// cpp

// char label[kVstMaxParamStrLen] = {0};
// plugin->dispatcher(effGetParamLabel, paramIndex, 0, label, 0);

// 🎯 13. Get parameter display (formatted value)
// cpp

// char display[kVstMaxParamStrLen] = {0};
// plugin->dispatcher(effGetParamDisplay, paramIndex, 0, display, 0);

// 🧠 Summary Table
// What you want	How to get it
// Plugin name	effGetProductString or effGetEffectName
// Vendor name	effGetVendorString
// Vendor version	effGetVendorVersion
// Number of programs	AEffect::numPrograms
// Current program	effGetProgram
// Set program	effSetProgram
// Program name	effGetProgramName
// Number of parameters	AEffect::numParams
// Parameter value	getParameter()
// Set parameter	setParameter()
// Parameter name	effGetParamName
// Parameter label	effGetParamLabel
// Parameter display	effGetParamDisplay


} // end namespace audio.
} // end namespace de.

#endif // BENNI_USE_VST2
