#include <de/audio/plugin/details/LV2_Plugin.h>

#ifdef BENNI_USE_LV2

#define LILV_EXPERIMENTAL 1
#include <lilv/lilv.h>
#include <lv2/core/lv2.h>
#include <lv2/atom/atom.h>
#include <lv2/atom/forge.h>
#include <lv2/midi/midi.h>
#include <lv2/urid/urid.h>

#include <de/audio/plugin/details/BasePluginUtils.h>
#include <QtWidgets/QWidget>
#include <vector>
#include <string>
#include <memory>
#include <App.h>

namespace de {
namespace audio {

// ============================================================================
class LV2_Plugin_Impl
// ============================================================================
{
public:
    LilvWorld* m_world;
    const LilvPlugin* m_plugin;
    LilvInstance* m_instance;
    IDspChainElement* m_inputSignal;
    PluginEditorWindow* m_editor;
    Track* m_track;

    // Ports
    struct PortInfo {
        uint32_t index;
        bool isAudioIn = false;
        bool isAudioOut = false;
        bool isControl = false;
        bool isMidi = false;
        float* buffer = nullptr;
    };
    std::vector<PortInfo> m_ports;

    // DSP buffers
    uint32_t m_blockSize = 0;
    uint32_t m_sampleRate = 0;
    uint32_t m_numAudioIn = 0;
    uint32_t m_numAudioOut = 0;
    uint32_t m_numEventIn = 0;
    uint32_t m_numEventOut = 0;
    uint32_t m_numPrograms = 0;
    uint32_t m_numParams = 0;

    // MIDI
    uint32_t m_midiPortIndex = UINT32_MAX;

    bool m_bIsBypassed;
    bool m_bIsPluginOpen;
    bool m_bIsSynth;
    bool m_bIsDummy;

    u32 m_pluginId;
    std::string m_uri;
    std::string m_pluginName;
    std::string m_pluginVendor;
    std::string m_pluginVersion;
    double m_pluginRuntime;

    NormalizedSumComputer m_normalizedSumComputer;
public:
    LV2_Plugin_Impl()
        : m_world{ nullptr }
        , m_plugin{ nullptr }
        , m_instance{ nullptr }
        , m_inputSignal{ nullptr }
        , m_editor{ nullptr }
        , m_track{ nullptr }
    {
        m_world = lilv_world_new();
        lilv_world_load_all(m_world);
    }

    ~LV2_Plugin_Impl()
    {
        closePlugin();
        lilv_world_free(m_world);
    }

    void closePlugin()
    {
        if (m_instance) {
            lilv_instance_deactivate(m_instance);
            lilv_instance_free(m_instance);
            m_instance = nullptr;
        }

        m_ports.clear();

        if (m_editor)
        {
            delete m_editor;
            m_editor = nullptr;
        }
    }

    void openPlugin(const std::string& uri)
    {
        closePlugin();

        LilvNode* pluginUri = lilv_new_uri(m_world, uri.c_str());
        const LilvPlugins* plugins = lilv_world_get_all_plugins(m_world);

        m_plugin = lilv_plugins_get_by_uri(plugins, pluginUri);
        lilv_node_free(pluginUri);

        if (!m_plugin)
            return;

        m_instance = lilv_plugin_instantiate(m_plugin, m_sampleRate ? m_sampleRate : 48000, nullptr);
        if (!m_instance)
            return;

        scanPorts();
        //scanPresets();
        //createEditor();

        const LilvUI* ui = nullptr;

        LilvUIs* uis = lilv_plugin_get_uis(m_plugin);
        LILV_FOREACH(uis, i, uis) {
            ui = lilv_uis_get(uis, i);
            break;
        }

        if (!ui) {
            m_editor = new PluginEditorWindow();
            m_editor->setWindowTitle("No UI");
            return;
        }

        // TODO: Instantiate LV2 UI (X11, Qt, Gtk, etc.)
        // For now, just create a placeholder widget:
        m_editor = new PluginEditorWindow();
        m_editor->setWindowTitle("LV2 UI Placeholder");
    }

    // Helpers
    void scanPorts()
    {
        uint32_t numPorts = lilv_plugin_get_num_ports(m_plugin);
        m_ports.resize(numPorts);

        for (uint32_t i = 0; i < numPorts; ++i) {
            m_ports[i].index = i;

            const LilvPort* port = lilv_plugin_get_port_by_index(m_plugin, i);

            if (lilv_port_is_a(m_plugin, port, lilv_new_uri(m_world, LV2_CORE__AudioPort))) {
                if (lilv_port_is_a(m_plugin, port, lilv_new_uri(m_world, LV2_CORE__InputPort)))
                    m_ports[i].isAudioIn = true;
                else
                    m_ports[i].isAudioOut = true;
            }

            if (lilv_port_is_a(m_plugin, port, lilv_new_uri(m_world, LV2_CORE__ControlPort)))
                m_ports[i].isControl = true;

            if (lilv_port_is_a(m_plugin, port, lilv_new_uri(m_world, LV2_MIDI__MidiEvent))) {
                m_ports[i].isMidi = true;
                m_midiPortIndex = i;
            }
        }
    }

    void scanParams()
    {
        // for (uint32_t pluginIndex = 0;; ++pluginIndex)
        // {
        //     const LV2_Descriptor* d = lv2_descriptor(pluginIndex);
        //     if (!d)
        //         break;

        //     printf("Found LV2 plugin: %s\n", d->URI);
        // }

        // const LV2_Descriptor* d = lv2_descriptor(index);

        // for (uint32_t i = 0; i < d->PortCount; ++i) {
        //     const LV2_PortDescriptor pd = d->PortDescriptors[i];

        //     if (pd & LV2_PORT_INPUT && pd & LV2_PORT_CONTROL) {
        //         const LV2_PortRangeHint* hint = &d->PortRangeHints[i];
        //         const char* name = d->PortNames[i];

        //         printf("Param %u: %s\n", i, name);

        //         if (hint->HintDescriptor & LV2_PORT_HINT_BOUNDED_BELOW)
        //             printf("  Min: %f\n", hint->LowerBound);

        //         if (hint->HintDescriptor & LV2_PORT_HINT_BOUNDED_ABOVE)
        //             printf("  Max: %f\n", hint->UpperBound);

        //         if (hint->HintDescriptor & LV2_PORT_HINT_DEFAULT_MASK)
        //             printf("  Default: %f\n", hint->DefaultValue);
        //     }
        // }

    }

    void list_presets(LilvWorld* world, const LilvPlugin* plugin) {
        const LilvNode* pset_Preset = lilv_new_uri(world, "http://lv2plug.in/ns/ext/presets#Preset");
        const LilvNode* pset_appliesTo = lilv_new_uri(world, "http://lv2plug.in/ns/ext/presets#appliesTo");
        const LilvNode* rdfs_label = lilv_new_uri(world, "http://www.w3.org/2000/01/rdf-schema#label");
        const LilvNode* plugin_uri = lilv_plugin_get_uri(plugin);

        LilvNodes* presets = lilv_world_find_nodes(world, NULL, pset_appliesTo, plugin_uri);

        LILV_FOREACH(nodes, i, presets) {
            const LilvNode* preset = lilv_nodes_get(presets, i);

            const LilvNode* label = lilv_world_get(world, preset, rdfs_label, NULL);

            printf("Preset: %s (%s)\n",
                   lilv_node_as_string(label),
                   lilv_node_as_uri(preset));
        }

        lilv_nodes_free(presets);
    }
/*
    void scanPresets()
    {
        // LilvWorld* world = lilv_world_new();
        // lilv_world_load_all(world);

        const LilvPlugin* p = lilv_plugins_get_by_uri(
            lilv_world_get_all_plugins(m_world),
            lilv_new_uri(m_world, m_uri.c_str()));

        LilvNodes* presets = lilv_plugin_get_presets(p);

        LILV_FOREACH(nodes, i, presets) {
            const LilvNode* preset = lilv_nodes_get(presets, i);
            const LilvNode* label  = lilv_world_get(world, preset,
                                                   lilv_new_uri(world, LILV_NS_RDFS "label"), NULL);

            printf("Preset: %s\n", lilv_node_as_string(label));
        }

        LilvNodes* presets = lilv_plugin_get_presets(plugin);

        LILV_FOREACH(nodes, i, presets) {
            const LilvNode* preset = lilv_nodes_get(presets, i);

            const LilvNode* label = lilv_world_get(
                world,
                preset,
                lilv_new_uri(world, LILV_NS_RDFS "label"),
                NULL
                );

            printf("Preset: %s (%s)\n",
                   lilv_node_as_string(label),
                   lilv_node_as_uri(preset));
        }

    }
*/
    void dsp_init(uint32_t frames, uint32_t channels, uint32_t sampleRate)
    {
        m_blockSize = frames;
        m_numAudioOut = channels;
        m_sampleRate = sampleRate;

        if (!m_instance)
            return;

        // Allocate buffers
        for (auto& p : m_ports) {
            if (p.isAudioIn || p.isAudioOut) {
                p.buffer = new float[frames];
                lilv_instance_connect_port(m_instance, p.index, p.buffer);
            }
            if (p.isControl) {
                p.buffer = new float(0.0f);
                lilv_instance_connect_port(m_instance, p.index, p.buffer);
            }
            if (p.isMidi) {
                // LV2 MIDI uses atom sequences
                // You would allocate an LV2_Atom_Sequence here
            }
        }

        lilv_instance_activate(m_instance);
    }

    void dsp_read(f64 pts,
                    u32 frames,
                    u32 sampleRate,
                    f32* __restrict__ L,
                    f32* __restrict__ R )
    {
        if (!m_instance)
            return;

        // Clear output buffers
        for (auto& p : m_ports)
            if (p.isAudioOut)
                memset(p.buffer, 0, sizeof(float) * frames);

        // Run DSP
        lilv_instance_run(m_instance, frames);

        // Copy to host output
        uint32_t outIndex = 0;
        for (auto& p : m_ports) {
            if (p.isAudioOut) {
                if (outIndex == 0)
                    memcpy(L, p.buffer, sizeof(float) * frames);
                else if (outIndex == 1)
                    memcpy(R, p.buffer, sizeof(float) * frames);
                outIndex++;
            }
        }
    }


    void onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
    {
        if ( !m_bIsSynth )
        {
            return;
        }

        // DE_DEBUG("events(",n,"), byte1(",dbHex(byte1),"), data1(",dbHex(data1),"), data2(",dbHex(data2),")")
    }

    void onMidiMessage(f64 pts, const midi::MidiMessage& msg)
    {
        DE_WARN("Not implemented, ", msg.size())
    }

};


// ============================================================================
LV2_Plugin::LV2_Plugin()
// ============================================================================
    : _d( new LV2_Plugin_Impl )
{
    DE_TRACE("")
}

LV2_Plugin::~LV2_Plugin()
{
    DE_TRACE("")
    App::instance()->getMidiCentral().deregisterListener(this);
    delete _d;
}

// ===================================================

void LV2_Plugin::dsp_init(u64 frames, u32 channels, u32 sampleRate)
{
    _d->dsp_init(frames, channels, sampleRate);
}

void LV2_Plugin::dsp_read(f64 pts,
                           u32 frames,
                           u32 sampleRate,
                           f32* __restrict__ L,
                           f32* __restrict__ R)
{
    _d->dsp_read(pts, frames, sampleRate, L, R);
}

u32 LV2_Plugin::dsp_getInputSignalCount() const
{
    return 1;
}

IDspChainElement* LV2_Plugin::dsp_getInputSignal(int i)
{
    return _d->m_inputSignal;
}

void LV2_Plugin::dsp_setInputSignal(IDspChainElement* pSignal, int i)
{
    _d->m_inputSignal = pSignal;
}

void LV2_Plugin::dsp_clearInputSignals()
{
    _d->m_inputSignal = nullptr;
}

bool LV2_Plugin::isBypassed() const
{
    return _d->m_bIsBypassed;
}

void LV2_Plugin::setBypassed( bool bBypassed )
{
    _d->m_bIsBypassed = bBypassed;
}

// ===================================================

const Track* LV2_Plugin::getTrack() const { return _d->m_track; }

Track* LV2_Plugin::getTrack() { return _d->m_track; }

void LV2_Plugin::setTrack(Track* track) { _d->m_track = track; }

// ===================================================

u32 LV2_Plugin::getPluginId() const { return _d->m_pluginId; }

void LV2_Plugin::setPluginId( u32 pluginId ) { _d->m_pluginId = pluginId; }

// ===================================================

std::string LV2_Plugin::getUri() const { return _d->m_uri; }

std::string LV2_Plugin::getName() const { return _d->m_pluginName; }

std::string LV2_Plugin::getVendor() const { return _d->m_pluginVendor; }

std::string LV2_Plugin::getVersion() const { return _d->m_pluginVersion; }

double LV2_Plugin::getRuntime() const { return _d->m_pluginRuntime; }

// ===================================================

void LV2_Plugin::openPlugin( std::string uri )
{
    _d->openPlugin( uri );
}

void LV2_Plugin::closePlugin()
{
    _d->closePlugin();
}

bool LV2_Plugin::isPluginOpen() const
{
    return _d->m_bIsPluginOpen;
}

bool LV2_Plugin::isSynth() const
{
    return _d->m_bIsSynth;
}

PluginEditorWindow* LV2_Plugin::getEditor()
{
    return _d->m_editor;
}

// ===================================================

void LV2_Plugin::onMidiMessage(f64 pts, const midi::MidiMessage& msg)
{
    _d->onMidiMessage(pts, msg);
}

void LV2_Plugin::onShortMidiMessage(f64 pts, const midi::ShortMidiMessage& msg)
{
    _d->onShortMidiMessage(pts, msg);
}

// ===================================================

u32 LV2_Plugin::getProgramCount() const
{
    return 1;
}

std::string LV2_Plugin::getProgramName( int i ) const
{
    return "Default";
}

int LV2_Plugin::getProgram() const
{
    return 0;
}

void LV2_Plugin::setProgram( int i )
{
    if (i < 0 || i >= getProgramCount())
    {
        DE_ERROR("Invalid index ",i," of ",getProgramCount())
        return;
    }
    //_d->dispatcher(effSetProgram, 0, i);
}

// ===================================================

u32 LV2_Plugin::getParameterCount() const
{
    return _d->m_numParams;
}

f32 LV2_Plugin::getParameter(int i) const
{
    // if (!_d->m_vst)
    // {
    //     DE_ERROR("No vst")
    //     return 0.0f;
    // }
    // return _d->m_vst->getParameter(_d->m_vst, i);
    return 0.0f;
}

std::string LV2_Plugin::getParameterName(int i) const
{
    return "Default";
}

void LV2_Plugin::setParameter(int i, f32 value)
{

}

float LV2_Plugin::getSpecialValue( eSpecialValue type ) const
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