#include <de/audio/plugin/details/VST2_Plugin.h>
#include <de/audio/plugin/details/VST2_Editor.h>

namespace de {
namespace audio {

//===============================
struct VST2_SampleBuffers
//===============================
{
    int m_maxChannels = 0;
    int m_blockSize = 0;

    std::vector<TAlignedVector<float>> m_iBuffers;
    std::vector<TAlignedVector<float>> m_oBuffers;

    std::vector<float*> m_iHeads;
    std::vector<float*> m_oHeads;

    void setup(int numInputs, int numOutputs, int blockSize)
    {
        // Input & Output side get same worst case amount
        // of channels to enable "in-place" legacy/old mode.
        const auto maxChannels = std::max(2,
                                    std::max(numInputs, numOutputs));

        // Only continue if necessary...
        if ((m_blockSize == blockSize) &&
            (m_maxChannels == maxChannels))
        {
            return; // Nothing todo
        }

        // Do work...
        m_blockSize  = blockSize;
        m_maxChannels = maxChannels;
        m_iBuffers.resize(maxChannels);
        m_oBuffers.resize(maxChannels);
        m_iHeads.resize(maxChannels);
        m_oHeads.resize(maxChannels);

        constexpr u64 GUARD = 64; // 64 extra bytes for "out-of-bounds" bugs.

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

//===============================
struct VST2_Plugin_Impl
//===============================
{
    u32 m_pluginId = 0;
    bool m_bPluginOpen = false;
    bool m_bEditorOpen = false;
    //bool m_bEditorVisible = false;
    bool m_bNeedSetup = true;
    bool m_bHasEditor = false;
    bool m_bIsSynth = false;
    bool m_bIsBypassed = false;

    ITrack* m_track = nullptr;
    PluginEditorWindow* m_editor = nullptr;

    u32 m_numPrograms = 0;
    u32 m_numParams = 0;
    u32 m_numInputs = 0;
    u32 m_numOutputs = 0;

    IDspChainElement* m_inputSignal = nullptr;
    u32 m_sampleRate = 0;     // rate in Hz
    u32 m_bufferFrames = 0;   // frames per channel
    std::atomic< u64 > m_framePos = 0;
    u64 m_dllHandle = 0; // HMODULE

    AEffect* m_vst = nullptr;
    VstTimeInfo m_timeInfo;

    std::string m_uri;                 // VST2_Plugin file name
    std::string m_directoryMultiByte;

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

    // ============================================================================
    VST2_Plugin_Impl()
    // ============================================================================
    {
        dsp_init( 64, 2, 48000 );
    }

    ~VST2_Plugin_Impl()
    {
        closePlugin();
    }

    PluginEditorWindow* getEditor()
    {
        return m_editor;
    }

    // void setBypassed( bool bypassed ) override;
    // void sendMidi( uint8_t byte1, uint8_t data1, uint8_t data2 ) override;
    // void setInputSignal( int i, de::audio::IDspChainElement* input ) override;
    // void clearInputSignals() override;
    // void aboutToStart( uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate ) override;
    // u64 readSamples( f64 pts, f32* dst, u32 dstFrames, u32 dstChannels, uint32_t dstRate ) override;
    // // =====================================
    // // interface: IPlugin
    // // =====================================
    // bool openPlugin( de::audio::PluginInfo const & pluginInfo );
    // void closePlugin();
    // void showEditor() { setEditorVisible( true ); }
    // void hideEditor() { setEditorVisible( false ); }
    // void moveEditor( int x, int y );
    // void setEditorVisible( bool visible );
    // void setExtraMoreVisible( bool visible );

    // =====================================
    // interface: IVst2Plugin|AEffectx
    // =====================================
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
        if ( m_bufferFrames != frames )
        {
            m_bufferFrames = frames;
            m_bNeedSetup = true;
        }

        if ( m_sampleRate != sampleRate )
        {
            m_sampleRate = sampleRate;
            m_bNeedSetup = true;
        }

        if ( m_vst && m_bNeedSetup )
        {
            m_bNeedSetup = false;

            dispatcher(effStopProcess);
            dispatcher(effMainsChanged, 0, 0);

            // Prepare input buffer + input channel heads ( planar = non-interleaved )
            // Prepare output buffer + output channel heads ( planar = non-interleaved )
            m_sampleBuffers.setup(m_numInputs,m_numOutputs, m_bufferFrames);

            // Setup VST plugin
            dispatcher(effSetSampleRate, 0, 0, 0, float( m_sampleRate ) );
            dispatcher(effSetBlockSize, 0, m_bufferFrames);
            dispatcher(effSetProcessPrecision, 0, kVstProcessPrecision32);
            dispatcher(effMainsChanged, 0, 1);
            dispatcher(effStartProcess);
            dispatcher(effSetProgram, 0, 0, 0);
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

        if ( !m_bPluginOpen || m_bIsBypassed)
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

        // Thank you for participating in our DspChain dear plugin.
    }

    void openPlugin( std::string uri )
    {
        closePlugin();
        //setBypassed( true );

        if ( uri.empty() )
        {
            DE_WARN("empty filename")
            return;
        }

        DE_TRACE("uri1 = ",uri)
        uri = de::FileSystem::makeAbsolute(uri);
        DE_TRACE("uri2 = ",uri)

        // VST2_Plugin needs path/directory of itself
        m_directoryMultiByte = dbFileDir(uri);
        DE_TRACE("uri3 = ",m_directoryMultiByte)

        /*
        {
            wchar_t buf[ MAX_PATH + 1 ] {};
            wchar_t* namePtr = nullptr;
            auto const r = GetFullPathName( pluginUri().c_str(), _countof(buf), buf, &namePtr );
            if ( r && namePtr )
            {
                *namePtr = 0;
                char mbBuf[ _countof(buf) * 4 ] {};
                int ok = WideCharToMultiByte(CP_OEMCP, 0, buf, -1, mbBuf, sizeof(mbBuf), 0, 0);
                if (ok)
                {
                    m_directoryMultiByte = mbBuf;
                }
            }
        }
        */

        HMODULE dll = LoadLibraryA( uri.c_str() );
        if ( !dll )
        {
            DE_WARN("No HMODULE ",uri)
            return;
        }

        typedef AEffect* (VstEntryProc)(audioMasterCallback);
        auto proc = reinterpret_cast< VstEntryProc* >( GetProcAddress(dll, "VSTPluginMain") );

        if ( !proc )
        {
            proc = reinterpret_cast< VstEntryProc* >( GetProcAddress(dll, "main") );
        }
        if ( !proc )
        {
            DE_WARN("No VST entry point found, ",uri)
            return;
        }

        m_dllHandle = uint64_t( dll );
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
        // m_pluginInfo.m_name = de::FileSystem::fileBase( pluginUri() );
        m_numPrograms = m_vst->numPrograms;
        m_numParams = m_vst->numParams;
        m_numInputs = m_vst->numInputs;
        m_numOutputs = m_vst->numOutputs;
        m_bIsSynth = getFlags( effFlagsIsSynth );
        m_bHasEditor = getFlags( effFlagsHasEditor );

        dispatcher(effOpen);

        m_bNeedSetup = true;
        dsp_init(64, 2, 48000);

        DE_DEBUG("VST plugin = ", dbFileBase(m_uri))
        DE_DEBUG("VST plugin dir = ", m_directoryMultiByte)
        DE_TRACE("VST plugin isSynth = ",m_bIsSynth)
        DE_TRACE("VST plugin hasEditor = ",m_bHasEditor)
        DE_TRACE("VST plugin programCount = ",m_numPrograms)
        DE_TRACE("VST plugin parameterCount = ",m_numParams)
        DE_TRACE("VST plugin inputCount = ",m_numInputs)
        DE_TRACE("VST plugin outputCount = ",m_numOutputs)
        DE_TRACE("VST plugin can float replacing = ",getFlags( effFlagsCanReplacing ))
        DE_TRACE("VST plugin can double replacing = ",getFlags( effFlagsCanDoubleReplacing ))
        DE_TRACE("VST plugin has program chunks = ",getFlags( effFlagsProgramChunks ))

        //connect( m_editorWindow, SIGNAL(closed()),
        //       this,           SLOT(on_editorClosed()), Qt::QueuedConnection );

        if (m_bHasEditor)
        {
            m_editor = new VST2_Editor(m_vst, nullptr );
        }
/*
        setBypassed( m_pluginInfo.m_isBypassed );

        //DE_TRACE("VST pluginInfo = ",de_mbstr(m_pluginInfo.toWString()))
        update();

        setBypassed( isBypassed() );
*/
        m_bIsBypassed = false;
        m_bPluginOpen = true;
    }

    void closePlugin()
    {
        if ( !m_bPluginOpen )
        {
            //DE_WARN("Not vst loaded")
            return;
        }

        DE_WARN("Close ",m_uri)

        m_bPluginOpen = false;  // Set this first, so the audio callback does bypass this dsp element.

        //   if ( isSynth() )
        //   {
        //      emit removedSynth( this ); // Unregister synth from MIDI keyboards
        //   }

        DE_WARN("Stop vst plugin")

        dispatcher(effMainsChanged, 0, 0);  // Stop plugin
        dispatcher(effStopProcess);         // Stop plugin

        if (m_editor)
        {
            m_editor->enableClosing();
            delete m_editor;
            m_editor = nullptr;
        }

        dispatcher(effClose);               // Stop plugin

        if ( m_dllHandle )                  // Close plugin
        {
            HMODULE hModule = reinterpret_cast< HMODULE >( m_dllHandle );
            FreeLibrary(hModule);
            m_dllHandle = 0;
        }

        m_framePos = 0;

        //m_loadButton->setIcon( QIcon() );
        //m_editorImage->hide();
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
        case audioMasterGetBlockSize:           return m_bufferFrames;
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
        m_vstMidiEvents.clear();
        if ( auto l = m_vstMidi.lock() )
        {
            std::swap( m_vstMidiEvents, m_vstMidi.events );
            //m_vstMidi.events.clear();
        }

        if ( !m_vstMidiEvents.empty() )
        {
            auto const n = m_vstMidiEvents.size();
            auto const m = sizeof( VstEvents ) + sizeof( VstEvent* ) * n;
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

    void onMidiMessage(f64 pts, const midi::MidiMessage& msg)
    {
        DE_WARN("Not implemented, ", msg.size())
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

        // Special event: All Notes Off (Bn 7B 00):
        if (((msg.status & 0xF0) == 0xB0) &&
             (msg.data1 == 0x7B) &&
             (msg.data2 == 0x00) )
        {
            if ( auto l = m_vstMidi.lock() )
            {
                m_vstMidi.events.clear();
            }
        }

        size_t n = 0;
        if ( auto l = m_vstMidi.lock() )
        {
            m_vstMidi.events.push_back( e );
            n = m_vstMidi.events.size();
        }

        // DE_DEBUG("events(",n,"), byte1(",dbHex(byte1),"), data1(",dbHex(data1),"), data2(",dbHex(data2),")")
    }

};


// ============================================================================
VST2_Plugin::VST2_Plugin()
// ============================================================================
   : _d( new VST2_Plugin_Impl )
{
    //aboutToStart( 64, 2, 48000 );
}

VST2_Plugin::~VST2_Plugin()
{
    closePlugin();
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

void VST2_Plugin::dsp_setInputSignal(IDspChainElement* inSignal, int i)
{
    _d->setInputSignal(inSignal, i);
}

void VST2_Plugin::dsp_clearInputSignals()
{
    _d->clearInputSignals();
}

// ===================================================

const ITrack* VST2_Plugin::getTrack() const { return _d->m_track; }

ITrack* VST2_Plugin::getTrack() { return _d->m_track; }

void VST2_Plugin::setTrack(ITrack* track) { _d->m_track = track; }

// ===================================================

u32 VST2_Plugin::getPluginId() const { return _d->m_pluginId; }

void VST2_Plugin::setPluginId( u32 pluginId ) { _d->m_pluginId = pluginId; }

// ===================================================

std::string VST2_Plugin::uri() const { return _d->m_uri; }

std::string VST2_Plugin::name() const { return dbFileBase(_d->m_uri); }

std::string VST2_Plugin::vendor() const { return dbFileBase(_d->m_uri); }

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
    return _d->m_bPluginOpen;
}

bool VST2_Plugin::isSynth() const
{
    return _d->m_bIsSynth;
}

bool VST2_Plugin::hasEditor() const
{
    return _d->m_bHasEditor;
}

PluginEditorWindow* VST2_Plugin::getEditor()
{
    return _d->getEditor();
}

// ===================================================

/*
void VST2_Plugin::openEditor( u64 parent )
{
    _d->openEditor( parent );
}

void VST2_Plugin::closeEditor()
{
    _d->closeEditor();
}

bool VST2_Plugin::isEditorOpen()
{
    return _d->m_bEditorOpen;
}

// ===================================================

bool VST2_Plugin::isEditorVisible()
{
    if ( _d->m_editorWindow )
    {
        return _d->m_editorWindow->isVisible();
    }
    else
    {
        return false;
    }
}

void VST2_Plugin::setEditorVisible( bool bVisible )
{
    if ( _d->m_editorWindow )
    {
        _d->m_editorWindow->setVisible( bVisible );
        if (_d->m_editorWindow->isVisible())
        {
            _d->m_editorWindow->raise();
        }
    }
}

void VST2_Plugin::moveEditor( int x, int y )
{
    if (_d->m_editorWindow )
    {
        _d->m_editorWindow->move( x,y );
    }
}
*/

void VST2_Plugin::onMidiMessage(f64 pts, const midi::MidiMessage& msg)
{
    _d->onMidiMessage(pts, msg);
}

void VST2_Plugin::onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
{
    _d->onShortMidiMessage(pts, msg);
}

//uint64_t
//VST2_Plugin::getSamplePos() const { return m_framePos; }
//uint32_t
//VST2_Plugin::getSampleRate() const { return m_sampleRate; }
//uint64_t
//VST2_Plugin::getBlockSize() const { return m_bufferFrames; }
//uint64_t
//VST2_Plugin::getChannelCount() const { return m_channelCount; }
//bool
//VST2_Plugin::isSynth() const { return getFlags(effFlagsIsSynth); }
//
// This function is called from refillCallback() which is running in audio thread.
//float**
//VST2_Plugin::processAudio( uint64_t frameCount, uint64_t & outputFrameCount )
//{
//   //frameCount = std::min( uint64_t(frameCount), uint64_t(m_outBuffer.size()) / m_outputChannels );
//   m_vst->processReplacing( m_vst, m_inBufferHeads.data(), m_outBufferHeads.data(), frameCount );
//   m_framePos += frameCount;
//   outputFrameCount = frameCount;
//   return m_outBufferHeads.data();
//}



} // end namespace audio.
} // end namespace de.


/*
 *


#include "VstEditorHost.h"
#include <QWindow>
#include <QVBoxLayout>
#include <windows.h>

VstEditorHost::VstEditorHost(AEffect* effect, QWidget* parent)
    : QWidget(parent), effect(effect)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    createHostWindow();
    openEditor();
}

VstEditorHost::~VstEditorHost()
{
    closeEditor();
    if (hostHwnd)
        DestroyWindow(hostHwnd);
}

void VstEditorHost::createHostWindow()
{
    HWND parentHwnd = (HWND)winId();

    hostHwnd = CreateWindowEx(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_VISIBLE,
        0, 0, width(), height(),
        parentHwnd,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );
}

void VstEditorHost::openEditor()
{
    if (!effect) return;

    effect->dispatcher(effect, effEditOpen, 0, 0, hostHwnd, 0);

    // Query editor size
    ERect* rect = nullptr;
    effect->dispatcher(effect, effEditGetRect, 0, 0, &rect, 0);

    if (rect)
    {
        int w = rect->right - rect->left;
        int h = rect->bottom - rect->top;
        resize(w, h);
        SetWindowPos(hostHwnd, nullptr, 0, 0, w, h, SWP_NOZORDER);
    }
}

void VstEditorHost::closeEditor()
{
    if (effect)
        effect->dispatcher(effect, effEditClose, 0, 0, nullptr, 0);
}

void VstEditorHost::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (hostHwnd)
    {
        SetWindowPos(
            hostHwnd,
            nullptr,
            0, 0,
            width(), height(),
            SWP_NOZORDER
        );
    }
}


    unsigned threadFunc() {
        ComInit comInit {};
        const HANDLE events[2] = { hClose, hRefillEvent };
        for(bool run = true; run; ) {
            const auto r = WaitForMultipleObjects(_countof(events), events, FALSE, INFINITE);
            if(WAIT_OBJECT_0 == r) {    // hClose
                run = false;
            } else if(WAIT_OBJECT_0+1 == r) {   // hRefillEvent
                UINT32 c = 0;
                audioClient->GetCurrentPadding(&c);

                const auto a = bufferFrameCount - c;
                float* data = nullptr;
                audioRenderClient->GetBuffer(a, reinterpret_cast<BYTE**>(&data));

                const auto r = refillFunc(data, a, mixFormat);
                audioRenderClient->ReleaseBuffer(a, r ? 0 : AUDCLNT_BUFFERFLAGS_SILENT);
            }
        }
        return 0;
    }

    HANDLE                  hThread { nullptr };
    IMMDeviceEnumerator*    mmDeviceEnumerator { nullptr };
    IMMDevice*              mmDevice { nullptr };
    IAudioClient*           audioClient { nullptr };
    IAudioRenderClient*     audioRenderClient { nullptr };
    WAVEFORMATEX*           mixFormat { nullptr };
    HANDLE                  hRefillEvent { nullptr };
    HANDLE                  hClose { nullptr };
    UINT32                  bufferFrameCount { 0 };
    RefillFunc              refillFunc {};
};


// This function is called from Wasapi::threadFunc() which is running in audio thread.
bool
refillCallback(
      VstPlugin& vstPlugin,
      float* const data,
      uint32_t availableFrameCount,
      const WAVEFORMATEX* const mixFormat)
{
    vstPlugin.processEvents();

    const auto nDstChannels = mixFormat->nChannels;
    const auto nSrcChannels = vstPlugin.getChannelCount();
    const auto vstSamplesPerBlock = vstPlugin.getBlockSize();

    int ofs = 0;
    while(availableFrameCount > 0) {
        size_t outputFrameCount = 0;
        float** vstOutput = vstPlugin.processAudio(availableFrameCount, outputFrameCount);

        // VST vstOutput[][] format :
        //  vstOutput[a][b]
        //      channel = a % vstPlugin.getChannelCount()
        //      frame   = b + floor(a/2) * vstPlugin.getBlockSize()

        // wasapi data[] format :
        //  data[x]
        //      channel = x % mixFormat->nChannels
        //      frame   = floor(x / mixFormat->nChannels);

        const auto nFrame = outputFrameCount;
        for(size_t iFrame = 0; iFrame < nFrame; ++iFrame) {
            for(size_t iChannel = 0; iChannel < nDstChannels; ++iChannel) {
                const int sChannel = iChannel % nSrcChannels;
                const int vstOutputPage = (iFrame / vstSamplesPerBlock) * sChannel + sChannel;
                const int vstOutputIndex = (iFrame % vstSamplesPerBlock);
                const int wasapiWriteIndex = iFrame * nDstChannels + iChannel;
                *(data + ofs + wasapiWriteIndex) = vstOutput[vstOutputPage][vstOutputIndex];
            }
        }

        availableFrameCount -= nFrame;
        ofs += nFrame * nDstChannels;
    }
    return true;
}


void mainLoop(const std::wstring& dllFilename)
{
    VstPlugin vstPlugin { dllFilename.c_str(), GetConsoleWindow() };

    Wasapi wasapi { [&vstPlugin](float* const data, uint32_t availableFrameCount, const WAVEFORMATEX* const mixFormat) {
        return refillCallback(vstPlugin, data, availableFrameCount, mixFormat);
    }};

    struct Key {
        Key(int midiNote) : midiNote { midiNote } {}
        int     midiNote {};
        bool    status { false };
    };

    std::map<int, Key> keyMap {
               {'2', {61}}, {'3', {63}},              {'5', {66}}, {'6', {68}}, {'7', {70}},
        {'Q', {60}}, {'W', {62}}, {'E', {64}}, {'R', {65}}, {'T', {67}}, {'Y', {69}}, {'U', {71}}, {'I', {72}},

               {'S', {49}}, {'D', {51}},              {'G', {54}}, {'H', {56}}, {'J', {58}},
        {'Z', {48}}, {'X', {50}}, {'C', {52}}, {'V', {53}}, {'B', {55}}, {'N', {57}}, {'M', {59}}, {VK_OEM_COMMA, {60}},
    };

    for(bool run = true; run; WaitMessage()) {
        MSG msg {};
        while(BOOL b = PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if(b == -1) {
                run = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        for(auto& e : keyMap) {
            auto& key = e.second;
            const auto on = (GetKeyState(e.first) & 0x8000) != 0;
            if(key.status != on) {
                key.status = on;
                vstPlugin.sendMidiNote(0, key.midiNote, on, 100);
            }
        }
    }
}


int main() {
    volatile ComInit comInit;

    const auto dllFilename = []() -> std::wstring {
        wchar_t fn[MAX_PATH+1] {};
        OPENFILENAME ofn { sizeof(ofn) };
        ofn.lpstrFilter = L"VSTi DLL(*.dll)\0*.dll\0All Files(*.*)\0*.*\0\0";
        ofn.lpstrFile   = fn;
        ofn.nMaxFile    = _countof(fn);
        ofn.lpstrTitle  = L"Select VST DLL";
        ofn.Flags       = OFN_FILEMUSTEXIST | OFN_ENABLESIZING;
        GetOpenFileName(&ofn);
        return fn;
    } ();

    try {
        mainLoop(dllFilename);
    } catch(std::exception &e) {
        std::cout << "Exception : " << e.what() << std::endl;
    }
}

*/
