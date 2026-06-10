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

    std::atomic<Steinberg::uint32> m_refCount;

    explicit VST3_PlugFrame(QWidget* widget)
        : m_parentWidget(widget)
        , m_refCount {1}
    {
        DE_BENNI("VST3_PlugFrame()")
    }

    ~VST3_PlugFrame()
    {
        DE_BENNI("~VST3_PlugFrame()")
    }

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

    VST3_Editor(Steinberg::IPlugView* plugView)
        : PluginEditorWindow(nullptr)
        , m_plugView(plugView)
        , m_plugFrame(nullptr)
    {
        DE_BENNI("VST3_Editor()")
        //setAttribute(Qt::WA_NativeWindow);   // REQUIRED
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
        DE_BENNI("~VST3_Editor()")
        // if (m_plugView)
        //     m_plugView->removed();
        m_plugView = nullptr;
        if (m_plugFrame)
        {
            delete m_plugFrame;
            m_plugFrame = nullptr;
        }
    }

    // void closeEvent(QCloseEvent* e) override
    // {
    //     DE_BENNI("VST3_Editor.closeEvent")
    //     PluginEditorWindow::closeEvent(e);
    // }

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

#if RAW_POINTERS

void VST3_Plugin::closePlugin()
{
    // ------------------------------------------------------------
    // BEFORE ANYTHING — starting refcounts (your measured values)
    // ------------------------------------------------------------
    // Dual:   PROC=5, COMP=5, CTRL=5, VIEW=0, HOSTH=4, HOSTA=1, MOD=1
    // Single: PROC=6, COMP=6, CTRL=6, VIEW=0, HOSTH=4, HOSTA=1, MOD=1


    // ============================================================
    // 1. Destroy editor FIRST (if you have one)
    // ============================================================
    delete m_editor;
    m_editor = nullptr;

    // Expected refcount change:
    // None — editor holds raw pointers only.


    // ============================================================
    // 2. Remove the view
    // ============================================================
    if (m_plugView)
    {
        m_plugView->removed();
        // Expected: plugin drops 1 internal ref to frame
        // No change to PROC/COMP/CTRL

        m_plugView = nullptr;
        // Expected: your IPtr drops 1 ref to the view
        // VIEW object refcount → 0 (dual)
        // VIEW object refcount → 0 (single)
    }


    // ============================================================
    // 3. Stop audio processing
    // ============================================================
    if (m_audioProcessor)
    {
        m_audioProcessor->setProcessing(false);
        // Expected: no refcount change

        m_audioProcessor = nullptr;
        // Expected: PROC-- (dual: 5→4, single: 6→5)
        // Because PROC, COMP, CTRL share the same object in single-component,
        // this also reduces COMP and CTRL by 1.
    }


    // ============================================================
    // 4. Release controller
    // ============================================================
    if (m_editController)
    {
        m_editController->setComponentHandler(nullptr);
        // Expected: HOSTH-- (dual: 4→3, single: 4→3)

        if (!m_bIsSingleComponent)
        {
            m_editController->terminate();
            // Expected: plugin internal refs drop (dual only)
            // Typically CTRL-- and maybe COMP-- internally
        }

        m_editController = nullptr;
        // Expected: CTRL-- (dual: 4→3, single: 5→4)
        // Also reduces COMP/PROC in single-component (shared object)
    }


    // ============================================================
    // 5. Release component
    // ============================================================
    if (m_component)
    {
        m_component->setActive(false);
        // Expected: no refcount change

        m_component->terminate();
        // Expected: plugin internal refs drop (dual and single)
        // COMP-- (dual: 3→2, single: 4→3)
        // Possibly more internal releases depending on plugin

        m_component = nullptr;
        // Expected: COMP-- (dual: 2→1, single: 3→2)
        // In single-component, this also reduces PROC/CTRL.
    }


    // ============================================================
    // 6. Release host interfaces
    // ============================================================
    m_hostHandler = nullptr;
    // Expected: HOSTH-- (dual: 3→2, single: 3→2)

    m_hostApp = nullptr;
    // Expected: HOSTA-- (dual: 1→0, single: 1→0)


    // ============================================================
    // 7. Unload module
    // ============================================================
    m_module = nullptr;
    // Expected: no VST3 refcount change
    // But if ANY plugin object still has refcount > 0,
    // the next release() call will crash because DLL is gone.
}


class VST3_Plugin_Impl
{
public:
    VST3_Plugin_Impl() = default;

    ~VST3_Plugin_Impl()
    {
        closePlugin();
    }

    // ------------------------------------------------------------
    // OPEN PLUGIN
    // ------------------------------------------------------------
    bool openPlugin(const std::wstring& dllPath, const Steinberg::FUID& classID)
    {
        closePlugin(); // ensure clean state

        // 1. Load DLL
        m_hModule = ::LoadLibraryW(dllPath.c_str());
        if (!m_hModule)
            return false;

        auto getFactory = reinterpret_cast<Steinberg::IPluginFactory* (*)()>(
            ::GetProcAddress(m_hModule, "GetPluginFactory"));
        if (!getFactory)
        {
            closePlugin();
            return false;
        }

        m_factory = getFactory();
        if (!m_factory)
        {
            closePlugin();
            return false;
        }

        // 2. Create component
        Steinberg::Vst::IComponent* comp = nullptr;
        if (m_factory->createInstance(classID,
                                      Steinberg::Vst::IComponent::iid,
                                      (void**)&comp) != Steinberg::kResultOk
            || !comp)
        {
            closePlugin();
            return false;
        }

        m_component = comp;
        m_component->addRef();

        // 3. Query audio processor
        Steinberg::Vst::IAudioProcessor* proc = nullptr;
        if (m_component->queryInterface(Steinberg::Vst::IAudioProcessor::iid,
                                        (void**)&proc) == Steinberg::kResultOk
            && proc)
        {
            m_audioProcessor = proc;
            m_audioProcessor->addRef();
        }

        // 4. Create or get controller
        Steinberg::Vst::IEditController* ctrl = nullptr;

        // Try separate controller (dual-component)
        if (m_factory->createInstance(classID,
                                      Steinberg::Vst::IEditController::iid,
                                      (void**)&ctrl) == Steinberg::kResultOk
            && ctrl)
        {
            m_bIsSingleComponent = false;
            m_editController = ctrl;
            m_editController->addRef();
        }
        else
        {
            // Single-component: controller is same object as component
            if (m_component->queryInterface(Steinberg::Vst::IEditController::iid,
                                            (void**)&ctrl) == Steinberg::kResultOk
                && ctrl)
            {
                m_bIsSingleComponent = true;
                m_editController = ctrl;
                m_editController->addRef();
            }
        }

        // 5. Create view
        if (m_editController)
        {
            Steinberg::IPlugView* view = m_editController->createView(Steinberg::Vst::ViewType::kEditor);
            if (view)
            {
                m_plugView = view;
                m_plugView->addRef();
            }
        }

        m_bIsPluginOpen = true;
        return true;
    }

    // ------------------------------------------------------------
    // CLOSE PLUGIN (FULL TEARDOWN + DLL UNLOAD)
    // ------------------------------------------------------------
    void closePlugin()
    {
        if (!m_bIsPluginOpen && !m_hModule)
            return;

        // 1. View
        if (m_plugView)
        {
            m_plugView->removed();
            m_plugView->release();
            m_plugView = nullptr;
        }

        // 2. Audio processor
        if (m_audioProcessor)
        {
            m_audioProcessor->setProcessing(false);
            m_audioProcessor->release();
            m_audioProcessor = nullptr;
        }

        // 3. Controller
        if (m_editController)
        {
            m_editController->setComponentHandler(nullptr);

            if (!m_bIsSingleComponent)
                m_editController->terminate();

            m_editController->release();
            m_editController = nullptr;
        }

        // 4. Component
        if (m_component)
        {
            m_component->setActive(false);
            m_component->terminate();
            m_component->release();
            m_component = nullptr;
        }

        // 5. Host interfaces (your own objects)
        if (m_hostHandler)
        {
            m_hostHandler->release();
            m_hostHandler = nullptr;
        }

        if (m_hostApp)
        {
            m_hostApp->release();
            m_hostApp = nullptr;
        }

        // 6. Factory (owned by DLL)
        m_factory = nullptr;

        // 7. Unload DLL
        if (m_hModule)
        {
            ::FreeLibrary(m_hModule);
            m_hModule = nullptr;
        }

        m_bIsPluginOpen      = false;
        m_bIsSingleComponent = false;
    }

private:
    // DLL + factory
    HMODULE                     m_hModule        = nullptr;
    Steinberg::IPluginFactory*  m_factory        = nullptr;

    // Plugin interfaces (raw pointers)
    Steinberg::Vst::IComponent*        m_component      = nullptr;
    Steinberg::Vst::IAudioProcessor*   m_audioProcessor = nullptr;
    Steinberg::Vst::IEditController*   m_editController = nullptr;
    Steinberg::IPlugView*              m_plugView       = nullptr;

    // Host interfaces (your own objects)
    Steinberg::Vst::IHostApplication*  m_hostApp        = nullptr;
    Steinberg::Vst::IComponentHandler* m_hostHandler    = nullptr;

    bool m_bIsSingleComponent = false;
    bool m_bIsPluginOpen      = false;
};

#endif
