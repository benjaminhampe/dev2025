#pragma once
#include <FL/Fl_Progress.H>
#include <FL/fl_draw.H>

// =============================================================
class XP_ProgressBar : public Fl_Progress
{
public:
    XP_ProgressBar(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Progress(X, Y, W, H, L)
    {
        box(FL_NO_BOX);  // wir zeichnen alles selbst
        color(fl_rgb_color(200, 200, 200));        // XP trough
        selection_color(fl_rgb_color(0, 180, 0));  // XP green
        labelcolor(FL_WHITE);
        minimum(0);
        maximum(1);
        value(0);
    }

    void draw_round_rect(int X, int Y, int W, int H, int r, Fl_Color c)
    {
        fl_color(c);
        fl_begin_polygon();

        // top-left arc
        fl_arc(X, Y, 2*r, 2*r, 90, 180);

        // left side
        fl_vertex(X, Y + r);
        fl_vertex(X, Y + H - r);

        // bottom-left arc
        fl_arc(X, Y + H - 2*r, 2*r, 2*r, 180, 270);

        // bottom side
        fl_vertex(X + r, Y + H);
        fl_vertex(X + W - r, Y + H);

        // bottom-right arc
        fl_arc(X + W - 2*r, Y + H - 2*r, 2*r, 2*r, 270, 360);

        // right side
        fl_vertex(X + W, Y + H - r);
        fl_vertex(X + W, Y + r);

        // top-right arc
        fl_arc(X + W - 2*r, Y, 2*r, 2*r, 0, 90);

        // top side
        fl_vertex(X + W - r, Y);
        fl_vertex(X + r, Y);

        fl_end_polygon();
    }

    void draw() override
    {
        fl_push_clip(x(), y(), w(), h());

        int r = 3; // XP typical corner radius

        // Trough (rounded)
        draw_round_rect(x(), y(), w(), h(), r, color());

        // Border
        fl_color(fl_rgb_color(160,160,160));
        fl_begin_loop();
        fl_arc(x(), y(), 2*r, 2*r, 90, 180);
        fl_arc(x(), y() + h() - 2*r, 2*r, 2*r, 180, 270);
        fl_arc(x() + w() - 2*r, y() + h() - 2*r, 2*r, 2*r, 270, 360);
        fl_arc(x() + w() - 2*r, y(), 2*r, 2*r, 0, 90);
        fl_end_loop();

        // Progress fraction
        float frac = (maximum() > minimum())
            ? (value() - minimum()) / (maximum() - minimum())
            : 0.0f;

        int pw = int(frac * w());

        if (pw > 0)
        {
            // Base XP green
            draw_round_rect(x(), y(), pw, h(), r, selection_color());

            // Glossy highlight (top half)
            draw_round_rect(x(), y(), pw, h()/2, r, fl_rgb_color(0, 220, 0));

            // XP stripes
            fl_color(fl_rgb_color(0, 140, 0));
            for (int sx = x(); sx < x() + pw; sx += 12)
                fl_rectf(sx, y(), 6, h());
        }

        // Label (percentage)
        char buf[64];
        snprintf(buf, sizeof(buf), "%d%%", int(frac * 100));

        fl_color(labelcolor());
        fl_font(FL_HELVETICA_BOLD, 12);
        fl_draw(buf, x(), y(), w(), h(), FL_ALIGN_CENTER);

        fl_pop_clip();
    }
};
