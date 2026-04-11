#pragma once
#include <de/audio/plugin/IPlugin.h>

#ifdef BENNI_USE_VST3

#include <de/Core.h>
#include <QWidget>
#include <QResizeEvent>
#include "pluginterfaces/gui/iplugview.h"

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

    ~VST3_Editor() override
    {
        // if (m_plugView)
        //     m_plugView->removed();
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

#endif