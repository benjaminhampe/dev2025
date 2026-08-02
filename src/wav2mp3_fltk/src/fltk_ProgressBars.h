/*
class XP_Progress : public Fl_Progress {
public:
    XP_Progress(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Progress(X, Y, W, H, L)
    {
        box(FL_FLAT_BOX);
        color(fl_rgb_color(200, 200, 200));   // trough background
        selection_color(fl_rgb_color(0, 120, 215)); // XP blue
        labelcolor(FL_WHITE);
    }

    void draw() override
    {
        // Draw trough background
        fl_push_clip(x(), y(), w(), h());
        fl_color(color());
        fl_rectf(x(), y(), w(), h());

        // Border
        fl_color(fl_rgb_color(160,160,160));
        fl_rect(x(), y(), w(), h());

        // Progress fraction
        float frac = 0.0f;
        if (maximum() > minimum())
            frac = (value() - minimum()) / (maximum() - minimum());

        int pw = int(frac * w());

        if (pw > 0)
        {
            // Base XP blue
            fl_color(selection_color());
            fl_rectf(x(), y(), pw, h());

            // Glossy highlight (top half)
            fl_color(fl_rgb_color(0, 180, 255));
            fl_rectf(x(), y(), pw, h() / 2);

            // XP stripes
            fl_color(fl_rgb_color(0, 100, 200));
            for (int sx = x(); sx < x() + pw; sx += 12)
                fl_rectf(sx, y(), 6, h());
        }

        // Draw label (percentage)
        char buf[64];
        snprintf(buf, sizeof(buf), "%d%%", int(frac * 100));

        fl_color(labelcolor());
        fl_font(FL_HELVETICA_BOLD, 12);
        fl_draw(buf, x(), y(), w(), h(), FL_ALIGN_CENTER);

        fl_pop_clip();
    }
};

class XP_Progress : public Fl_Progress {
public:
    XP_Progress(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Progress(X, Y, W, H, L)
    {
        box(FL_FLAT_BOX);
        color(fl_rgb_color(200, 200, 200));   // trough background
        selection_color(fl_rgb_color(0, 120, 215)); // XP blue-ish
        labelcolor(FL_WHITE);
    }

    void draw() override
    {
        // Draw trough
        fl_color(color());
        fl_rectf(x(), y(), w(), h());

        // Border
        fl_color(fl_rgb_color(160,160,160));
        fl_rect(x(), y(), w(), h());

        // Progress width
        float frac = (maximum() > minimum())
            ? (value() - minimum()) / (maximum() - minimum())
            : 0.0f;

        int pw = int(frac * w());

        if (pw > 0)
        {
            // XP blue base
            fl_color(selection_color());
            fl_rectf(x(), y(), pw, h());

            // Glossy highlight (top half)
            fl_color(fl_rgb_color(0, 180, 255));
            fl_rectf(x(), y(), pw, h() / 2);

            // XP stripes
            fl_color(fl_rgb_color(0, 100, 200));
            for (int sx = x(); sx < x() + pw; sx += 12)
                fl_rectf(sx, y(), 6, h());
        }

        // Draw label (percentage)
        char buf[64];
        snprintf(buf, sizeof(buf), "%d%%", int(frac * 100));

        fl_color(labelcolor());
        fl_font(FL_HELVETICA_BOLD, 12);
        fl_draw(buf, x(), y(), w(), h(), FL_ALIGN_CENTER);
    }
};

class XP_Green_Progress : public Fl_Progress {
public:
    XP_Green_Progress(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Progress(X, Y, W, H, L)
    {
        box(FL_FLAT_BOX);
        color(fl_rgb_color(200, 200, 200));        // trough background
        selection_color(fl_rgb_color(0, 180, 0));  // XP green base
        labelcolor(FL_WHITE);
    }

    void draw() override
    {
        fl_push_clip(x(), y(), w(), h());

        // Draw trough
        fl_color(color());
        fl_rectf(x(), y(), w(), h());

        // Border
        fl_color(fl_rgb_color(160,160,160));
        fl_rect(x(), y(), w(), h());

        // Progress fraction
        float frac = 0.0f;
        if (maximum() > minimum())
            frac = (value() - minimum()) / (maximum() - minimum());

        int pw = int(frac * w());

        if (pw > 0)
        {
            // Base XP green
            fl_color(selection_color());
            fl_rectf(x(), y(), pw, h());

            // Glossy highlight (top half)
            fl_color(fl_rgb_color(0, 220, 0));
            fl_rectf(x(), y(), pw, h() / 2);

            // XP stripes (dark green)
            fl_color(fl_rgb_color(0, 140, 0));
            for (int sx = x(); sx < x() + pw; sx += 12)
                fl_rectf(sx, y(), 6, h());
        }

        // Draw label (percentage)
        char buf[64];
        snprintf(buf, sizeof(buf), "%d%%", int(frac * 100));

        fl_color(labelcolor());
        fl_font(FL_HELVETICA_BOLD, 12);
        fl_draw(buf, x(), y(), w(), h(), FL_ALIGN_CENTER);

        fl_pop_clip();
    }
};
*/