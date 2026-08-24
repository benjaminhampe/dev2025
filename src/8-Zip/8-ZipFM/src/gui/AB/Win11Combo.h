#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Window.H>
#include <FL/fl_draw.H>
#include <string>
#include <vector>
#include <functional>

#include <de/Core.h> // DE_ERROR()

// ===============================
// Win11 Colors
// ===============================
static Fl_Color WIN11_BORDER_NORMAL    = fl_rgb_color(200, 200, 200);
static Fl_Color WIN11_BORDER_HOVER     = fl_rgb_color(0, 120, 215);
static Fl_Color WIN11_BORDER_FOCUS     = fl_rgb_color(0, 120, 215);

static Fl_Color WIN11_FILL_NORMAL      = fl_rgb_color(255, 255, 255);
static Fl_Color WIN11_FILL_HOVER       = fl_rgb_color(245, 248, 255);

static Fl_Color WIN11_TEXT_NORMAL      = fl_rgb_color(0, 0, 0);
static Fl_Color WIN11_TEXT_SELECTED    = fl_rgb_color(255, 255, 255);

static Fl_Color WIN11_POPUP_FILL       = fl_rgb_color(255, 255, 255);
static Fl_Color WIN11_POPUP_HOVER      = fl_rgb_color(240, 244, 255);
static Fl_Color WIN11_POPUP_SELECTED   = fl_rgb_color(0, 120, 215);


// ===============================
// Arrow Button
// ===============================
class Win11Combo_ArrowButton : public Fl_Widget
{
public:
    bool m_hover = false;

    Win11Combo_ArrowButton(int X, int Y, int W, int H)
        : Fl_Widget(X, Y, W, H)
        {}

    void draw() override {
        Fl_Color fill = m_hover ? WIN11_FILL_HOVER : WIN11_FILL_NORMAL;

        fl_color(fill);
        fl_rectf(x(), y(), w(), h());

        fl_color(WIN11_BORDER_NORMAL);
        fl_rect(x(), y(), w(), h());

        fl_color(WIN11_TEXT_NORMAL);
        int cx = x() + w()/2;
        int cy = y() + h()/2;
        fl_polygon(cx-4, cy-2, cx+4, cy-2, cx, cy+4);
    }

    int handle(int e) override {
        switch (e) {
        case FL_ENTER: m_hover = true;  redraw(); return 1;
        case FL_LEAVE: m_hover = false; redraw(); return 1;
        case FL_PUSH:  do_callback(); return 1;
        }
        return 0;
    }
};

class Win11Combo_EditBox : public Fl_Input {
public:
    bool hover = false;
    bool pressed = false;

    Win11Combo_EditBox(int X, int Y, int W, int H, const char* L = 0)
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

// ===============================
// EditBox with Cursor
// ===============================
class Win11Combo_EditBox2 : public Fl_Widget
{
public:
    std::string m_text;
    int m_cursor = 0;
    bool m_hover = false;
    bool m_focus = false;

    Win11Combo_EditBox2(int X, int Y, int W, int H)
        : Fl_Widget(X, Y, W, H) {}

    void draw_caret(int cx) {
        fl_color(WIN11_BORDER_FOCUS);
        fl_rectf(cx, y() + 4, 1, h() - 8);
    }

    void draw() override {
        Fl_Color fill   = m_hover ? WIN11_FILL_HOVER : WIN11_FILL_NORMAL;
        Fl_Color border = m_focus ? WIN11_BORDER_FOCUS :
                          m_hover ? WIN11_BORDER_HOVER :
                                   WIN11_BORDER_NORMAL;

        fl_color(fill);
        fl_rectf(x(), y(), w(), h());

        fl_color(border);
        fl_rect(x(), y(), w(), h());

        fl_color(WIN11_TEXT_NORMAL);
        fl_font(FL_HELVETICA, 14);

        int px = x() + 8;
        int baseline = y() + h() - 8;
        fl_draw(m_text.c_str(), px, baseline);

        if (m_focus) {
            int cx = px;
            for (int i = 0; i < m_text.size(); ++i)
                cx += fl_width(m_text[i]);
            draw_caret(cx);
        }
    }

    int handle(int e) override {
        switch (e) {

        case FL_ENTER: m_hover = true; redraw(); return 1;
        case FL_LEAVE: m_hover = false; redraw(); return 1;

        case FL_FOCUS:   m_focus = true;  redraw(); return 1;
        case FL_UNFOCUS: m_focus = false; redraw(); return 1;

        case FL_PUSH: {
            take_focus();
            m_focus = true;

            int mx = Fl::event_x();
            int px = x() + 8;

            m_cursor = m_text.size();

            for (int i = 0; i < m_text.size(); ++i) {
                double cw = fl_width(m_text[i]);
                if (mx < px + cw/2) {
                    m_cursor = i;
                    break;
                }
                px += cw;
            }

            redraw();
            return 1;
        }

        case FL_KEYDOWN: {
            int key = Fl::event_key();
            const char* t = Fl::event_text();

            if (key == FL_BackSpace) {
                if (m_cursor > 0) {
                    m_text.erase(m_cursor - 1, 1);
                    m_cursor--;
                }
            }
            else if (key == FL_Left) {
                if (m_cursor > 0) m_cursor--;
            }
            else if (key == FL_Right) {
                if (m_cursor < m_text.size()) m_cursor++;
            }
            else if (t && t[0] >= 32) {
                m_text.insert(m_cursor, 1, t[0]);
                m_cursor++;
            }

            redraw();
            return 1;
        }
        }

        return 0;
    }
};


// ===============================
// Popup Window
// ===============================
class Win11Combo_Popup : public Fl_Menu_Window
{
public:
    std::vector<std::string> m_items;
    int m_hoverIndex = -1;
    int m_selectedIndex = -1;

    // typedef void (*FN_onSelect)(int, void*);

    typedef std::function<void(int, void*)> FN_onSelect;

    FN_onSelect onSelect = nullptr;

    Win11Combo_Popup(int W, int H)
        : Fl_Menu_Window(W, H)
    {
        clear_border();
    }

    void draw() override
    {
        fl_push_clip(0, 0, w(), h());
        fl_color(WIN11_POPUP_FILL);
        fl_rectf(0, 0, w(), h());

        fl_font(FL_HELVETICA, 14);
        int lh = fl_height() + 8;

        for (int i = 0; i < m_items.size(); ++i) {
            int y = i * lh;

            Fl_Color fill = WIN11_POPUP_FILL;
            Fl_Color text = WIN11_TEXT_NORMAL;

            if (i == m_hoverIndex)    fill = WIN11_POPUP_HOVER;
            if (i == m_selectedIndex) { fill = WIN11_POPUP_SELECTED; text = WIN11_TEXT_SELECTED; }

            fl_color(fill);
            fl_rectf(0, y, w(), lh);

            fl_color(text);
            fl_draw(m_items[i].c_str(), 8, y + lh - 6);
        }

        fl_pop_clip();
    }

    int handle(int e) override
    {
        fl_font(FL_HELVETICA, 14);
        int lh = fl_height() + 8;

        switch (e) {
        case FL_MOVE:
        case FL_DRAG: {
            int idx = Fl::event_y() / lh;
            m_hoverIndex = (idx >= 0 && idx < m_items.size()) ? idx : -1;
            redraw();
            return 1;
        }

        case FL_PUSH: {
            int idx = Fl::event_y() / lh;
            if (idx >= 0 && idx < m_items.size()) {
                m_selectedIndex = idx;
                redraw();
            }
            return 1;
        }

        case FL_RELEASE: {
            int idx = Fl::event_y() / lh;
            if (idx >= 0 && idx < m_items.size())
            {
                m_selectedIndex = idx;
                if (onSelect)
                {
                    onSelect(idx, this);
                }
            }
            Fl::grab(0);
            hide();
            return 1;
        }
        }

        return Fl_Menu_Window::handle(e);
    }
};

// ===============================
// Win11 Combo Class
// ===============================
class Win11Combo : public Fl_Group
{
public:
    int m_spacing;
    // ===============================
    // Win11Combo Members
    // ===============================
    Win11Combo_EditBox* m_edit;
    Win11Combo_ArrowButton* m_arrow;
    Win11Combo_Popup* m_popup;

    // typedef void (*FN_onChange)(int /* index */, std::string /* text */ );

    typedef std::function<void(int /* index */, std::string /* text */ )> FN_onChange;

    FN_onChange onChange = nullptr;

    // ===============================
    // Constructor
    // ===============================
    Win11Combo(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing{ spacing }
    {
        box(FL_NO_BOX);

        int arrowW = H;

        m_edit  = new Win11Combo_EditBox(X, Y, W - arrowW, H);
        m_arrow = new Win11Combo_ArrowButton(X + W - arrowW, Y, arrowW, H);
        m_popup = new Win11Combo_Popup(W, 200);

        m_arrow->callback(
            [](Fl_Widget* w, void* widget)
            {
                auto combo = (Win11Combo*)widget;
                combo->show_popup();
            },
            this);

        m_popup->onSelect =
            [this] (int idx, void* userData)
            {
                auto popup = (Win11Combo_Popup*)userData;
                if (!popup)
                {
                    DE_ERROR("No popup")
                    return;
                }
                if (idx < 0 || idx >= int(popup->m_items.size()))
                {
                    DE_ERROR("Invalid idx = ",idx)
                }
                else
                {
#if 0
                    m_edit->m_text = popup->m_items[idx];
                    m_edit->m_cursor = m_edit->m_text.size();
#else
                    m_edit->value( popup->m_items[idx].c_str() );
#endif
                    m_edit->redraw();
                    if (onChange)
                    {
#if 0
                        onChange(idx, m_edit->m_text);
#else
                        onChange(idx, m_edit->value());
#endif
                    }
                    else
                    {
                        DE_ERROR("No onChange")
                    }
                }
            };

        end();
    }

    // ===============================
    // API
    // ===============================
    void set_items(const std::vector<std::string>& items)
    {
        m_popup->m_items = items;
    }

    void show_popup()
    {
        fl_font(FL_HELVETICA, 14);
        int lh = fl_height() + 8;
        int ph = m_popup->m_items.size() * lh;

        m_popup->resize(0,0,w(), ph);
        m_popup->position(x(), y() + h());
        m_popup->show();
        Fl::grab(m_popup);
    }

    // const std::string&
    // value() const
    // {
    //     return m_edit->m_text;
    // }

    void
    resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);
        int arrowW = H;
        m_edit->resize(X, Y, W - arrowW, H);
        m_arrow->resize(X + W - arrowW, Y, arrowW, H);
    }
};


/*

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

// Win11Combo hier einfügen

int main(int argc, char** argv) {
    Fl_Window win(400, 200, "Win11 Combo Test");

    Win11Combo* combo = new Win11Combo(50, 50, 220, 32);
    combo->set_items({"Apfel", "Banane", "Kirsche", "Mango"});

    combo->on_change = [](Win11Combo* c, const char* val){
        printf("Selected: %s\n", val);
    };

    win.end();
    win.show(argc, argv);
    return Fl::run();
}

*/
