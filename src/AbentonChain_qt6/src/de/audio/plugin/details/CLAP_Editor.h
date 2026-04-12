#pragma once
#include <de/audio/plugin/IPlugin.h>

#ifdef BENNI_USE_CLAP

/*
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

*/

#endif