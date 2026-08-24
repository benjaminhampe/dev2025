/*

Kurzfassung: In FLTK bekommst du Textbreite und Texthöhe über die globalen Font‑Funktionen:

    Breite eines Strings:

width=fl_width(text)

    Texthöhe des aktuellen Fonts:

height=fl_height()

    Font setzen:  
    fl_font(face, size);

Das ist die komplette Kontrolle, die du für dein Custom‑Rendering brauchst.
🟦 Textbreite eines Strings bestimmen
cpp

fl_font(FL_HELVETICA, 14);
double w = fl_width("Hallo Welt");

Das gibt dir die Pixelbreite des Strings im aktuell gesetzten Font.
Wichtige Details

    fl_width() berücksichtigt Kerning, Glyph‑Breite, Font‑Face, Font‑Size.

    Du kannst auch einzelne Zeichen messen:

cpp

double w = fl_width('A');

    Für sehr präzise Layouts kannst du Zeichenweise summieren:

cpp

double total = 0;
for (char c : text)
    total += fl_width(c);

🟧 Texthöhe bestimmen
cpp

fl_font(FL_HELVETICA, 14);
int h = fl_height();

Das ist die Font‑Lineheight, also:

    ascent + descent

    ohne zusätzlichen Padding

    perfekt für Popup‑Zeilenhöhe

🟩 Text‑Baseline bestimmen

FLTK zeichnet Text am Baseline‑Offset, nicht am oberen Rand.

Die Baseline bekommst du über:
cpp

int ascent = fl_ascent();
int descent = fl_descent();

Beispiel:
cpp

int baseline = y + ascent;
fl_draw(text, x, baseline);

🟦 Textfarbe setzen
cpp

fl_color(FL_BLACK);
fl_draw("Hallo", x, y);

Oder RGB:
cpp

fl_color(fl_rgb_color(0, 120, 215)); // Windows blue

🟧 Popup‑Lineheight kontrollieren
cpp

fl_font(FL_HELVETICA, 14);
int lh = fl_height() + 6; // 6px Padding

Damit steuerst du:

    Hover‑Bereich

    Click‑Bereich

    Hit‑Test

    Popup‑Fensterhöhe

🟩 Hover‑Highlight einer Popup‑Zeile
cpp

int idx = Fl::event_y() / lh;

fl_color(fl_rgb_color(200, 220, 255)); // light blue
fl_rectf(0, idx * lh, w(), lh);

🟪 Click‑Highlight einer Popup‑Zeile
cpp

selectedIndex = Fl::event_y() / lh;

fl_color(fl_rgb_color(0, 120, 215)); // Windows blue
fl_rectf(0, selectedIndex * lh, w(), lh);

fl_color(FL_WHITE);
fl_draw(items[selectedIndex].c_str(), 4, selectedIndex * lh + lh - 4);

🟫 Bestimmen, welche Zeile geklickt wurde
cpp

int idx = Fl::event_y() / lh;

Das ist exakt wie Win32 Hit‑Testing.
🟦 Zusammenfassung der relevanten Funktionen
Zweck	FLTK Funktion
Font setzen	fl_font(face, size)
Textbreite	fl_width(text)
Texthöhe	fl_height()
Ascent	fl_ascent()
Descent	fl_descent()
Text zeichnen	fl_draw(text, x, y)
Farbe setzen	fl_color(r,g,b)
Rect zeichnen	fl_rect(x,y,w,h)
Rect füllen	fl_rectf(x,y,w,h)
🟩 Wenn du willst, baue ich dir jetzt:

*/