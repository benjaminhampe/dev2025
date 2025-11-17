#include "Editor.h"
#include "Synth.h"

namespace {

    //🖌️ Drawing a Colored Rectangle Border
    inline void
    drawLineRect(NVGcontext* vg, int x, int y, int w, int h, const NVGcolor& color, float strokeWidth = 4.0f)
    {
        nvgBeginPath(vg);                     // Start a new path
        nvgRect(vg, x, y, w, h);              // Define the rectangle
        nvgStrokeWidth(vg, strokeWidth);     // Set border thickness
        nvgStrokeColor(vg, color); // Set border color (red)
        nvgStroke(vg);                        // Render the stroke
    }

    //🖌️ Drawing a Colored Rectangle Border
    inline void
    drawLineRect(NVGcontext* vg, const de::Recti& pos, const NVGcolor& color, float strokeWidth = 4.0f)
    {
        drawLineRect(vg,pos.x,pos.y,pos.w,pos.h,color,strokeWidth);
    }

} // end namespace.

// ------------------ Editor Implementation ------------------

Editor::Editor(Synth* effect)
    : m_synth(effect)
    , m_window(nullptr)
    , m_vg(nullptr)
    , m_doPartialDawing(false)
    , m_mouseX(0.0)
    , m_mouseY(0.0)
{
    m_preview.init( m_synth->m_partials );
}

Editor::~Editor()
{}

void Editor::open()
{
    if (!glfwInit()) return;
    m_window = glfwCreateWindow(m_screenWidth, m_screenHeight, "Sine Overtones", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);

    ensureDesktopOpenGL();

    glfwSetWindowUserPointer(m_window, this);
    glfwSetMouseButtonCallback(m_window, mouseButtonEvent);
    glfwSetCursorPosCallback(m_window, mouseMoveEvent);
    glfwSetWindowSizeCallback(m_window, resizeEvent);
    glfwSetKeyCallback(m_window, keyboardEvent);

    m_vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    m_fontNotoEmojiMedium = nvgCreateFontMem(m_vg, "NotoEmojiMedium",
        const_cast<unsigned char*>(fonts_NotoEmojiMedium_ttf),
        fonts_NotoEmojiMedium_ttf_len, 0);

    m_fontShareTechMonoRegular = nvgCreateFontMem(m_vg, "ShareTechMonoRegular",
        const_cast<unsigned char*>(fonts_ShareTechMonoRegular_ttf),
        fonts_ShareTechMonoRegular_ttf_len, 0);
/*
    int font = nvgCreateFont(m_vg, "sans", "path/to/font.ttf");
    nvgFontFace(m_vg, "NotoEmojiMedium");
    nvgFontSize(m_vg, 18.0f);  // in pixels
    nvgTextAlign(m_vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFillColor(m_vg, nvgRGBA(255, 255, 255, 255));  // white
    nvgBeginPath(m_vg);
    nvgText(m_vg, x, y, "Hello, NanoVG!", nullptr);

    nvgFontSize(m_vg, 16.0f);
    nvgFontFace(m_vg, "sans");
    nvgTextAlign(m_vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFillColor(m_vg, nvgRGBA(255, 255, 0, 255));
    nvgText(m_vg, mouseX + 10, mouseY + 10, "Mouse here!", nullptr);
*/
    updateLayout();
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

void Editor::close()
{
    nvgDeleteGL3(m_vg);
    glfwDestroyWindow(m_window);
    glfwTerminate();
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

void Editor::draw()
{
    int32_t w = m_screenWidth;
    int32_t h = m_screenHeight;

    //int winW, winH;
    //glfwGetFramebufferSize(m_window, &winW, &winH);
    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(m_vg, w, h, 1.0f);

    // nvgFontFace(m_vg, "NotoEmojiMedium");
    nvgFontFace(m_vg, "ShareTechMonoRegular");
    nvgFontSize(m_vg, 24.0f);  // in pixels
    nvgTextAlign(m_vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

    drawLineRect(m_vg, m_rHeader, nvgRGBA(255,0,255,255));
    drawLineRect(m_vg, m_rFooter, nvgRGBA(0,0,0,255));
    drawLineRect(m_vg, m_rPreview, nvgRGBA(255,128,0,255));
    drawLineRect(m_vg, m_rButtons, nvgRGBA(255,200,100,255));
    drawLineRect(m_vg, m_rPartial, nvgRGBA(255,100,100,255));
    drawLineRect(m_vg, m_rVolume, nvgRGBA(100,100,255,255));

    m_preview.update( m_synth->m_partials );
    m_preview.updatePoints( m_rPreview );
    m_preview.draw( m_vg, m_rPreview );

    // Draw overtone bars
    for (int i = 0; i < NUM_PARTIALS; ++i)
    {
        const auto & partial = m_synth->m_partials[i];
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
    glfwSwapBuffers(m_window);
}

void Editor::resizeEvent(GLFWwindow* window, int w, int h)
{
    Editor* self = static_cast<Editor*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    m_screenWidth = w;
    m_screenHeight = h;

    // Optional: update NanoVG or layout logic
    //glViewport(0, 0, w, h);
    self->updateLayout();
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
        m_synth->setParameter(bar, A);
    }
}

void Editor::mouseButtonEvent(GLFWwindow* window, int button, int action, int mods)
{
    Editor* self = static_cast<Editor*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            self->m_doPartialDawing = true;
        }
        else
        {
            self->m_doPartialDawing = false;
        }
    }
    self->doPartialDrawing();
}

void Editor::mouseMoveEvent(GLFWwindow* window, double xpos, double ypos)
{
    Editor* self = static_cast<Editor*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    self->m_mouseX = xpos;
    self->m_mouseY = ypos;

    self->doPartialDrawing();
}

void Editor::keyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        //glfwSetWindowShouldClose(window, true); // Close window on ESC
    }

    Editor* self = static_cast<Editor*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        self->m_synth->setPartialsToRect();
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        self->m_synth->setPartialsToSaw();
    }
}
