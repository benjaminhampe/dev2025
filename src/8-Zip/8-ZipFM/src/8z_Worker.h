#pragma once
#include <gui/Widgets.h>
#include <8z_ArgParser.h>

// #include <gui/Base.h>
// #include <FL/Fl.H>
// #include <FL/Fl_Choice.H>
// #include <FL/Fl_Box.H>
// #include <gui/XP_ProgressBar.h>
// #include <gui/LogBox.h>

// #include <8z_ArgParser.h>
// #include <de/Core.h>

namespace EightZip {
namespace worker {

// ======================================================
// 🟧
// ======================================================
class Worker : public DoubleWindow
{
public:
    Worker(const Job& job, int W, int H, const char* title);

    void resize(int X, int Y, int W, int H) override;
};

} // end namespace worker.
} // end namespace EightZip.
