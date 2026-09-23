#include "8z_Worker.h"
#include "8z_Worker_private.h"
// #include "8z_Worker_compress.h"
// #include "8z_Worker_compressTar.h"
// #include "8z_Worker_compressZst.h"

namespace EightZip {
namespace worker {

// =============================================================
Worker::Worker(const Job& job, int W, int H, const char* title)
// =============================================================
    : DoubleWindow(W, H, title)
{
    ui.job = job;
    DE_DEBUG("Worker.Job = ", ui.job.str())
    ui.window = this;

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
    ui.lblTimeCurr = new Label(x,y,w1,h1,"Elapsed Time:"); x += w1 + sx;
    ui.edtTimeCurr = new Label(x,y,w1,h1,"00:00:00"); x += w1 + sx + w0;
    ui.lblTotalBytes = new Label(x,y,w1,h1,"Total Size:"); x += w1 + sx;
    ui.edtTotalBytes = new Label(x,y,w1,h1,"420 MB"); //x += w1 + sx;
    y += h1 + sy;

    ui.lblTimeCurr->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtTimeCurr->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.lblTotalBytes->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtTotalBytes->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[2]
    x = ml;
    ui.lblTimeLeft = new Label(x,y,w1,h1,"Remaining Time:"); x += w1 + sx;
    ui.edtTimeLeft = new Label(x,y,w1,h1,"00:00:00"); x += w1 + sx + w0;
    ui.lblSpeed = new Label(x,y,w1,h1,"Speed:"); x += w1 + sx;
    ui.edtSpeed = new Label(x,y,w1,h1,"59 MB/s"); //x += w1 + sx;
    y += h1 + sy;

    ui.lblTimeLeft->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtTimeLeft->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.lblSpeed->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtSpeed->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[3]
    x = ml;
    ui.lblNumFiles = new Label(x,y,w1,h1,"Files:"); x += w1 + sx;
    ui.edtFileIndex = new Label(x,y,w1,h1,"3140"); x += w1 + sx + w0;
    ui.lblProcessed = new Label(x,y,w1,h1,"Processed:"); x += w1 + sx;
    ui.edtProcessed = new Label(x,y,w1,h1,"123 MB"); //x += w1 + sx;
    y += h1 + sy;

    ui.lblNumFiles->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtFileIndex->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.lblProcessed->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtProcessed->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[4]
    x = ml + w1 + sx;
    ui.edtFileCount = new Label(x,y,w1,h1,"4280"); x += w1 + sx + w0;
    ui.lblCompressed = new Label(x,y,w1,h1,"Compressed:"); x += w1 + sx;
    ui.edtCompressed = new Label(x,y,w1,h1,"46 MB"); //x += w1 + sx;
    y += h1 + sy;

    //ui.lblNumFiles->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtFileCount->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.lblCompressed->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtCompressed->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[5]
    x = ml + 2*(w1 + sx) + w0;
    ui.lblCompressRatio = new Label(x,y,w1,h1,"Compress Ratio:"); x += w1 + sx;
    ui.edtCompressRatio = new Label(x,y,w1,h1,"34%"); //x += w1 + sx;
    y += h1 + sy;

    //ui.lblNumFiles->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    //ui.edtFileCount->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.lblCompressRatio->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    ui.edtCompressRatio->align(FL_ALIGN_RIGHT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[6]
    x = ml;
    ui.edtModus = new Label(x,y,mw,h1,"Compress:");
    y += h1 + sy;

    ui.edtModus->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[7]
    x = ml;
    ui.edtDir = new Label(x,y,mw,h1,"DirectoryName");
    y += h1 + sy;

    ui.edtDir->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

    // Row[8]
    x = ml;
    ui.edtFile = new Label(x,y,mw,h1,"FileName");
    y += h1 + sy;

    ui.edtFile->align(FL_ALIGN_LEFT | FL_ALIGN_TOP | FL_ALIGN_INSIDE);

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
    ui.btnBackground = new Button(x,y,wBackG,h2,"Background"); x += wBackG + sx;
    ui.btnPause = new Button(x,y,wPause,h2,"Pause"); x += wPause + sx;
    ui.btnCancel = new Button(x,y,wCancel,h2,"Cancel");

    // m_edtArchiveName->onChange = [](int index, std::string text)
    // {
    //     DE_OK("Selected[",index,"] ", text)
    // };

    ui.btnBackground->callback(start_cb);

    end();

    // Fl::add_awake_handler_(awakeHandler, &ui);

    // resizable(ui.edtFile);

    // initial_filescan();

    ui.edtDir->copy_label( ui.job.directory.c_str() );
    ui.edtFile->copy_label( ui.job.fileName.c_str() );
    ui.edtFileIndex->copy_label("0");
    ui.edtFileCount->copy_label("0");
    ui.edtTotalBytes->copy_label("0");
    ui.edtSpeed->copy_label("0 MB/s");
}

void Worker::resize(int X, int Y, int W, int H)
{
    // 1. Basis-Resize
    Fl_Double_Window::resize(X, Y, W, H);

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
    ui.edtTotalBytes->resize(x,y,w1,h1); //x += w1 + sx;
    y += h1 + sy;

    // Row[2]
    x = ml;
    ui.lblTimeLeft->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtTimeLeft->resize(x,y,w1,h1); x += w1 + sx + w0;
    ui.lblSpeed->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtSpeed->resize(x,y,w1,h1); //x += w1 + sx;
    y += h1 + sy;

    // Row[3]
    x = ml;
    ui.lblNumFiles->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtFileIndex->resize(x,y,w1,h1); x += w1 + sx + w0;
    ui.lblProcessed->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtProcessed->resize(x,y,w1,h1); //x += w1 + sx;
    y += h1 + sy;

    // Row[4]
    x = ml + w1 + sx;
    ui.edtFileCount->resize(x,y,w1,h1); x += w1 + sx + w0;
    ui.lblCompressed->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtCompressed->resize(x,y,w1,h1); //x += w1 + sx;
    y += h1 + sy;

    // Row[5]
    x = ml + 2*(w1 + sx) + w0;
    ui.lblCompressRatio->resize(x,y,w1,h1); x += w1 + sx;
    ui.edtCompressRatio->resize(x,y,w1,h1); //x += w1 + sx;
    y += h1 + sy;

    // Row[6]
    x = ml;
    ui.edtModus->resize(x,y,mw,h1);
    y += h1 + sy;

    // Row[7]
    x = ml;
    ui.edtDir->resize(x,y,mw,h1);
    y += h1 + sy;

    // Row[8]
    x = ml;
    ui.edtFile->resize(x,y,mw,h1);
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

} // end namespace worker.
} // end namespace EightZip.
