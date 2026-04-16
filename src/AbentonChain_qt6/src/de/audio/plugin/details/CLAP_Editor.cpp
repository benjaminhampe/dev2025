#include <de/audio/plugin/details/CLAP_Editor.h>

#ifdef BENNI_USE_CLAP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>


CLAP_Editor::CLAP_Editor(
        bool& bIgnoreQtResize,
        const clap_plugin* plugin,
        const clap_plugin_gui_t* gui,
        QWidget* parent)
    : PluginEditorWindow(parent)
    , m_bIgnoreQtResize(bIgnoreQtResize)
    , m_plugin(plugin)
    , m_gui(gui)
    , m_hostHwnd(0)
{
    DE_DEBUG("")

    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    // createHostWindow();

    HWND parentHwnd = (HWND)winId();

    m_hostHwnd = CreateWindowEx(
        0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE,
        0, 0, width(), height(),
        parentHwnd,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );


    clap_window_t win{};
#if defined(Q_OS_WIN)
    win.api = CLAP_WINDOW_API_WIN32;
    win.win32 = (HWND)m_hostHwnd;
#elif defined(Q_OS_LINUX)
    win.api = CLAP_WINDOW_API_X11;
    win.x11 = (clap_x11_window)winId();
#elif defined(Q_OS_MAC)
    win.api = CLAP_WINDOW_API_COCOA;
    win.cocoa = (clap_nsview)winId();
#endif

    m_gui->set_parent(m_plugin, &win);
    m_gui->show(m_plugin);

    // DPI scaling
    double scale = devicePixelRatioF();
    DE_TRACE("GuiDpiScaling = ",scale)
    // m_gui->set_scale(plugin, scale);


    uint32_t w, h;
    m_gui->get_size(m_plugin, &w, &h);

    resize(w, h);
    SetWindowPos(m_hostHwnd, nullptr, 0, 0, w, h, SWP_NOZORDER);


    // m_gui->hide(plugin);
    // m_gui->destroy(plugin);

    // bool host_can_resize = true;

    // bool on_plugin_request_resize(uint32_t width, uint32_t height) {
    //     // resize your host window
    //     gui->set_size(plugin, width, height);
    //     return true;
    // }
}

CLAP_Editor::~CLAP_Editor()
{
    DE_DEBUG("")

    if (m_hostHwnd)
        DestroyWindow(m_hostHwnd);
}

void CLAP_Editor::resizeEvent(QResizeEvent* event)
{
    if (!m_gui || m_bIgnoreQtResize)
        return;

    uint32_t w = event->size().width();
    uint32_t h = event->size().height();

    // Ask plugin to adjust size
    m_gui->adjust_size(m_plugin, &w, &h);

    // Apply plugin-approved size
    m_gui->set_size(m_plugin, w, h);

    if (m_hostHwnd)
    {
        SetWindowPos((HWND)m_hostHwnd, nullptr, 0, 0, w, h, SWP_NOZORDER);
    }

    QWidget::resizeEvent(event);
}

#endif
