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
namespace {
	
constexpr u64 GUARD = 256;

/*
clap_process_status st = plugin->process(plugin, &p);

switch (st) {
    case CLAP_PROCESS_CONTINUE:
        // keep processing normally
        break;

    case CLAP_PROCESS_CONTINUE_IF_NOT_QUIET:
        if (pluginIsSilent(outBuffers)) {
            // optional optimization: skip next block
            skipNextBlock = true;
        }
        break;

    case CLAP_PROCESS_SLEEP:
        // plugin wants to sleep
        processingEnabled = false;
        break;

    case CLAP_PROCESS_ERROR:
        // plugin is broken
        processingEnabled = false;
        plugin->stop_processing(plugin);
        // optionally deactivate or unload
        break;
}
*/

std::string getStatusStr( clap_process_status e )
{
    switch (e)
    {
    // keep processing normally
    case CLAP_PROCESS_CONTINUE: return "CONTINUE";
    // optional optimization: skip next block
    case CLAP_PROCESS_CONTINUE_IF_NOT_QUIET: return "CONTINUE_IF_NOT_QUIET";
    // Rely upon the plugin's tail to determine if the plugin should continue to process.
    // see clap_plugin_tail
    case CLAP_PROCESS_TAIL: return "CLAP_PROCESS_TAIL";
    // plugin wants to sleep
    case CLAP_PROCESS_SLEEP: return "SLEEP";
    // plugin is broken
    case CLAP_PROCESS_ERROR: return "BROKEN";
    default:
        return std::string("Unknown") + std::to_string((int)e);
    }
}

inline std::string clapParamFlagsToString(clap_param_info_flags flags)
{
    std::string out;

    auto add = [&](const char* s)
    {
        if (!out.empty()) out += " | ";
        out += s;
    };

    if (flags & CLAP_PARAM_IS_STEPPED)            add("STEPPED");
    if (flags & CLAP_PARAM_IS_PERIODIC)           add("PERIODIC");
    if (flags & CLAP_PARAM_IS_HIDDEN)             add("HIDDEN");
    if (flags & CLAP_PARAM_IS_READONLY)           add("READONLY");
    if (flags & CLAP_PARAM_IS_BYPASS)             add("BYPASS");
    if (flags & CLAP_PARAM_IS_AUTOMATABLE)        add("AUTOMATABLE");
    if (flags & CLAP_PARAM_IS_AUTOMATABLE_PER_NOTE_ID) add("AUTO_PER_NOTE");
    if (flags & CLAP_PARAM_IS_AUTOMATABLE_PER_KEY)     add("AUTO_PER_KEY");
    if (flags & CLAP_PARAM_IS_AUTOMATABLE_PER_CHANNEL) add("AUTO_PER_CHANNEL");
    if (flags & CLAP_PARAM_IS_AUTOMATABLE_PER_PORT)    add("AUTO_PER_PORT");
    if (flags & CLAP_PARAM_IS_MODULATABLE)        add("MODULATABLE");
    if (flags & CLAP_PARAM_IS_MODULATABLE_PER_NOTE_ID) add("MOD_PER_NOTE");
    if (flags & CLAP_PARAM_IS_MODULATABLE_PER_KEY)     add("MOD_PER_KEY");
    if (flags & CLAP_PARAM_IS_MODULATABLE_PER_CHANNEL) add("MOD_PER_CHANNEL");
    if (flags & CLAP_PARAM_IS_MODULATABLE_PER_PORT)    add("MOD_PER_PORT");

    if (out.empty())
        out = "None";

    return out;
}

u32 dumpParams(const clap_plugin* plugin)
{
    auto params = (const clap_plugin_params_t*)
    plugin->get_extension(plugin, CLAP_EXT_PARAMS);

    if (!params)
    {
        DE_ERROR("No CLAP_EXT_PARAMS")
        return 0;
    }

    const u32 n = params->count(plugin);
    DE_DEBUG("--------------------------------------------------");
    DE_DEBUG("CLAP Params.Count = ",n)
    DE_DEBUG("--------------------------------------------------");
    for (uint32_t i = 0; i < n; i++)
    {
        clap_param_info_t info;
        if (!params->get_info(plugin, i, &info))
        {
            DE_ERROR("[",i,"] NOT_EXIST")
            continue;
        }

        double value;
        if (!params->get_value(plugin, info.id, &value))
        {
            DE_ERROR("[",i,"].Value = NOT_EXIST")
        }

        std::string flagsStr = clapParamFlagsToString(info.flags);

        DE_TRACE("CLAP Params[",i,"] "
                         "ID(", dbHex(info.id),"), "
                 "Name(",info.name,"), "
                 "Val(", value, "), "
                 "Min(", info.min_value, "), "
                 "Max(", info.max_value, "), "
                 "Def(", info.default_value, "), "
                 "Flags(", flagsStr, "), "
                 "Cookie(", (uintptr_t)info.cookie, "), "
                 "Module(", info.module, ")")

        if (info.flags & CLAP_PARAM_IS_STEPPED)
            DE_TRACE("  step_count:", (int)(info.max_value - info.min_value + 1));
    }

    return n;
}

u32 dumpPresets(const clap_plugin* plugin)
{
    // auto presetLoad = (const clap_plugin_preset_load_t*)
    //     plugin->get_extension(plugin, CLAP_EXT_PRESET_LOAD);

    if (!plugin)
    {
        // DE_TRACE("dumpPresets(CLAP): plugin is null");
        return 0;
    }

    // presetLoad->from_location(m_plugin, "/path/to/preset.clap-preset");

    // auto state = (const clap_plugin_state_t*)
    //     m_plugin->get_extension(m_plugin, CLAP_EXT_STATE);

    // state->save(m_plugin, stream);
    // state->load(m_plugin, stream);

    // auto presets = //(const clap_plugin_factory_presets_t*)
    //     m_plugin->get_extension(m_plugin, CLAP_EXT_FACTORY_PRESETS);

    // auto* presetDiscovery = (const clap_plugin_preset_discovery_t*)
    //     m_plugin->get_extension(m_plugin, CLAP_EXT_PRESET_DISCOVERY);

    return 0;
}

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
            DE_TRACE("[",i,"] "
                             "ID(",dbHex(info.id), "), "
                     "Name(",info.name, "), "
                     "Ch(", info.channel_count, "), "
                     "Flags(",dbHex(info.flags), "), "
                     "Type(", (info.port_type ? info.port_type : "unknown"), "), "
                     "PairID(",dbHex(info.in_place_pair), ")")
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
            DE_TRACE("[",i,"] "
                             "ID(",dbHex(info.id), "), "
                     "Name(",info.name, "), "
                     "Ch(", info.channel_count, "), "
                     "Flags(",dbHex(info.flags), "), "
                     "Type(", (info.port_type ? info.port_type : "unknown"), "), "
                     "PairID(",dbHex(info.in_place_pair), ")")
        }
    }
    return n;
}


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

/*
class CLAP_EventPool
{
public:
    static constexpr int MAX_EVENTS = 2048;
    static constexpr int EVENT_SIZE = 256;
private:
    void* m_pool[MAX_EVENTS];
    std::atomic<int> m_index;
public:
    CLAP_EventPool()
    {
        for (int i = 0; i < MAX_EVENTS; ++i)
        {
            m_pool[i] = _aligned_malloc(64, EVENT_SIZE);
        }
        reset();
    }

    ~CLAP_EventPool()
    {
        for (int i = 0; i < MAX_EVENTS; ++i)
        {
            _aligned_free(m_pool[i]);
        }
    }

    void reset()
    {
        m_index.store(0, std::memory_order_relaxed);
    }

    template<typename T>
    T* alloc()
    {
        int i = m_index.fetch_add(1, std::memory_order_relaxed);
        return (i < MAX_EVENTS) ?
            reinterpret_cast<T*>(m_pool[i]) : nullptr;
    }
};

class CLAP_EventBus
{
private:
    CLAP_EventPool& m_pool;

    std::vector<clap_event_header_t*> m_pendingEvents;
    std::vector<clap_event_header_t*> m_inputEvents;
    std::mutex m_mutex;

    clap_input_events_t inEvents{};

    // --- STATIC CALLBACKS REQUIRED BY CLAP ---

    static uint32_t
    sizeCallback(const clap_input_events_t* self)
    {
        auto q = static_cast<const CLAP_EventBus*>(self->ctx);
        return q->m_inputEvents.size();
    }

    static const clap_event_header_t*
    getCallback( const clap_input_events_t* self, uint32_t index)
    {
        auto q = static_cast<const CLAP_EventBus*>(self->ctx);
        return q->m_inputEvents[index];
    }

public:
    CLAP_EventBus(CLAP_EventPool& pool)
        : m_pool(pool)
    {
        m_inputEvents.reserve(1024);

        inEvents.ctx  = this;
        inEvents.size = &CLAP_EventBus::sizeCallback;
        inEvents.get  = &CLAP_EventBus::getCallback;
    }

    void sendShortMidi(u32 deltaFrames, uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
    {
        uint8_t status = b0 & 0xF0;
        uint8_t channel = b0 & 0x0F;

        // --- NOTE ON ---
        if (status == 0x90 && b2 > 0) {
            auto* ev = m_pool.alloc<clap_event_note_t>();
            if (!ev) return;

            ev->header.size     = sizeof(clap_event_note_t);
            ev->header.time     = deltaFrames;
            ev->header.space_id = CLAP_CORE_EVENT_SPACE_ID;
            ev->header.type     = CLAP_EVENT_NOTE_ON;
            ev->header.flags    = 0;

            ev->note_id    = -1;      // let plugin assign voice
            ev->port_index = 0;
            ev->channel    = channel;
            ev->key        = b1;
            ev->velocity   = b2 / 127.0f;

            std::lock_guard<std::mutex> lock(mutex);
            pendingEvents.push_back(&ev->header);
            return;
        }

        // --- NOTE OFF (either 0x80 or 0x90 with velocity 0) ---
        if (status == 0x80 || (status == 0x90 && b2 == 0)) {
            auto* ev = pool.alloc<clap_event_note_t>();
            if (!ev) return;

            ev->header.size     = sizeof(clap_event_note_t);
            ev->header.time     = deltaFrames;
            ev->header.space_id = CLAP_CORE_EVENT_SPACE_ID;
            ev->header.type     = CLAP_EVENT_NOTE_OFF;
            ev->header.flags    = 0;

            ev->note_id    = -1;
            ev->port_index = 0;
            ev->channel    = channel;
            ev->key        = b1;
            ev->velocity   = 0.0f;

            std::lock_guard<std::mutex> lock(mutex);
            pendingEvents.push_back(&ev->header);
            return;
        }

        // --- OTHER MIDI (CC, pitchbend, etc.) ---
        auto* ev = pool.alloc<clap_event_midi_t>();
        if (!ev) return;

        ev->header.size     = sizeof(clap_event_midi_t);
        ev->header.time     = deltaFrames;
        ev->header.space_id = CLAP_CORE_EVENT_SPACE_ID;
        ev->header.type     = CLAP_EVENT_MIDI;
        ev->header.flags    = 0;

        ev->port_index = 0;
        ev->data[0] = b0;
        ev->data[1] = b1;
        ev->data[2] = b2;
        ev->data[3] = b3;

        std::lock_guard<std::mutex> lock(mutex);
        pendingEvents.push_back(&ev->header);
    }

    // Called from GUI/MIDI thread
    void sendShortMidi(u32 deltaFrames, uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
    {
        auto* ev = pool.alloc<clap_event_midi_t>();
        if (!ev)
            return;

        ev->header.size     = sizeof(clap_event_midi_t);
        ev->header.time     = deltaFrames;
        ev->header.space_id = CLAP_CORE_EVENT_SPACE_ID;
        ev->header.type     = CLAP_EVENT_MIDI;
        ev->header.flags    = 0;

        ev->port_index = 0;
        ev->data[0] = b0;
        ev->data[1] = b1;
        ev->data[2] = b2;
        ev->data[3] = b3;

        std::lock_guard<std::mutex> lock(mutex);
        pendingEvents.push_back(&ev->header);
    }

    // Called from audio thread
    void beginBlock() {
        std::lock_guard<std::mutex> lock(mutex);
        inputEvents.clear();
        inputEvents.insert(inputEvents.end(),
                           pendingEvents.begin(),
                           pendingEvents.end());
        pendingEvents.clear();
    }

    void endBlock()
    {
        std::lock_guard<std::mutex> lock(mutex);
        pool.reset();
    }

    const clap_input_events_t* in() const { return &inEvents; }

};
*/

//===============================
struct CLAP_AudioBuffers
//===============================
{
    TAlignedVector<float> m_L;
    TAlignedVector<float> m_R;

    // Audio bus storage
    std::vector<clap_audio_buffer_t > m_iBuses;
    std::vector<clap_audio_buffer_t > m_oBuses;

    // Bus[].Channels[].Buffer
    std::vector<std::vector<TAlignedVector<float>>> m_iBuffers;
    std::vector<std::vector<TAlignedVector<float>>> m_oBuffers;

    // Bus[].Channels*
    std::vector<std::vector<float*>> m_iHeads;
    std::vector<std::vector<float*>> m_oHeads;

    // const clap_plugin* m_plugin,
    void setup(
        const clap_plugin* m_plugin,
        const clap_plugin_audio_ports_t* m_ports,
        u32 blockSize)
    {
        if (!m_plugin)
        {
            DE_ERROR("No clap_plugin")
            return;
        }

        if (!m_ports)
        {
            DE_ERROR("No clap_plugin_audio_ports_t")
            return;
        }

        DE_WARN("========= SETUP =========== blockSize = ", blockSize)

        m_L.resize(blockSize + GUARD);
        m_R.resize(blockSize + GUARD);

        const bool bOutput = false;
        const bool bInput = true;
        const u32 nPortOut = m_ports->count(m_plugin, bOutput);
        const u32 nPortIn = m_ports->count(m_plugin, bInput);

        m_iBuses.resize(nPortIn);
        m_oBuses.resize(nPortOut);

        m_iBuffers.resize(nPortIn);
        m_oBuffers.resize(nPortOut);

        m_iHeads.resize(nPortIn);
        m_oHeads.resize(nPortOut);

        // ============
        // Inputs
        // ============

        DE_DEBUG("Port.In.Count = ",nPortIn)

        u32 nChI = 0;
        for (u32 i = 0; i < nPortIn; i++)
        {
            clap_audio_port_info_t pi;
            if (!m_ports->get(m_plugin, i, bInput, &pi))
            {
                DE_ERROR("No Input[",i,"]")
            }

            nChI += pi.channel_count;

            m_iBuffers[i].resize(pi.channel_count);
            m_iHeads[i].resize(pi.channel_count);
            for (auto & b : m_iBuffers[i])
            {
                b.resize(blockSize + GUARD);
            }
            for (u32 k = 0; k < pi.channel_count; k++)
            {
                m_iHeads[i][k] = m_iBuffers[i][k].data();
            }

            clap_audio_buffer_t & bus = m_iBuses[i];
            bus.data32 = m_iHeads[i].data();
            bus.data64 = nullptr;
            bus.channel_count = m_iHeads[i].size();
            bus.latency = 0;
            bus.constant_mask = 0;
        }

        DE_DEBUG("Port.In.Channels = ",nChI)

        // ============
        // Outputs
        // ============

        u32 nChO = 0;

        DE_DEBUG("Port.Out.Count = ",nPortOut)

        for (u32 i = 0; i < nPortOut; i++)
        {
            clap_audio_port_info_t pi;
            if (!m_ports->get(m_plugin, i, bOutput, &pi))
            {
                DE_ERROR("No Output[",i,"]")
            }

            nChO += pi.channel_count;

            m_oBuffers[i].resize(pi.channel_count);
            m_oHeads[i].resize(pi.channel_count);
            for (auto & b : m_oBuffers[i])
            {
                b.resize(blockSize + GUARD);
            }
            for (u32 k = 0; k < pi.channel_count; k++)
            {
                m_oHeads[i][k] = m_oBuffers[i][k].data();
            }

            clap_audio_buffer_t & bus = m_oBuses[i];
            bus.data32 = m_oHeads[i].data();
            bus.data64 = nullptr;
            bus.channel_count = m_oHeads[i].size();
            bus.latency = 0;
            bus.constant_mask = 0;
        }

        DE_DEBUG("Port.Out.Channels = ",nChO)
    }

    void copy1( u32 blockSize )
    {
        if (blockSize < 1)
        {
            DE_ERROR("No blockSize")
            return;
        }

        // Fill all with zeroes, just to make sure...
        for (auto & b : m_iBuffers)
        {
            for (auto & c : b)
            {
                std::fill(c.begin(), c.end(), 0.0f);
            }
        }

        const auto bytesPerChannel = u64(blockSize) * sizeof(f32);

        // Copy L+R to vst3 buffers, if any...
        u32 n = 0;
        for (auto & b : m_iBuffers)
        {
            for (auto & c : b)
            {
                if (n == 0)
                {
                    std::memcpy(m_L.data(), c.data(), bytesPerChannel);
                    n++;
                }
                else if (n == 1)
                {
                    std::memcpy(m_R.data(), c.data(), bytesPerChannel);
                    n++;
                }
                else
                {
                    break;
                }
            }
        }
    }
};

//===============================
struct CLAP_NoteProcessor
//===============================
{
    std::vector<clap_event_note_t> m_notes;

    CLAP_NoteProcessor()
    {
        m_notes.reserve(1024);
    }

    void clear()
    {
        m_notes.clear();
    }

    static uint32_t
    size(const clap_input_events_t* self)
    {
        auto q = (CLAP_NoteProcessor*)self->ctx;
        return q->m_notes.size();
    }

    static const clap_event_header_t*
    get(const clap_input_events_t* self, uint32_t index)
    {
        auto q = (CLAP_NoteProcessor*)self->ctx;
        return &q->m_notes[index].header;
    }
};

//===============================
struct CLAP_NoteIncoming // We push notes here.
//===============================
{
    std::vector<clap_event_note_t> m_notes;
    std::mutex mutable m_mutex;

    //std::unique_lock< std::mutex >
    //lock() const { return std::unique_lock<std::mutex>(m_mutex); }

    CLAP_NoteIncoming()
    {
        std::unique_lock<std::mutex> l(m_mutex);
        m_notes.reserve(1024);
    }

    void clear()
    {
        std::unique_lock<std::mutex> l(m_mutex);
        m_notes.clear();
    }

    void push( clap_event_note_t note )
    {
        std::unique_lock<std::mutex> l(m_mutex);
        m_notes.emplace_back( std::move( note ) );
        // if (m_notes.size())
        // {
        //     DE_DEBUG("Got ", m_notes.size(), " notes")
        // }
    }

    void swap(CLAP_NoteProcessor & processor)
    {
        std::unique_lock<std::mutex> l(m_mutex);

        // if (m_notes.size())
        // {
        //     DE_DEBUG("Had ", m_notes.size(), " notes")
        // }

        std::swap(m_notes, processor.m_notes);
    }
};


// | Event type                 | Struct            |
// | -------------------------- | ----------------- |
// | CLAP_EVENT_NOTE_ON         | clap_event_note_t |
// | CLAP_EVENT_NOTE_OFF        | clap_event_note_t |
// | CLAP_EVENT_NOTE_CHOKE      | clap_event_note_t |
// | CLAP_EVENT_NOTE_EXPRESSION | clap_event_note_expression_t |
// | CLAP_EVENT_PARAM_VALUE     | clap_event_param_value_t |
// | CLAP_EVENT_PARAM_MOD       | clap_event_param_mod_t |
// | CLAP_EVENT_PARAM_GESTURE_BEGIN | clap_event_param_gesture_t |
// | CLAP_EVENT_PARAM_GESTURE_END | clap_event_param_gesture_t |
// | CLAP_EVENT_TRANSPORT       | clap_event_transport_t |
// | CLAP_EVENT_MIDI            | clap_event_midi_t |
// | CLAP_EVENT_MIDI_SYSEX      | clap_event_midi_sysex_t |
// | CLAP_EVENT_MIDI2           | clap_event_midi2_t |

//===============================
struct CLAP_OutputEventQueue
//===============================
{
    size_t m_counter = 0;
    std::vector<clap_event_header_t*> m_events;

    static bool
    try_push(const clap_output_events_t* self,
             const clap_event_header_t* event)
    {
        auto q = (CLAP_OutputEventQueue*)self->ctx;
        q->m_counter++;

        //q->events.push_back((clap_event_header_t*)event);

        return true;
    }

    void clear()
    {
        if (m_counter > 0)
        {
            DE_DEBUG("Got ",m_counter," output events")
            m_counter = 0;
        }
        m_events.clear();
    }
};

} // end namespace

//===============================
class CLAP_Plugin_Impl
//===============================
{
public:
    u32 m_pluginId;
    bool m_bIsPluginOpen;
    bool m_bIsSynth;
    bool m_bIsBypassed;
    bool m_bIgnoreQtResize; // to avoid feedback loops;
    Track* m_track;
    PluginEditorWindow* m_editor;
    IDspChainElement* m_inputSignal;
    const clap_plugin_entry_t* m_entry;
    const clap_plugin_factory* m_factory;
    const clap_plugin* m_plugin;
    const clap_plugin_audio_ports_t* m_ports;
    const clap_plugin_gui_t* m_gui;

    u32 m_sampleRate;
    u32 m_blockSize;
    u32 m_numInputs;
    u32 m_numOutputs;
    u32 m_numPrograms;
    u32 m_numParams;

    double m_timeStart;
    std::atomic< u64 > m_framePos;

    std::string m_uri;
    std::string m_directoryMultiByte;
    std::string m_pluginName;
    std::string m_pluginVendor;
    std::string m_pluginVersion;
    double m_pluginRuntime;

    SymbolLoader m_symLoader;
    PluginTimer m_perfTimer;
    PluginClock m_midiClock;

    CLAP_AudioBuffers m_buffers;
    NormalizedSumComputer m_normalizedSumComputer;

    static void
    host_resize_hints_changed(const clap_host_t *host)
    {
        auto me = static_cast<CLAP_Plugin_Impl*>(host->host_data);
        if (!me)
        {
            DE_ERROR("No host")
            return;
        }
        DE_TRACE("")

        clap_gui_resize_hints_t hints;
        if (!me->m_gui->get_resize_hints)
        {
            DE_ERROR("No get_resize_hints")
            return;
        }

        if (me->m_gui->get_resize_hints(me->m_plugin, &hints))
        {
            DE_TRACE("can_resize_horizontally = ",hints.can_resize_horizontally)
            DE_TRACE("can_resize_vertically = ",hints.can_resize_vertically)
            DE_TRACE("preserve_aspect_ratio = ",hints.preserve_aspect_ratio)
            DE_TRACE("aspect_ratio_width = ",hints.aspect_ratio_width)
            DE_TRACE("aspect_ratio_height = ",hints.aspect_ratio_height)

            // Update your window constraints
            //set_window_min_size(plugin->window, hints.min_width, hints.min_height);
            //set_window_max_size(plugin->window, hints.max_width, hints.max_height);
        }
    }

    static bool
    host_request_resize(const clap_host_t* host, uint32_t w, uint32_t h)
    {
        auto me = static_cast<CLAP_Plugin_Impl*>(host->host_data);
        if (!me)
        {
            DE_ERROR("No host")
            return false;
        }
        if (!me->m_editor)
        {
            DE_ERROR("No editor")
            return false;
        }

        DE_TRACE("w(",w,"), h(",h,")")
        me->m_bIgnoreQtResize = true;          // avoid triggering host→plugin resize back
        me->m_editor->resize(w, h);
        me->m_editor->updateGeometry();
        me->m_bIgnoreQtResize = false;

        return true; // accepted
    }

    static bool
    host_request_show(const clap_host_t *host)
    {
        auto me = static_cast<CLAP_Plugin_Impl*>(host->host_data);
        if (!me)
        {
            DE_ERROR("No host")
            return false;
        }
        if (!me->m_editor)
        {
            DE_ERROR("No editor")
            return false;
        }

        DE_TRACE("")
        me->m_editor->show();
        return true;
    }

    static bool
    host_request_hide(const clap_host_t *host)
    {
        auto me = static_cast<CLAP_Plugin_Impl*>(host->host_data);
        if (!me)
        {
            DE_ERROR("No host")
            return false;
        }
        if (!me->m_editor)
        {
            DE_ERROR("No editor")
            return false;
        }
        DE_TRACE("")
        me->m_editor->hide();
        return true;
    }

    static void
    host_closed(const clap_host_t *host, bool was_destroyed)
    {
        DE_TRACE("")
    }

    static const void*
    host_get_extension(const clap_host_t *host, const char *id)
    {
        auto me = static_cast<CLAP_Plugin_Impl*>(host->host_data);
        DE_TRACE("")
        if (!strcmp(id, CLAP_EXT_GUI))
            return &me->m_host_gui;
        return nullptr;
    }

    static void
    host_request_restart(const clap_host_t *host)
    {
        //auto me = static_cast<CLAP_Plugin_Impl*>(host->host_data);
        //DE_TRACE("")

        // WHY:
        // Plugin changed something fundamental (ports, latency, tail, etc.)
        // Host must re-query plugin metadata.
    }

    static void
    host_request_process(const clap_host_t *host)
    {
        //auto me = static_cast<CLAP_Plugin_Impl*>(host->host_data);
        //DE_TRACE("")

        // WHY:
        // Plugin wants the host to call process() again.
        // Usually because it woke from sleep.
        // me->isProcessing = true;
    }

    static void
    host_request_callback(const clap_host_t *host)
    {
        //auto me = static_cast<CLAP_Plugin_Impl*>(host->host_data);
        //DE_TRACE("")

        // WHY:
        // Plugin needs a main-thread callback.
        // You schedule it in Qt:
        // QMetaObject::invokeMethod(
        //     me->m_editor,
        //     [](){ /* call plugin->on_main_thread() */ },
        //     Qt::QueuedConnection
        // );
    }

    CLAP_NoteProcessor m_noteProcessor;
    CLAP_NoteIncoming m_noteIncoming;
    CLAP_OutputEventQueue m_outputEventQueue;
    clap_input_events_t m_inEvents;
    clap_output_events_t m_outEvents;

    clap_host_t m_host;
    clap_host_gui_t m_host_gui;

    // ============================================================================
    CLAP_Plugin_Impl()
    // ============================================================================
        : m_pluginId{ 0 }
        , m_bIsPluginOpen{ false }
        , m_bIsSynth{ false }
        , m_bIsBypassed{ false }
        , m_bIgnoreQtResize{ false }
        , m_track{ nullptr }
        , m_editor{ nullptr }
        , m_inputSignal{ nullptr }
        , m_entry{ nullptr }
        , m_factory{ nullptr }
        , m_plugin{ nullptr }
        , m_ports{ nullptr }
        , m_gui{ nullptr }
        , m_sampleRate{ 0 }
        , m_blockSize{ 0 }
        , m_numInputs{ 0 }
        , m_numOutputs{ 0 }
        , m_numPrograms{ 0 }
        , m_numParams{ 0 }
        , m_timeStart{ 0.0 }
        , m_framePos{ 0 }
    {
        DE_DEBUG("")

        m_host.clap_version     = CLAP_VERSION;
        m_host.name             = "Abenton Instrument Qt6";
        m_host.vendor           = "<benjaminhampe@gmx.de>";
        m_host.url              = "https://github.com/benjaminhampe";
        m_host.version          = "1.0";
        m_host.host_data        = this;
        m_host.get_extension    = host_get_extension;
        m_host.request_restart  = host_request_restart;
        m_host.request_process  = host_request_process;
        m_host.request_callback = host_request_callback;

        m_host_gui.resize_hints_changed = host_resize_hints_changed;
        m_host_gui.request_resize = host_request_resize;
        m_host_gui.request_show   = host_request_show;
        m_host_gui.request_hide   = host_request_hide;
        m_host_gui.closed         = host_closed;
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

        if (m_editor)
        {
            DE_TRACE("Close editor")
            //if (m_gui) m_gui->hide(m_plugin);
            if (m_gui) m_gui->destroy(m_plugin);
            m_editor->enableClosing();
            m_editor->close();
            m_editor->deleteLater();
            m_editor = nullptr;
        }

        m_gui = nullptr;

        DE_WARN("Stop vst plugin")
        if (m_plugin)
        {
            m_plugin->destroy(m_plugin);
            m_plugin = nullptr;
        }

        if (m_entry)
        {
            m_entry->deinit();
            m_entry = nullptr;
        }

        m_symLoader.close();

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
        m_directoryMultiByte = dbFileDir(uri);
        m_pluginName = dbFileBase(uri);
        m_pluginVendor = "";
        m_pluginVersion = "";

        DE_TRACE("uri = ",m_uri)
        DE_TRACE("dir = ",m_directoryMultiByte)

        m_symLoader.open(uri);
        if ( !m_symLoader.is_open() )
        {
            DE_ERROR("No library ",uri)
            return;
        }

        //typedef const clap_plugin_entry_t*(__cdecl *ClapEntryProc)(void);

        auto sym = m_symLoader.getSymbol("clap_entry");
        if (!sym)
        {
            DE_ERROR("No 'clap_entry' sym found. ",uri)
            return;
        }

        // DE_TRACE("Got sym")

        m_entry = reinterpret_cast<const clap_plugin_entry_t*>(sym);
        if (!m_entry)
        {
            DE_ERROR("No entry. ",uri)
            return;
        }

        // DE_TRACE("Got entry")

        if (!m_entry->init( uri.c_str()))
        {
            DE_ERROR("No entry->init. ",uri)
            return;
        }

        // DE_TRACE("Got init")

        m_factory = (const clap_plugin_factory*)m_entry->get_factory(CLAP_PLUGIN_FACTORY_ID);
        if (!m_factory)
        {
            DE_ERROR("No factory. ",uri)
            return;
        }

        // DE_TRACE("Got factory")

        u32 numPlugins = m_factory->get_plugin_count(m_factory);
        if (numPlugins == 0)
        {
            DE_ERROR("No plugins counted. ",uri)
            return;
        }

        DE_OK("Got (",numPlugins,") plugins.")

        const clap_plugin_descriptor_t* desc =
            m_factory->get_plugin_descriptor(m_factory, 0);
        if (!desc)
        {
            DE_ERROR("No plugin[0] descriptor. ",uri)
            return;
        }

        m_pluginName = desc->name ? desc->name : "nullptr!";
        m_pluginVendor = desc->vendor ? desc->vendor : "nullptr!";
        m_pluginVersion = desc->version ? desc->version : "nullptr!";

        DE_TRACE("CLAP plugin id = ", desc->id ? desc->id : "nullptr!")
        DE_TRACE("CLAP plugin name = ", desc->name ? desc->name : "nullptr!")
        DE_TRACE("CLAP plugin vendor = ", desc->vendor ? desc->vendor : "nullptr!")
        DE_TRACE("CLAP plugin url = ", desc->url ? desc->url : "nullptr!")
        DE_TRACE("CLAP plugin manual_url = ", desc->manual_url ? desc->manual_url : "nullptr!")
        DE_TRACE("CLAP plugin support_url = ", desc->support_url ? desc->support_url : "nullptr!")
        DE_TRACE("CLAP plugin version = ", desc->version ? desc->version : "nullptr!")
        DE_TRACE("CLAP plugin description = ", desc->description ? desc->description : "nullptr!")

        if (desc->features != nullptr)
        {
            for (uint32_t i = 0; desc->features[i] != nullptr; ++i)
            {
                const char* feature = desc->features[i];
                DE_TRACE("CLAP plugin feature[",i,"] = ", feature)
            }

            m_bIsSynth = false;

            for (uint32_t i = 0; desc->features[i]; i++)
            {
                DE_DEBUG("Feature[",i,"] ",desc->features[i])

                if (strcmp(desc->features[i], CLAP_PLUGIN_FEATURE_INSTRUMENT) == 0)
                {
                    m_bIsSynth = true;
                }
            }
        }
        else
        {
            DE_ERROR("CLAP plugin features = nullptr! No SYNTH detection possible!")
        }

        m_plugin = m_factory->create_plugin(m_factory, &m_host, desc->id);
        if (!m_plugin)
        {
            DE_ERROR("No plugin[0] created. ",uri)
            return;
        }

        // DE_TRACE("Got plugin")

        if (!m_plugin->init(m_plugin))
        {
            DE_ERROR("No plugin[0] initialized. ",uri)
            m_plugin->destroy(m_plugin);
            m_plugin = nullptr;
            return;
        }

        if (!m_plugin->activate)
        {
            DE_ERROR("No activate")
            return;
        }

        m_ports = (const clap_plugin_audio_ports_t*)
            m_plugin->get_extension(m_plugin, CLAP_EXT_AUDIO_PORTS);
        if (!m_ports)
        {
            DE_ERROR("No clap_plugin_audio_ports_t. ",uri)
            return;
        }


        m_numInputs = dumpInputs(m_plugin);
        m_numOutputs = dumpOutputs(m_plugin);
        m_numParams = dumpParams(m_plugin);

        // DE_TRACE("m_numInputs = ",m_numInputs)
        // DE_TRACE("m_numOutputs = ",m_numOutputs)
        // DE_TRACE("m_numParams = ",m_numParams)
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

        dsp_init( 128, 2, 48000 );
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

        m_gui = (const clap_plugin_gui_t*)
            m_plugin->get_extension(m_plugin, CLAP_EXT_GUI);

        if (!m_gui)
        {
            DE_ERROR("No gui")
        }
        else
        {
            if (!m_gui->is_api_supported(m_plugin, CLAP_WINDOW_API_WIN32, false))
            {
                DE_ERROR("plugin does not support Win32 GUI")
            }
            else
            {
                if (!m_gui->create(m_plugin, CLAP_WINDOW_API_WIN32, false))
                {
                    DE_ERROR("failed to create GUI")
                }
                else
                {
                    m_editor = new CLAP_Editor(m_bIgnoreQtResize,
                                               m_plugin, m_gui, nullptr );
                }
            }
        }

        m_timeStart = dbTimeInSeconds();
        m_framePos = 0;
        m_midiClock.restart();
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
        bool bNeedRealloc = false;
        bool bNeedReconfig = false;

        if ( m_blockSize != frames )
        {

            bNeedRealloc = true;
            bNeedReconfig = true;
        }

        if ( m_sampleRate != sampleRate )
        {

            bNeedReconfig = true;
        }

        if ( !m_plugin )
        {
            DE_ERROR("No plugin")
            return;
        }

        if ( bNeedReconfig )
        {
            m_blockSize = frames;
            m_sampleRate = sampleRate;

            // 1. Stop calling process()
            m_plugin->stop_processing(m_plugin);

            // 2. Stop calling process()
            m_plugin->deactivate(m_plugin);

            // Prepare input buffer + input channel heads ( planar = non-interleaved )
            // Prepare output buffer + output channel heads ( planar = non-interleaved )
            if (bNeedRealloc)
            {
                m_buffers.setup(m_plugin, m_ports, m_blockSize);
            }

            // 3. Re-activate with new sample rate and block size
            if (!m_plugin->activate(m_plugin, m_sampleRate, m_blockSize, m_blockSize))
            {
                DE_ERROR("No activate")
            }

            // 4. Re-start processing
            if (!m_plugin->start_processing(m_plugin))
            {
                DE_ERROR("No start_processing")
            }
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
            throw std::runtime_error("No dst audio dsp buffer in VST2_Plugin::readSamples()!");
        }

        //===============================
        // Bypassed:
        //===============================

        if ( !m_bIsPluginOpen || m_bIsBypassed)
        {
            if ( m_inputSignal )
            {
                m_inputSignal->dsp_read( pts,
                    frames, sampleRate, outL, outR );
            }
            else
            {
                std::fill(outL,outL+frames,0.0f);
                std::fill(outR,outR+frames,0.0f);
            }

            return; // We relayed samples or filled output with zeroes
        }

        if ( !m_plugin )
        {
            DE_ERROR("No plugin")
            return;
        }

        if ( !m_plugin->process )
        {
            DE_ERROR("No plugin->process")
            return;
        }

        //===============================
        // Active:
        //===============================

        dsp_init(frames,2,sampleRate);

        if ( m_inputSignal )
        {
            m_inputSignal->dsp_read( pts, frames, sampleRate,
                m_buffers.m_L.data(),
                m_buffers.m_R.data() );
        }
        else
        {
            std::fill(m_buffers.m_L.begin(),
                      m_buffers.m_L.end(), 0.0f);
            std::fill(m_buffers.m_R.begin(),
                      m_buffers.m_R.end(), 0.0f);
        }

        m_buffers.copy1( frames );

        // ======================================================
        // Transport:
        // ======================================================

        clap_event_transport_t tr{};
        tr.header.size     = sizeof(tr);
        tr.header.time     = 0;   // event happens at start of block
        tr.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
        tr.header.type     = CLAP_EVENT_TRANSPORT;
        tr.header.flags    = 0;

        // What changed since last block?
        tr.flags = CLAP_TRANSPORT_IS_PLAYING
                 | CLAP_TRANSPORT_HAS_SECONDS_TIMELINE
                 // | CLAP_TRANSPORT_HAS_TEMPO
                 // | CLAP_TRANSPORT_HAS_TIME_SIGNATURE
                 //| CLAP_TRANSPORT_HAS_BEATS_TIMELINE

                 //| CLAP_TRANSPORT_HAS_LOOP
                 //| CLAP_TRANSPORT_IS_RECORDING
                 //| CLAP_TRANSPORT_IS_LOOP_ACTIVE
        ;

        // Transport state
        double t = dbTimeInSeconds() - m_timeStart;
        tr.song_pos_beats   = std::round(128.0 * double(CLAP_BEATTIME_FACTOR)); // currently at beat 128 (bar 33 in 4/4)
        tr.song_pos_seconds = std::round(t * double(CLAP_SECTIME_FACTOR)); // 60.123 seconds into the song

        // Tempo
        tr.tempo = 120.0;                // 120 BPM

        // Time signature
        tr.tsig_num = 4;
        tr.tsig_denom = 4;

        // Loop region
        tr.loop_start_beats   = std::round(128.0 * double(CLAP_BEATTIME_FACTOR)); // loop starts at beat 128
        tr.loop_end_beats     = std::round(136.0 * double(CLAP_BEATTIME_FACTOR)); // loop ends at beat 136
        tr.loop_start_seconds = std::round(60.0  * double(CLAP_SECTIME_FACTOR));
        tr.loop_end_seconds   = std::round(63.75 * double(CLAP_SECTIME_FACTOR));

        clap_process_t p{};

        // ======================================================
        // Process MIDI:
        // ======================================================
        processMidiInputEvents( p );

        // ======================================================
        // Process Audio:
        // ======================================================
        p.steady_time = m_framePos;
        p.frames_count = frames;
        p.transport = &tr;
        p.audio_inputs = m_buffers.m_iBuses.data();
        p.audio_outputs = m_buffers.m_oBuses.data();
        p.audio_inputs_count = m_buffers.m_iBuses.size();
        p.audio_outputs_count = m_buffers.m_oBuses.size();

        //clap_process_status e =
            m_plugin->process(m_plugin, &p);
        // if (e != CLAP_PROCESS_CONTINUE)
        // {
        //     DE_WARN("m_plugin->process(). ", getStatusStr(e))
        // }

        m_framePos += frames; // atomic.

        // 5. Handle output events (automation, note events, etc.)
        processMidiOutputEvents();

        // ======================================================
        // Write (L+R) CLAP audio output back to DspChain.
        // ======================================================

        const auto bytesPerChannel = u64(frames) * sizeof(float);

        u32 n = 0;
        for (auto & b : m_buffers.m_oBuffers)
        {
            for (auto & c : b)
            {
                // Copy [L]eft channel:
                if (n == 0)
                {
                    std::memcpy(outL, c.data(), bytesPerChannel);
                    n++;
                }
                // Copy [R]ight channel:
                else if (n == 1)
                {
                    std::memcpy(outR, c.data(), bytesPerChannel);
                    n++;
                }
                else
                {
                    break;
                }
            }
        }

        // For audio-level-meter
        m_normalizedSumComputer.calc(outL, outR, frames);

        // Thank you for participating in our DspChain dear plugin.
        const double timeEnd = m_perfTimer.now();

        m_pluginRuntime = timeEnd - timeStart;
    }

    // This function is called from refillCallback() which is running in audio thread.
    void processMidiInputEvents( clap_process_t& p)
    {
        m_midiClock.restart();

        m_noteProcessor.clear();
        m_noteIncoming.swap( m_noteProcessor );

        m_inEvents.ctx  = &m_noteProcessor;
        m_inEvents.size = CLAP_NoteProcessor::size;
        m_inEvents.get  = CLAP_NoteProcessor::get;

        m_outEvents.ctx = &m_outputEventQueue;
        m_outEvents.try_push = CLAP_OutputEventQueue::try_push;

        p.in_events = &m_inEvents;
        p.out_events = &m_outEvents;
    }

    void processMidiOutputEvents()
    {
        m_outputEventQueue.clear();
    }

    void onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
    {
        if ( !m_bIsSynth )
        {
            return;
        }

        double dt = m_midiClock.now(); // Clock is restarted every callback call.
        int deltaFrames = std::clamp(
                            int(dt * m_sampleRate),
                            int(0),
                            int(m_blockSize) - 10);

        // m_eventBus.sendShortMidi(static_cast<u32>(deltaFrames),
        //                         msg.status,msg.data1,msg.data2,msg.data3);

        uint8_t command = msg.status & 0xF0;
        uint8_t channel = msg.status & 0x0F;
        uint8_t velocity = msg.data2;

        // if (command == 0x90 && msg.data2 != 0) // Note On
        // {
        //     e.type = Steinberg::Vst::Event::kNoteOnEvent;
        //     e.noteOn.channel = int16_t(channel);
        //     e.noteOn.pitch = int16_t(msg.data1);
        //     e.noteOn.velocity = float(msg.data2) / 127.0f;
        //     e.sampleOffset = deltaFrames; // at start of block
        // }
        // else if (command == 0x80 || (command == 0x90 && msg.data2 == 0)) // Note Off
        // {

        // --- NOTE ON ---
        if (command == 0x90 && velocity > 0)
        {
            clap_event_note_t note;
            note.header.size     = sizeof(clap_event_note_t);
            note.header.time     = deltaFrames;
            note.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
            note.header.type     = CLAP_EVENT_NOTE_ON;
            note.header.flags    = CLAP_EVENT_IS_LIVE;

            note.note_id    = -1; // let plugin assign voice
            note.port_index = -1;
            note.channel    = static_cast<s16>(channel);
            note.key        = static_cast<s16>(msg.data1);
            note.velocity   = double(velocity) / 127.0;

            //std::lock_guard<std::mutex> lock(mutex);
            //pendingEvents.push_back(&ev->header);
            m_noteIncoming.push( note );
        }
        // --- NOTE OFF ---
        else if (command == 0x80 || (command == 0x90 && velocity == 0))
        {
            clap_event_note_t note;
            note.header.size     = sizeof(clap_event_note_t);
            note.header.time     = deltaFrames;
            note.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
            note.header.type     = CLAP_EVENT_NOTE_OFF;
            note.header.flags    = CLAP_EVENT_IS_LIVE;

            note.note_id    = -1;
            note.port_index = -1;
            note.channel    = static_cast<s16>(channel);
            note.key        = static_cast<s16>(msg.data1);
            note.velocity   = double(velocity) / 127.0;

            //std::lock_guard<std::mutex> lock(mutex);
            //pendingEvents.push_back(&ev->header);
            m_noteIncoming.push( note );
        }
        // // Special event: All Notes Off (Bn 7B 00):
        // else if (((msg.status & 0xF0) == 0xB0) &&
        //      (msg.data1 == 0x7B) &&
        //      (msg.data2 == 0x00) )
        // {
        //     if ( auto l = m_vstMidi.lock() )
        //     {
        //         m_vstMidi.events.clear();
        //     }
        //     return;
        // }

        // DE_DEBUG("events(",n,"), byte1(",dbHex(byte1),"), data1(",dbHex(data1),"), data2(",dbHex(data2),")")
    }

    void onMidiMessage(f64 pts, const midi::MidiMessage& msg)
    {
        DE_WARN("Not implemented, ", msg.size())

/*
        // --- OTHER MIDI (CC, pitchbend, etc.) ---
        auto* ev = pool.alloc<clap_event_midi_t>();
        if (!ev) return;

        ev->header.size     = sizeof(clap_event_midi_t);
        ev->header.time     = deltaFrames;
        ev->header.space_id = CLAP_CORE_EVENT_SPACE_ID;
        ev->header.type     = CLAP_EVENT_MIDI;
        ev->header.flags    = 0;

        ev->port_index = 0;
        ev->data[0] = b0;
        ev->data[1] = b1;
        ev->data[2] = b2;
        ev->data[3] = b3;

        std::lock_guard<std::mutex> lock(mutex);
        pendingEvents.push_back(&ev->header);
*/
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

const Track* CLAP_Plugin::getTrack() const { return _d->m_track; }

Track* CLAP_Plugin::getTrack() { return _d->m_track; }

void CLAP_Plugin::setTrack(Track* track) { _d->m_track = track; }

// ===================================================

u32 CLAP_Plugin::getPluginId() const { return _d->m_pluginId; }

void CLAP_Plugin::setPluginId( u32 pluginId ) { _d->m_pluginId = pluginId; }

// ===================================================

std::string CLAP_Plugin::getUri() const { return _d->m_uri; }

std::string CLAP_Plugin::getName() const { return _d->m_pluginName; }

std::string CLAP_Plugin::getVendor() const { return _d->m_pluginVendor; }

std::string CLAP_Plugin::getVersion() const { return _d->m_pluginVersion; }

double CLAP_Plugin::getRuntime() const { return _d->m_pluginRuntime; }

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
    return 1;
}

std::string CLAP_Plugin::getProgramName( int i ) const
{
    return "Default";
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
    auto plugin = _d->m_plugin;
    auto params = (const clap_plugin_params_t*)
        plugin->get_extension(plugin, CLAP_EXT_PARAMS);

    if (!params)
    {
        DE_ERROR("No CLAP_EXT_PARAMS")
        return 0;
    }

    return params->count(plugin);
}

f32 CLAP_Plugin::getParameter(int i) const
{
    auto plugin = _d->m_plugin;
    auto params = (const clap_plugin_params_t*)
                  plugin->get_extension(plugin, CLAP_EXT_PARAMS);

    if (!params)
    {
        DE_ERROR("No params")
        return 0.0f;
    }

    u32 n = params->count(plugin);

    clap_param_info_t pi;
    if (!params->get_info(plugin, i, &pi))
    {
        DE_ERROR("No Param[",i,"].info.")
        return 0.0f;
    }

    double value;
    if (!params->get_value(plugin, pi.id, &value))
    {
        DE_ERROR("No Param[",i,"].value.")
        return 0.0f;
    }

    return value;
}

std::string CLAP_Plugin::getParameterName(int i) const
{
    auto plugin = _d->m_plugin;
    auto params = (const clap_plugin_params_t*)
                  plugin->get_extension(plugin, CLAP_EXT_PARAMS);

    if (!params)
    {
        DE_ERROR("No params")
        return "";
    }

    u32 n = params->count(plugin);

    clap_param_info_t pi;
    if (!params->get_info(plugin, i, &pi))
    {
        DE_ERROR("No Param[",i,"].info.")
        return "";
    }

    return pi.name;
}

void setClapParamRT(const clap_plugin_t* plugin,
                    const clap_host_t* host,
                    clap_id paramId,
                    double value)
{
    auto hostParams = (const clap_host_params*)
        host->get_extension(host, CLAP_EXT_PARAMS);

    if (!hostParams || !hostParams->request_flush)
        return;

    clap_event_param_value ev{};
    ev.header.size      = sizeof(ev);
    ev.header.time      = 0; // sample offset
    ev.header.space_id  = CLAP_CORE_EVENT_SPACE_ID;
    ev.header.type      = CLAP_EVENT_PARAM_VALUE;
    ev.header.flags     = 0;

    ev.param_id         = paramId;
    ev.cookie           = nullptr;
    ev.note_id          = -1;
    ev.port_index       = -1;
    ev.channel          = -1;
    ev.key              = -1;
    ev.value            = value;

    // Queue the event for the plugin to process
    hostParams->request_flush(host);

    // The plugin will pull this event during process()
}

void CLAP_Plugin::setParameter(int i, f32 value)
{
    auto plugin = _d->m_plugin;
    auto params = (const clap_plugin_params_t*)
            plugin->get_extension(plugin, CLAP_EXT_PARAMS);

    if (!params)
    {
        DE_ERROR("No params")
        return;
    }

    clap_param_info_t pi;
    if (!params->get_info(plugin, i, &pi))
    {
        DE_ERROR("No Param[",i,"].info.")
        return;
    }

    setClapParamRT(plugin, &_d->m_host, pi.id, value);
}

float CLAP_Plugin::getSpecialValue( eSpecialValue type ) const
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