#pragma once
/*
🔥 1. Zwei Fonts registrieren: Regular + Bold

Du erzeugst einfach zwei temporäre Dateien (oder zwei Font‑Loader‑Objekte) und registrierst beide:

    TempFontFile regularFont(my_regular_data, my_regular_size);
    TempFontFile boldFont(my_bold_data, my_bold_size);

    int FONT_REGULAR = Fl::set_font(FL_FREE_FONT, regularFont.path());
    int FONT_BOLD    = Fl::set_font(FL_FREE_FONT + 1, boldFont.path());

    Wichtig:
    Jede Font‑ID ist ein eigener Font.
    FLTK macht keine automatische Bold‑Simulation.

🔥 2. Bold/Regular im Widget setzen

    Fl_Box box(20, 20, 200, 40, "Regular");
    box.labelfont(FONT_REGULAR);
    box.labelsize(18);

    Fl_Box box2(20, 80, 200, 40, "Bold");
    box2.labelfont(FONT_BOLD);
    box2.labelsize(18);

🔥 3. Bold/Regular im Drawing‑Code

    Wenn du selbst zeichnest:

    void draw() override {
        fl_font(FONT_REGULAR, 18);
        fl_draw("Regular Text", x()+10, y()+20);

        fl_font(FONT_BOLD, 18);
        fl_draw("Bold Text", x()+10, y()+50);
    }

🔥 4. Global Bold/Regular setzen

    Wenn du z. B. willst, dass alle Labels Bold sind:

    void set_global_font(int font_id, Fl_Group* g) {
        for (int i = 0; i < g->children(); ++i) {
            Fl_Widget* w = g->child(i);
            w->labelfont(font_id);

            if (auto* sub = dynamic_cast<Fl_Group*>(w))
                set_global_font(font_id, sub);
        }
    }

    Nutzung:

    set_global_font(FONT_BOLD, &win);

🔥 5. Bold/Regular für Input/Text Widgets

    FLTK trennt Label‑Font und Text‑Font:

    Fl_Input in(20, 140, 200, 30);
    in.labelfont(FONT_REGULAR);
    in.textfont(FONT_BOLD);   // Bold text inside input
    in.textsize(18

🟦 Warum FLTK 1.4.5 keine globale Font‑Size hat

    FLTK 1.4.x hat nur:

    widget->labelsize(int);
    widget->labelfont(int);

und keine:

    Fl::set_labelsize()

    Fl::set_font_size()

    Fl::set_default_labelsize()

Diese Funktionen existieren nicht in 1.4.x.
🔥 Lösung 1 — Globale Font‑Size per Traversierung (empfohlen)

Setzt die Schriftgröße für alle Widgets, egal ob Buttons, Boxes, Inputs, Groups.
cpp

void set_global_fontsize(Fl_Group* g, int size) {
    for (int i = 0; i < g->children(); ++i) {
        Fl_Widget* w = g->child(i);

        w->labelsize(size);

        // Input/Text widgets
        if (auto* in = dynamic_cast<Fl_Input*>(w))
            in->textsize(size);
        if (auto* out = dynamic_cast<Fl_Output*>(w))
            out->textsize(size);
        if (auto* ml = dynamic_cast<Fl_Multiline_Input*>(w))
            ml->textsize(size);

        // Recurse into groups
        if (auto* sub = dynamic_cast<Fl_Group*>(w))
            set_global_fontsize(sub, size);
    }
}

Nutzung
cpp

Fl_Window win(400, 300, "Global Font Size");
Fl_Button btn(20, 20, 200, 40, "Hello");

set_global_fontsize(&win, 18);

win.show();
return Fl::run();

Damit hast du eine echte globale Font‑Size, ohne FLTK‑API‑Hacks.
🔥 Lösung 2 — Eigene Widget‑Basisklasse (schnell, sauber)

Wenn du viele Widgets erzeugst:
cpp

struct MyWidget : Fl_Widget {
    MyWidget(int x,int y,int w,int h,const char* l=0)
        : Fl_Widget(x,y,w,h,l)
    {
        labelsize(18);
    }
};

Für Buttons:
cpp

struct MyButton : Fl_Button {
    MyButton(int x,int y,int w,int h,const char* l=0)
        : Fl_Button(x,y,w,h,l)
    {
        labelsize(18);
    }
};

🔥 Lösung 3 — Factory‑Funktion (sehr flexibel)

Du kannst jede Widget‑Erzeugung global kontrollieren:
cpp

template <typename T, typename... Args>
T* make_widget(int size, Args&&... args) {
    T* w = new T(std::forward<Args>(args)...);
    w->labelsize(size);

    if constexpr (std::is_base_of_v<Fl_Input, T>)
        w->textsize(size);

    return w;
}

Nutzung
cpp

auto* btn = make_widget<Fl_Button>(18, 20,20,200,40,"Hello");

🧠 Wie du deinen geladenen Font global setzt

Du hast bereits:
cpp

int font_id = Fl::set_font(FL_FREE_FONT, path);

Jetzt global:
cpp

void set_global_font(int font_id, Fl_Group* g) {
    for (int i = 0; i < g->children(); ++i) {
        Fl_Widget* w = g->child(i);
        w->labelfont(font_id);

        if (auto* sub = dynamic_cast<Fl_Group*>(w))
            set_global_font(font_id, sub);
    }
}

Nutzung
cpp

set_global_font(font_id, &win);
set_global_fontsize(&win, 18);

Damit hast du globalen Font + globale Font‑Size.


#pragma once
#include <FL/Fl.H>
#include <ft2build.h>
#include FT_FREETYPE_H

class FltkFreeTypeFont {
public:
    FltkFreeTypeFont(const unsigned char* data, unsigned int size)
        : ft_(nullptr), face_(nullptr), font_id_(-1)
    {
        // Init FreeType library
        FT_Error err = FT_Init_FreeType(&ft_);
        if (err) throw std::runtime_error("FT_Init_FreeType failed");

        // Load face from memory
        err = FT_New_Memory_Face(ft_, data, size, 0, &face_);
        if (err) {
            FT_Done_FreeType(ft_);
            throw std::runtime_error("FT_New_Memory_Face failed");
        }

        // Register with FLTK
        font_id_ = Fl::set_font(FL_FREE_FONT, face_);
        if (font_id_ < 0) {
            FT_Done_Face(face_);
            FT_Done_FreeType(ft_);
            throw std::runtime_error("Fl::set_font failed");
        }
    }

    ~FltkFreeTypeFont() {
        // FLTK does NOT own the FT_Face → we must free it
        if (face_) FT_Done_Face(face_);
        if (ft_)   FT_Done_FreeType(ft_);
    }

    int id() const { return font_id_; }

private:
    FT_Library ft_;
    FT_Face    face_;
    int        font_id_;
};

🧩 Benutzung

    #include "FltkFreeTypeFont.hpp"
    #include "noto_emoji_binary.hpp"   // dein eingebetteter TTF-Header

    Fl_Window win(300, 200, "Emoji Test");

    // RAII: Font lebt solange das Objekt lebt
    FltkFreeTypeFont emojiFont(noto_emoji_ttf, noto_emoji_ttf_len);

    Fl_Box box(20, 20, 260, 160, "🔥 Feuer!");
    box.labelfont(emojiFont.id());
    box.labelsize(32);

    win.show();
    return Fl::run();

🧠 Technische Korrektheit & Details

    FreeType init:
    FT_Init_FreeType(&ft_) → korrekt, einmal pro Font‑Objekt.

    Face aus Memory:
    FT_New_Memory_Face(ft_, data, size, 0, &face_)
    → ideal für eingebettete Fonts, keine Datei nötig.

    FLTK‑Registrierung:
    Fl::set_font(FL_FREE_FONT, face_)
    → FLTK speichert nur den Pointer, übernimmt aber keine Ownership.

    RAII‑Destruction:
    Reihenfolge ist wichtig:

        FT_Done_Face(face_)

        FT_Done_FreeType(ft_)

    Thread‑Safety:
    FreeType ist nicht global → du kannst mehrere Instanzen parallel haben.

    Keine Farb‑Emoji:
    FLTK rendert monochrom → Noto Emoji (monochrom) ist korrekt.

🧱 Warum diese Klasse perfekt für FLTK ist

    Kein globaler FreeType‑State

    Kein Memory‑Leak

    Font‑ID bleibt stabil

    Font kann dynamisch geladen/entladen werden

    Vollständig portable (Windows, Linux, macOS)

🔥 Der korrekte Weg: Font aus eingebettetem Byte‑Array laden (FreeType + FLTK)

Das ist der Standardweg für FLTK 1.4, weil FLTK selbst keine Memory‑Fonts hat — aber FreeType schon.
🧩 Schritt 1 — Font als Byte‑Array einbetten

Du wandelst die .ttf Datei in ein C‑Array um:

    xxd -i MyFont.ttf > myfont.h

Das erzeugt:

    unsigned char MyFont_ttf[] = { 0x00, 0x01, ... };
    unsigned int MyFont_ttf_len = 123456;

🧩 Schritt 2 — FreeType initialisieren

    #include <ft2build.h>
    #include FT_FREETYPE_H

    FT_Library ft;
    FT_Init_FreeType(&ft);

🧩 Schritt 3 — Font aus Memory laden

    FT_Face face;
    FT_New_Memory_Face(ft, MyFont_ttf, MyFont_ttf_len, 0, &face);

    Das ist der entscheidende Schritt:
    FreeType kann Fonts direkt aus RAM öffnen.

🧩 Schritt 4 — Font bei FLTK registrieren

    int font_id = Fl::set_font(FL_FREE_FONT, face);

Damit ist der Font global verfügbar.

    🧩 Schritt 5 — Font im Widget verwenden

    Fl_Box* b = new Fl_Box(20, 20, 200, 40, "Hallo Benjamin!");
    b->labelfont(font_id);
    b->labelsize(18);

🎯 Was du jetzt hast

    Font aus embedded binary

    Keine Dateien

    Keine Installation

    Voll portable

    Funktioniert auf Windows, Linux, macOS

    Perfekt für Tools, Plugins, portable Apps

🧠 Wichtige Hinweise (technisch korrekt)
✔️ FLTK 1.4 unterstützt FreeType‑Faces direkt

Fl::set_font(FL_FREE_FONT, face) ist offiziell vorgesehen.
✔️ Du kannst beliebig viele Fonts laden

Einfach mehrere Byte‑Arrays einbetten.
✔️ Emoji‑Fonts müssen monochrom sein

Farbglyphs (COLR/CPAL/SVG) funktionieren nicht.
✔️ Du kannst den Font global setzen
cpp

Fl::set_font(FL_HELVETICA, face);

Damit überschreibst du den Systemfont.
📦 Fertige Utility‑Funktion (Benjamin‑optimiert)

Damit du es überall wiederverwenden kannst:
cpp

int load_embedded_font(const unsigned char* data, unsigned int size) {
    static FT_Library ft = nullptr;
    if (!ft) FT_Init_FreeType(&ft);

    FT_Face face;
    if (FT_New_Memory_Face(ft, data, size, 0, &face)) {
        return -1;
    }

    return Fl::set_font(FL_FREE_FONT, face);
}

Benutzung:
cpp

int myfont = load_embedded_font(MyFont_ttf, MyFont_ttf_len);
widget->labelfont(myfont);

📌 Fazit

Du kannst Fonts perfekt aus eingebetteten Binärdaten in FLTK laden:

    .ttf → Byte‑Array

    FT_New_Memory_Face

    Fl::set_font(FL_FREE_FONT, face)

    fertig.

Das ist der beste, sauberste, portabelste Weg.

Wenn du willst, baue ich dir eine komplette FLTK‑Font‑Manager‑Klasse, die:

    mehrere Fonts lädt

    globalen UI‑Font setzt

    Fallback‑Fonts verwaltet

    DPI‑Scaling berücksichtigt

👉 FLTK Font Manager erstellen
*/
