#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Window.H>
#include <FL/fl_draw.H>

class EditableComboPopup : public Fl_Group {
public:
    Fl_Input* input;
    Fl_Button* arrow;
    Fl_Menu_Window* popup;

    std::vector<std::string> items;
    void (*on_change)(EditableComboPopup*, const char*) = nullptr;

    EditableComboPopup(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Group(X, Y, W, H, L)
    {
        box(FL_NO_BOX);

        input = new Fl_Input(X, Y, W - H, H);
        input->callback([](Fl_Widget* w, void* v){
            auto* self = (EditableComboPopup*)v;
            if (self->on_change)
                self->on_change(self, self->input->value());
        }, this);

        arrow = new Fl_Button(X + W - H, Y, H, H, "@2");
        arrow->callback([](Fl_Widget* w, void* v){
            ((EditableComboPopup*)v)->show_popup();
        }, this);

        popup = new Fl_Menu_Window(200, 200);
        popup->begin();
        popup->end();
        popup->clear_border(); // optional: ohne Fensterrahmen

        end();
    }

    void set_items(const std::vector<std::string>& list) {
        items = list;
    }

    void show_popup() {
        int X = arrow->x();
        int Y = arrow->y() + arrow->h();

        popup->resize(X, Y, w(), items.size() * 22);
        popup->show();

        Fl::grab(popup); // Modalität erzwingen

        Fl::add_timeout(0.01, [](void* v){
            ((EditableComboPopup*)v)->draw_popup();
        }, this);
    }

    void draw_popup() {
        popup->make_current();
        fl_push_clip(0, 0, popup->w(), popup->h());
        fl_color(FL_WHITE);
        fl_rectf(0, 0, popup->w(), popup->h());

        fl_color(FL_BLACK);
        int y = 0;
        for (size_t i = 0; i < items.size(); ++i) {
            fl_draw(items[i].c_str(), 4, y + 16);
            y += 22;
        }

        fl_pop_clip();
        popup->redraw();

        popup->callback([](Fl_Widget* w, void* v){
            auto* self = (EditableComboPopup*)v;
            int mx = Fl::event_x();
            int my = Fl::event_y();

            int idx = my / 22;
            if (idx >= 0 && idx < (int)self->items.size()) {
                self->input->value(self->items[idx].c_str());
                if (self->on_change)
                    self->on_change(self, self->items[idx].c_str());
            }

            Fl::grab(0);
            self->popup->hide();
        }, this);
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Group::resize(X, Y, W, H);
        input->resize(X, Y, W - H, H);
        arrow->resize(X + W - H, Y, H, H);
    }
};


/*

EditableComboPopup* combo = new EditableComboPopup(20, 20, 200, 28);
combo->set_items({"Apfel", "Banane", "Kirsche"});

combo->on_change = [](EditableComboPopup* c, const char* val){
    printf("Neuer Wert: %s\n", val);
};

*/