#pragma once
#include <8z_ArgParser.h>
#include <gui/Widgets.h>

namespace EightZip {
namespace builder {

typedef std::function<void()> FN_onOk;

typedef std::function<void()> FN_onCancel;

typedef std::function<void()> FN_onHelp;

// =============================================================
class Dialog : public Window
// =============================================================
{
public:

    void setCallback_onOk(const FN_onOk& onOk);

    void setCallback_onCancel(const FN_onCancel& onCancel);

    void setCallback_onHelp(const FN_onHelp& onHelp);

    Job getJob() const;

    Dialog(int W, int H, const char* title);

    void resize(int X, int Y, int W, int H) override;
};


} // end namespace builder.
} // end namespace EightZip.

