#include <gui/Base.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Box.H>
#include <gui/XP_ProgressBar.h>
#include <gui/LogBox.h>

namespace EightZip {
namespace compressor {

struct Operation
{
    CompressOptions opts;

    int bitrate; // bitrate in kbit, e.g. 128, not 128000
    int quality; // quality 0..9

    // std::string getSrcUri() const { return inFile->edtUri->value(); }
    // std::string getDstUri() const { return outFile->edtUri->value(); }
};

struct UI
{
    // Title: 59% <Entpacken|Komprimieren> C:\Github\__lib_neu\2_demos.zst
    Fl_Box* lblTimeCurr = nullptr; // Verstrichene Zeit:
    Fl_Box* edtTimeCurr = nullptr;
    Fl_Box* lblTimeLeft = nullptr; // Verbleibende Zeit:
    Fl_Box* edtTimeLeft = nullptr;

    Fl_Box* lblNumFiles = nullptr; // Dateien:
    Fl_Box* edtFileIndex = nullptr;
    Fl_Box* edtFileCount = nullptr;

    Fl_Box* lblTotalBytes = nullptr;  // Gesamtdatenmenge:
    Fl_Box* edtTotalBytes = nullptr;
    Fl_Box* lblSpeed = nullptr;     // Geschwindigket:
    Fl_Box* edtSpeed = nullptr;

    Fl_Box* lblProcessed = nullptr;   // Verarbeitet:
    Fl_Box* edtProcessed = nullptr;

    Fl_Box* lblCompressed = nullptr;  // Komprimiert:
    Fl_Box* edtCompressed = nullptr;

    Fl_Box* lblCompressRatio = nullptr;  // Kompressionsrate:
    Fl_Box* edtCompressRatio = nullptr;

    Fl_Box* lblModus = nullptr;  // Hinzufügen|Entpacken:
    Fl_Box* lblDir = nullptr;   // Current DirectoryName
    Fl_Box* lblFile = nullptr;  // Current FileName

    XP_ProgressBar* progressBar = nullptr;

    LogBox* logBox = nullptr;

    Fl_Button* btnBackground = nullptr;
    Fl_Button* btnPause = nullptr;
    Fl_Button* btnCancel = nullptr;

    std::atomic<bool> bCancelFlag{false};
    std::atomic<bool> bPauseFlag{false};
    // std::atomic<bool> reloadFile{true};

    std::thread worker;
};

UI ui;



// ---------------- callbacks ----------------
static void pause_cb(Fl_Widget*, void*)
{
    ui.bPauseFlag = !ui.bPauseFlag;
}

// ---------------- callbacks ----------------
static void cancel_cb(Fl_Widget*, void*)
{
    if (ui.bCancelFlag)
    {
        return;
    }

    int r = fl_choice(
        "\n"
        "Do you like cancel the operation?\n"
        "\n",
        "Cancel operation",  // Button 0
        "Abort this dialog", // Button 1
        nullptr
    );

    if (r == 0) /* Cancel operation */
    {
        DE_OK("Pressed Cancel")
        ui.bCancelFlag = true;
    }
    else if (r == 1) /* Abort */
    {
        DE_OK("Pressed Abort")
    }
}

// ---------------- callbacks ----------------
static void progress_awake(void* data)
{
    int percent = *static_cast<int*>(data);
    ui.progressBar->value(0.01f * percent);
    delete static_cast<int*>(data);
}

// ---------------- Fl::awake ----------------
static void async_progress(int pc)
{
    auto progress = new int;
    *progress = pc;
    Fl::awake(progress_awake, progress);
}

// ---------------- worker ----------------
void async_compress_worker()
{
    ui.bCancelFlag = false;
/*
    std::string srcUri = ui.getSrcUri();
    std::string dstUri = ui.getDstUri();

    async_log_ok(dbStr("Compress Start: qualityPreset = ",ui.quality));
    async_progress(0);

    FileNamesA fileNames;
    collectFileNames(srcUri, fileNames);

    if (fileNames.empty())
    {
        async_log_error("No files: ");
        async_log_error(dbStr("Compress srcUri = ",srcUri));
        async_log_error(dbStr("Compress dstUri = ",dstUri));
        async_progress(0);
        return;
    }

    auto tarUri = dstUri+".tar";
    if (!tar_writer(fileNames,tarUri))
    {
        async_log_error("TAR failed: ");
        async_log_error(dbStr("srcUri = ",srcUri));
        async_log_error(dbStr("tarUri = ",tarUri));
        async_progress(0);
        return;
    }

    async_log_ok("TAR file created. ");
    async_log_ok(dbStr("TAR srcUri = ",srcUri));
    async_log_ok(dbStr("TAR tarUri = ",tarUri));
    async_progress(5);

    // CompressOptions opts;
    // opts.bCancelFlag = &ui.bCancelFlag;
    // opts.onLogger = [](int level, std::string msg) { async_log_common(msg,level); };
    // opts.onProgress = [](int pc) { async_progress(pc); };

    // if (!compress_zstd(tarUri,dstUri, opts))
    // {
    //     async_log_error("Compress failed: ");
    //     async_log_error(dbStr("Compress srcUri = ",srcUri));
    //     async_log_error(dbStr("Compress tarUri = ",tarUri));
    //     async_log_error(dbStr("Compress dstUri = ",dstUri));
    //     async_progress(0);
    //     return;
    // }

    // async_log_ok("TAR + ZStd compress Finished OK.");
    // async_log_ok(dbStr("Compress srcUri = ",srcUri));
    // async_log_ok(dbStr("Compress tarUri = ",tarUri));
    // async_log_ok(dbStr("Compress dstUri = ",dstUri));
*/
    async_progress(100);
}

void compress_cb(Fl_Widget*, void*)
{
/*
    std::string src_uri = ui.getSrcUri();
    std::string dst_uri = ui.getDstUri();
    if (src_uri == dst_uri)
    {
        DE_ERROR("Src and Dst fileNames must differ! Abort")
        return;
    }

    ui.bitrate = 0; // ui.encoder->getBitrate();
    ui.quality = 5; // ui.encoder->getQuality();
*/
    ui.worker = std::thread(async_compress_worker);
    ui.worker.detach();
}

/*
Fl_Choice* combo = new Fl_Choice(20, 40, 120, 28);
    combo->add(".tar");
    combo->add(".zst");

    combo->callback([](Fl_Widget* w){
        Fl_Choice* c = (Fl_Choice*)w;
        printf("selected: %s\n", c->mvalue()->label());
    });
*/

class Dialog : public Fl_Window
{
public:
    Dialog(int W, int H, const char* title) : Fl_Window(W, H, title)
    {
        begin();

        const float zoom = Fl::screen_scale(0);
        const int ml = 10 * zoom;
        const int mt = 10 * zoom;
        const int mr = 10 * zoom;
        const int mb = 10 * zoom;

        const int mw = W - ml - mr;
        const int mh = H - mt - mb;

        const int sx = 5 * zoom;
        const int sy = 3 * zoom;
        const int h1 = 12 * zoom;
        const int h2 = 30 * zoom;
        const int hLogBox = mh - 8*(h1+sy) - 2*(h2+sy); // hLogBox

        const int w0 = 40 * zoom;
        const int w1 = ((mw - w0) / 4) - sx;
        const int wBackG = (mw / 2) - sx;
        const int wPause = (mw / 4) - sx;
        const int wCancel = (mw / 4) - sx;

        // Row[1]
        int x = ml;
        int y = mt;
        ui.lblTimeCurr = new Fl_Box(x,y,w1,h1,"Elapsed Time:"); x += w1 + sx;
        ui.edtTimeCurr = new Fl_Box(x,y,w1,h1,"00:00:00"); x += w1 + sx + w0;
        ui.lblTotalBytes = new Fl_Box(x,y,w1,h1,"Total Size:"); x += w1 + sx;
        ui.edtTotalBytes = new Fl_Box(x,y,w1,h1,"420 MB"); x += w1 + sx;
        y += h1 + sy;

        ui.lblTimeCurr->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.edtTimeCurr->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.lblTotalBytes->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.edtTotalBytes->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

        // Row[2]
        x = ml;
        ui.lblTimeLeft = new Fl_Box(x,y,w1,h1,"Remaining Time:"); x += w1 + sx;
        ui.edtTimeLeft = new Fl_Box(x,y,w1,h1,"00:00:00"); x += w1 + sx + w0;
        ui.lblSpeed = new Fl_Box(x,y,w1,h1,"Speed:"); x += w1 + sx;
        ui.edtSpeed = new Fl_Box(x,y,w1,h1,"59 MB/s"); x += w1 + sx;
        y += h1 + sy;

        ui.lblTimeLeft->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.edtTimeLeft->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.lblSpeed->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.edtSpeed->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

        // Row[3]
        x = ml;
        ui.lblNumFiles = new Fl_Box(x,y,w1,h1,"Files:"); x += w1 + sx;
        ui.edtFileIndex = new Fl_Box(x,y,w1,h1,"3140"); x += w1 + sx + w0;
        ui.lblProcessed = new Fl_Box(x,y,w1,h1,"Processed:"); x += w1 + sx;
        ui.edtProcessed = new Fl_Box(x,y,w1,h1,"123 MB"); x += w1 + sx;
        y += h1 + sy;

        ui.lblNumFiles->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.edtFileIndex->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.lblProcessed->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.edtProcessed->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

        // Row[4]
        x = ml + w1 + sx;
        ui.edtFileCount = new Fl_Box(x,y,w1,h1,"4280"); x += w1 + sx + w0;
        ui.lblCompressed = new Fl_Box(x,y,w1,h1,"Compressed:"); x += w1 + sx;
        ui.edtCompressed = new Fl_Box(x,y,w1,h1,"46 MB"); x += w1 + sx;
        y += h1 + sy;

        //ui.lblNumFiles->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.edtFileCount->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.lblCompressed->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.edtCompressed->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

        // Row[5]
        x = ml + 2*(w1 + sx) + w0;
        ui.lblCompressRatio = new Fl_Box(x,y,w1,h1,"Compress Ratio:"); x += w1 + sx;
        ui.edtCompressRatio = new Fl_Box(x,y,w1,h1,"34%"); x += w1 + sx;
        y += h1 + sy;

        //ui.lblNumFiles->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        //ui.edtFileCount->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.lblCompressRatio->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
        ui.edtCompressRatio->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

        // Row[6]
        x = ml;
        ui.lblModus = new Fl_Box(x,y,mw,h1,"Compress:");
        y += h1 + sy;

        ui.lblModus->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

        // Row[7]
        x = ml;
        ui.lblDir = new Fl_Box(x,y,mw,h1,"DirectoryName");
        y += h1 + sy;

        ui.lblDir->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

        // Row[8]
        x = ml;
        ui.lblFile = new Fl_Box(x,y,mw,h1,"FileName");
        y += h1 + sy;

        ui.lblFile->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

        // Row[9]
        x = ml;
        ui.progressBar = new XP_ProgressBar(x,y,mw,h2);
        y += h2 + sy;

        // Row[10]
        x = ml;
        ui.logBox = new LogBox(x,y,mw,hLogBox,0);
        y += hLogBox + sy;

        // Row[11]
        x = ml;
        ui.btnBackground = new Fl_Button(x,y,wBackG,h2,"Background"); x += wBackG + sx;
        ui.btnPause = new Fl_Button(x,y,wPause,h2,"Pause"); x += wPause + sx;
        ui.btnCancel = new Fl_Button(x,y,wCancel,h2,"Cancel");

        // m_edtArchiveName->onChange = [](int index, std::string text)
        // {
        //     DE_OK("Selected[",index,"] ", text)
        // };

        ui.btnBackground->callback([](Fl_Widget* w)
        {
            Fl_Choice* c = (Fl_Choice*)w;
            DE_DEBUG("selected: ", c->value())

            if ( ui.logBox->visible() != 0)
            {
                ui.logBox->hide();
            }
            else
            {
                ui.logBox->show();
            }
        });

        ui.btnPause->callback([](Fl_Widget* w)
        {
            Fl_Choice* c = (Fl_Choice*)w;
            DE_DEBUG("selected: ", c->value())
        });

        ui.btnCancel->callback([](Fl_Widget* w)
        {
            Fl_Choice* c = (Fl_Choice*)w;
            DE_DEBUG("selected: ", c->value())
        });

        end();
    }

    void resize(int X, int Y, int W, int H) override
    {
        Fl_Window::resize(X, Y, W, H);

        const float zoom = Fl::screen_scale(0);
        const int ml = 10 * zoom;
        const int mt = 10 * zoom;
        const int mr = 10 * zoom;
        const int mb = 10 * zoom;

        const int mw = W - ml - mr;
        const int mh = H - mt - mb;

        const int sx = 5 * zoom;
        const int sy = 3 * zoom;
        const int h1 = 12 * zoom;
        const int h2 = 30 * zoom;
        const int hLogBox = mh - 8*(h1+sy) - 2*(h2+sy); // hLogBox

        const int w0 = 40 * zoom;
        const int w1 = ((mw - w0) / 4) - sx;
        const int wBackG = (mw / 2) - sx;
        const int wPause = (mw / 4) - sx;
        const int wCancel = (mw / 4) - sx;

        // Row[1]
        int x = ml;
        int y = mt;
        ui.lblTimeCurr->resize(x,y,w1,h1); x += w1 + sx;
        ui.edtTimeCurr->resize(x,y,w1,h1); x += w1 + sx + w0;
        ui.lblTotalBytes->resize(x,y,w1,h1); x += w1 + sx;
        ui.edtTotalBytes->resize(x,y,w1,h1); x += w1 + sx;
        y += h1 + sy;

        // Row[2]
        x = ml;
        ui.lblTimeLeft->resize(x,y,w1,h1); x += w1 + sx;
        ui.edtTimeLeft->resize(x,y,w1,h1); x += w1 + sx + w0;
        ui.lblSpeed->resize(x,y,w1,h1); x += w1 + sx;
        ui.edtSpeed->resize(x,y,w1,h1); x += w1 + sx;
        y += h1 + sy;

        // Row[3]
        x = ml;
        ui.lblNumFiles->resize(x,y,w1,h1); x += w1 + sx;
        ui.edtFileIndex->resize(x,y,w1,h1); x += w1 + sx + w0;
        ui.lblProcessed->resize(x,y,w1,h1); x += w1 + sx;
        ui.edtProcessed->resize(x,y,w1,h1); x += w1 + sx;
        y += h1 + sy;

        // Row[4]
        x = ml + w1 + sx;
        ui.edtFileCount->resize(x,y,w1,h1); x += w1 + sx + w0;
        ui.lblCompressed->resize(x,y,w1,h1); x += w1 + sx;
        ui.edtCompressed->resize(x,y,w1,h1); x += w1 + sx;
        y += h1 + sy;

        // Row[5]
        x = ml + 2*(w1 + sx) + w0;
        ui.lblCompressRatio->resize(x,y,w1,h1); x += w1 + sx;
        ui.edtCompressRatio->resize(x,y,w1,h1); x += w1 + sx;
        y += h1 + sy;

        // Row[6]
        x = ml;
        ui.lblModus->resize(x,y,mw,h1);
        y += h1 + sy;

        // Row[7]
        x = ml;
        ui.lblDir->resize(x,y,mw,h1);
        y += h1 + sy;

        // Row[8]
        x = ml;
        ui.lblFile->resize(x,y,mw,h1);
        y += h1 + sy;

        // Row[9]
        x = ml;
        ui.progressBar->resize(x,y,mw,h2);
        y += h2 + sy;

        // Row[10]
        x = ml;
        ui.logBox->resize(x,y,mw,hLogBox);
        y += hLogBox + sy;

        // Row[11]
        x = ml;
        ui.btnBackground->resize(x,y,wBackG,h2); x += wBackG + sx;
        ui.btnPause->resize(x,y,wPause,h2); x += wPause + sx;
        ui.btnCancel->resize(x,y,wCancel,h2);
    }
};


} // end namespace compressor.
} // end namespace EightZip.
