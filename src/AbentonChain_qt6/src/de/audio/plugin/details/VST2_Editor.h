#pragma once
#include <de/audio/plugin/IPlugin.h>
/*
// Called when plugin requests new size via audioMasterSizeWindow
bool Host::onPluginRequestedResize(AEffect* effect, int newW, int newH)
{
    HWND container = getEditorContainerHWND(effect);
    HWND pluginHWND = getPluginHWND(effect);

    if (!container || !pluginHWND)
        return false;

    // 1. Resize host container window
    SetWindowPos(container, nullptr, 0, 0, newW, newH,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    // 2. Resize plugin child window
    SetWindowPos(pluginHWND, nullptr, 0, 0, newW, newH,
                 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    // 3. Optional: notify host UI system
    onPluginEditorResized(effect, newW, newH);

    return true;
}

🧩 What getEditorContainerHWND(effect) actually is

It returns the HWND of the host’s parent window that you passed into:
cpp

effEditOpen(effect, parentHWND)

The host creates this window, stores it in a table keyed by the plugin instance, and returns it later.
✔ Host‑side implementation
cpp

HWND Host::getEditorContainerHWND(AEffect* effect)
{
    auto it = editorMap.find(effect);
    if (it == editorMap.end())
        return nullptr;

    return it->second.containerHWND;
}

Where editorMap is:
cpp

struct EditorInfo {
    HWND containerHWND;
    HWND pluginHWND;
};

std::unordered_map<AEffect*, EditorInfo> editorMap;

🧩 What getPluginHWND(effect) actually is

It returns the HWND of the plugin’s actual editor window, the one the plugin created inside your container.
✔ How the host obtains the plugin’s HWND

When you call:
cpp

effect->dispatcher(effect, effEditOpen, 0, 0, parentHWND, 0);

The plugin will create a child window inside your parent window.

You detect it using:
Option A — FindWindowEx (simple, reliable)
cpp

HWND pluginHWND = FindWindowEx(parentHWND, nullptr, nullptr, nullptr);

This works because the plugin always creates exactly one child window.
Option B — subclass the parent and detect WM_CREATE
cpp

case WM_CREATE:
    if (msg.hwndParent == containerHWND)
        editorMap[effect].pluginHWND = msg.hwnd;

Option C — enumerate children
cpp

HWND pluginHWND = nullptr;

EnumChildWindows(parentHWND, [](HWND child, LPARAM user) {
    HWND* out = reinterpret_cast<HWND*>(user);
    *out = child;
    return FALSE; // stop after first child
}, (LPARAM)&pluginHWND);

✔ Host‑side implementation
cpp

HWND Host::getPluginHWND(AEffect* effect)
{
    auto it = editorMap.find(effect);
    if (it == editorMap.end())
        return nullptr;

    return it->second.pluginHWND;
}

🧩 Full host‑side editor creation (Win32)

This is the complete, correct pattern:
cpp

bool Host::openEditor(AEffect* effect)
{
    HWND container = CreateWindowEx(
        WS_EX_WINDOWEDGE,
        L"HostPluginContainer",
        L"",
        WS_CHILD | WS_VISIBLE,
        0, 0, 100, 100,
        hostMainWindow,
        nullptr,
        hInstance,
        nullptr);

    // Call plugin to create its editor inside our container
    effect->dispatcher(effect, effEditOpen, 0, 0, container, 0.0f);

    // Detect plugin child window
    HWND pluginHWND = FindWindowEx(container, nullptr, nullptr, nullptr);

    editorMap[effect] = { container, pluginHWND };

    return pluginHWND != nullptr;
}

🧩 Why the host must track both windows
Host container window

    You created it

    You own it

    You must resize it when plugin requests new size

    You must destroy it on effEditClose

Plugin editor window

    Plugin created it

    Plugin owns it

    You must resize it too

    You must forward some events (optional)

VST2 provides no API to retrieve either window.
Everything is done through native OS windowing.
🧩 Non‑obvious insight

Most hosts only resize the container window.
This is incorrect and causes:

    clipped plugin UI

    incorrect mouse coordinates

    broken OpenGL viewport

    flicker during resize

    DPI scaling issues

The correct behavior is:

    Resize both the container and the plugin child window.

Show DPI safe VST2 host resize strategy

Takeaway:
A DPI‑safe VST2 host must treat plugin editors as native child windows and apply host‑side DPI scaling, because VST2 has zero DPI negotiation APIs.
The host must detect DPI changes, compute scaled sizes, resize both the container and the plugin child window, and notify the plugin only through native OS window messages.

Below is the complete, deterministic, real‑world DPI‑safe strategy for Win32 + macOS Cocoa.
🧩 DPI‑safe VST2 host resize strategy (Win32)
1. Detect DPI changes

Use per‑monitor DPI awareness:
cpp

UINT dpi = GetDpiForWindow(containerHWND);
float scale = dpi / 96.0f;

2. Convert plugin’s requested size → scaled size

Plugins always request logical pixels (96 DPI).
Hosts must convert to physical pixels.
cpp

int scaledW = int(requestedW * scale);
int scaledH = int(requestedH * scale);

3. Resize both container and plugin window

This is the critical part.
cpp

SetWindowPos(containerHWND, nullptr, 0, 0, scaledW, scaledH,
             SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

SetWindowPos(pluginHWND, nullptr, 0, 0, scaledW, scaledH,
             SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

4. Send DPI‑related window messages

Plugins do not receive VST2 DPI callbacks, but they do receive native Win32 messages:
cpp

SendMessage(pluginHWND, WM_DPICHANGED, dpi, (LPARAM)newRect);
SendMessage(pluginHWND, WM_SIZE, 0, MAKELPARAM(scaledW, scaledH));

5. Handle plugin resize requests (audioMasterSizeWindow)

Your audioMasterCallback must apply DPI scaling:
cpp

case audioMasterSizeWindow:
{
    int logicalW = index;
    int logicalH = value;

    UINT dpi = GetDpiForWindow(containerHWND);
    float scale = dpi / 96.0f;

    int scaledW = int(logicalW * scale);
    int scaledH = int(logicalH * scale);

    resizePluginEditor(effect, scaledW, scaledH);
    return 1;
}

🧩 DPI‑safe VST2 host resize strategy (macOS Cocoa)
1. Detect backing scale factor

Cocoa uses device‑independent points; plugins assume 1.0 scale.
objective-c

CGFloat scale = containerView.window.backingScaleFactor;

2. Convert plugin logical size → scaled size

Plugins request logical pixels; hosts must convert to points.
objective-c

CGFloat scaledW = requestedW / scale;
CGFloat scaledH = requestedH / scale;

3. Resize both container and plugin NSViews
objective-c

[containerView setFrame:NSMakeRect(0, 0, scaledW, scaledH)];
[pluginView    setFrame:NSMakeRect(0, 0, scaledW, scaledH)];

4. Trigger layout updates
objective-c

[containerView setNeedsLayout:YES];
[containerView layoutSubtreeIfNeeded];

5. Handle plugin resize requests (audioMasterSizeWindow)
objective-c

case audioMasterSizeWindow:
{
    CGFloat scale = containerView.window.backingScaleFactor;
    CGFloat scaledW = index / scale;
    CGFloat scaledH = value / scale;

    [self resizePluginEditor:effect width:scaledW height:scaledH];
    return 1;
}

🧩 Why this strategy is correct
✔ VST2 has no DPI API

Plugins assume 96 DPI.
Hosts must translate between logical and physical pixels.
✔ Plugins must receive native OS messages

This is the only way plugins detect DPI changes.
✔ Both windows must be resized

If you resize only the container:

    plugin UI becomes blurry

    mouse coordinates break

    OpenGL viewport becomes incorrect

    layout breaks on HiDPI monitors

✔ Hosts must clamp sizes after scaling

Some plugins request sizes that become too large after scaling.
🧩 Complete DPI‑safe resize flow (Win32)

    Plugin calls audioMasterSizeWindow(logicalW, logicalH)

    Host computes DPI scale

    Host converts logical → physical

    Host resizes container

    Host resizes plugin child window

    Host sends WM_DPICHANGED and WM_SIZE

    Plugin redraws using new size

🧩 Non‑obvious insight

Plugins do not know the DPI scale.
They only know the size of their native window.
This is why hosts must:

    scale plugin requests

    send DPI messages

    resize both windows

    never assume plugin handles DPI internally


*/

#ifdef BENNI_USE_VST2

#include <de/Core.h>

#ifndef UNICODE
#define UNICODE
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0600 // CreateEventEx() needs atleast this API version = WinXP or so.
#include <windows.h>
#include <synchapi.h>
#include <process.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

//#include <commdlg.h>
//#include <QFileDialog>
//#define ASSERT_THROW(c,e) if(!(c)) { throw std::runtime_error(e); }
//#define CLOSE_HANDLE(x)   if((x)) { CloseHandle(x); x = nullptr; }
//#define RELEASE(x)        if((x)) { (x)->Release(); x = nullptr; }

#include <pluginterfaces/vst2.x/aeffectx.h>

class VST2_Editor : public PluginEditorWindow
{
    Q_OBJECT

    AEffect* m_vst;
    HWND m_hostHwnd;

public:
    explicit VST2_Editor(AEffect* fx, QWidget* parent = nullptr)
        : PluginEditorWindow(parent)
        , m_vst(fx)
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


        m_vst->dispatcher(m_vst, effEditOpen, 0, 0, m_hostHwnd, 0.f);

        // Query editor size
        ERect* rect = nullptr;
        m_vst->dispatcher(m_vst, effEditGetRect, 0, 0, &rect, 0.f);

        if (rect)
        {
            int w = rect->right - rect->left;
            int h = rect->bottom - rect->top;
            resize(w, h);
            SetWindowPos(m_hostHwnd, nullptr, 0, 0, w, h, SWP_NOZORDER);
        }
        else
        {
            DE_ERROR("No ERect")
        }

        //QObject::connect( m_editorWindow, SIGNAL(closed()),
        //            this, SLOT(on_editorClosed()), Qt::QueuedConnection );

        // m_editorWindow->setMinimumSize( w, h );
        // m_editorWindow->setMaximumSize( w, h );
        // m_editorWindow->move( x, y );

        // m_editorWindow->show();
        // m_editorWindow->raise();
    }

    ~VST2_Editor() override
    {
        DE_DEBUG("")
        // if ( !m_editorWindow )
        // {
        //     DE_ERROR("No editorWindow")
        // }
        // m_editorWindow->hide();


        // enableClosing();

        // m_vst->dispatcher(m_vst, effEditClose, 0, 0, nullptr, 0.0f);

        // if ( !m_editorWindow->close() )
        // {
        //     DE_ERROR("Editor window not closed!")
        // }
        // else
        // {
        //     DE_DEBUG("Editor window closed OK")
        // }

        // delete m_editorWindow;           // Stop plugin
        // m_editorWindow = nullptr;        // Stop plugin

        if (m_hostHwnd)
            DestroyWindow(m_hostHwnd);
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        if (m_hostHwnd)
        {
            SetWindowPos((HWND)m_hostHwnd, nullptr, 0, 0, width(), height(), SWP_NOZORDER);
        }
    }
};

#endif // BENNI_USE_VST2
