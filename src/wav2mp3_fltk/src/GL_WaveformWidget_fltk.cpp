#include "GL_WaveformWidget_fltk.h"

#ifdef USE_BENNI_GL
    //#include <de_opengl.h>
    #include <FL/gl.h> // for gl_texture_reset()
#endif

GL_WaveformWidget::GL_WaveformWidget(int X, int Y, int W, int H)
    #ifdef USE_BENNI_GL
    : Fl_Gl_Window(X, Y, W, H)
    #else
    : Fl_Widget(X, Y, W, H)
    #endif
    , m_sound(nullptr)
{
    #ifdef USE_BENNI_GL
    mode(FL_RGB8 | FL_DOUBLE | FL_OPENGL3);
    #else
    m_img = de::Image(W,H);
    #endif

    #ifdef USE_EXTERNAL_SCROLLBAR
    // m_scroll = new Fl_Scrollbar(X, Y + H - 16, W, 16);
    // m_scroll->type(FL_HORIZONTAL);
    // m_scroll->callback(scroll_cb, this);
    update_scroll_range();
    #endif
}

// static
// void GL_WaveformWidget::scroll_cb(Fl_Widget*, void* userdata)
// {
//     auto* self = static_cast<GL_WaveformWidget*>(userdata);
//     self->redraw();
// }

void GL_WaveformWidget::setSound(de::Sound* snd)
{
    m_sound = snd;
    m_converter = nullptr;
    m_zoomBeg = 0;
    m_zoomEnd = 0;
    m_loopBeg = -1;
    m_loopEnd = -1;
    //m_accessor.setSound( m_sound );

    if (!m_sound)
    {
        DE_ERROR("No sound")
    }
    else
    {
        m_sampleBuf.resize(m_sound->channels());

        auto srcType = m_sound->sampleType();
        auto dstType = de::SampleType::F32;
        m_converter = de::SampleTypeConverter::getConverter(srcType,dstType);
        if (!m_converter)
        {
            DE_ERROR("No converter")
        }

        m_zoomEnd = m_sound->frames();
        //m_loopEnd = m_sound->frames();
        m_bImageDirty = true;
        redraw();
    }

#ifdef USE_EXTERNAL_SCROLLBAR
    update_scroll_range();
#endif
    redraw();
}

void GL_WaveformWidget::resize(int X, int Y, int W, int H)
{
    Fl_Widget::resize(X, Y, W, H);
    m_bImageDirty = true;
    redraw();
}

void GL_WaveformWidget::draw()
{
    if (!m_sound)
    {
        return;
    }

#ifdef USE_BENNI_GL
    int w = this->pixel_w(); //pixel_w();
    int h = this->pixel_h(); //pixel_h();
#else
    int w = this->w();
    int h = this->h();
#endif
    const int rulerHeight = 32;
    const double sr = m_sound->m_sampleRate;

    const double fpp = getFramesPerPixel();
    const double ppf = 1.0f / fpp;

    const int64_t visFrames = std::min(zoomDelta(), 100ll * w);

    const double majorSec = 1.0;   // 1 second
    const double minorSec = 0.1;   // 100 ms

    const int64_t majorFrames = (int64_t)(majorSec * sr);
    const int64_t minorFrames = (int64_t)(minorSec * sr);
    // ticks
    const int64_t firstMajor = ((m_zoomBeg + majorFrames - 1) / majorFrames) * majorFrames;
    const int64_t firstMinor = ((m_zoomBeg + minorFrames - 1) / minorFrames) * minorFrames;

#ifdef USE_BENNI_GL

    glViewport(0,0,w,h);
    glClearColor(0,0,0.5,1);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Ruler background
    {
        glColor4f(.4f, .4f, .4f, 1.0f);
        glBegin(GL_QUADS);
        int x1 = 0;
        int y1 = 0;
        int x2 = w;
        int y2 = rulerHeight;
        glVertex2f(x1,y1);
        glVertex2f(x2,y1);
        glVertex2f(x2,y2);
        glVertex2f(x1,y2);
        glEnd();
    }

    // Ruler Major ticks
    glColor4f(.6f, .6f, .6f, .4f);
    glBegin(GL_LINES);

    for (int64_t i = firstMajor; i < m_zoomEnd; i += majorFrames)
    {
        int32_t px = std::lroundf((i - m_zoomBeg) * ppf);
        int x1 = px;
        int y1 = 0;
        int x2 = px;
        int y2 = h;
        glVertex2f(x1,y1);
        glVertex2f(x2,y2);
    }
    glEnd();

    // Minor ticks
    glColor4f(.7f, .7f, .7f, .6f);
    glBegin(GL_LINES);

    for (int64_t i = firstMinor; i < m_zoomEnd; i += minorFrames)
    {
        int32_t px = std::lroundf((i - m_zoomBeg) * ppf);
        int x1 = px;
        int y1 = rulerHeight;
        int x2 = px;
        int y2 = h;
        glVertex2f(x1,y1);
        glVertex2f(x2,y2);
    }
    glEnd();


    // Draw Middle Line
    glColor4f(1.f, 1.f, 0.f, 1.0f);
    glBegin(GL_LINES);
    int x1 = 0;
    int y1 = rulerHeight + (h - rulerHeight)/2;
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
        const int waveformY = rulerHeight + waveformM;

        m_sound->read_frames(
                    m_converter,
                    m_sampleBuf.data(),
                    1,
                    m_zoomBeg); // [-1,1]

        float s1 = m_sampleBuf[0];
        //float s1 = m_accessor.getSamplef(m_zoomFrameStart); // [-1,1]
        float x1 = 0;
        float y1 = waveformY - float(s1 * waveformM);

        float alpha = std::clamp(ppf,0.01,1.0);

        glColor4f(1.f, 1.f, 1.f,alpha);
        glBegin(GL_LINES);

        float df = double(zoomDelta())/double(visFrames);
        float dx = double(w)/double(zoomDelta() > 0 ? zoomDelta() : 1);

        for (int64_t i = 1; i < visFrames; ++i)
        {
            m_sound->read_frames(
                    m_converter,
                    m_sampleBuf.data(),
                    1,
                    m_zoomBeg + std::llround(df * i)); // [-1,1]

            float s2 = m_sampleBuf[0];
            //float s2 = m_accessor.getSamplef(m_zoomFrameStart + std::llround(fpp * i)); // [-1,1]
            float x2 = dx * i;
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
         "start(",m_zoomBeg,"), "
         "count(",zoomDelta(),"), "
         "vis(",visFrames,"), "
         "alpha(",alpha,")")
    }

    // --- Cut begin ---
    if (m_loopBeg > -1)
    {
        const int x1 = std::lround(double(m_loopBeg - m_zoomBeg) * ppf);
        const int y1 = 0;
        const int x2 = x1;
        const int y2 = h;
        glColor4f(0.f, 1.f, 0.f, 0.9f);
        glBegin(GL_LINES);
        glVertex2f(x1,y1);
        glVertex2f(x2,y2);
        glEnd();
    }

    // --- Cut begin ---
    if (m_loopEnd > -1)
    {
        const int x1 = std::lround(double(m_loopEnd - m_zoomBeg) * ppf);
        const int y1 = 0;
        const int x2 = x1;
        const int y2 = h;
        glColor4f(1.f, 0.f, 0.f, 0.9f);
        glBegin(GL_LINES);
        glVertex2f(x1,y1);
        glVertex2f(x2,y2);
        glEnd();
    }

    // --- Mouse Cursor ---
    if (m_mouseFrame > -1)
    {
        const int x1 = std::lround(double(m_mouseFrame - m_zoomBeg) * ppf);
        const int y1 = 0;
        const int x2 = x1;
        const int y2 = h;
        glColor4f(1.f, 0.5f, 0.f, 0.9f);
        glBegin(GL_LINES);
        glVertex2f(x1,y1);
        glVertex2f(x2,y2);
        glEnd();
    }

    // --- Cut region ---
    if (m_selecting || (m_loopBeg >= 0 && m_loopEnd > m_loopBeg))
    {
        const int sx = std::llround(double(m_loopBeg - m_zoomFrameStart) * ppf);
        const int ex = std::llround(double(m_loopEnd   - m_zoomFrameStart) * ppf);
        const int x1 = sx;
        const int x2 = (ex - sx);
        const int y1 = rulerHeight;
        const int y2 = h - rulerHeight;
        glColor4f(1.f, 0.f, 0.f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(x1,y1);
        glVertex2f(x2,y1);
        glVertex2f(x2,y2);
        glVertex2f(x1,y2);
        glEnd();
    }

    Fl_Gl_Window::draw();
#else

    updateImage();

    fl_draw_image(m_img.data(), x(), y(), m_img.w(), m_img.h(), 4);

    // {
    //     fl_color(FL_DARK_YELLOW);
    //     fl_line(m_mx,y(),m_mx,y()+h);
    // }

    {
        int x1 = frameToPixel(m_loopBeg);
        fl_color(FL_GREEN);
        fl_line(x1 + 2,y(),x1 + 2,y()+h);
    }

    {
        int x1 = frameToPixel(m_loopEnd);
        fl_color(FL_RED);
        fl_line(x1 + 2,y(),x1 + 2,y()+h);
    }

    {
        int x1 = frameToPixel(m_mouseFrame);
        fl_color(FL_YELLOW);
        fl_line(x1 + 2,y(),x1 + 2,y()+h);
    }

    // Fl_Widget::draw();
#endif



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

void GL_WaveformWidget::updateImage()
{
    if (m_bImageDirty || w() != m_img.w() || h() != m_img.h())
    {
        if (w() != m_img.w() || h() != m_img.h())
        {
            m_img.resize(w(),h());
            // DE_INFO("ImageResize(",w(),",",h(),")")
        }

        de::sound::Sound2Image::draw(
            *m_sound,
            0,
            m_zoomBeg,
            m_zoomEnd,
            m_img,
            de::Recti(0,0,w(),h()),
            m_bDarkMode ? dbRGB(255,128,55) : dbRGB(255,255,255),
            m_bDarkMode ? dbRGB(0,0,0) : dbRGB(68,68,168)
            // m_bDarkMode ? dbRGB(255,255,255) : dbRGB(0,0,0),
            // m_bDarkMode ? dbRGB(0,0,0) : dbRGB(255,255,255)
        );

        m_bImageDirty = false;
    }
}

int GL_WaveformWidget::handle(int e)
{
    if (!m_sound)
    {
        #ifdef USE_BENNI_GL
        return Fl_Gl_Window::handle(e);
        #else
        return Fl_Widget::handle(e);
        #endif
    }

    switch (e)
    {
        case FL_ENTER:
        {
            m_bHovered = true;
            redraw();
            return 1; // Enable mouse move tracking
        }
        case FL_LEAVE:
        {
            m_bHovered = false;
            m_mouseFrame = -1;
            redraw();
            return 0;
        }
        case FL_MOVE:
        {
            m_mx = Fl::event_x();
            m_my = Fl::event_y();
            m_mouseFrame = pixelToFrame(m_mx);

            // const double t = double(m_mouseFrame - m_zoomBeg) / double(zoomDelta());

            // DE_OK("mouse(",m_mx,",",m_my,"), "
            //       "t(",t,"), "
            //       "frame(",m_mouseFrame,"), "
            //       "zoom(",m_zoom,"), "
            //       "beg(",m_zoomBeg,"), "
            //       "end(",m_zoomEnd,")"
            //       )

            redraw();
            return 0;
        }
        case FL_PUSH:
        {
            const int mx = Fl::event_x();
            const int my = Fl::event_y();
            if (!dbMouseOver(mx,my,x(),y(),x()+w()-1,y()+h()-1))
            {
                break;
            }
            if (Fl::event_button() == FL_LEFT_MOUSE)
            {
                //m_selecting = true;
                m_loopBeg = pixelToFrame(Fl::event_x());
                //m_loopEnd   = m_loopFrameStart;
                redraw();
            }
            else if (Fl::event_button() == FL_RIGHT_MOUSE)
            {
                //m_selecting = true;
                m_loopEnd = pixelToFrame(Fl::event_x());
                //m_loopEnd   = m_loopFrameStart;
                redraw();
            }

            return 1;
        }
        case FL_DRAG:
        {
            const int mx = Fl::event_x();
            const int my = Fl::event_y();
            if (!dbMouseOver(mx,my,x(),y(),x()+w()-1,y()+h()-1))
            {
                break;
            }
            /*
            if (m_selecting)
            {
                m_loopFrameEnd = pixelToFrame(Fl::event_x());
                if (m_loopFrameEnd < 0) m_loopFrameEnd = 0;
                redraw();
            }
            */
            return 1;
        }
        case FL_RELEASE:
        {
            const int mx = Fl::event_x();
            const int my = Fl::event_y();
            if (!dbMouseOver(mx,my,x(),y(),x()+w()-1,y()+h()-1))
            {
                break;
            }
            /*
            if (m_selecting)
            {
                m_selecting = false;
                if (m_loopFrameEnd < m_loopFrameStart)
                    std::swap(m_loopFrameStart, m_loopFrameEnd);
                redraw();
            }
            */
            return 1;
        }
        case FL_MOUSEWHEEL:
        {
            const int mx = Fl::event_x();
            const int my = Fl::event_y();
            const auto r = de::Recti(x(),y(),w(),h());

            if (!dbMouseOver(mx,my,r))
            {
                break;
            }

            const double t = double(mx - x()) / double(w());

            const int64_t mouseFrame = pixelToFrame(mx);

            // Zoom factor per wheel step
            int64_t newDelta = zoomDelta();
            if (Fl::event_dy() > 0) // Zoom out
            {
                //m_zoom = std::clamp<double>(m_zoom * 1.15, 0.000001, 1000000.0);
                newDelta = std::clamp<int64_t>(double(newDelta) * 1.15, 10, m_sound->frames());
                //m_zoomBeg = std::clamp<int64_t>(double(m_zoomBeg) * 1.15, 0, m_sound->frames());
                //m_zoomEnd = std::clamp<int64_t>(double(m_zoomEnd) * 1.15 , 0, m_sound->frames());
            }
            else if (Fl::event_dy() < 0) // Zoom In
            {
                //m_zoom = std::clamp<double>(m_zoom / 1.15, 0.000001, 1000000.0);
                newDelta = std::clamp<int64_t>(double(newDelta) / 1.15, 10, m_sound->frames());
                //m_zoomBeg = std::clamp<int64_t>(double(m_zoomBeg)/1.15, 0, m_sound->frames());
                //m_zoomEnd = std::clamp<int64_t>(double(m_zoomEnd)/1.15, 0, m_sound->frames());
            }

            int64_t newBeg = mouseFrame - (double(newDelta) * (t));
            int64_t newEnd = mouseFrame + (double(newDelta) * (1.0 - t));

            newBeg = std::clamp(newBeg, 0ll, m_sound->frames() - 10);
            newEnd = std::clamp(newEnd, 10ll, m_sound->frames());

            m_zoomBeg = newBeg;
            m_zoomEnd = newEnd;

            onZoom( m_zoomBeg, m_zoomEnd, m_sound->frames() );

#if 0
            DE_OK("mouse(",mx,",",my,"), "
                  "r(",r.str(),"), "
                  "t(",t,"), "
                  "frame(",mouseFrame,"), "
                  "zoom(",zoom(),"), "
                  "beg(",m_zoomBeg,"), "
                  "end(",m_zoomEnd,")"
                  )
#endif
            //const double oldZoom = m_zoom;

            //const int64_t oldFrameCount = m_sound->m_frames * m_zoom;
#if 0
            const int64_t newFrames = std::llround( m_zoom * double(m_sound->m_frames) );
            const int64_t maxBeg = std::max( 0ll, m_sound->m_frames - newFrameCount );
            const int64_t newEnd = std::clamp( std::llround(t * double(newFrameCount)), 0ll, maxFrameIndex);
#ifdef USE_EXTERNAL_SCROLLBAR
            const double t2 = std::clamp( double(newFrameStart) / double(m_sound->m_frames), 0.0, 1.0);
            m_scroll->value(t2);
#endif
#endif
            m_bImageDirty = true;
            redraw();
            return 1;
        }
    default:
        break;
    }

    #ifdef USE_BENNI_GL
    return Fl_Gl_Window::handle(e);
    #else
    return Fl_Widget::handle(e);
    #endif
}


void GL_WaveformWidget::setZoomFromScrollBar(double pc)
{
    if (!m_sound) return;

    int64_t nMax = m_sound->frames();
    int64_t nZoom = zoomDelta();

    int64_t newBeg = pc * nMax;
    int64_t newEnd = newBeg + nZoom;

    newBeg = std::clamp(newBeg, 0ll, m_sound->frames() - nZoom);
    newEnd = std::clamp(newEnd, nZoom, m_sound->frames());

    m_zoomBeg = newBeg;
    m_zoomEnd = newEnd;

    m_bImageDirty = true;
    redraw();
}


// --- 64-bit safe scroll range ---
#ifdef USE_EXTERNAL_SCROLLBAR
void GL_WaveformWidget::update_scroll_range()
{
    if (!m_sound) return;

    const double d = double(m_zoomFrameStart) / double(m_sound->m_frames);
    m_scroll->bounds(0.0, 1.0);
    m_scroll->value(d);
}
#endif

// --- 64-bit safe pixel → frame ---
int64_t GL_WaveformWidget::pixelToFrame(int px)
{
    if (!m_sound) { DE_ERROR("No sound") return 0; }

    if (m_sound->m_frames < 2) { DE_ERROR("Empty sound") return 0; }

#ifdef USE_BENNI_GL
    const double t = std::clamp(double(px) / double(pixel_w()), 0.0, 1.0);
#else
    const double t = std::clamp(double(px - x()) / double(w()), 0.0, 1.0);
#endif
    int64_t frame = m_zoomBeg + std::llround( t * zoomDelta() );

    return std::clamp(frame, 0ll, m_sound->m_frames);
}


// --- 64-bit safe frame → pixel ---
int32_t GL_WaveformWidget::frameToPixel(int64_t frame)
{
    if (!m_sound) { DE_ERROR("No sound") return 0; }

    if (m_sound->m_frames < 2) { DE_ERROR("Empty sound") return 0; }

    const double t = std::clamp(double(frame - m_zoomBeg) / double(zoomDelta()), 0.0, 1.0);

#ifdef USE_BENNI_GL
    const int32_t px = std::lround( t * pixel_w() );
#else
    const int32_t px = std::lround( t * w() );
#endif
    return x() + px;
}
