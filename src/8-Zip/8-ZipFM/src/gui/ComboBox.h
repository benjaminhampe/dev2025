#include <FL/Fl.H>
#include <FL/fl_draw.H>

class ComboDisplay : public Fl_Widget {
public:
    std::wstring value;

    ComboDisplay(int X, int Y, int W, int H)
        : Fl_Widget(X, Y, W, H) {}

    void draw() override {
        fl_push_clip(x(), y(), w(), h());
        fl_color(FL_WHITE);
        fl_rectf(x(), y(), w(), h());

        fl_color(FL_BLACK);
        fl_draw(value.c_str(), x() + 4, y() + h() - 6);

        // draw arrow
        fl_draw("@2", x() + w() - 16, y() + 2);

        fl_pop_clip();
    }

    int handle(int e) override {
        if (e == FL_PUSH) {
            openPopupMenu();
            return 1;
        }
        return 0;
    }

    void openPopupMenu() {
        // your Fl_Menu_Window popup
    }
};
