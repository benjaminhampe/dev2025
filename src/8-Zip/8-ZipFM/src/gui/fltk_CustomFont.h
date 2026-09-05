#pragma once
#include <cstdint>
#include <de/Core.h>

int setFontLiberationSansRegular();


/*

class Fl_CustomFont
{
public:
    Fl_CustomFont();
    Fl_CustomFont(const uint8_t* data, uint32_t size);
    ~Fl_CustomFont();
    bool open(const uint8_t* data, uint32_t size);
    void close();
    int id() const;

private:
    struct Impl;
    Impl* _d = nullptr;
};

🧩 Benutzung

    #include "FltkFreeTypeFont.hpp"
    #include "noto_emoji_binary.hpp"   // dein eingebetteter TTF-Header

    Fl_Window win(300, 200, "Emoji Test");

    // RAII: Font lebt solange das Objekt lebt
    Fl_CustomFont emojiFont(noto_emoji_ttf, noto_emoji_ttf_len);

    Fl_Box box(20, 20, 260, 160, "🔥 Feuer!");
    box.labelfont(emojiFont.id());
    box.labelsize(32);

    win.show();
    return Fl::run();
*/
