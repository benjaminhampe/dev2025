#pragma once
#include <gui/Base.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>
#include <gui/AB/Win11Combo.h>

#include <8z_ArgParser.h>

/*
Fl_Choice* combo = new Fl_Choice(20, 40, 120, 28);
    combo->add(".tar");
    combo->add(".zst");

    combo->callback([](Fl_Widget* w){
        Fl_Choice* c = (Fl_Choice*)w;
        printf("selected: %s\n", c->mvalue()->label());
    });
*/

namespace EightZip {
namespace builder {

// =============================================================
struct UI
// =============================================================
{
    // Top
    Fl_Box* lblArchive = nullptr;
    Fl_Box* edtDir = nullptr;
    Win11Combo* edtArchive = nullptr;
    Fl_Button* btnChoose = nullptr;

    // Body Column[0]
    Fl_Box* lblFormat = nullptr;
    Fl_Choice* cbxFormat = nullptr;

    Fl_Box* lblQuality = nullptr;
    Fl_Choice* cbxQuality = nullptr;

    Fl_Box* lblAlgorithm = nullptr;
    Fl_Choice* cbxAlgorithm = nullptr;

    Fl_Box* lblDictSize = nullptr;
    Fl_Choice* cbxDictSize = nullptr;

    Fl_Box* lblWordSize = nullptr;
    Fl_Choice* cbxWordSize = nullptr;

    Fl_Box* lblBlockSize = nullptr;
    Fl_Choice* cbxBlockSize = nullptr;

    Fl_Box* lblCpuThreads = nullptr;
    Fl_Choice* cbxCpuThreads = nullptr;

    Fl_Box* lblCompressRamMax = nullptr;
    Fl_Choice* cbxCompressRamMax = nullptr;

    Fl_Box* lblExtractRamMax = nullptr;
    Fl_Choice* cbxExtractRamMax = nullptr;

    Fl_Box* lblSplitSize = nullptr;
    Fl_Choice* cbxSplitSize = nullptr;

    Fl_Box* lblParameter = nullptr;
    Fl_Choice* cbxParameter = nullptr;

    Fl_Button* btnOptions = nullptr;

    // Body Column[1]
    Fl_Box* lblUpdateType = nullptr;
    Fl_Choice* cbxUpdateType = nullptr;

    Fl_Box* lblDirStruct = nullptr;
    Fl_Choice* cbxDirStruct = nullptr;

    Fl_Box* groupOpts = nullptr;
    Fl_Check_Button* optSelfExtract = nullptr;
    Fl_Check_Button* optIncWriteFile = nullptr;
    Fl_Check_Button* optDeleteFiles = nullptr;

    Fl_Box* groupCrypt = nullptr;
    Fl_Box* lblPassword1 = nullptr;
    Fl_Input* edtPassword1 = nullptr;
    Fl_Box* lblPassword2 = nullptr;
    Fl_Input* edtPassword2 = nullptr;
    Fl_Check_Button* optShowPassword = nullptr;
    Fl_Box* lblCryptAlgo = nullptr;
    Fl_Choice* cbxCryptAlgo = nullptr;
    Fl_Check_Button* optEncryptNames = nullptr;

    // Footer
    Fl_Button* btnOk = nullptr;
    Fl_Button* btnCancel = nullptr;
    Fl_Button* btnHelp = nullptr;


    Job getJob() const
    {
        Job job;
        job.bCompress = true;
        return job;
    }
};

static UI ui;

// =============================================================
class Dialog : public Fl_Window
// =============================================================
{
public:
    Dialog(int W, int H, const char* title) : Fl_Window(W, H, title)
    {
        begin();

        const float zoom = Fl::screen_scale(0);

        const int ml = 5 * zoom;
        const int mt = 5 * zoom;
        const int mr = 5 * zoom;
        const int mb = 5 * zoom;

        const int mw = W - ml - mr;
        const int mh = H - mt - mb;

        const int h0 = 30 * zoom;
        const int h1 = 14 * zoom;
        const int y1 = (h0 - h1)/2;

        const int w2 = mw / 2;
        const int w4 = mw / 4;

        const int s = 4 * zoom;

        int x = ml;
        int y = mt;

        // Top
        ui.lblArchive = new Fl_Box(x,y,mw,h1,"Archive:");
        ui.edtDir = new Fl_Box(x,y,mw,h1,"C:\\Hello\\World\\");
        ui.edtArchive = new Win11Combo(x,y,mw,h1,s);
        ui.btnChoose = new Fl_Button(x,y,mw,h1,"...");

        // Body Column[0]
        ui.lblFormat = new Fl_Box(x,y,mw,h1,"Archive:");
        ui.cbxFormat = new Fl_Choice(x,y,mw,h1);

        ui.lblQuality = new Fl_Box(x,y,mw,h1,"Compress-Quality:");
        ui.cbxQuality = new Fl_Choice(x,y,mw,h1);

        ui.lblAlgorithm = new Fl_Box(x,y,mw,h1,"Compress Algorithm:");
        ui.cbxAlgorithm = new Fl_Choice(x,y,mw,h1);

        ui.lblDictSize = new Fl_Box(x,y,mw,h1,"Dictionary Size:");
        ui.cbxDictSize = new Fl_Choice(x,y,mw,h1);

        ui.lblWordSize = new Fl_Box(x,y,mw,h1,"Word Size:");
        ui.cbxWordSize = new Fl_Choice(x,y,mw,h1);

        ui.lblBlockSize = new Fl_Box(x,y,mw,h1,"BlockSize:");
        ui.cbxBlockSize = new Fl_Choice(x,y,mw,h1);

        ui.lblCpuThreads = new Fl_Box(x,y,mw,h1,"CPU Threads:");
        ui.cbxCpuThreads = new Fl_Choice(x,y,mw,h1);

        ui.lblCompressRamMax = new Fl_Box(x,y,mw,h1,"Compress RAM Usage:");
        ui.cbxCompressRamMax = new Fl_Choice(x,y,mw,h1);

        ui.lblExtractRamMax = new Fl_Box(x,y,mw,h1,"Extract RAM Usage:");
        ui.cbxExtractRamMax = new Fl_Choice(x,y,mw,h1);

        ui.lblSplitSize = new Fl_Box(x,y,mw,h1,"Split Size:");
        ui.cbxSplitSize = new Fl_Choice(x,y,mw,h1);

        ui.lblParameter = new Fl_Box(x,y,mw,h1,"Parameter:");
        ui.cbxParameter = new Fl_Choice(x,y,mw,h1);

        ui.btnOptions = new Fl_Button(x,y,mw,h1,"Options");

        // Body Column[1]
        ui.lblUpdateType = new Fl_Box(x,y,mw,h1,"Update Type:");
        ui.cbxUpdateType = new Fl_Choice(x,y,mw,h1);

        ui.lblDirStruct = new Fl_Box(x,y,mw,h1,"Directory Struct:");
        ui.cbxDirStruct = new Fl_Choice(x,y,mw,h1);

        ui.groupOpts = new Fl_Box(x,y,mw,h1,"Update Type:");
        ui.optSelfExtract = new Fl_Check_Button(x,y,mw,h1,"Selfextract Archive");
        ui.optIncWriteFile = new Fl_Check_Button(x,y,mw,h1,"Include Write Files");
        ui.optDeleteFiles = new Fl_Check_Button(x,y,mw,h1,"Delete Files");

        ui.groupCrypt = new Fl_Box(x,y,mw,h1,"Encryption:");
        ui.lblPassword1 = new Fl_Box(x,y,mw,h1,"Password:");
        ui.edtPassword1 = new Fl_Input(x,y,mw,h1,"*******");
        ui.lblPassword2 = new Fl_Box(x,y,mw,h1,"Password:");
        ui.edtPassword2 = new Fl_Input(x,y,mw,h1,"*******");
        ui.optShowPassword = new Fl_Check_Button(x,y,mw,h1,"Show Password");
        ui.lblCryptAlgo = new Fl_Box(x,y,mw,h1,"Encrypt Mode:");
        ui.cbxCryptAlgo = new Fl_Choice(x,y,mw,h1);
        ui.optEncryptNames = new Fl_Check_Button(x,y,mw,h1,"Encrypt FileNames");

        // Footer
        ui.btnOk = new Fl_Button(x,y,mw,h1,"Ok");
        ui.btnCancel = new Fl_Button(x,y,mw,h1,"Ok");
        ui.btnHelp = new Fl_Button(x,y,mw,h1,"Ok");

        // m_edtArchiveName->onChange = [](int index, std::string text)
        // {
        //     DE_OK("Selected[",index,"] ", text)
        // };

        // m_cbxExtension->add(".tar");
        // m_cbxExtension->add(".zst");

        // m_cbxExtension->callback([](Fl_Widget* w)
        // {
        //     Fl_Choice* c = (Fl_Choice*)w;
        //     printf("selected: %s\n", c->mvalue()->label());
        // });

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

    }
};


} // end namespace builder.
} // end namespace EightZip.

