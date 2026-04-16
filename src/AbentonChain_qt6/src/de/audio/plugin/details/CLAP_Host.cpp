#include "CLAP_Host.h"

#if 0

#include "ClapPluginInstance.h" // your class that wraps clap_plugin_t*

#include <cstring>
#include <cstdio>

// ----------------------
// Static extension structs
// ----------------------

const clap_host_gui_t CLAP_Host::s_gui_ext = {
    .request_resize  = CLAP_Host::s_gui_request_resize,
    .request_show    = CLAP_Host::s_gui_request_show,
    .request_hide    = CLAP_Host::s_gui_request_hide,
    .request_destroy = nullptr,
};

const clap_host_log_t CLAP_Host::s_log_ext = {
    .log = CLAP_Host::s_log,
};

const clap_host_thread_check_t CLAP_Host::s_thread_ext = {
    .is_main_thread  = CLAP_Host::s_is_main_thread,
    .is_audio_thread = CLAP_Host::s_is_audio_thread,
};

// ----------------------
// Constructor
// ----------------------

CLAP_Host::CLAP_Host(const char *name,
                   const char *vendor,
                   const char *url,
                   const char *version)
{
    std::memset(&m_host, 0, sizeof(m_host));

    m_host.name    = name;
    m_host.vendor  = vendor;
    m_host.url     = url;
    m_host.version = version;

    m_host.host_data       = this;
    m_host.get_extension   = &CLAP_Host::s_get_extension;
    m_host.request_restart = &CLAP_Host::s_request_restart;
    m_host.request_process = &CLAP_Host::s_request_process;
    m_host.request_callback = &CLAP_Host::s_request_callback;
}

// ----------------------
// Plugin registration
// ----------------------

void CLAP_Host::registerPluginInstance(ClapPluginInstance *inst)
{
    m_plugins.push_back(inst);
}

void CLAP_Host::unregisterPluginInstance(ClapPluginInstance *inst)
{
    auto it = std::find(m_plugins.begin(), m_plugins.end(), inst);
    if (it != m_plugins.end())
        m_plugins.erase(it);
}

// ----------------------
// Core host callbacks
// ----------------------

const void* CLAP_Host::s_get_extension(const clap_host_t *host, const char *id)
{
    auto *self = fromHost(host);

    if (!std::strcmp(id, CLAP_EXT_GUI))
        return &s_gui_ext;

    if (!std::strcmp(id, CLAP_EXT_LOG))
        return &s_log_ext;

    if (!std::strcmp(id, CLAP_EXT_THREAD_CHECK))
        return &s_thread_ext;

    // Add more extensions here as you implement them

    return nullptr;
}

void CLAP_Host::s_request_restart(const clap_host_t *host)
{
    auto *self = fromHost(host);
    self->onRequestRestart();
}

void CLAP_Host::s_request_process(const clap_host_t *host)
{
    auto *self = fromHost(host);
    self->onRequestProcess();
}

void CLAP_Host::s_request_callback(const clap_host_t *host)
{
    auto *self = fromHost(host);
    self->onRequestCallback();
}

// ----------------------
// GUI extension callbacks
// ----------------------

bool CLAP_Host::s_gui_request_resize(const clap_host_t *host, uint32_t w, uint32_t h)
{
    auto *self = fromHost(host);

    // You need to decide which plugin this applies to.
    // Simplest: assume single plugin for now:
    if (self->m_plugins.empty())
        return false;

    self->onGuiRequestResize(self->m_plugins.front(), w, h);
    return true;
}

bool CLAP_Host::s_gui_request_show(const clap_host_t *host)
{
    auto *self = fromHost(host);
    if (self->m_plugins.empty())
        return false;

    self->onGuiRequestShow(self->m_plugins.front());
    return true;
}

bool CLAP_Host::s_gui_request_hide(const clap_host_t *host)
{
    auto *self = fromHost(host);
    if (self->m_plugins.empty())
        return false;

    self->onGuiRequestHide(self->m_plugins.front());
    return true;
}

// ----------------------
// Log extension callbacks
// ----------------------

void CLAP_Host::s_log(const clap_host_t *host, clap_log_severity severity, const char *msg)
{
    const char *sev = "INFO";
    switch (severity) {
    case CLAP_LOG_DEBUG: sev = "DEBUG"; break;
    case CLAP_LOG_INFO: sev = "INFO"; break;
    case CLAP_LOG_WARNING: sev = "WARN"; break;
    case CLAP_LOG_ERROR: sev = "ERROR"; break;
    case CLAP_LOG_FATAL: sev = "FATAL"; break;
    default: break;
    }

    std::fprintf(stderr, "[CLAP %s] %s\n", sev, msg ? msg : "");
}

// ----------------------
// Thread check extension callbacks
// ----------------------

bool CLAP_Host::s_is_main_thread(const clap_host_t *host)
{
    // TODO: integrate with your real threading model.
    // For now, assume true.
    return true;
}

bool CLAP_Host::s_is_audio_thread(const clap_host_t *host)
{
    // TODO: integrate with your audio engine.
    return false;
}

// ----------------------
// Instance-level hooks (you implement these)
// ----------------------

void CLAP_Host::onRequestRestart()
{
    // Plugin says: "I changed something fundamental, re-scan me."
    // You might:
    // - re-query audio ports
    // - re-query note ports
    // - update latency, tail, etc.
}

void CLAP_Host::onRequestProcess()
{
    // Plugin says: "Please call process() again."
    // You might:
    // - wake up your audio engine
    // - mark plugin as active
}

void CLAP_Host::onRequestCallback()
{
    // Plugin says: "I need a main-thread callback soon."
    // In a Qt app, you might schedule a queued connection here.
}

// GUI hooks: you wire these to your Qt editor widgets

void CLAP_Host::onGuiRequestResize(ClapPluginInstance *inst, uint32_t w, uint32_t h)
{
    // Example: inst->editor()->requestResizeFromPlugin(w, h);
}

void CLAP_Host::onGuiRequestShow(ClapPluginInstance *inst)
{
    // Example: inst->editor()->show();
}

void CLAP_Host::onGuiRequestHide(ClapPluginInstance *inst)
{
    // Example: inst->editor()->hide();
}
#endif
