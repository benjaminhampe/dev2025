#pragma once
#include <de/audio/plugin/IPlugin.h>

#ifndef BENNI_USE_VST3
#define BENNI_USE_VST3
#endif

#ifdef BENNI_USE_VST3

#include <de/Core.h>
#include <QWidget>
#include <QResizeEvent>
#include "pluginterfaces/gui/iplugview.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

/**
class IPlugFrame : public FUnknown
{
public:
    /// Called to inform the host about the resize of a given view.
    /// Afterwards the host has to call IPlugView::onSize ().
    virtual tresult PLUGIN_API resizeView (IPlugView* view, ViewRect* newSize) = 0;

    static const FUID iid;
};
*/


class VST3_PlugFrame : public Steinberg::IPlugFrame
{
public:
    QWidget* m_parentWidget;

    std::atomic<Steinberg::uint32> m_refCount {1};

    Steinberg::uint32 PLUGIN_API addRef() override { return ++m_refCount; }
    Steinberg::uint32 PLUGIN_API release() override
    {
        auto r = --m_refCount;
        if (r == 0)
            delete this;
        return r;
    }

    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID iid, void** obj) override
    {
        if (Steinberg::FUnknownPrivate::iidEqual(iid, Steinberg::IPlugFrame::iid))
        {
            *obj = static_cast<Steinberg::IPlugFrame*>(this);
            addRef();
            return Steinberg::kResultOk;
        }
        *obj = nullptr;
        return Steinberg::kNoInterface;
    }


    // // Required by FUnknown
    // DELEGATE_REFCOUNT(Steinberg::FUnknown)
    // Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID iid, void** obj) override
    // {
    //     QUERY_INTERFACE(iid, obj, Steinberg::IPlugFrame::iid, Steinberg::IPlugFrame)
    //     *obj = nullptr;
    //     return Steinberg::kNoInterface;
    // }


    VST3_PlugFrame(QWidget* widget)
        : m_parentWidget(widget)
    {}

    // VST3 view requests resize
    Steinberg::tresult PLUGIN_API
    resizeView(Steinberg::IPlugView* view, Steinberg::ViewRect* newSize) override
    {
        if (!m_parentWidget)
        {
            DE_ERROR("")
            return Steinberg::kResultFalse;
        }

        m_parentWidget->setGeometry(
            m_parentWidget->x(),
            m_parentWidget->y(),
            newSize->right - newSize->left,
            newSize->bottom - newSize->top
        );

        return Steinberg::kResultOk;
    }

};

class VST3_Editor : public PluginEditorWindow
{
public:
    Steinberg::IPlugView* m_plugView;
    VST3_PlugFrame* m_plugFrame;

    VST3_Editor(Steinberg::IPlugView* plugView, QWidget* parent = nullptr)
        : PluginEditorWindow(parent)
        , m_plugView(plugView)
        , m_plugFrame(nullptr)
    {
        setAttribute(Qt::WA_NativeWindow);   // REQUIRED
        setAttribute(Qt::WA_PaintOnScreen);  // REQUIRED

        m_plugFrame = new VST3_PlugFrame(this);

        m_plugView->setFrame(m_plugFrame);

        Steinberg::ViewRect r(0, 0, width(), height());
        void* nativeHandle = reinterpret_cast<void*>(winId());
        m_plugView->attached(nativeHandle, "HWND");   // Windows
        //m_plugView->attached(nativeHandle, "NSView"); // macOS
        //m_plugView->attached(nativeHandle, "X11EmbedWindowID");
    }

    ~VST3_Editor()
    {
        if (m_plugView)
            m_plugView->removed();
        delete m_plugFrame;
    }

    void resizeEvent(QResizeEvent* e) override
    {
        QWidget::resizeEvent(e);

        if (m_plugView)
        {
            Steinberg::ViewRect r(0, 0, width(), height());
            m_plugView->onSize(&r);
        }
    }
};


/*
class VST3_Editor : public PluginEditorWindow
{
    Q_OBJECT
    Steinberg::IPlugView* m_plugView;
    HWND m_hostHwnd;

public:
    explicit VST3_Editor(Steinberg::IPlugView* plugView, QWidget* parent = nullptr)
        : QWidget(parent)
        , m_plugView(plugView)
        , m_hostHwnd(nullptr)
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


        // Attach to parent window
        m_plugView->setFrame(nullptr); // optional, for resize notifications
        m_plugView->attached((void*)m_hostHwnd, "HWND");

        Steinberg::ViewRect r;
        Steinberg::tresult e = m_plugView->getSize(&r);
        if (e == Steinberg::kResultOk)
        {
            int w = r.right - r.left;
            int h = r.bottom - r.top;
            resize(w, h);
            SetWindowPos(m_hostHwnd, nullptr, 0, 0, w, h, SWP_NOZORDER);
        }
        else
        {
            DE_ERROR("No getSize(&r)")
        }

    }

    ~VST3_Editor() override
    {
        DE_DEBUG("")

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
*/






#endif