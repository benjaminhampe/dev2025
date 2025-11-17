#include "Editor.h"
#include "Plugin.h"

#include "fonts/fonts_ShareTechMonoRegular_ttf.h"

#include <GL/glew.h>
#include <GL/wglew.h>

#include "../res/resource.h"

// ------------------ Editor Implementation ------------------

Editor::Editor(Plugin* plugin)
    : m_plugin(plugin)
    , m_vg(nullptr)
    , m_updateTimerId(0)
    , m_screenWidth(800)
    , m_screenHeight(600)
    , m_mouseX(0)
    , m_mouseY(0)
    , m_paintEventEnabled(false)
    , m_doPartialDawing(false)
{
    m_erect.left = 0;
    m_erect.top = 0;
    m_erect.right = m_screenWidth;
    m_erect.bottom = m_screenHeight;
}

Editor::~Editor()
{
}

bool
Editor::create(void* parent)
{
    DE_DEBUG("")

    m_window = new de::Window_WGL(this);

    de::WindowOptions params;
    params.width = 800;
    params.height = 600;
    params.parent = parent;
    if (!m_window->create(params))
    {
        DE_ERROR("Cannot create window")
        delete m_window;
        m_window = nullptr;
    }

    m_window->setResizable( true );
    m_window->setWindowTitle( "hambe SineMachine4" );
    m_window->setWindowIcon( aaaa );
    //m_window->setHideOnClose( false ); // Overrides WM_CLOSE (X-Button) handler to hide window and not destroy it (no WM_DESTROY called).
    //m_window->setPostQuitMessage( false );

    de::Recti clientRect = m_window->getClientRect();
    // m_erect.left = clientRect.x;
    // m_erect.top = clientRect.y;
    // m_erect.right = clientRect.x + clientRect.w;
    // m_erect.bottom = clientRect.y + clientRect.h;
    // DE_OK("clientRect = {", clientRect.str(), "}")

    glewExperimental = GL_TRUE;
    glewInit();

    m_vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    m_fontShareTechMonoRegular = nvgCreateFontMem(m_vg, "ShareTechMonoRegular",
                                                  const_cast<unsigned char*>(fonts_ShareTechMonoRegular_ttf),
                                                  fonts_ShareTechMonoRegular_ttf_len, 0);

    updateLayout();

    m_preview.init( m_plugin->getSynth()->m_partials );

    m_paintEventEnabled = true;

    // m_updateTimerId = m_window->startTimer(13);

    return true;
}


void Editor::destroy()
{
    DE_DEBUG("")

    if (m_window && m_updateTimerId)
    {
        m_window->killTimer(m_updateTimerId);
        m_updateTimerId = 0;
    }

    if (m_vg)
    {
        nvgDeleteGL3(m_vg);
        m_vg = nullptr;
    }
    // glfwDestroyWindow(m_window);
    // glfwTerminate();

    if (m_window)
    {
        m_window->requestClose();
        delete m_window;
        m_window = nullptr;
    }

    // wglMakeCurrent(nullptr, nullptr);
    // wglDeleteContext(glrc);
    // ReleaseDC(hwnd, hdc);
    // DestroyWindow(hwnd);
    // hwnd = nullptr;
}

void Editor::paintEvent( const de::PaintEvent& event )
{
    if (!m_paintEventEnabled)
    {
        DE_ERROR("Not ready")
        return;
    }

    if (!m_vg)
    {
        DE_ERROR("nanovg not ready")
        return;
    }

    int32_t w = m_screenWidth;
    int32_t h = m_screenHeight;

    //int winW, winH;
    //glfwGetFramebufferSize(m_window, &winW, &winH);
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(m_vg, w, h, 1.0f);

    nvgFontFace(m_vg, "ShareTechMonoRegular");
    nvgFontSize(m_vg, 24.0f);  // in pixels
    nvgTextAlign(m_vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

    drawLineRect(m_vg, m_rHeader, nvgRGBA(255,0,255,255));
    drawLineRect(m_vg, m_rFooter, nvgRGBA(0,0,0,255));
    drawLineRect(m_vg, m_rPreview, nvgRGBA(255,128,0,255));
    drawLineRect(m_vg, m_rButtons, nvgRGBA(255,200,100,255));
    drawLineRect(m_vg, m_rPartial, nvgRGBA(255,100,100,255));
    drawLineRect(m_vg, m_rVolume, nvgRGBA(100,100,255,255));

    m_preview.update( m_plugin->getSynth()->m_partials );
    m_preview.updatePoints( m_rPreview );
    m_preview.draw( m_vg, m_rPreview );

    // Draw overtone bars
    for (int i = 0; i < NUM_PARTIALS; ++i)
    {
        const auto & partial = m_plugin->getSynth()->m_partials[i];
        float amp = partial.amplitude;
        float barW = float(m_rPartial.w) / float(NUM_PARTIALS);
        float barH = amp * m_rPartial.h;
        float x = float(m_rPartial.x) + barW * i;
        float y = float(m_rPartial.y) + float(m_rPartial.h) - barH;

        nvgBeginPath(m_vg);
        nvgRect(m_vg, x + 2, y, barW - 4, barH);
        nvgFillColor(m_vg, partial.color);
        nvgFill(m_vg);

        // white text
        nvgFontSize(m_vg, 20.0f);  // in pixels
        nvgTextAlign(m_vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgFillColor(m_vg, nvgRGBA(0, 0, 0, 255));
        nvgText(m_vg, x + barW/2.0f, y + 16, partial.str().c_str(), nullptr);
    }

    // Draw mouse position as a small circle
    nvgBeginPath(m_vg);
    nvgCircle(m_vg, m_mouseX, m_mouseY, 5.0f);
    nvgFillColor(m_vg, nvgRGBA(255, 100, 100, 255));
    nvgFill(m_vg);

    nvgEndFrame(m_vg);
}

void Editor::updateLayout()
{
    int32_t w = m_screenWidth;
    int32_t h = m_screenHeight;

    int p = 20;
    int hHeader = 64; // PresetBar
    int hFooter = 64; // Active SynthNote Display
    int hBody = h - hHeader - hFooter;

    int h1 = hBody / 2; // Preview + Main Buttons
    int h2 = hBody - h1; // Partial editor + Volume ctrl
    int hButtons = 64;
    int hVolume = 64;
    int hPreview = h1 - hButtons -p-p-p;
    int hPartial = h2 - hVolume -p-p;
    m_rHeader = de::Recti(0,0,w,hHeader);
    m_rFooter = de::Recti(0,h-1-hFooter,w,h-hHeader);
    m_rPreview = de::Recti(p,hHeader+p, w-p-p, hPreview);
    m_rButtons = de::Recti(p,hHeader+p+hPreview+p, w-p-p,hButtons);
    m_rPartial = de::Recti(p,hHeader+p+hPreview+p+hButtons+p, w-p-p,hPartial);
    m_rVolume = de::Recti(p,hHeader+p+hPreview+p+hButtons+p+hPartial+p, w-p-p,hVolume);
}

void Editor::doPartialDrawing()
{
    if (!m_doPartialDawing)
    {
        return;
    }

    float scale = float(NUM_PARTIALS) / float(m_rPartial.w);
    int bar = (m_mouseX - m_rPartial.x) * scale;
    if (bar >= 0 && bar < NUM_PARTIALS)
    {
        float t = (float(m_mouseY) - float(m_rPartial.y)) / float(m_rPartial.h);
        float A = std::clamp(1.0f - t, 0.0f, 1.0f);
        m_plugin->setParameter(bar, A);
    }
}

void Editor::timerEvent( const de::TimerEvent& event )
{
    // if (event.id == m_updateTimerId)
    // {
    //     if (m_window)
    //     {
    //         m_window->update();
    //     }
    // }
}

void Editor::resizeEvent( const de::ResizeEvent& event )
{
    int w = event.w;
    int h = event.h;

    m_screenWidth = w;
    m_screenHeight = h;

    // if (m_window)
    // {
    //     de::Recti clientRect = m_window->getClientRect();
    //     m_erect.left = clientRect.x;
    //     m_erect.top = clientRect.y;
    //     m_erect.right = clientRect.x + clientRect.w;
    //     m_erect.bottom = clientRect.y + clientRect.h;
    //     DE_OK("clientRect = {", clientRect.str(), "}")
    // }
    // else
    // {
    //     m_erect.left = 0;
    //     m_erect.top = 0;
    //     m_erect.right = m_screenWidth;
    //     m_erect.bottom = m_screenHeight;
    //     DE_OK("No clientRect")
    // }

    updateLayout();
}

void Editor::moveEvent( const de::MoveEvent& event )
{
    if (m_window)
    {
        de::Recti clientRect = m_window->getClientRect();
        m_erect.left = clientRect.x;
        m_erect.top = clientRect.y;
        m_erect.right = clientRect.x + clientRect.w;
        m_erect.bottom = clientRect.y + clientRect.h;
        DE_OK("clientRect = {", clientRect.str(), "}")
    }
    else
    {
        m_erect.left = event.x;
        m_erect.top = event.y;
        m_erect.right = event.x + m_screenWidth;
        m_erect.bottom = event.y + m_screenHeight;
        DE_OK("No clientRect")
    }

    updateLayout();
}


void Editor::mousePressEvent( const de::MousePressEvent& event )
{
    if (event.isLeft())
    {
        m_doPartialDawing = true;
    }
    doPartialDrawing();
}

void Editor::mouseReleaseEvent( const de::MouseReleaseEvent& event )
{
    if (event.isLeft())
    {
        m_doPartialDawing = false;
    }
    doPartialDrawing();
}

void Editor::mouseMoveEvent( const de::MouseMoveEvent& event )
{
    m_mouseX = event.x;
    m_mouseY = event.y;

    doPartialDrawing();
}

void Editor::mouseWheelEvent( const de::MouseWheelEvent& event )
{
}

void Editor::mouseDblClickEvent( const de::MouseDblClickEvent& event )
{
}

void Editor::keyPressEvent(const de::KeyPressEvent& event)
{
    if (event.key == de::KEY_ESCAPE)
    {
        //glfwSetWindowShouldClose(window, true); // Close window on ESC
    }

    if (event.key == de::KEY_1)
    {
        m_plugin->getSynth()->setPartialsToRect();
    }

    if (event.key == de::KEY_2)
    {
        m_plugin->getSynth()->setPartialsToSaw();
    }

    if (event.key == de::KEY_3)
    {
        m_plugin->getSynth()->setPartialsToSawRev();
    }

    if (event.key == de::KEY_4)
    {
        m_plugin->getSynth()->setPartialsToTriangle();
    }
}


void Editor::keyReleaseEvent(const de::KeyReleaseEvent& event)
{
}

/*
 *
void Editor::toggleFullscreen()
{
    fullscreen = !fullscreen;
    SetWindowLong(hwnd, GWL_STYLE, fullscreen ? WS_POPUP : WS_CHILD | WS_VISIBLE);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, fullscreen ? 1920 : 800, fullscreen ? 1080 : 600, SWP_FRAMECHANGED);
}

void Editor::run()
{
    double timeInSecNow = glfwGetTime();
    double timeLastFrameBufferUpdate = 0;
    double timeLastWindowTitleUpdate = 0;
    while (!glfwWindowShouldClose(m_window) && !shouldClose)
    {
        timeInSecNow = glfwGetTime();

        if (timeInSecNow - timeLastFrameBufferUpdate >= 1./30.0)
        {
            draw();
            timeLastFrameBufferUpdate = glfwGetTime();
        }

        glfwPollEvents();
    }
    close();
}

void Editor::requestClose()
{
    shouldClose = true;
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}
*/
