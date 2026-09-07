#include "8z_Builder.h"
#include "8z_App.h"
#include <gui/AB/Win11Combo.h>
#include <de/archive/ZstHeader.h>

namespace EightZip {
namespace builder {

// =============================================================
struct UI
// =============================================================
{
    Fl_Button* btnZoomIn = nullptr;
    Fl_Button* btnZoomOut = nullptr;

    // Top
    Fl_Box* lblArchive = nullptr;
    Fl_Box* edtDir = nullptr;
    Win11Combo* edtArchive = nullptr;
    Fl_Button* btnChoose = nullptr;

    // Body Column[0]
    Fl_Box* lblFormat = nullptr;
    Fl_Choice* cbxFormat = nullptr;

    Fl_Box* lblPreset = nullptr;
    Fl_Choice* cbxPreset = nullptr;

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

    GroupBox* groupOpts = nullptr;
    // Fl_Box* groupOpts = nullptr;
    Fl_Check_Button* optSelfExtract = nullptr;
    Fl_Check_Button* optIncWriteFile = nullptr;
    Fl_Check_Button* optDeleteFiles = nullptr;

    GroupBox* groupCrypt = nullptr;
    // Fl_Box* groupCrypt = nullptr;
    Fl_Box* lblPassword1 = nullptr;
    Fl_Input* edtPassword1 = nullptr;
    Fl_Box* lblPassword2 = nullptr;
    Fl_Input* edtPassword2 = nullptr;
    Fl_Check_Button* optShowPassword = nullptr;
    Fl_Box* lblCryptAlgo = nullptr;
    Fl_Choice* cbxCryptAlgo = nullptr;
    Fl_Check_Button* optEncryptNames = nullptr;

    // Footer Buttons:
    Fl_Button* btnOk = nullptr;
    Fl_Button* btnCancel = nullptr;
    Fl_Button* btnHelp = nullptr;

    // Footer Buttons callbacks:
    FN_onOk onOk;
    FN_onCancel onCancel;
    FN_onHelp onHelp;

    Job getJob() const
    {
        Job job;
        job.bCompress = true;
        job.iPreset = cbxPreset->value();
        job.bCompress = true;
        job.baseDir = App::getInstance()->getExeDirA();
        job.baseName = edtArchive->label();
        job.extension = cbxFormat->value();

        return job;
    }

    std::string getArchiveFormat() const
    {
        static std::array<std::string,2> my_map
        {
            "tar",
            "zst"
        };

        int i = cbxFormat->value();
        if (i < 0 || i > my_map.size())
        {
            DE_ERROR("Invalid index ",i)
            return "";
        }
        return my_map[i];
    }

    ZstPreset getZstPreset() const
    {
        const auto& presets = ZstPresets::get();

        int i = cbxPreset->value();
        if (i < 0 || i > presets.size())
        {
            DE_ERROR("Invalid index ",i)
            return {};
        }
        return presets[i];
    }

    // int getQuality()
    // {
    //     int quality_map[] = {0,1,5,7,9};
    //     return quality_map[quality->value()];
    // }
};

static UI ui;

void Dialog::setCallback_onOk(const FN_onOk& onOk)
{
    ui.onOk = onOk;
}

void Dialog::setCallback_onCancel(const FN_onCancel& onCancel)
{
    ui.onCancel = onCancel;
}

void Dialog::setCallback_onHelp(const FN_onHelp& onHelp)
{
    ui.onHelp = onHelp;
}

Job Dialog::getJob() const
{
    return ui.getJob();
}

template <typename T, typename... Args>
T* make_widget(int size, Args&&... args) {
    T* w = new T(std::forward<Args>(args)...);
    w->labelsize(size);

    if constexpr (std::is_base_of_v<Fl_Input, T>)
        w->textsize(size);

    return w;
}

// =============================================================
Dialog::Dialog(int W, int H, const char* title)
// =============================================================
    : Window(W, H, title)
{
    const float zoom = Fl::screen_scale(0);

    const int ml = 5 * zoom;
    const int mt = 5 * zoom;
    const int mr = 5 * zoom;
    const int mb = 5 * zoom;
    const int mw = W - ml - mr;
    const int h0 = 30 * zoom;
    const int h1 = 14 * zoom;
    const int s = 4 * zoom;

    int x = ml;
    int y = mt;

    ui.btnZoomIn = new Button(x,y,mw,h1,"+");
    ui.btnZoomOut = new Button(x,y,mw,h1,"-");

    // Top
    ui.lblArchive = new Label(x,y,mw,h1,"Archive:");
    ui.lblArchive->labelsize(18);
    ui.edtDir = new Label(x,y,mw,h1,"C:\\Hello\\World\\");
    ui.edtDir->align(FL_ALIGN_LEFT | FL_ALIGN_BOTTOM | FL_ALIGN_INSIDE);
    ui.edtArchive = new Win11Combo(x,y,mw,h1,s);
    ui.btnChoose = new Button(x,y,mw,h1,"...");

    // Body Column[0]
    ui.lblFormat = new Label(x,y,mw,h1,"Archive:");
    ui.cbxFormat = new Fl_Choice(x,y,mw,h1);

    ui.lblPreset = new Label(x,y,mw,h1,"Preset:");
    ui.cbxPreset = new Fl_Choice(x,y,mw,h1);

    // ui.lblQuality = new Label(x,y,mw,h1,"Compress-Quality:");
    // ui.cbxQuality = new Fl_Choice(x,y,mw,h1);

    ui.lblAlgorithm = new Label(x,y,mw,h1,"Compress Algorithm:");
    ui.cbxAlgorithm = new Fl_Choice(x,y,mw,h1);

    ui.lblDictSize = new Label(x,y,mw,h1,"Dictionary Size:");
    ui.cbxDictSize = new Fl_Choice(x,y,mw,h1);

    ui.lblWordSize = new Label(x,y,mw,h1,"Word Size:");
    ui.cbxWordSize = new Fl_Choice(x,y,mw,h1);

    ui.lblBlockSize = new Label(x,y,mw,h1,"BlockSize:");
    ui.cbxBlockSize = new Fl_Choice(x,y,mw,h1);

    ui.lblCpuThreads = new Label(x,y,mw,h1,"CPU Threads:");
    ui.cbxCpuThreads = new Fl_Choice(x,y,mw,h1);

    ui.lblCompressRamMax = new Label(x,y,mw,h1,"Compress RAM Usage:");
    ui.cbxCompressRamMax = new Fl_Choice(x,y,mw,h1);

    ui.lblExtractRamMax = new Label(x,y,mw,h1,"Extract RAM Usage:");
    ui.cbxExtractRamMax = new Fl_Choice(x,y,mw,h1);

    ui.lblSplitSize = new Label(x,y,mw,h1,"Split Size:");
    ui.cbxSplitSize = new Fl_Choice(x,y,mw,h1);

    ui.lblParameter = new Label(x,y,mw,h1,"Parameter:");
    ui.cbxParameter = new Fl_Choice(x,y,mw,h1);

    ui.btnOptions = new Button(x,y,mw,h1,"Options");

    // Body Column[1]
    ui.lblUpdateType = new Label(x,y,mw,h1,"Update Type:");
    ui.cbxUpdateType = new Fl_Choice(x,y,mw,h1);

    ui.lblDirStruct = new Label(x,y,mw,h1,"Directory Struct:");
    ui.cbxDirStruct = new Fl_Choice(x,y,mw,h1);

    //ui.groupOpts = new Label(x,y,mw,h1,"Update Type:");
    ui.groupOpts = new GroupBox(x,y,mw,h1,"Options:");
    ui.optSelfExtract = new Fl_Check_Button(x,y,mw,h1,"Selfextract Archive");
    ui.optIncWriteFile = new Fl_Check_Button(x,y,mw,h1,"Include Write Files");
    ui.optDeleteFiles = new Fl_Check_Button(x,y,mw,h1,"Delete Files");

    //ui.groupCrypt = new Label(x,y,mw,h1,"Encryption:");
    ui.groupCrypt = new GroupBox(x,y,mw,h1,"Encryption:");
    ui.lblPassword1 = new Label(x,y,mw,h1,"Password:");
    ui.edtPassword1 = new Fl_Input(x,y,mw,h1);
    ui.lblPassword2 = new Label(x,y,mw,h1,"Password:");
    ui.edtPassword2 = new Fl_Input(x,y,mw,h1);
    ui.edtPassword2->value("*******");
    ui.optShowPassword = new Fl_Check_Button(x,y,mw,h1,"Show Password");
    ui.lblCryptAlgo = new Label(x,y,mw,h1,"Encrypt Mode:");
    ui.cbxCryptAlgo = new Fl_Choice(x,y,mw,h1);
    ui.optEncryptNames = new Fl_Check_Button(x,y,mw,h1,"Encrypt FileNames");

    // Footer
    ui.btnOk = new Button(x,y,mw,h1,"Ok");
    ui.btnCancel = new Button(x,y,mw,h1,"Cancel");
    ui.btnHelp = new Button(x,y,mw,h1,"Help");

    ui.btnOk->callback(
        [](Fl_Widget*, void*)
        {
            if (ui.onOk) { ui.onOk(); }
            else { DE_ERROR("No onOk callback.") }
        });

    ui.btnCancel->callback(
        [](Fl_Widget*, void*)
        {
            if (ui.onCancel) { ui.onCancel(); }
            else { DE_ERROR("No onCancel callback.") }
        });

    ui.btnHelp->callback(
        [](Fl_Widget*, void*)
        {
            if (ui.onHelp) { ui.onHelp(); }
            else { DE_ERROR("No onHelp callback.") }
        });

    ui.btnZoomIn->callback(
        [](Fl_Widget*, void*)
        {
            float z = Fl::screen_scale(0);
            z += 0.1f;
            if (z > 2.5f) z = 2.5f;
            Fl::screen_scale(0, z);
        });

    ui.btnZoomOut->callback(
        [](Fl_Widget*, void*)
        {
            float z = Fl::screen_scale(0);
            z -= 0.1f;
            if (z < 0.5f) z = 0.5f;
            Fl::screen_scale(0, z);
        });

    ui.cbxFormat->add(".tar - TAR Archive");
    ui.cbxFormat->add(".zst - ZSTD Archive");
    // ui.cbxFormat->add(".zip - ZIP Archive");
    // ui.cbxFormat->add(".bz2 - BZIP2 Archive");
    // ui.cbxFormat->add(".gz - GZIP Archive");
    // ui.cbxFormat->add(".xz - XZ Archive");
    // ui.cbxFormat->add(".7z - 7-Zip Archive");
    ui.cbxFormat->value(0);

    /*
    ui.cbxQuality->add("0 - No compression");
    ui.cbxQuality->add("1 - Very fast");
    ui.cbxQuality->add("3 - Fast");
    ui.cbxQuality->add("5 - Normal");
    ui.cbxQuality->add("7 - Max");
    ui.cbxQuality->add("9 - Ultra");
    ui.cbxQuality->value(0);

    🧩 Fully custom preset:
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_strategy, ZSTD_btopt);
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_windowLog, 20);
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_hashLog, 18);
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_chainLog, 19);
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_searchLog, 5);
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_searchLength, 4);
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_targetLength, 16);

    🧩 Existing Presets for FastMode:
    ZSTD_CCtx_setParameter(cctx, ZSTD_c_fast, N=30); // N = 1 … 1000+
    ZSTD_c_fast	Überschreibt ZSTD_c_compressionLevel, erzwingt ZSTD_fast, setzt alle internen Parameter neu

    🧩 Existing Presets for High‑Level: (überschreiben alles andere)
    ZSTD_c_compressionLevel	1–22
        Setzt alle internen Parameter (WindowLog, ChainLog, HashLog, SearchLog, SearchLength, TargetLength, Strategy)

    Empfohlene Presets (UI‑tauglich)
        Fast‑1 — leicht schneller als Level 1, Ratio noch ok
        Fast‑3 — guter Kompromiss, oft verwendet
        Fast‑5 — deutlich schneller, Ratio spürbar schlechter
        Fast‑10 — sehr schnell, Ratio niedrig
        Fast‑20 — extrem schnell, Ratio sehr niedrig
        Fast‑50 — für Telemetrie/Logs
        Fast‑100 — für High‑Throughput Pipelines
        Fast‑200 — maximale Geschwindigkeit, Ratio minimal

    🧠 Warum diese Werte?

    Fast‑Mode ist ein kontinuierlicher Parameter, aber:
        ab Fast=1–5 ist Ratio noch brauchbar
        ab Fast=10–20 wird Ratio deutlich schlechter
        ab Fast=50–200 ist Ratio fast egal, nur Speed zählt
        über Fast=200 gibt es kaum noch messbare Vorteile

    Kombobox: Fast‑Mode

    Off
    Fast‑1 (leicht schneller)
    Fast‑3 (Standard‑Fast)
    Fast‑5 (schnell)
    Fast‑10 (sehr schnell)
    Fast‑20 (extrem schnell)
    Fast‑50 (Logs/Telemetry)
    Fast‑100 (High‑Throughput)

    Level	Strategie	Qualität
    −N Fast	ZSTD_fast	extrem schnell, geringste Ratio
    1	ZSTD_fast	schnell
    2	ZSTD_fast	schnell
    3	ZSTD_dfast	Standard‑Default
    4	ZSTD_dfast	besser
    5	ZSTD_greedy	mittlere Ratio
    6	ZSTD_lazy	höhere Ratio
    7	ZSTD_lazy	höhere Ratio
    8	ZSTD_lazy2	hohe Ratio
    9	ZSTD_lazy2	hohe Ratio
    10–12	ZSTD_lazy2	sehr hohe Ratio
    13–15	ZSTD_btlazy2	sehr hohe Ratio
    16–19	ZSTD_btopt	maximal
    20–22 Ultra	ZSTD_btultra	höchste Ratio, extrem langsam
    */
    const auto & zstPresets = ZstPresets::get();
    for (size_t i = 0; i < zstPresets.size(); ++i)
    {
        ui.cbxPreset->add(zstPresets[i].name.c_str());
    }

    ui.cbxPreset->value(16);

    ui.cbxPreset->callback([](Fl_Widget* w)
    {
        Fl_Choice* c = (Fl_Choice*)w;
        DE_DEBUG("[Preset] selected index: ", c->value())
        DE_DEBUG("[Preset] selected data: ", c->mvalue()->label())
    });

    end();
}

void Dialog::resize(int X, int Y, int W, int H)
{
    Fl_Window::resize(X, Y, W, H);


    const float zoom = Fl::screen_scale(0);

    const int ml = 5 * zoom;
    const int mt = 5 * zoom;
    const int mr = 5 * zoom;
    const int mb = 5 * zoom;

    const int mw = W - ml - mr;
    const int mh = H - mt - mb;

    //const int h0 = 30 * zoom;
    const int h1 = 22 * zoom;
    //const int y1 = (h0 - h1)/2;

    const int wM = 30 * zoom;
    const int w2 = (mw - wM) / 2;
    const int w4 = w2 / 2;

    const int s = 4 * zoom;

    int x = ml;
    int y = mt;
    int ln = h1 + s;

    int b = 16 * zoom;
    ui.btnZoomIn->resize(ml + mw - b,mt,b,b);
    ui.btnZoomOut->resize(ml + mw - 2*b-s,mt,b,b);

    // Top
    ui.lblArchive->resize(x,y,100,2*h1);
    ui.edtDir->resize(x+100,y,mw-200,h1);
    ui.edtArchive->resize(x+100,y+h1,mw-200,h1);
    ui.btnChoose->resize(x+mw - 50,y+h1,50,h1);
    y += ln + ln;

    // Body Column[0]
    ui.lblFormat->resize(x,   y,w4,h1);
    ui.cbxFormat->resize(x+w4,y,w4,h1);
    y += ln;

    ui.lblPreset->resize(x,   y,w4,h1);
    ui.cbxPreset->resize(x+w4,y,w4,h1);
    y += ln;

    ui.lblAlgorithm->resize(x,   y,w4,h1);
    ui.cbxAlgorithm->resize(x+w4,y,w4,h1);
    y += ln;

    ui.lblDictSize->resize(x,   y,w4,h1);
    ui.cbxDictSize->resize(x+w4,y,w4,h1);
    y += ln;

    ui.lblWordSize->resize(x,   y,w4,h1);
    ui.cbxWordSize->resize(x+w4,y,w4,h1);
    y += ln;

    ui.lblBlockSize->resize(x,   y,w4,h1);
    ui.cbxBlockSize->resize(x+w4,y,w4,h1);
    y += ln;

    ui.lblCpuThreads->resize(x,   y,w4,h1);
    ui.cbxCpuThreads->resize(x+w4,y,w4,h1);
    y += ln;

    ui.lblCompressRamMax->resize(x,   y,w4,h1);
    ui.cbxCompressRamMax->resize(x+w4,y,w4,h1);
    y += ln;

    ui.lblExtractRamMax->resize(x,   y,w4,h1);
    ui.cbxExtractRamMax->resize(x+w4,y,w4,h1);
    y += ln;

    ui.lblSplitSize->resize(x,y,w2,h1);
    y += ln;
    ui.cbxSplitSize->resize(x,y,w2,h1);
    y += ln;

    ui.lblParameter->resize(x,y,w2,h1);
    y += ln;
    ui.cbxParameter->resize(x,y,w2,h1);
    y += ln;
    ui.btnOptions->resize(x,y,w4,h1);
    y += ln;

    // Body Column[1]
    x = ml + w2 + wM;
    y = mt + 2*ln;
    ui.lblUpdateType->resize(x,   y,w4,h1);
    ui.cbxUpdateType->resize(x+w4,y,w4,h1);
    y += ln;

    ui.lblDirStruct->resize(x,   y,w4,h1);
    ui.cbxDirStruct->resize(x+w4,y,w4,h1);
    y += ln;

    int y2 = y;
    int wC = 25*zoom;
    ui.groupOpts->resize(x,y,w2,4*h1);
    y += h1;
    ui.optSelfExtract->resize(x+wC,y,w2-2*wC,h1);
    y += h1;
    ui.optIncWriteFile->resize(x+wC,y,w2-2*wC,h1);
    y += h1;
    ui.optDeleteFiles->resize(x+wC,y,w2-2*wC,h1);

    y = y2 + 4*ln;

    y2 = y;
    ui.groupCrypt->resize(x,y,w2,8*h1);
    y += ln;
    ui.lblPassword1->resize(x+wC,y,w2-wC*2,h1);
    y += h1;
    ui.edtPassword1->resize(x+wC,y,w2-wC*2,h1);
    y += ln;
    ui.lblPassword2->resize(x+wC,y,w2-wC*2,h1);
    y += h1;
    ui.edtPassword2->resize(x+wC,y,w2-wC*2,h1);
    y += ln;
    ui.optShowPassword->resize(x+wC,y,w2-wC*2,h1);
    y += ln;
    ui.lblCryptAlgo->resize(x+wC,y,w2-wC*2,h1);
    y += ln;
    ui.cbxCryptAlgo->resize(x+wC,y,w2-wC*2,h1);
    y += ln;
    ui.optEncryptNames->resize(x+wC,y,w2-wC*2,h1);
    y += h1;

    // Footer

    int sB = 10*zoom;
    int wB = (w2 + wM - 2*sB) / 3;
    x = ml + w2;
    ui.btnOk->resize(x,y,wB,h1); x += wB + sB;
    ui.btnCancel->resize(x,y,wB,h1); x += wB + sB;
    ui.btnHelp->resize(x,y,wB,h1);
}

} // end namespace builder.
} // end namespace EightZip.

