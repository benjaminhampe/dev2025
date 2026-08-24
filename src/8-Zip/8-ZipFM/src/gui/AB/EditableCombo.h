#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/fl_draw.H>

class EditableCombo : public Fl_Group {
public:
    Fl_Input* input;
    Fl_Button* arrow;
    Fl_Menu_Button* menu;

    // Callback, wenn der User etwas auswählt oder eintippt
    void (*on_change)(EditableCombo*, const char*) = nullptr;

    EditableCombo(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Group(X, Y, W, H, L)
    {
        box(FL_NO_BOX);

        input = new Fl_Input(X, Y, W - H, H);
        input->callback([](Fl_Widget* w, void* v){
            EditableCombo* self = (EditableCombo*)v;
            if (self->on_change)
                self->on_change(self, self->input->value());
        }, this);

        arrow = new Fl_Button(X + W - H, Y, H, H, "@2");
        arrow->box(FL_UP_BOX);
        arrow->callback([](Fl_Widget* w, void* v){
            EditableCombo* self = (EditableCombo*)v;
            const Fl_Menu_Item* m = self->menu->popup();
            if (m && m->label()) {
                self->input->value(m->label());
                if (self->on_change)
                    self->on_change(self, m->label());
            }
        }, this);

        menu = new Fl_Menu_Button(X, Y + H, W, 0);
        menu->type(Fl_Menu_Button::POPUP3);

        end();
    }

    // Menüeinträge hinzufügen
    void add_item(const char* label) {
        menu->add(label);
    }

    // Komplettes Menü ersetzen
    void set_items(const std::vector<std::string>& items) {
        menu->clear();
        for (auto& s : items)
            menu->add(s.c_str());
    }

    // Wert setzen
    void value(const char* v) {
        input->value(v);
    }

    // Wert holen
    const char* value() const {
        return input->value();
    }

    // Resize korrekt weiterreichen
    void resize(int X, int Y, int W, int H) override {
        Fl_Group::resize(X, Y, W, H);
        input->resize(X, Y, W - H, H);
        arrow->resize(X + W - H, Y, H, H);
        menu->resize(X, Y + H, W, 0);
    }
};

/*
EditableCombo* combo = new EditableCombo(20, 20, 200, 28);
combo->set_items({"Apfel", "Banane", "Kirsche"});
combo->value("Apfel");

combo->on_change = [](EditableCombo* c, const char* val){
    printf("Neuer Wert: %s\n", val);
};

*/
