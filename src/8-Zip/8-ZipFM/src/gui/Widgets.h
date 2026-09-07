#pragma once
#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>
/*
Fl_Choice* combo = new Fl_Choice(20, 40, 120, 28);
    combo->add(".tar");
    combo->add(".zst");

    combo->callback([](Fl_Widget* w){
        Fl_Choice* c = (Fl_Choice*)w;
        printf("selected: %s\n", c->mvalue()->label());
    });
*/
// =============================================================
class Window : public Fl_Window
// =============================================================
{
public:
    Window(int W, int H, const char* title)
        : Fl_Window(W, H, title)
    {
        color(fl_rgb_color(240, 240, 240));
        begin();
    }
};

// ======================================================
// 🟧
// ======================================================
class DoubleWindow : public Fl_Double_Window
{
public:
    DoubleWindow(int W, int H, const char* title)
        : Fl_Double_Window(W, H, title)
    {
        color(fl_rgb_color(240, 240, 240));
        begin();
    }
};

// =============================================================
class Label : public Fl_Box
// =============================================================
{
public:
    Label(int X, int Y, int W, int H, const char* Title)
        : Fl_Box(X, Y, W, H, Title)
    {
        align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE); //  | FL_ALIGN_TOP
        // labelfont(FL_FREE_FONT);
        labelsize(10*Fl::screen_scale(0));
    }

    void draw() override
    {
        //int cw = 20 * Fl::screen_scale(0) + fl_width(label());

        fl_push_clip(x(), y(), w(), h());

        Fl_Box::draw();

        // --- Hintergrund ---

        Fl_Color border_blue = fl_rgb_color(0,120,215);     // Windows blue
        fl_color(border_blue);
        fl_rect(x(), y(), w(), h());
        fl_pop_clip();
    }
};

// =============================================================
class Button : public Fl_Button
// =============================================================
{
public:
    Button(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Button(X, Y, W, H, L)
    {
        box(FL_NO_BOX);   // Wir zeichnen selbst
    }

    void draw() override
    {
        const bool hover = Fl::belowmouse() == this;
        const bool down  = value();

        Fl_Color textColor = fl_rgb_color(0,0,0);
        Fl_Color fillColor;
        Fl_Color borderColor;
        if (down)
        {
            fillColor = fl_rgb_color(204, 228, 247);
            borderColor = fl_rgb_color( 0,  84, 153);
        }
        else if (hover)
        {
            fillColor = fl_rgb_color(224, 238, 249);
            borderColor = fl_rgb_color( 0, 120, 212);
        }
        else
        {
            fillColor = fl_rgb_color(253, 253, 253);
            borderColor = fl_rgb_color(208, 208, 208);
        }

        fl_color(fillColor);
        fl_rounded_rectf(x(), y(), w(), h(), 1);

        fl_color(borderColor);
        fl_rounded_rect(x(), y(), w(), h(), 1);

        fl_color(textColor);
        fl_font(FL_HELVETICA, 14 * Fl::screen_scale(0));
        fl_draw(label(), x(), y(), w(), h(), FL_ALIGN_CENTER);
    }

    int handle(int e) override
    {
        switch (e)
        {
        case FL_ENTER:
            redraw();
            break;
        case FL_LEAVE:
            redraw();
            break;
        default:
            break;
        }

        return Fl_Button::handle(e);
    }
};


// 🟩 Code: Perfekte FLTK‑GroupBox (Win32‑Style)

class GroupBox : public Fl_Group
{
public:
    Fl_Box* title;

    GroupBox(int X, int Y, int W, int H, const char* label)
        : Fl_Group(X, Y, W, H)
    {
        box(FL_NO_BOX); // wir zeichnen den Rahmen selbst

        // Titel oben im Rahmen
        title = new Fl_Box(X + 10, Y, W - 20, 20, label);
        title->box(FL_NO_BOX);
        title->labelfont(FL_BOLD);

        // Kinderbereich leicht eingerückt
        this->begin();
        // Benutzer fügt hier seine Widgets ein
        this->end();
    }

    void draw() override {
        // Rahmen zeichnen
        fl_color(FL_DARK3);
        fl_rect(x(), y() + 10, w(), h() - 10);

        // Standard FLTK draw
        Fl_Group::draw();
    }
};

/*
// 🟧 Wie du es benutzt

    GroupBox* gb = new GroupBox(20, 20, 300, 120, "Audio Settings");
    gb->begin();

    new Fl_Check_Button(40, 50, 120, 25, "Enable DSP");
    new Fl_Input(40, 80, 200, 25, "Buffer:");

    gb->end();
*/

// =============================================================
class LineEdit : public Fl_Input
// =============================================================
{
public:
    bool hover = false;
    bool pressed = false;

    LineEdit(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Input(X, Y, W, H, L)
    {
        // box(FL_NO_BOX);     // wir zeichnen alles selbst
    }

    int handle(int e) override {
        switch (e) {
        case FL_ENTER:
            hover = true;
            redraw();
            break;
            // return 1;

        case FL_LEAVE:
            hover = false;
            redraw();
            break;
            // return 1;

        case FL_PUSH:
            pressed = true;
            redraw();
            break;
            // return 1;

        case FL_RELEASE:
            pressed = false;
            redraw();
            break;
            // return 1;
        default:
            break;
        }
        return Fl_Input::handle(e);
    }

    void draw() override {

        // if (input_type() == FL_HIDDEN_INPUT) return;

        // --- Farben ---
        Fl_Color bg_normal   = fl_rgb_color(245,245,245);
        Fl_Color bg_hover    = fl_rgb_color(220,235,255);   // light blue
        Fl_Color border_blue = fl_rgb_color(0,120,215);     // Windows blue
        Fl_Color text_color  = fl_rgb_color(20,20,20);

        // --- Hintergrund ---
        fl_push_clip(x(), y(), w(), h());

        Fl_Boxtype b = box();
        if (damage() & FL_DAMAGE_ALL)
        {
            // --- Hintergrund ---
            fl_color(hover ? bg_hover : bg_normal);
            fl_rectf(x(), y(), w(), h());
            // draw_box(b, color());
        }

        Fl_Input_::drawtext(x()+Fl::box_dx(b), y()+Fl::box_dy(b),
                      w()-Fl::box_dw(b), h()-Fl::box_dh(b));

        // --- Border ---
        fl_color(border_blue);
        fl_rect(x(), y(), w(), h());

        fl_pop_clip();

    /*
        // --- Text ---
        fl_color(text_color);
        fl_font(FL_HELVETICA, 14);   // Textgröße
        int lh = fl_height();        // Lineheight

        // Textposition
        int tx = x() + 4;
        int ty = y() + lh + 2;
        const char* t = value();

        // Zeichnen
        fl_draw(t, tx, ty);
    */

    }
};
