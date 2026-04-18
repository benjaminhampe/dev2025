#include <de/audio/plugin/details/LV2_Plugin.h>

#ifdef BENNI_USE_LV2

#include <lilv/lilv.h>
#include <QtWidgets/QWidget>
#include <vector>
#include <string>
#include <memory>

namespace de {
namespace audio {

// ============================================================================
class LV2_Plugin_Impl
// ============================================================================
{
public:
    // LV2 world
    LilvWorld* world = nullptr;
    const LilvPlugin* plugin = nullptr;
    LilvInstance* instance = nullptr;

    // Ports
    struct PortInfo {
        uint32_t index;
        bool isAudioIn = false;
        bool isAudioOut = false;
        bool isControl = false;
        bool isMidi = false;
        float* buffer = nullptr;
    };
    std::vector<PortInfo> ports;

    // DSP buffers
    uint32_t blockSize = 0;
    uint32_t numChannels = 0;
    uint32_t sampleRate = 0;

    // MIDI
    uint32_t midiPortIndex = UINT32_MAX;

    // UI
    QWidget* editorWidget = nullptr;

public:
    LV2_Plugin_Impl();
    ~LV2_Plugin_Impl();

    bool openPlugin(const std::string& uri);
    void closePlugin();

    bool dsp_init(uint32_t frames, uint32_t channels, uint32_t sampleRate);
    void dsp_read(float* outL, float* outR, uint32_t frames);

    void sendMidi(uint8_t status, uint8_t data1, uint8_t data2);

    QWidget* getEditorWidget() const { return editorWidget; }


    // Helpers
    void scanPorts();
    void scanPresets();
    void createEditor();
};



LV2_Plugin_Impl::LV2_Plugin_Impl()
{
    world = lilv_world_new();
    lilv_world_load_all(world);
}

LV2_Plugin_Impl::~LV2_Plugin_Impl()
{
    closePlugin();
    lilv_world_free(world);
}

bool LV2_Plugin_Impl::openPlugin(const std::string& uri)
{
    closePlugin();

    LilvNode* pluginUri = lilv_new_uri(world, uri.c_str());
    const LilvPlugins* plugins = lilv_world_get_all_plugins(world);

    plugin = lilv_plugins_get_by_uri(plugins, pluginUri);
    lilv_node_free(pluginUri);

    if (!plugin)
        return false;

    instance = lilv_plugin_instantiate(plugin, sampleRate ? sampleRate : 48000, nullptr);
    if (!instance)
        return false;

    scanPorts();
    scanPresets();
    createEditor();

    return true;
}

void LV2_Plugin_Impl::createEditor()
{
    const LilvUI* ui = nullptr;

    LilvUIs* uis = lilv_plugin_get_uis(plugin);
    LILV_FOREACH(uis, i, uis) {
        ui = lilv_uis_get(uis, i);
        break;
    }

    if (!ui) {
        editorWidget = new QWidget();
        editorWidget->setWindowTitle("No UI");
        return;
    }

    // TODO: Instantiate LV2 UI (X11, Qt, Gtk, etc.)
    // For now, just create a placeholder widget:
    editorWidget = new QWidget();
    editorWidget->setWindowTitle("LV2 UI Placeholder");
}


void LV2_Plugin_Impl::closePlugin()
{
    if (instance) {
        lilv_instance_deactivate(instance);
        lilv_instance_free(instance);
        instance = nullptr;
    }

    ports.clear();

    if (editorWidget) {
        delete editorWidget;
        editorWidget = nullptr;
    }
}


void LV2_Plugin_Impl::scanPorts()
{
    uint32_t numPorts = lilv_plugin_get_num_ports(plugin);
    ports.resize(numPorts);

    for (uint32_t i = 0; i < numPorts; ++i) {
        ports[i].index = i;

        const LilvPort* port = lilv_plugin_get_port_by_index(plugin, i);

        if (lilv_port_is_a(plugin, port, lilv_new_uri(world, LV2_CORE__AudioPort))) {
            if (lilv_port_is_a(plugin, port, lilv_new_uri(world, LV2_CORE__InputPort)))
                ports[i].isAudioIn = true;
            else
                ports[i].isAudioOut = true;
        }

        if (lilv_port_is_a(plugin, port, lilv_new_uri(world, LV2_CORE__ControlPort)))
            ports[i].isControl = true;

        if (lilv_port_is_a(plugin, port, lilv_new_uri(world, LV2_MIDI__MidiEvent))) {
            ports[i].isMidi = true;
            midiPortIndex = i;
        }
    }
}

bool LV2_Plugin_Impl::dsp_init(uint32_t frames, uint32_t channels, uint32_t sr)
{
    blockSize = frames;
    numChannels = channels;
    sampleRate = sr;

    if (!instance)
        return false;

    // Allocate buffers
    for (auto& p : ports) {
        if (p.isAudioIn || p.isAudioOut) {
            p.buffer = new float[frames];
            lilv_instance_connect_port(instance, p.index, p.buffer);
        }
        if (p.isControl) {
            p.buffer = new float(0.0f);
            lilv_instance_connect_port(instance, p.index, p.buffer);
        }
        if (p.isMidi) {
            // LV2 MIDI uses atom sequences
            // You would allocate an LV2_Atom_Sequence here
        }
    }

    lilv_instance_activate(instance);
    return true;
}

void LV2_Plugin_Impl::dsp_read(float* outL, float* outR, uint32_t frames)
{
    if (!instance)
        return;

    // Clear output buffers
    for (auto& p : ports)
        if (p.isAudioOut)
            memset(p.buffer, 0, sizeof(float) * frames);

    // Run DSP
    lilv_instance_run(instance, frames);

    // Copy to host output
    uint32_t outIndex = 0;
    for (auto& p : ports) {
        if (p.isAudioOut) {
            if (outIndex == 0)
                memcpy(outL, p.buffer, sizeof(float) * frames);
            else if (outIndex == 1)
                memcpy(outR, p.buffer, sizeof(float) * frames);
            outIndex++;
        }
    }
}

void LV2_Plugin_Impl::sendMidi(uint8_t status, uint8_t data1, uint8_t data2)
{
    if (midiPortIndex == UINT32_MAX)
        return;

    // TODO: Fill LV2_Atom_Sequence with a MIDI event
    // Example structure:
    //
    // LV2_Atom_Sequence* seq = (LV2_Atom_Sequence*)ports[midiPortIndex].buffer;
    // LV2_Atom_Event* ev = lv2_atom_sequence_append_event(seq, ...);
    // ev->body.size = 3;
    // ev->body.type = midiType;
    // uint8_t* msg = (uint8_t*)(ev + 1);
    // msg[0] = status;
    // msg[1] = data1;
    // msg[2] = data2;
}


} // end namespace audio.
} // end namespace de.

#endif