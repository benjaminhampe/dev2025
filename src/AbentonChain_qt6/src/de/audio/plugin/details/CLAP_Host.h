#pragma once

#if 0
#include <vector>
#include <string>
#include <clap/clap.h>

class ClapPluginInstance; // forward

class CLAP_Host {
public:
    CLAP_Host();

    ~CLAP_Host() = default;

    // Access to raw CLAP host struct (passed to plugins)
    const clap_host_t *clapHost() const { return &m_host; }

    // Plugin management hooks (you implement these around this class)
    void registerPluginInstance(ClapPluginInstance *inst);
    void unregisterPluginInstance(ClapPluginInstance *inst);

    // Called by callbacks (per-plugin context is resolved by you)
    void onRequestRestart();
    void onRequestProcess();
    void onRequestCallback();

    // GUI-related hooks (you implement these in your app)
    void onGuiRequestResize(ClapPluginInstance *inst, uint32_t w, uint32_t h);
    void onGuiRequestShow(ClapPluginInstance *inst);
    void onGuiRequestHide(ClapPluginInstance *inst);

private:
    clap_host_t m_host;
    std::vector<ClapPluginInstance*> m_plugins;

    // Static callbacks passed to CLAP (C-style)
    static const void* s_get_extension(const clap_host_t *host, const char *id);
    static void s_request_restart(const clap_host_t *host);
    static void s_request_process(const clap_host_t *host);
    static void s_request_callback(const clap_host_t *host);

    // Host extensions
    static const clap_host_gui_t s_gui_ext;
    static const clap_host_log_t s_log_ext;
    static const clap_host_thread_check_t s_thread_ext;

    // Helpers to get `this` from clap_host_t
    static CLAP_Host* fromHost(const clap_host_t *host) {
        return static_cast<CLAP_Host*>(host->host_data);
    }

    // GUI extension callbacks
    static bool s_gui_request_resize(const clap_host_t *host, uint32_t w, uint32_t h);
    static bool s_gui_request_show(const clap_host_t *host);
    static bool s_gui_request_hide(const clap_host_t *host);

    // Log extension callbacks
    static void s_log(const clap_host_t *host, clap_log_severity severity, const char *msg);

    // Thread check extension callbacks
    static bool s_is_main_thread(const clap_host_t *host);
    static bool s_is_audio_thread(const clap_host_t *host);
};

#endif
