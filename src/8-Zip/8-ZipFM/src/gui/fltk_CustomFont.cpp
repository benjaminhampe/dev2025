#include "fltk_CustomFont.h"

// #include <BinaryData/Fonts.h>
#include <FL/Fl.H>


#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    // #define _WIN32_WINNT  0x0A00   // Windows 8 API freischalten
    // #define WINVER        0x0A00
    #define _WIN32_WINNT  0x0602   // Windows 8 API freischalten
    #define WINVER        0x0602
    #include <windows.h>            // only for Window ICOn
    //#include <shellapi.h>
    // #include <winbase.h>
    // #include <winnt.h>
    //#include "../res/resource.h"    // only for Window ICOn
    // #include <shellscalingapi.h>    // Für SetProcessDpiAwarenessContext()
    // #include <winuser.h>
    // #include <dwmapi.h>
#endif

/*
class TempFontFile {
public:
    TempFontFile(const unsigned char* data, size_t size) {
        char tmpPath[MAX_PATH];
        GetTempPathA(MAX_PATH, tmpPath);

        char tmpFile[MAX_PATH];
        GetTempFileNameA(tmpPath, "ftf", 0, tmpFile);

        path_ = tmpFile;

        std::ofstream out(path_, std::ios::binary);
        out.write((const char*)data, size);
    }

    const char* path() const { return path_.c_str(); }

    ~TempFontFile() {
        DeleteFileA(path_.c_str());
    }

private:
    std::string path_;
};
*/

class TempFontFile {
public:
    TempFontFile(const unsigned char* data, size_t size) {
        char tempPath[MAX_PATH];
        char tempFile[MAX_PATH];

        // Get temp directory
        DWORD len = GetTempPathA(MAX_PATH, tempPath);
        if (len == 0 || len > MAX_PATH)
            throw std::runtime_error("GetTempPathA failed");

        // Create temp file
        if (!GetTempFileNameA(tempPath, "ftf", 0, tempFile))
            throw std::runtime_error("GetTempFileNameA failed");

        path_ = tempFile;

        // Create file handle
        HANDLE h = CreateFileA(
            path_.c_str(),
            GENERIC_WRITE,
            0,
            nullptr,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_TEMPORARY,
            nullptr
        );

        if (h == INVALID_HANDLE_VALUE)
            throw std::runtime_error("CreateFileA failed");

        // Write binary font data
        DWORD written = 0;
        if (!WriteFile(h, data, (DWORD)size, &written, nullptr) || written != size) {
            CloseHandle(h);
            throw std::runtime_error("WriteFile failed");
        }

        CloseHandle(h);
    }

    const char* path() const { return path_.c_str(); }

    ~TempFontFile() {
        DeleteFileA(path_.c_str());
    }

private:
    std::string path_;
};

int setFontLiberationSansRegular()
{
#if 1
    DE_ERROR("Not implemented")
    return -1;
#else
    TempFontFile fontFile(BinaryData::LiberationSansRegular_ttf,
                          BinaryData::LiberationSansRegular_ttfSize);
    Fl::set_font(FL_FREE_FONT, fontFile.path());
    Fl::set_font(FL_HELVETICA, FL_FREE_FONT);
    return FL_FREE_FONT;
#endif
}

/*
#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Fl_CustomFont::Impl
{
    FT_Library m_ft;
    FT_Face    m_face;
    int        m_font_id;

    Impl()
        : m_ft(nullptr)
        , m_face(nullptr)
        , m_font_id(-1)
    {}

    bool open(const uint8_t* data, uint32_t size)
    {
        // Init FreeType library
        FT_Error err = FT_Init_FreeType(&m_ft);
        if (err) throw std::runtime_error("FT_Init_FreeType failed");

        // Load face from memory
        err = FT_New_Memory_Face(m_ft, data, size, 0, &m_face);
        if (err)
        {
            FT_Done_FreeType(m_ft);
            throw std::runtime_error("FT_New_Memory_Face failed");
        }

        // Register with FLTK
        m_font_id = Fl::set_font(FL_FREE_FONT, m_face);
        if (font_id_ < 0) {
            FT_Done_Face(face_);
            FT_Done_FreeType(ft_);
            throw std::runtime_error("Fl::set_font failed");
        }
    }

    void close()
    {
        // FLTK does NOT own the FT_Face → we must free it
        if (face_) FT_Done_Face(face_);
        if (ft_)   FT_Done_FreeType(ft_);
    }

    int Fl_CustomFont::id() const { return font_id_; }
};

Fl_CustomFont::Fl_CustomFont()
    : _d( new Impl() )
{

}

bool Fl_CustomFont::open(const uint8_t* data, uint32_t size)
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

void Fl_CustomFont::close()
{
    // FLTK does NOT own the FT_Face → we must free it
    if (face_) FT_Done_Face(face_);
    if (ft_)   FT_Done_FreeType(ft_);
}

Fl_CustomFont::Fl_CustomFont(const uint8_t* data, uint32_t size)
    : Fl_CustomFont()
{
    open(data,size);
}

Fl_CustomFont::~Fl_CustomFont()
{

}

int Fl_CustomFont::id() const { return font_id_; }

private:
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
