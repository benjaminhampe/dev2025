#pragma once
#include <QWidget>
#include <QCloseEvent>

// ============================================================================
class PluginEditorWindow : public QWidget
// ============================================================================
{
   Q_OBJECT
public:
   PluginEditorWindow( QWidget* parent = 0 );
   ~PluginEditorWindow() override;

signals:
   void closed();

public slots:
   void enableClosing();
   void disableClosing();
   
protected:
   void closeEvent( QCloseEvent* event ) override;
   
protected:
   bool m_enableClosing;
};

/*
 *
#include "VstEditorHost.h"
#include "Vst2Plugin.h"

VstEditorHost::VstEditorHost(Vst2Plugin* plugin, QWidget* parent)
    : QWidget(parent), plugin(plugin)
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
        0, L"STATIC", L"",
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
    plugin->openEditor(hostHwnd);

    ERect rect;
    if (plugin->getEditorRect(rect))
    {
        int w = rect.right - rect.left;
        int h = rect.bottom - rect.top;

        resize(w, h);
        SetWindowPos(hostHwnd, nullptr, 0, 0, w, h, SWP_NOZORDER);
    }
}

void VstEditorHost::closeEditor()
{
    plugin->closeEditor();
}

void VstEditorHost::resizeEvent(QResizeEvent*)
{
    if (hostHwnd)
    {
        SetWindowPos(hostHwnd, nullptr, 0, 0, width(), height(), SWP_NOZORDER);
    }
}




Vst2Plugin* plugin = new Vst2Plugin(L"C:/Plugins/MySynth.dll");

if (plugin->isValid())
{
    auto* host = new VstEditorHost(plugin);
    host->show();
}



















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

*/