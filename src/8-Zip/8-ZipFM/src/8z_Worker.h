#pragma once
#include <gui/Widgets.h>
#include <8z_ArgParser.h> // Job

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
