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

// ======================================================
// 🟧
// ======================================================
class Dialog : public Fl_Double_Window
{
public:
    Dialog(int W, int H, const char* title);

    void resize(int X, int Y, int W, int H) override;
};

} // end namespace worker.
} // end namespace EightZip.
