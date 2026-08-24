#include <gui/Base.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>
#include <gui/AB/Win11Combo.h>

/*
Fl_Choice* combo = new Fl_Choice(20, 40, 120, 28);
    combo->add(".tar");
    combo->add(".zst");

    combo->callback([](Fl_Widget* w){
        Fl_Choice* c = (Fl_Choice*)w;
        printf("selected: %s\n", c->mvalue()->label());
    });
*/

class ArchiveBuilder : public Fl_Double_Window
{
    Win11Combo* m_edtArchiveName = nullptr;
    Fl_Choice* m_cbxExtension = nullptr;


    std::atomic<bool> bCancelFlag{false};
    // std::atomic<bool> reloadFile{true};

    std::thread worker;

    int bitrate; // bitrate in kbit, e.g. 128, not 128000
    int quality; // quality 0..9

    // std::string getSrcUri() const { return inFile->edtUri->value(); }
    // std::string getDstUri() const { return outFile->edtUri->value(); }

public:
    ArchiveBuilder(int W, int H, const char* title)
        : Fl_Double_Window(W, H, title)
    {
        begin();

        const float zoom = Fl::screen_scale(0);
        const int d = 5 * zoom;
        const int h1 = 30 * zoom;
        const int hR = H - 3*h1 - 5*d; // hRemain
        const int h2 = 2 * hR / 3;
        const int h3 = hR - h2;
        int x = d;
        int y = d;

        m_edtArchiveName = new Win11Combo(x,y,W-2*d,h1,d); y += h1 + d;
        m_cbxExtension = new Fl_Choice(x,y,W-2*d,h1); y += h1 + d;

        m_edtArchiveName->set_items(
                        {"Untitled1.zst",
                         "Untitled2.tar"});
        m_edtArchiveName->onChange = [](int index, std::string text)
        {
            DE_OK("Selected[",index,"] ", text)
        };

        m_cbxExtension->add(".tar");
        m_cbxExtension->add(".zst");

        m_cbxExtension->callback([](Fl_Widget* w)
        {
            Fl_Choice* c = (Fl_Choice*)w;
            printf("selected: %s\n", c->mvalue()->label());
        });


        end();
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Window::resize(X, Y, W, H);

        const float zoom = Fl::screen_scale(0);
        const int d = 5 * zoom;
        const int h0 = 25 * zoom;
        const int h1 = 30 * zoom;
        const int hR = H - h0 - 3*h1 - 5*d; // hRemain
        const int h2 = 2 * hR / 3;
        const int h3 = hR - h2;
        int x = d;
        int y = d;

        m_edtArchiveName->resize(x,y,W-2*d,h1); y += h1 + d;
        m_cbxExtension->resize(x,y,W-2*d,h1); y += h1 + d;
    }
};
