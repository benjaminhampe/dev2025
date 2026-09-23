#include "8z_Builder.h"
#include "8z_App.h"
#include <gui/AB/Win11Combo.h>
#include <gui/ComboBox.h>
#include <de/archive/ZstHeader.h>
#include <filesystem>

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
    Fl_Box* lblAlgorithm = nullptr;
    ComboBox* cbxAlgorithm = nullptr;

    Fl_Box* lblDictSize = nullptr;
    ComboBox* cbxDictSize = nullptr;

    Fl_Box* lblWordSize = nullptr;
    ComboBox* cbxWordSize = nullptr;

    Fl_Box* lblBlockSize = nullptr;
    ComboBox* cbxBlockSize = nullptr;

    Fl_Box* lblCpuThreads = nullptr;
    ComboBox* cbxCpuThreads = nullptr;

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


static void cbxFormat_cb(Fl_Widget* widget, void* data)
{
    // Das generische Widget in ein Fl_Choice-Objekt umwandeln
    auto self = (Fl_Choice*)widget;

    int index = self->value();

    // Den Text des ausgewählten Elements abrufen
    const char* label = self->text();

    ui.produceFileName();

/*
    ui.edtFile->value(s.c_str());

    if (ext == "tar")
    {
        ui.iLastPreset = ui.cbxPreset->value();
        ui.cbxPreset->value(0);
        ui.cbxPreset->deactivate();
    }
    else if (ext == "zst")
    {
        ui.cbxPreset->activate();
        ui.cbxPreset->value(ui.iLastPreset);
    }
*/
}

static void cbxPreset_cb(Fl_Widget* widget, void* data)
{
    // Das generische Widget in ein Fl_Choice-Objekt umwandeln
    auto self = (Fl_Choice*)widget;

    int index = self->value();

    // Den Text des ausgewählten Elements abrufen
    const char* label = self->text();

    DE_DEBUG("[Preset] "
                "index(",index,"), "
                "label(", (label ? label : "nullptr"), "), "
                "algo(", ui.selectedPresetZst().algo,"), "
                "level(", ui.selectedPresetZst().level,")")
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

    // ui.lblQuality = new Label(x,y,mw,h1,"Compress-Quality:");
    // ui.cbxQuality = new ComboBox(x,y,mw,h1);

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
    const auto & zstPresets = ZstPresets::get();
    for (size_t i = 0; i < zstPresets.size(); ++i)
    {
        ui.cbxPreset->add(zstPresets[i].name.c_str());
    }
    ui.cbxPreset->value(10);
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
    ui.edtFile->resize(x+100,y+h1,mw-200,h1);
    ui.btnChoose->resize(x+mw - 50,y+h1,50,h1);
    y += ln + ln;

    // Body Column[0]
    ui.lblFormat->resize(x,   y,w4,h1);
    ui.cbxFormat->resize(x+w4,y,w4,h1);
    y += ln;

    ui.lblPreset->resize(x,   y,w4,h1);
    ui.cbxPreset->resize(x+w4,y,w4,h1);
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

