#include "WaveformWidget_fltk.h"

WaveformWidget::WaveformWidget(int X, int Y, int W, int H)
    : Fl_Widget(X, Y, W, H)
    , m_sound(nullptr)
{
    m_scroll = new Fl_Scrollbar(X, Y + H - 16, W, 16);
    m_scroll->type(FL_HORIZONTAL);
    m_scroll->callback(scroll_cb, this);
    update_scroll_range();
}

void WaveformWidget::setSound(de::Sound* snd)
{
    m_sound = snd;
    m_accessor.setSound( snd );
    redraw();
}

void WaveformWidget::setZoom(double z)
{
    if (z < 0.001) z = 0.001;
    if (z > 1000.0) z = 1000.0;
    m_zoom = z;
    update_scroll_range();
    redraw();
}

double WaveformWidget::getZoom() const { return m_zoom; }

int64_t WaveformWidget::startFrame() const { return m_startFrame; }
int64_t WaveformWidget::endFrame()   const { return m_endFrame; }

void WaveformWidget::draw()
{
    fl_push_clip(x(), y(), w(), h()-16);

    // Background
    fl_color(FL_BLACK);
    fl_rectf(x(), y(), w(), h()-16);

    if (!m_sound)
    {
        fl_pop_clip();
        return;
    }

    const int rulerHeight = 20;
    const double framesPerPixel = 1.0 / m_zoom;
    const int64_t totalFrames = m_sound->m_frames;

    const int64_t startFrame = (int64_t)m_scroll->value();
    const int64_t visibleFrames = (int64_t)(w() * framesPerPixel);
    const int64_t endFrame = startFrame + visibleFrames;

    // --- Time ruler + grid ---
    draw_time_ruler_and_grid(startFrame, framesPerPixel);

    // --- Waveform ---
    const int midY = y() + rulerHeight + (h()-16-rulerHeight)/2;
    const int amp = (h()-16-rulerHeight)/2 - 4;

    fl_color(FL_WHITE);


    for (int px = 0; px < w(); ++px)
    {
        int64_t frame = startFrame + (int64_t)(px * framesPerPixel);
        if (frame < 0 || frame >= totalFrames) continue;

        float s = m_accessor.getSamplef(frame); // [-1,1]
        int sy = midY - int(s * amp);
        fl_point(x() + px, sy);
    }

    // --- Cut region ---
    if (m_selecting || (m_startFrame >= 0 && m_endFrame > m_startFrame))
    {
        fl_color(FL_RED);
        int sx = x() + (int)((m_startFrame - startFrame) / framesPerPixel);
        int ex = x() + (int)((m_endFrame   - startFrame) / framesPerPixel);
        fl_rect(sx, y()+rulerHeight, ex - sx, h()-16-rulerHeight);
    }

    fl_pop_clip();
}

int WaveformWidget::handle(int e)
{
    if (!m_sound)
        return Fl_Widget::handle(e);

    switch (e)
    {
        case FL_PUSH:
            if (Fl::event_button() == FL_LEFT_MOUSE)
            {
                m_selecting = true;
                m_startFrame = pixelToFrame(Fl::event_x());
                m_endFrame   = m_startFrame;
                redraw();
            }
            return 1;

        case FL_DRAG:
            if (m_selecting)
            {
                m_endFrame = pixelToFrame(Fl::event_x());
                if (m_endFrame < 0) m_endFrame = 0;
                redraw();
            }
            return 1;

        case FL_RELEASE:
            if (m_selecting)
            {
                m_selecting = false;
                if (m_endFrame < m_startFrame)
                    std::swap(m_startFrame, m_endFrame);
                redraw();
            }
            return 1;

        // case FL_MOUSEWHEEL:
        //     setZoom(m_zoom + Fl::event_dy() * -0.1);
        //     return 1;
        // }
        case FL_MOUSEWHEEL:
        {
            const int mx = Fl::event_x();
            const double oldZoom = m_zoom;

            // Zoom factor per wheel step
            const double factor = (Fl::event_dy() < 0) ? 1.15 : 1.0 / 1.15;

            // Frame under cursor BEFORE zoom
            const int64_t startFrame = (int64_t)m_scroll->value();
            const double framesPerPixel_old = 1.0 / oldZoom;
            const int64_t frameUnderCursor =
                startFrame + (int64_t)((mx - x()) * framesPerPixel_old);

            // Apply zoom
            setZoom(oldZoom * factor);

            // Frame under cursor AFTER zoom
            const double framesPerPixel_new = 1.0 / m_zoom;

            // Compute new startFrame so the cursor stays anchored
            int64_t newStartFrame =
                frameUnderCursor - (int64_t)((mx - x()) * framesPerPixel_new);

            // Clamp
            const int64_t totalFrames = m_sound->m_frames;
            const int64_t visibleFrames = (int64_t)(w() * framesPerPixel_new);
            const int64_t maxStart = (totalFrames > visibleFrames)
                                   ? (totalFrames - visibleFrames)
                                   : 0;

            if (newStartFrame < 0) newStartFrame = 0;
            if (newStartFrame > maxStart) newStartFrame = maxStart;

            m_scroll->value((double)newStartFrame);
            redraw();
            return 1;
        }
    default:
        break;
    }
    return Fl_Widget::handle(e);
}

// static
void WaveformWidget::scroll_cb(Fl_Widget*, void* userdata)
{
    auto* self = static_cast<WaveformWidget*>(userdata);
    self->redraw();
}

// --- 64-bit safe scroll range ---
void WaveformWidget::update_scroll_range()
{
    if (!m_sound) return;

    const int64_t totalFrames = m_sound->m_frames;
    const double framesPerPixel = 1.0 / m_zoom;
    const int64_t visibleFrames = (int64_t)(w() * framesPerPixel);

    int64_t maxStart = 0;
    if (totalFrames > visibleFrames)
        maxStart = totalFrames - visibleFrames;

    m_scroll->bounds(0.0, (double)maxStart);

    double v = m_scroll->value();
    if (v < 0.0) v = 0.0;
    if (v > (double)maxStart) v = (double)maxStart;
    m_scroll->value(v);
}

// --- 64-bit safe pixel→frame ---
int64_t WaveformWidget::pixelToFrame(int px) const
{
    const double framesPerPixel = 1.0 / m_zoom;
    const int64_t startFrame = (int64_t)m_scroll->value();
    const int64_t delta = (int64_t)((px - x()) * framesPerPixel);
    return startFrame + delta;
}

// --- Time ruler + grid ---
void WaveformWidget::draw_time_ruler_and_grid(int64_t startFrame, double framesPerPixel)
{
    const int rulerHeight = 20;
    const int rulerY = y();
    const int waveformY = y() + rulerHeight;
    const int waveformH = h() - 16 - rulerHeight;

    const double sr = m_sound->m_sampleRate;

    const double majorSec = 1.0;   // 1 second
    const double minorSec = 0.1;   // 100 ms

    const int64_t majorFrames = (int64_t)(majorSec * sr);
    const int64_t minorFrames = (int64_t)(minorSec * sr);

    const int64_t totalFrames = m_sound->m_frames;
    const int64_t endFrame = startFrame + (int64_t)(w() * framesPerPixel);

    // Ruler background
    fl_color(FL_DARK3);
    fl_rectf(x(), rulerY, w(), rulerHeight);

    // Major ticks
    int64_t firstMajor = ((startFrame + majorFrames - 1) / majorFrames) * majorFrames;

    for (int64_t f = firstMajor; f < endFrame; f += majorFrames) {
        int px = x() + (int)((f - startFrame) / framesPerPixel);

        fl_color(FL_GRAY);
        fl_line(px, waveformY, px, waveformY + waveformH);

        double sec = double(f) / sr;
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", sec);

        fl_color(FL_WHITE);
        fl_draw(buf, px + 2, rulerY + rulerHeight - 5);
    }

    // Minor ticks
    int64_t firstMinor = ((startFrame + minorFrames - 1) / minorFrames) * minorFrames;

    fl_color(FL_DARK2);
    for (int64_t f = firstMinor; f < endFrame; f += minorFrames) {
        int px = x() + (int)((f - startFrame) / framesPerPixel);
        fl_line(px, waveformY, px, waveformY + waveformH);
    }
}
