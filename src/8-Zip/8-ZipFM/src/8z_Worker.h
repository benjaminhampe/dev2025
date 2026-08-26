#pragma once
#include <gui/Base.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Box.H>
#include <gui/XP_ProgressBar.h>
#include <gui/LogBox.h>

#include <8z_ArgParser.h>

#include <de/Core.h>

namespace EightZip {
namespace worker {

struct UI
{
    Job job;

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

    Fl_Box* edtModus = nullptr;  // Hinzufügen|Entpacken:
    Fl_Box* edtDir = nullptr;   // Current DirectoryName
    Fl_Box* edtFile = nullptr;  // Current FileName

    XP_ProgressBar* progressBar = nullptr;

    LogBox* logBox = nullptr;

    Fl_Button* btnBackground = nullptr;
    Fl_Button* btnPause = nullptr;
    Fl_Button* btnCancel = nullptr;

    std::atomic<bool> bRunFlag{false};
    std::atomic<bool> bCancelFlag{false};
    std::atomic<bool> bPauseFlag{false};
    // std::atomic<bool> reloadFile{true};

    std::thread worker;
};

static UI ui;


class Dialog : public Fl_Window
{
public:
    Dialog(int W, int H, const char* title);

    void resize(int X, int Y, int W, int H) override;
};


} // end namespace worker.
} // end namespace EightZip.
