#pragma once
#include <gui/Base.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>
#include <gui/AB/Win11Combo.h>

#include <8z_ArgParser.h>
#include <de/win32/win32_LongPath.h>

/*
Fl_Choice* combo = new Fl_Choice(20, 40, 120, 28);
    combo->add(".tar");
    combo->add(".zst");

    combo->callback([](Fl_Widget* w){
        Fl_Choice* c = (Fl_Choice*)w;
        printf("selected: %s\n", c->mvalue()->label());
    });
*/

namespace EightZip {
namespace builder {

typedef std::function<void()> FN_onOk;

typedef std::function<void()> FN_onCancel;

typedef std::function<void()> FN_onHelp;

// =============================================================
class Dialog : public Fl_Window
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

