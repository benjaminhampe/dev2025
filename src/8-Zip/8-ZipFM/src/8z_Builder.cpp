#include "8z_Builder.h"
#include "8z_App.h"
#include <gui/AB/Win11Combo.h>
#include <gui/ComboBox.h>
#include <de/archive/ZstWriter.h>
#include <de/CoreUtil.h> // de_powi()
// #include <filesystem>

namespace EightZip {
namespace builder {
namespace {

std::string getLastDirectory(const std::string& uri)
{
    auto s1 = dbMakePosix(uri);

    auto p2 = s1.find_last_of('/');
    if (p2 == std::string::npos || p2 < 1)
    {
        DE_ERROR("Invalid p2(",p2,") in uri(", uri,")")
        return "";
    }

    auto p1 = s1.find_last_of('/',p2-1);
    if (p1 == std::string::npos)
    {
        DE_ERROR("No p1 in uri(", uri,")")
        auto s2 = s1.substr(0, p2);
        DE_DEBUG("Got s1(", s1,")")
        DE_DEBUG("Got s2(", s2,")")
        return s2;
    }
    else
    {
        // "/a/b.c" -> "a"
        // p1 = 0
        // p2 = 2
        // s2 = s1.substr(1, 1);
        auto s2 = s1.substr(p1+1, p2-p1-1);
        DE_DEBUG("Got s1(", s1,")")
        DE_DEBUG("Got s2(", s2,")")
        return s2;
    }
}

/*
std::string replaceExtension(std::string uri, std::string ext)
{
    auto p1 = std::filesystem::u8path(uri);
    auto p2 = std::filesystem::u8path(ext);
    return p1.replace_extension(p2).u8string();
}


template <typename T, typename... Args>
T* make_widget(int size, Args&&... args) {
    T* w = new T(std::forward<Args>(args)...);
    w->labelsize(size);

    if constexpr (std::is_base_of_v<Fl_Input, T>)
        w->textsize(size);

    return w;
}
*/

} // end namespace

// =============================================================
struct UI
// =============================================================
{
    int iLastPreset = 0; // No compression

    Job job;

    Fl_Button* btnZoomIn = nullptr;
    Fl_Button* btnZoomOut = nullptr;

    // Top
    Fl_Box* lblArchive = nullptr;
    Fl_Input* edtDir = nullptr;
    Fl_Input* edtFile = nullptr;
    Fl_Button* btnChoose = nullptr;
    //Win11Combo* edtArchive = nullptr;

    // Body Column[0]
    Fl_Box* lblFormat = nullptr;
    ComboBox* cbxFormat = nullptr;

    Fl_Box* lblPreset = nullptr;
    ComboBox* cbxPreset = nullptr;

/*
    preset.data[ZSTD_c_compressionLevel] = 19;
    preset.data[ZSTD_c_nbWorkers] = numThreads;
    preset.data[ZSTD_c_jobSize] = 2 * 1024 * 1024;
    preset.data[ZSTD_c_windowLog] = 26;
    preset.data[ZSTD_c_enableLongDistanceMatching] = 1;
*/

    Fl_Box* lblCompressLevel = nullptr;
    ComboBox* edtCompressLevel = nullptr;

    Fl_Box* lblCpuThreads = nullptr;
    ComboBox* edtCpuThreads = nullptr;

    Fl_Box* lblJobSize = nullptr;
    ComboBox* edtJobSize = nullptr;

    Fl_Box* lblWindowLog = nullptr;
    ComboBox* edtWindowLog = nullptr;

    Fl_Box* lblLongDistMatching = nullptr;
    ComboBox* edtLongDistMatching = nullptr;

/*

    Fl_Box* lblDictSize = nullptr;
    ComboBox* cbxDictSize = nullptr;

    Fl_Box* lblWordSize = nullptr;
    ComboBox* cbxWordSize = nullptr;

    Fl_Box* lblBlockSize = nullptr;
    ComboBox* cbxBlockSize = nullptr;


    Fl_Box* lblCompressRamMax = nullptr;
    ComboBox* cbxCompressRamMax = nullptr;

    Fl_Box* lblExtractRamMax = nullptr;
    ComboBox* cbxExtractRamMax = nullptr;

    Fl_Box* lblSplitSize = nullptr;
    ComboBox* cbxSplitSize = nullptr;

    Fl_Box* lblParameter = nullptr;
    ComboBox* cbxParameter = nullptr;

    Fl_Button* btnOptions = nullptr;

    // Body Column[1]
    Fl_Box* lblUpdateType = nullptr;
    ComboBox* cbxUpdateType = nullptr;

    Fl_Box* lblDirStruct = nullptr;
    ComboBox* cbxDirStruct = nullptr;

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
    ComboBox* cbxCryptAlgo = nullptr;
    Fl_Check_Button* optEncryptNames = nullptr;
*/

    // Footer Buttons:
    Fl_Button* btnOk = nullptr;
    Fl_Button* btnCancel = nullptr;
    Fl_Button* btnHelp = nullptr;

    // Footer Buttons callbacks:
    FN_onOk onOk;
    FN_onCancel onCancel;
    FN_onHelp onHelp;


    std::string selectedFormat() const
    {
        static std::array<std::string,2> formats
        {
            "tar", "zst"
        };

        int i = cbxFormat->value();
        if (i < 0 || i > formats.size())
        {
            DE_ERROR("Invalid format ",i, " of ",formats.size())
            return "";
        }
        return formats[i];
    }

/*
    ZstPreset selectedPresetZst() const
    {
        const auto& presets = ZstPresets::get();

        int i = cbxPreset->value();
        if (i < 0 || i > presets.size())
        {
            DE_ERROR("Invalid preset ",i, " of ",presets.size())
            return {};
        }
        return presets[i];
    }
*/

    // int getQuality()
    // {
    //     int quality_map[] = {0,1,5,7,9};
    //     return quality_map[quality->value()];
    // }

    static bool dbExistDirectory(const de::FileInfo& fileInfo)
    {
        return fileInfo.isDir();
    }

    static bool dbExistFile(const de::FileInfo& fileInfo)
    {
        return fileInfo.isFile();
    }

    void produceDirName()
    {
        std::string dir = App::getInstance()->getExeDirA();

        if (job.filesIn.size() > 0)
        {
            dir = dbFileDir(job.filesIn[0].uriA());
        }

        edtDir->value( dir.c_str() );
    }

    void produceFileName()
    {
        // fileName
        std::string fn = edtFile->value() ? edtFile->value() : "";

        const std::string ext = selectedFormat();

        //===========================================
        // Remove suffix, leave baseName
        //===========================================
        DE_DEBUG("[Cut] Input ",fn)

        if (dbStrEndsWith(fn,".tar.zst"))
        {
            DE_DEBUG("[Cut] Before (.tar.zst) ",fn)
            fn = fn.substr(0,fn.size() - 8);
            DE_DEBUG("[Cut] After (.tar.zst) ",fn)
        }
        if (dbStrEndsWith(fn,".zst"))
        {
            DE_DEBUG("[Cut] Before (.zst) ",fn)
            fn = fn.substr(0,fn.size() - 4);
            DE_DEBUG("[Cut] After (.zst) ",fn)
        }
        if (dbStrEndsWith(fn,".tar"))
        {
            DE_DEBUG("[Cut] Before (.tar.zst) ",fn)
            fn = fn.substr(0,fn.size() - 4);
            DE_DEBUG("[Cut] After (.tar) ",fn)
        }

        //===========================================
        // Create baseName, take job.filesIn[] into account
        //===========================================
        if (fn.empty())
        {
            if (job.filesIn.size() == 0)
            {
                fn = "8z_untitled";
                DE_DEBUG("[Empty] ",fn)
            }
            else if (job.filesIn.size() == 1)
            {
                const auto& fi = job.filesIn[0];

                if (ext == "tar")
                {
                    fn = dbFileName(fi.uriA());
                    DE_DEBUG("[Generic] ",fn)
                }
                else if (ext == "zst")
                {
                    if (dbExistDirectory(fi))
                    {
                        fn = dbFileName(fi.uriA());
                        DE_DEBUG("[Dir] ",fn)
                    }
                    else
                    {
                        fn = dbFileName(fi.uriA());
                        DE_DEBUG("[File] ",fn)
                    }
                }
            }
            else
            {
                const auto& fi = job.filesIn[0];
                DE_DEBUG("job.filesIn[0] = ",job.filesIn[0].str())
                DE_DEBUG("job.filesIn[1] = ",job.filesIn[1].str())
                fn = getLastDirectory(fi.uriA());
                DE_DEBUG("getLastDirectory() = ",fn)
            }
        }

        //=====================================================
        // Create suffix, also takes job.filesIn[] into account
        //=====================================================
        // For compressor only (zst, gz, bz2, etc..).
        // + 1 file "video.mp4" becomes "video.mp4.zst"
        // + 1 dir "fol.der" becomes "fol.der.tar.zst"
        // + 1 file "arch.tar" becomes "arch.tar.zst"
        //=====================================================
        if (ext == "tar")
        {
            fn += ".tar"; // Same for single or multiple files and dirs.
        }
        else if (ext == "zst")
        {
            if (job.filesIn.size() == 1)
            {
                const auto& fi = job.filesIn[0]; // Single file or dir

                if (dbExistDirectory(fi))
                {
                    fn += ".tar.zst"; // Single dir
                }
                else
                {
                    fn += ".zst"; // Single file
                }
            }
            else
            {
                fn += ".tar.zst"; // Multiple files or dirs
            }
        }

        edtFile->value(fn.c_str());

        if (ext == "tar")
        {
            iLastPreset = cbxPreset->value();
            cbxPreset->value(0);
            cbxPreset->deactivate();
        }
        else if (ext == "zst")
        {
            cbxPreset->activate();
            cbxPreset->value(iLastPreset);
        }

        // DE_DEBUG("[Format] index(",index,"), "
        //         "label(", (label ? label : "nullptr"), "), "
        //         "ext(", ext,")")

    }
};

static UI ui;

static void populateTarPresets()
{
    ui.cbxPreset->clear();
    ui.cbxPreset->add("0 - Save all files and dirs uncompressed.");
    ui.cbxPreset->value(0);
}

static void populateZstPresets()
{
    ui.cbxPreset->clear();

    auto zstPresets = ZstPresets::getInstance();
    auto zstDefaultPreset = zstPresets->getDefaultIndex();
    for (size_t i = 0; i < zstPresets->getPresetCount(); ++i)
    {
        const auto& zstPreset = zstPresets->getPreset(i);
        ui.cbxPreset->add(zstPreset.name.c_str());
    }
    ui.cbxPreset->value(zstDefaultPreset);
}

static void cbxFormat_cb(Fl_Widget* widget, void* data)
{
    // Das generische Widget in ein Fl_Choice-Objekt umwandeln
    auto self = (Fl_Choice*)widget;

    int index = self->value();

    // Den Text des ausgewählten Elements abrufen
    const char* label = self->text();

    ui.produceFileName();

    if (index == 1)
    {
        populateZstPresets();
    }
    else
    {
        populateTarPresets();
    }
}

static void cbxPreset_cb(Fl_Widget* widget, void* data)
{
    // Das generische Widget in ein Fl_Choice-Objekt umwandeln
    auto self = (Fl_Choice*)widget;

    int index = self->value();

    // Den Text des ausgewählten Elements abrufen
    const char* label = self->text();

    // ZstPresets::getInstance()

    // DE_DEBUG("[Preset] "
    //             "index(",index,"), "
    //             "label(", (label ? label : "nullptr"), "), "
    //             "algo(", ui.selectedPresetZst().algo,"), "
    //             "level(", ui.selectedPresetZst().level,")")
}

void Builder::setCallback_onOk(const FN_onOk& onOk)
{
    ui.onOk = onOk;
}

void Builder::setCallback_onCancel(const FN_onCancel& onCancel)
{
    ui.onCancel = onCancel;
}

void Builder::setCallback_onHelp(const FN_onHelp& onHelp)
{
    ui.onHelp = onHelp;
}

Job Builder::getJob() const
{
    Job job = ui.job;
    job.bCompress = true;
    job.directory = "";
    job.fileName = "";
    job.iPreset = -1;

    // ======== baseDir =========================
    if (ui.edtDir && ui.edtDir->value())
    {
        job.directory = ui.edtDir->value();
    }
    else
    {
        DE_ERROR("Got empty ui.edtDir->value()")
    }

    if (job.directory.empty())
    {
        job.directory = App::getInstance()->getExeDirA();
    }

    // ======== baseName =========================
    if (ui.edtFile && ui.edtFile->value())
    {
        job.fileName = ui.edtFile->value();
    }
    else
    {
        DE_ERROR("Got empty ui.edtArchive->value()")
    }

    if (job.fileName.empty())
    {
        job.fileName = "8z_untitled.tar";
    }

    // ======== extension =========================
    // if (ui.cbxFormat && !ui.cbxFormat->currentData().toString().empty())
    // {
    //     job.extension = ui.cbxFormat->currentData().toString();
    // }
    // else
    // {
    //     DE_ERROR("Got empty ui.cbxFormat->currentData().toString()")
    // }

    // if (job.extension.empty())
    // {
    //     DE_ERROR("Got empty extension, fallback to .tar")
    //     job.extension = "tar";
    // }

    // ======== iPreset =========================
    if (ui.cbxPreset)
    {
        job.iPreset = ui.cbxPreset->value();
    }

    if (job.iPreset < 0)
    {
        auto suffix = dbFileSuffix(job.fileName);
        if (suffix == "zst")
        {
            DE_ERROR("Got invalid zst preset, fallback to 5")
            job.iPreset = 5;
        }
    }

    DE_DEBUG("Job: ",job.str())

    return job;
}

void Builder::setJob(const Job& job)
{
    ui.job = job;

    DE_DEBUG("Got Job: ")
    DE_DEBUG(ui.job.str())

    ui.produceDirName();
    ui.produceFileName();

    /*
    if (ui.job.directory.empty())
    {
        if (ui.job.filesIn.size() > 0)
        {
            ui.job.directory = dbFileDir(ui.job.filesIn[0]);
        }
        else
        {
            ui.job.directory = App::getInstance()->getExeDirA();
            DE_ERROR("Fallback job.directory")
        }
    }

    if (ui.job.fileName.empty())
    {
        if (ui.job.filesIn.size() > 1)
        {
            DE_DEBUG("ui.job.filesIn[0] = ",ui.job.filesIn[0])
            DE_DEBUG("ui.job.filesIn[1] = ",ui.job.filesIn[1])
            ui.job.fileName = getLastDirectory(ui.job.filesIn[0]);
            DE_DEBUG("getLastDirectory() = ",ui.job.fileName)
        }
        else if (ui.job.filesIn.size() == 1)
        {
            DE_DEBUG("ui.job.filesIn[0] = ",ui.job.filesIn[0])
            if (dbExistDirectory(ui.job.filesIn[0]))
            {
                ui.job.fileName = dbFileName(ui.job.filesIn[0]);
                DE_DEBUG("[Dir] dbFileName() = ",ui.job.fileName)
            }
            else
            {
                ui.job.fileName = dbFileBase(ui.job.filesIn[0]);
                DE_DEBUG("[File] dbFileBase() = ",ui.job.fileName)
            }
        }
        else
        {
            ui.job.fileName = "8z_Untitled";
            DE_ERROR("Fallback job.fileName = ", ui.job.fileName)
        }

        ui.job.fileName += ".";
        ui.job.fileName += ui.selectedFormat();
    }

    ui.edtDir->value( ui.job.directory.c_str() );
    ui.edtFile->value( ui.job.fileName.c_str() );

    // job.baseDir = App::getInstance()->getExeDirA();
    // job.baseName = ui.edtArchive->label();
    // job.bCompress = true;
    // job.extension = ui.cbxFormat->currentData().toString();
    // job.iPreset = ui.cbxPreset->currentData().toInt();
    */
}

// =============================================================
Builder::Builder(int W, int H, const char* title)
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
    ui.edtDir = new Fl_Input(x,y,mw,h1);
    ui.edtDir->align(FL_ALIGN_LEFT | FL_ALIGN_BOTTOM | FL_ALIGN_INSIDE);
    //ui.edtArchive = new Win11Combo(x,y,mw,h1,s);
    ui.edtFile = new Fl_Input(x,y,mw,h1);
    ui.btnChoose = new Button(x,y,mw,h1,"...");

    // Body Column[0]
    ui.lblFormat = new Label(x,y,mw,h1,"Archive:");
    ui.cbxFormat = new ComboBox(x,y,mw,h1);

    ui.lblPreset = new Label(x,y,mw,h1,"Preset:");
    ui.cbxPreset = new ComboBox(x,y,mw,h1);

    // ZstPreset:

    ui.lblCompressLevel = new Label(x,y,mw,h1,"Compress Level:");
    ui.edtCompressLevel = new ComboBox(x,y,mw,h1);
    ui.edtCompressLevel->copy_tooltip(ZstUtil::cpHelpStr(100).c_str());
    // ui.edtCompressLevel->add(dbStr(ZstUtil::cpMin(100)).c_str());
    // ui.edtCompressLevel->add(dbStr(ZstUtil::cpMax(100)).c_str());
    for (int i = 0; i <= 22; ++i)
    {
        ui.edtCompressLevel->add(dbStr(i).c_str());
    }

    ui.lblCpuThreads = new Label(x,y,mw,h1,"CPU Threads:");
    ui.edtCpuThreads = new ComboBox(x,y,mw,h1);
    ui.edtCpuThreads->copy_tooltip(ZstUtil::cpHelpStr(400).c_str());
    for (int i = ZstUtil::cpMin(400); i <= ZstUtil::cpMax(400); ++i)
    {
        ui.edtCpuThreads->add(dbStr(i).c_str());
    }

    ui.lblJobSize = new Label(x,y,mw,h1,"JobSize in kB:");
    ui.edtJobSize = new ComboBox(x,y,mw,h1);
    ui.edtJobSize->copy_tooltip(ZstUtil::cpHelpStr(401).c_str());

    ui.edtJobSize->add("0 - Auto");
    int i = 0;
    do
    {
        i++;
        int64_t po2 = de_powi(2,i+8);
        if (po2 > ZstUtil::cpMax(401))
        {
            break;
        }
        ui.edtJobSize->add(dbStr(i," = ",dbStrBytes(po2)).c_str());

    } while (i < 40);
    ui.edtJobSize->add(dbStr(i," = ",dbStrBytes(ZstUtil::cpMax(401))).c_str());

    ui.lblWindowLog = new Label(x,y,mw,h1,"WindowLog:");
    ui.edtWindowLog = new ComboBox(x,y,mw,h1);
    ui.edtWindowLog->copy_tooltip(ZstUtil::cpHelpStr(101).c_str());
    for (int i = ZstUtil::cpMin(101); i <= ZstUtil::cpMax(101); ++i)
    {
        ui.edtWindowLog->add(dbStr(i," = ", dbStrBytes(de_powi(2,i))).c_str());
    }

    ui.lblLongDistMatching = new Label(x,y,mw,h1,"LongDistMatching:");
    ui.edtLongDistMatching = new ComboBox(x,y,mw,h1);
    ui.edtLongDistMatching->copy_tooltip(ZstUtil::cpHelpStr(160).c_str());
    for (int i = ZstUtil::cpMin(160); i <= ZstUtil::cpMax(160); ++i)
    {
        ui.edtLongDistMatching->add(dbStr(i).c_str());
    }

#if 0
    ui.lblAlgorithm = new Label(x,y,mw,h1,"Compress Algorithm:");
    ui.cbxAlgorithm = new ComboBox(x,y,mw,h1);

    ui.lblDictSize = new Label(x,y,mw,h1,"Dictionary Size:");
    ui.cbxDictSize = new ComboBox(x,y,mw,h1);

    ui.lblWordSize = new Label(x,y,mw,h1,"Word Size:");
    ui.cbxWordSize = new ComboBox(x,y,mw,h1);

    ui.lblBlockSize = new Label(x,y,mw,h1,"BlockSize:");
    ui.cbxBlockSize = new ComboBox(x,y,mw,h1);

    ui.lblCpuThreads = new Label(x,y,mw,h1,"CPU Threads:");
    ui.cbxCpuThreads = new ComboBox(x,y,mw,h1);

    ui.lblCompressRamMax = new Label(x,y,mw,h1,"Compress RAM Usage:");
    ui.cbxCompressRamMax = new ComboBox(x,y,mw,h1);

    ui.lblExtractRamMax = new Label(x,y,mw,h1,"Extract RAM Usage:");
    ui.cbxExtractRamMax = new ComboBox(x,y,mw,h1);

    ui.lblSplitSize = new Label(x,y,mw,h1,"Split Size:");
    ui.cbxSplitSize = new ComboBox(x,y,mw,h1);

    ui.lblParameter = new Label(x,y,mw,h1,"Parameter:");
    ui.cbxParameter = new ComboBox(x,y,mw,h1);

    ui.btnOptions = new Button(x,y,mw,h1,"Options");

    // Body Column[1]
    ui.lblUpdateType = new Label(x,y,mw,h1,"Update Type:");
    ui.cbxUpdateType = new ComboBox(x,y,mw,h1);

    ui.lblDirStruct = new Label(x,y,mw,h1,"Directory Struct:");
    ui.cbxDirStruct = new ComboBox(x,y,mw,h1);

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
    ui.cbxCryptAlgo = new ComboBox(x,y,mw,h1);
    ui.optEncryptNames = new Fl_Check_Button(x,y,mw,h1,"Encrypt FileNames");
#endif

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

    // ========================================================
    ui.cbxFormat->add(".tar - TAR Archive");
    ui.cbxFormat->add(".zst - ZSTD Archive");
    // ui.cbxFormat->add(".zip - ZIP Archive");
    // ui.cbxFormat->add(".bz2 - BZIP2 Archive");
    // ui.cbxFormat->add(".gz - GZIP Archive");
    // ui.cbxFormat->add(".xz - XZ Archive");
    // ui.cbxFormat->add(".7z - 7-Zip Archive");
    ui.cbxFormat->value(1);
    ui.cbxFormat->callback(cbxFormat_cb);
    // ========================================================
    populateZstPresets();
    ui.cbxPreset->callback(cbxPreset_cb);
    // ========================================================
    end();
}

void Builder::resize(int X, int Y, int W, int H)
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
    const int wE = 3 * w2 / 5;  // Left Edit/Combo width
    const int wL = w2 - wE;     // Left Label width

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
    ui.edtFile->resize(x+100,y+h1,mw-200,h1);
    ui.btnChoose->resize(x+mw - 50,y+h1,50,h1);
    y += ln + ln;

    // Body Column[0]
    int x1 = ml;
    int x2 = ml + wL;
    ui.lblFormat->resize(x1,y,wL,h1);
    ui.cbxFormat->resize(x2,y,wE,h1);
    y += ln;

    ui.lblPreset->resize(x1,y,wL,h1);
    ui.cbxPreset->resize(x2,y,wE,h1);
    y += ln;

    ui.lblCompressLevel->resize(x1,y,wL,h1);
    ui.edtCompressLevel->resize(x2,y,wE,h1);
    y += ln;

    ui.lblCpuThreads->resize(x1,y,wL,h1);
    ui.edtCpuThreads->resize(x2,y,wE,h1);
    y += ln;

    ui.lblJobSize->resize(x1,y,wL,h1);
    ui.edtJobSize->resize(x2,y,wE,h1);
    y += ln;

    ui.lblWindowLog->resize(x1,y,wL,h1);
    ui.edtWindowLog->resize(x2,y,wE,h1);
    y += ln;

    ui.lblLongDistMatching->resize(x1,y,wL,h1);
    ui.edtLongDistMatching->resize(x2,y,wE,h1);
    y += ln;

#if 0
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
#endif

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

