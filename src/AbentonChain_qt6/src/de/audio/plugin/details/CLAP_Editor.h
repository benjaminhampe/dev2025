#pragma once
#include <de/audio/plugin/IPlugin.h>

#ifdef BENNI_USE_CLAP

#include <de/Core.h>
#include <clap/clap.h>

namespace de {
namespace audio {

class CLAP_Plugin_Impl;

} // end namespace audio.
} // end namespace de.

class CLAP_Editor : public PluginEditorWindow
{
    Q_OBJECT

    bool & m_bIgnoreQtResize;
    const clap_plugin* m_plugin;
    const clap_plugin_gui_t* m_gui;

    HWND m_hostHwnd;

public:
    explicit CLAP_Editor(
            bool& bIgnoreQtResize,
            const clap_plugin* plugin,
            const clap_plugin_gui_t* gui,
            QWidget* parent = nullptr);

    ~CLAP_Editor() override;
protected:
    void resizeEvent(QResizeEvent* event) override;
};

#endif