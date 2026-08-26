#pragma once
#include <gui/XP_ProgressBar.h>
#include <gui/LogBox.h>
#include <gui/FM/DropList.h>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>

// =============================================================
class UI_FileInputField : public Fl_Input
{
public:
    Fl_Button* btnInput = nullptr;
    Fl_Button* btnZip = nullptr;
    Fl_Input* edtOutput = nullptr;

    UI_FileInputField(int X, int Y, int W, int H, const char* L = 0)
        : Fl_Input(X, Y, W, H, L)
    {
        value("Drag & Drop a file or directory here...");
    }

    void setZipButton(Fl_Button* btn) { btnZip = btn; }

    void setInputButton(Fl_Button* btn) { btnInput = btn; }

    void setOutputLineEdit(Fl_Input* edt) { edtOutput = edt; }

    // void setOutputLineEdit(Fl_Input* edt) { edtOutput = edt; }

    int handle(int event) override
    {
        // FLTK schreibt beim Drop den Text selbst ins Input,
        // wir reagieren nur auf FL_PASTE.
        if (event == FL_PASTE)
        {
            // Clear
            value("");

            // Handle paste
            int r = Fl_Input::handle(event); // Text wird gesetzt

            //
            std::string uri = de::FileSystem::makePosixPath(value());
            value(uri.c_str());

            if (dbExistFile(uri))
            {
                auto ext = dbFileSuffix(uri);
                if (ext == "zst" || ext == "zstd")
                {
                    DE_OK("Got .zstd file to decompress", uri)

                    if (btnInput) btnInput->label("Archive");
                    if (btnZip) btnZip->activate();
                    if (btnZip) btnZip->label("Decompress");

                    auto dst = dbParentDir(uri) + "/" + dbFileBase(uri);
                    if (edtOutput) edtOutput->value(dst.c_str());
                }
                else
                {
                    DE_OK("Got file to compress", uri)

                    if (btnInput) btnInput->label("File");
                    if (btnZip) btnZip->activate();
                    if (btnZip) btnZip->label("Compress");

                    auto dst = uri + ".zst";
                    if (edtOutput) edtOutput->value(dst.c_str());
                }
            }
            else if (dbExistDirectory(uri))
            {
                DE_OK("Got directory ", uri)

                if (btnInput) btnInput->label("Directory");
                if (btnZip) btnZip->activate();
                if (btnZip) btnZip->label("Compress");

                auto dst = uri + ".zst";
                if (edtOutput) edtOutput->value(dst.c_str());
            }
            else
            {
                DE_ERROR("Input not a file, archive or directory. ", uri)
            }
            return r;
        }
        return Fl_Input::handle(event);
    }
};
// =============================================================
class UI_FileInput : public Fl_Group
{
public:
    int m_spacing = 5;
    Fl_Button* btnLoad = nullptr;
    Fl_Box* lblLoad = nullptr;
    UI_FileInputField* edtUri = nullptr;
    // Fl_Button* btnChoose = nullptr;

    UI_FileInput(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - 2*s;
        int x = X;
        int y = Y;
        btnLoad  = new Fl_Button(x, y, w1, H, "Unselected"); x += w1 + s;
        lblLoad  = new Fl_Box(x, y, w2, H, "Input:"); x += w2 + s;
        edtUri = new UI_FileInputField(x, y, w3, H, ""); // x += w3 + s;
        // btnChoose  = new Fl_Button(x, y, w4, H, "...");
        btnLoad->tooltip("Loads file to preview and cut it");
        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - 2*s;
        int x = X;
        int y = Y;

        btnLoad->resize(x, y, w1, H);    x += w1 + s;
        lblLoad->resize(x, y, w2, H);    x += w2 + s;
        edtUri->resize(x, y, w3, H);  // x += w3 + s;
        //btnChoose->resize(x, y, w4, H);
    }
};
// =============================================================
class UI_FileOutput : public Fl_Group
{
public:
    int m_spacing = 5;

    Fl_Button* btnZip = nullptr;
    Fl_Box* lblOutput = nullptr;
    Fl_Input* edtUri = nullptr;
    // Fl_Button* btnChoose = nullptr;

    UI_FileOutput(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - s*2;
        int x = X;
        int y = Y;

        //ui.img1 = new ImageWidget(x,y,40,H);
        btnZip = new Fl_Button(x,y,w1,H, "Start Zip"); x += w1 + s;
        lblOutput = new Fl_Box(x,y,w2,H,"Output:"); x += w2 + s;
        edtUri = new Fl_Input(x,y,w3,H,""); x += w3 + s;
        // btnChoose = new Fl_Button(x,y,w4,H,"...");

        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - s*2;
        int x = X;
        int y = Y;

        btnZip->resize(x,y,w1,H); x += w1 + s;
        lblOutput->resize(x,y,w2,H); x += w2 + s;
        edtUri->resize(x,y,w3,H); // x += w3 + s;
        // btnChoose->resize(x,y,w4,H);
    }
};

// =============================================================
class UI_DropList : public Fl_Group
{
public:
    int m_spacing = 5;
    Fl_Button* btnLoad = nullptr;
    Fl_Box* lblLoad = nullptr;
    DropList* dropList = nullptr;
    // Fl_Button* btnChoose = nullptr;

    UI_DropList(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - 2*s;
        int x = X;
        int y = Y;
        btnLoad  = new Fl_Button(x, y, w1, H, "Unselected"); x += w1 + s;
        lblLoad  = new Fl_Box(x, y, w2, H, "Input:"); x += w2 + s;
        dropList = new DropList(x, y, w3, H, ""); // x += w3 + s;
        // btnChoose  = new Fl_Button(x, y, w4, H, "...");
        btnLoad->tooltip("Loads file to preview and cut it");
        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = 100 * zoom;
        int w2 = 60 * zoom;
        int w3 = W - w1-w2 - 2*s;
        int x = X;
        int y = Y;

        btnLoad->resize(x, y, w1, H);    x += w1 + s;
        lblLoad->resize(x, y, w2, H);    x += w2 + s;
        dropList->resize(x, y, w3, H);  // x += w3 + s;
        //btnChoose->resize(x, y, w4, H);
    }
};


// =============================================================
class UI_Progress : public Fl_Group
{
public:
    int m_spacing = 5;
    Fl_Button* btnCompare;
    Fl_Progress* progress;
    Fl_Button* btnCancel;
    Fl_Button* btnDarkMode;

    UI_Progress(int X, int Y, int W, int H, int spacing)
        : Fl_Group(X, Y, W, H)
        , m_spacing(spacing)
    {
        begin();

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = (150 - m_spacing) * zoom;
        int w3 = (100 - m_spacing) * zoom;
        int w4 = (120 - m_spacing) * zoom;
        int w2 = W - w1 - w3 - w4 - 3*s;
        int x = X;
        int y = Y;
        btnCompare = new Fl_Button(x,y,w1,H,"Compare in/out"); x += w1 + s;
        progress = new XP_ProgressBar(x,y,w2,H); x += w2 + s;
        btnCancel = new Fl_Button(x,y,w3,H,"Cancel"); x += w3 + s;
        btnDarkMode = new Fl_Button(x,y,w4,H,"DarkMode");

        progress->minimum(0);
        progress->maximum(1);
        progress->value(0);

        end();
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Group::resize(X, Y, W, H);

        float zoom = Fl::screen_scale(0);
        int s = m_spacing * zoom;
        int w1 = (150 - m_spacing) * zoom;
        int w3 = (100 - m_spacing) * zoom;
        int w4 = (120 - m_spacing) * zoom;
        int w2 = W - w1 - w3 - w4 - 3*s;

        int x = X;
        int y = Y;
        btnCompare->resize(x,y,w1,H); x += w1 + s;
        progress->resize(x,y,w2,H); x += w2 + s;
        btnCancel->resize(x,y,w3,H); x += w3 + s;
        btnDarkMode->resize(x,y,w4,H);
    }
};


/*
void hscroll_cb(Fl_Widget* w, void* data)
{
    auto scrollBar = (Fl_Scrollbar*)w;

    // ui.waveform->setZoomStart( scrollBar->value() );

    ui.waveform->waveform->setZoomFromScrollBar( scrollBar->value() );
}
*/

