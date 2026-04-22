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

        int w = newSize->getWidth();
        int h = newSize->getHeight();
        if (w < 1)
        {
            DE_ERROR("Got stupid plugin width ",w)
            w = 100;
        }
        else if (w > 10000)
        {
            DE_ERROR("Got stupid plugin width ",w)
            w = 1024;
        }

        if (h < 1)
        {
            DE_ERROR("Got stupid plugin height ",h)
            h = 100;
        }
        else if (h > 10000)
        {
            DE_ERROR("Got stupid plugin height ",h)
            h = 1024;
        }
        m_parentWidget->setGeometry( m_parentWidget->x(), m_parentWidget->y(), w,h);

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

        //Steinberg::ViewRect r(0, 0, width(), height());
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

            int w = width();
            int h = height();
            if (w < 1)
            {
                DE_ERROR("Got stupid widget width ",w)
                w = 100;
            }
            else if (w > 10000)
            {
                DE_ERROR("Got stupid widget width ",w)
                w = 1024;
            }

            if (h < 1)
            {
                DE_ERROR("Got stupid widget height ",h)
                h = 100;
            }
            else if (h > 10000)
            {
                DE_ERROR("Got stupid widget height ",h)
                h = 1024;
            }

            Steinberg::ViewRect r(0, 0, w, h);
            m_plugView->onSize(&r);
        }
    }
};

#endif