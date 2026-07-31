#include "GL_WaveformWidget_fltk.h"

//#include <de_opengl.h>
#include <FL/gl.h> // for gl_texture_reset()

GL_WaveformWidget::GL_WaveformWidget(int X, int Y, int W, int H)
    : Fl_Gl_Window(X, Y, W, H)
    , m_sound(nullptr)
{
    mode(FL_RGB8 | FL_DOUBLE | FL_OPENGL3);
    m_scroll = new Fl_Scrollbar(X, Y + H - 16, W, 16);
    m_scroll->type(FL_HORIZONTAL);
    m_scroll->callback(scroll_cb, this);
    update_scroll_range();
}

// static
void GL_WaveformWidget::scroll_cb(Fl_Widget*, void* userdata)
{
    auto* self = static_cast<GL_WaveformWidget*>(userdata);
    self->redraw();
}

void GL_WaveformWidget::setSound(de::Sound* snd)
{
    m_sound = snd;
    m_accessor.setSound( m_sound );
    m_zoom = 1.0;
    m_zoomFrameStart = 0;
    update_scroll_range();
    redraw();
}

void GL_WaveformWidget::setZoomStart(double pc)
{
    if (!m_sound) return;
    m_zoomFrameStart = std::llround(pc * m_zoom * m_sound->m_frames);
    redraw();
}

void GL_WaveformWidget::setZoom(double z)
{
    m_zoom = std::clamp<double>(z, 0.000001, 1000000.0);
    redraw();
}

double GL_WaveformWidget::getZoom() const { return m_zoom; }

int64_t GL_WaveformWidget::startFrame() const { return m_loopFrameStart; }
int64_t GL_WaveformWidget::endFrame()   const { return m_loopFrameEnd; }

void GL_WaveformWidget::draw()
{
    if (!valid())
    {
        //DE_BENNI("Initialize OpenGL")
        //ensureDesktopOpenGL();
    }

    int w = pixel_w();
    int h = pixel_h();

    glViewport(x(),y(),w,h);
    glClearColor(0,0,0.5,1);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (!m_sound)
    {
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const int rulerHeight = 32;
    const double sr = m_sound->m_sampleRate;

    const double fpp = getFramesPerPixel();
    const double ppf = 1.0f / fpp;

    const int64_t zoomFrameCount = std::llround(double(m_sound->m_frames) * m_zoom);
    const int64_t visFrames = std::min(zoomFrameCount, 1000ll * w);

    const double majorSec = 1.0;   // 1 second
    const double minorSec = 0.1;   // 100 ms

    const int64_t majorFrames = (int64_t)(majorSec * sr);
    const int64_t minorFrames = (int64_t)(minorSec * sr);
    const int64_t endFrame = m_zoomFrameStart + std::llround(fpp * double(w));

    // ticks
    const int64_t firstMajor = ((m_zoomFrameStart + majorFrames - 1) / majorFrames) * majorFrames;
    const int64_t firstMinor = ((m_zoomFrameStart + minorFrames - 1) / minorFrames) * minorFrames;

    // Ruler background
    {
        glColor4f(.4f, .4f, .4f, 1.0f);
        glBegin(GL_QUADS);
        int x1 = x();
        int y1 = y();
        int x2 = x1 + w;
        int y2 = y1 + rulerHeight;
        glVertex2f(x1,y1);
        glVertex2f(x2,y1);
        glVertex2f(x2,y2);
        glVertex2f(x1,y2);
        glEnd();
    }

    // Ruler Major ticks
    glColor4f(.6f, .6f, .6f, .4f);
    glBegin(GL_LINES);

    for (int64_t i = firstMajor; i < endFrame; i += majorFrames)
    {
        int32_t px = x() + std::lroundf((i - m_zoomFrameStart) * ppf);
        int x1 = px;
        int y1 = y();
        int x2 = px;
        int y2 = y() + h;
        glVertex2f(x1,y1);
        glVertex2f(x2,y2);
    }
    glEnd();

    // Minor ticks
    glColor4f(.7f, .7f, .7f, .6f);
    glBegin(GL_LINES);

    for (int64_t i = firstMinor; i < endFrame; i += minorFrames)
    {
        int32_t px = x() + std::lroundf((i - m_zoomFrameStart) * ppf);

        int x1 = px;
        int y1 = y() + rulerHeight;
        int x2 = px;
        int y2 = y() + h;
        glVertex2f(x1,y1);
        glVertex2f(x2,y2);
    }
    glEnd();

    // Draw Middle Line
    glColor4f(1.f, 1.f, 1.f, 1.0f);
    glBegin(GL_LINES);
    int x1 = 0;
    int y1 = y() + (h - rulerHeight)/2;
    int x2 = w;
    int y2 = y1;
    glVertex2f(x1,y1);
    glVertex2f(x2,y2);
    glEnd();

    // Draw WaveForm
    if (visFrames >= 2)
    {
        const int waveformH = h - rulerHeight;
        const int waveformM = waveformH * 0.5f;
        const int waveformY = y() + rulerHeight + waveformM;

        float s1 = m_accessor.getSamplef(m_zoomFrameStart); // [-1,1]
        float x1 = x();
        float y1 = waveformY - float(s1 * waveformM);

        float alpha = std::clamp(ppf,0.01,1.0);

        glColor4f(1.f, 1.f, 1.f,alpha);
        glBegin(GL_LINES);

        for (int64_t i = 1; i < visFrames; ++i)
        {
            float s2 = m_accessor.getSamplef(m_zoomFrameStart + std::llround(fpp * i)); // [-1,1]
            float x2 = float(x()) + (float(i) * float(ppf));
            float y2 = waveformY - float(s2 * waveformM);

            glVertex2f(x1,y1);
            glVertex2f(x2,y2);

            x1 = x2;
            y1 = y2;
        }
        glEnd();

        DE_BENNI("w(",w,"), h(",h,"), "
         "zoom(",m_zoom,"), "
         "fpp(",fpp,"), "
         "start(",m_zoomFrameStart,"), "
         "count(",zoomFrameCount,"), "
         "vis(",visFrames,"), "
         "alpha(",alpha,")")
    }

    // --- Cut region ---
    if (m_selecting || (m_loopFrameStart >= 0 && m_loopFrameEnd > m_loopFrameStart))
    {
        const int sx = std::llround(double(m_loopFrameStart - m_zoomFrameStart) * ppf);
        const int ex = std::llround(double(m_loopFrameEnd   - m_zoomFrameStart) * ppf);
        const int x1 = x() + sx;
        const int x2 = x() + (ex - sx);
        const int y1 = y() + rulerHeight;
        const int y2 = y() + h - rulerHeight;
        glColor4f(1.f, 0.f, 0.f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(x1,y1);
        glVertex2f(x2,y1);
        glVertex2f(x2,y2);
        glVertex2f(x1,y2);
        glEnd();
    }

    Fl_Gl_Window::draw();

    /*
    for (int64_t f = firstMajor; f < endFrame; f += majorFrames)
    {
        int px = x() + (int)((f - startFrame) / framesPerPixel);

        double sec = double(f) / sr;
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", sec);

        fl_color(FL_WHITE);
        fl_draw(buf, px + 2, rulerY + rulerHeight - 5);
    }
    */
}

int GL_WaveformWidget::handle(int e)
{
    if (!m_sound)
        return Fl_Gl_Window::handle(e);

    switch (e)
    {
        case FL_PUSH:
            if (Fl::event_button() == FL_LEFT_MOUSE)
            {
                m_selecting = true;
                m_loopFrameStart = pixelToFrame(Fl::event_x());
                m_loopFrameEnd   = m_loopFrameStart;
                redraw();
            }
            return 1;

        case FL_DRAG:
            if (m_selecting)
            {
                m_loopFrameEnd = pixelToFrame(Fl::event_x());
                if (m_loopFrameEnd < 0) m_loopFrameEnd = 0;
                redraw();
            }
            return 1;

        case FL_RELEASE:
            if (m_selecting)
            {
                m_selecting = false;
                if (m_loopFrameEnd < m_loopFrameStart)
                    std::swap(m_loopFrameStart, m_loopFrameEnd);
                redraw();
            }
            return 1;

        case FL_MOUSEWHEEL:
        {
            const int mx = Fl::event_x();
            //const double oldZoom = m_zoom;
            const int64_t oldFrameStart = m_zoomFrameStart;
            //const int64_t oldFrameCount = m_sound->m_frames * m_zoom;
            const int64_t frameUnderCursor = pixelToFrame(mx);

            const double t = std::clamp( double(frameUnderCursor - oldFrameStart) / double(m_sound->m_frames), 0.0, 1.0);

            // Zoom factor per wheel step
            if (Fl::event_dy() > 0) // Zoom out
            {
                m_zoom = std::clamp<double>(m_zoom * 1.15, 0.000001, 1000000.0);
            }
            else if (Fl::event_dy() < 0) // Zoom In
            {
                m_zoom = std::clamp<double>(m_zoom / 1.15, 0.000001, 1000000.0);
            }

            const int64_t newFrameCount = std::llround( m_zoom * double(m_sound->m_frames) );
            const int64_t maxFrameIndex = std::max( 0ll, m_sound->m_frames - newFrameCount );
            const int64_t newFrameStart = std::clamp( std::llround(t * double(newFrameCount)), 0ll, maxFrameIndex);
            const double t2 = std::clamp( double(newFrameStart) / double(m_sound->m_frames), 0.0, 1.0);
            m_scroll->value(t2);
            redraw();
            return 1;
        }
    default:
        break;
    }
    return Fl_Gl_Window::handle(e);
}

// --- 64-bit safe scroll range ---
void GL_WaveformWidget::update_scroll_range()
{
    if (!m_sound) return;

    const double d = double(m_zoomFrameStart) / double(m_sound->m_frames);
    m_scroll->bounds(0.0, 1.0);
    m_scroll->value(d);
}

// --- 64-bit safe pixel→frame ---
int64_t GL_WaveformWidget::pixelToFrame(int px)
{
    if (!m_sound)
    {
        DE_ERROR("No sound")
        return 0;
    }

    if (m_sound->m_frames < 2)
    {
        DE_ERROR("No m_sound->m_frames >= 2")
        return 0;
    }

    const double t = std::clamp<double>(double(px - x()) / double(pixel_w()), 0.0, 1.0);

    int64_t frame = m_zoomFrameStart + std::llround( t * double(m_sound->m_frames) * m_zoom );

    return std::clamp(frame, 0ll, m_sound->m_frames - 1);
}
