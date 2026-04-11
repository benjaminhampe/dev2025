#include "CLAP_Plugin.h"

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