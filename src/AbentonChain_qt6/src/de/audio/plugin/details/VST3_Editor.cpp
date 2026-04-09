#include "VST3_Editor.h"

#ifdef BENNI_USE_VST3

#include "Vst3Plugin.h"

#include <QResizeEvent>

VST3_Editor::VST3_Editor(Vst3Plugin* plugin, QWidget* parent)
    : QWidget(parent), plugin(plugin)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    createHostWindow();
    openEditor();
}

VST3_Editor::~VST3_Editor()
{
    closeEditor();
    if (hostHwnd)
        DestroyWindow(hostHwnd);
}

void VST3_Editor::createHostWindow()
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

void VST3_Editor::openEditor()
{
    if (!plugin || !hostHwnd)
        return;

    plugin->openEditor(hostHwnd);

    int w = 400, h = 300;
    if (plugin->getEditorSize(w, h))
    {
        resize(w, h);
        SetWindowPos(hostHwnd, nullptr, 0, 0, w, h, SWP_NOZORDER);
    }
}

void VST3_Editor::closeEditor()
{
    if (plugin)
        plugin->closeEditor();
}

void VST3_Editor::resizeEvent(QResizeEvent* event)
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

#endif