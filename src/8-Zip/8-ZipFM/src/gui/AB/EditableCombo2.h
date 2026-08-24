#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Window.H>
#include <FL/fl_draw.H>

class EditableCombo : public Fl_Group {
public:
    // --- Subwidgets ---
    class EditBox : public Fl_Widget {
    public:
        std::string text;
        bool hover = false;
        bool focused = false;

        EditBox(int X, int Y, int W, int H)
            : Fl_Widget(X, Y, W, H) {}

        void draw() override {
            // Hintergrund
            if (hover)
                fl_color(fl_rgb_color(200, 220, 255)); // light blue
            else
                fl_color(FL_WHITE);
            fl_rectf(x(), y(), w(), h());

            // Border
            if (hover)
                fl_color(fl_rgb_color(0, 120, 215)); // Windows blue
            else
                fl_color(FL_DARK3);
            fl_rect(x(), y(), w(), h());

            // Text
            fl_color(FL_BLACK);
            fl_font(FL_HELVETICA, 14);
            fl_draw(text.c_str(), x() + 4, y() + h() - 6);
        }

        int handle(int e) override {
            switch (e) {
            case FL_ENTER:
                hover = true;
                redraw();
                return 1;

            case FL_LEAVE:
                hover = false;
                redraw();
                return 1;

            case FL_FOCUS:
                focused = true;
                redraw();
                return 1;

            case FL_UNFOCUS:
                focused = false;
                redraw();
                return 1;

            case FL_KEYDOWN:
                if (Fl::event_key() == FL_BackSpace) {
                    if (!text.empty()) text.pop_back();
                } else {
                    const char* t = Fl::event_text();
                    if (t && t[0] >= 32)
                        text.push_back(t[0]);
                }
                redraw();
                return 1;
            }
            return 0;
        }
    };

    class ArrowButton : public Fl_Widget {
    public:
        bool hover = false;

        ArrowButton(int X, int Y, int W, int H)
            : Fl_Widget(X, Y, W, H) {}

        void draw() override {
            // Hintergrund
            if (hover)
                fl_color(fl_rgb_color(230, 240, 255));
            else
                fl_color(fl_rgb_color(245, 245, 245));
            fl_rectf(x(), y(), w(), h());

            // Border
            fl_color(FL_DARK3);
            fl_rect(x(), y(), w(), h());

            // Pfeil
            fl_color(FL_BLACK);
            int cx = x() + w()/2;
            int cy = y() + h()/2;
            fl_polygon(cx-4, cy-2, cx+4, cy-2, cx, cy+4);
        }

        int handle(int e) override {
            switch (e) {
            case FL_ENTER:
                hover = true;
                redraw();
                return 1;
            case FL_LEAVE:
                hover = false;
                redraw();
                return 1;
            case FL_PUSH:
                do_callback();
                return 1;
            }
            return 0;
        }
    };

    class Popup : public Fl_Menu_Window {
    public:
        std::vector<std::string> items;
        int hoverIndex = -1;
        int selectedIndex = -1;

        Popup(int W, int H)
            : Fl_Menu_Window(W, H) {
            clear_border();
        }

        void draw() override {
            fl_push_clip(0, 0, w(), h());
            fl_color(FL_WHITE);
            fl_rectf(0, 0, w(), h());

            fl_font(FL_HELVETICA, 14);
            int lh = fl_height() + 6;

            for (int i = 0; i < items.size(); ++i) {
                int y = i * lh;

                // Hover highlight
                if (i == hoverIndex) {
                    fl_color(fl_rgb_color(200, 220, 255));
                    fl_rectf(0, y, w(), lh);
                }

                // Selected highlight
                if (i == selectedIndex) {
                    fl_color(fl_rgb_color(0, 120, 215));
                    fl_rectf(0, y, w(), lh);
                    fl_color(FL_WHITE);
                } else {
                    fl_color(FL_BLACK);
                }

                fl_draw(items[i].c_str(), 4, y + lh - 4);
            }

            fl_pop_clip();
        }

        int handle(int e) override {
            int lh = fl_height() + 6;

            switch (e) {
            case FL_MOVE:
            case FL_DRAG:
                hoverIndex = Fl::event_y() / lh;
                redraw();
                return 1;

            case FL_PUSH:
                selectedIndex = Fl::event_y() / lh;
                redraw();
                return 1;

            case FL_RELEASE:
                hide();
                return 1;
            }
            return Fl_Menu_Window::handle(e);
        }
    };

    // --- EditableCombo members ---
    EditBox* edit;
    ArrowButton* arrow;
    Popup* popup;

    void (*on_change)(EditableCombo*, const char*) = nullptr;

    EditableCombo(int X, int Y, int W, int H)
        : Fl_Group(X, Y, W, H)
    {
        box(FL_NO_BOX);

        edit = new EditBox(X, Y, W - H, H);
        arrow = new ArrowButton(X + W - H, Y, H, H);

        arrow->callback([](Fl_Widget* w, void* v){
            ((EditableCombo*)v)->show_popup();
        }, this);

        popup = new Popup(W, 200);

        end();
    }

    void set_items(const std::vector<std::string>& items) {
        popup->items = items;
    }

    void show_popup() {
        popup->resize(w(), popup->items.size() * (fl_height() + 6));
        popup->position(x(), y() + h());
        popup->show();
        Fl::grab(popup);
    }

    const char* value() const {
        return edit->text.c_str();
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Group::resize(X, Y, W, H);
        edit->resize(X, Y, W - H, H);
        arrow->resize(X + W - H, Y, H, H);
    }
};

/*

EditableCombo* combo = new EditableCombo(20, 20, 200, 28);
combo->set_items({"Apfel", "Banane", "Kirsche"});

combo->on_change = [](EditableCombo* c, const char* val){
    printf("Neuer Wert: %s\n", val);
};

*/