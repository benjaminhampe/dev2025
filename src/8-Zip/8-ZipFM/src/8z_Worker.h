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

#include <moodycamel/concurrentqueue.h>

namespace EightZip {
namespace worker {

class Dialog : public Fl_Double_Window
{
public:
    Dialog(int W, int H, const char* title);

    void resize(int X, int Y, int W, int H) override;


};


/*
🟧
struct GuiUpdateItem
{
    float value;
    const char* text;
};
🟧
struct GuiUpdate
{
    std::optional<GuiUpdateItem> slider1;
    std::optional<GuiUpdateItem> slider2;
    std::optional<GuiUpdateItem> slider3;
    std::optional<GuiUpdateItem> label1;
    std::optional<GuiUpdateItem> label2;
    std::optional<GuiUpdateItem> label3;
};

🟧
static constexpr uintptr_t MAGIC_GUI = 0xF1XED123;

🟧
class FixedDispatcher
{
public:
    FixedDispatcher(Fl_Slider* s1, Fl_Slider* s2, Fl_Slider* s3,
                    Fl_Box* l1, Fl_Box* l2, Fl_Box* l3)
        : slider1(s1), slider2(s2), slider3(s3),
          label1(l1), label2(l2), label3(l3)
    {
        front.store(0, std::memory_order_relaxed);
    }

    // Worker: Back-Buffer referenzieren
    GuiUpdate& back() {
        int fi = front.load(std::memory_order_relaxed);
        return buffers[1 - fi];
    }

    // Worker: nach dem Schreiben aufwecken
    void notify() {
        Fl::awake((void*)MAGIC_GUI);
    }

    // Mainthread: Flip + Anwenden
    void dispatch() {
        int oldFront = front.load(std::memory_order_relaxed);
        int newFront = 1 - oldFront;

        front.store(newFront, std::memory_order_release);

        auto& buf = buffers[newFront];
        apply(buf);
        clear(buf);
    }

private:
    std::atomic<int> front;
    GuiUpdate buffers[2];

    Fl_Slider* slider1;
    Fl_Slider* slider2;
    Fl_Slider* slider3;
    Fl_Box*    label1;
    Fl_Box*    label2;
    Fl_Box*    label3;

    void apply(const GuiUpdate& u) {
        if (u.slider1) { slider1->value(u.slider1->value); slider1->redraw(); }
        if (u.slider2) { slider2->value(u.slider2->value); slider2->redraw(); }
        if (u.slider3) { slider3->value(u.slider3->value); slider3->redraw(); }

        if (u.label1) { label1->copy_label(u.label1->text); label1->redraw(); }
        if (u.label2) { label2->copy_label(u.label2->text); label2->redraw(); }
        if (u.label3) { label3->copy_label(u.label3->text); label3->redraw(); }
    }

    void clear(GuiUpdate& u) {
        u.slider1.reset();
        u.slider2.reset();
        u.slider3.reset();
        u.label1.reset();
        u.label2.reset();
        u.label3.reset();
    }
};

🟧Awake‑Handler mit MAGIC‑Routing

FixedDispatcher* g_dispatcher = nullptr;

void awakeHandler(void* payload) {
    uintptr_t magic = (uintptr_t)payload;
    if (magic == MAGIC_GUI && g_dispatcher)
        g_dispatcher->dispatch();
}

🟧Anwendung im Worker‑Thread

void workerThread(FixedDispatcher* d)
{
    while (true) {
        GuiUpdate& b = d->back();

        b.slider1 = GuiUpdateItem{0.25f, nullptr};
        b.slider2 = GuiUpdateItem{0.75f, nullptr};
        b.label1  = GuiUpdateItem{0.0f, "Hallo Benjamin"};
        b.label2  = GuiUpdateItem{0.0f, "Neuer Wert"};
        b.label3  = GuiUpdateItem{0.0f, "Noch ein Wert"};

        d->notify();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

🟧Main‑Setup

int main() {
    Fl_Window* win = new Fl_Window(400, 300);

    Fl_Slider* s1 = new Fl_Slider(50, 50, 300, 30);
    Fl_Slider* s2 = new Fl_Slider(50, 100, 300, 30);
    Fl_Slider* s3 = new Fl_Slider(50, 150, 300, 30);

    Fl_Box* l1 = new Fl_Box(50, 200, 300, 30, "Label 1");
    Fl_Box* l2 = new Fl_Box(50, 240, 300, 30, "Label 2");
    Fl_Box* l3 = new Fl_Box(50, 280, 300, 30, "Label 3");

    FixedDispatcher dispatcher(s1, s2, s3, l1, l2, l3);
    g_dispatcher = &dispatcher;

    Fl::add_awake_handler(awakeHandler);

    std::thread worker(workerThread, &dispatcher);
    worker.detach();

    win->end();
    win->show();

    return Fl::run();
}
*/

} // end namespace worker.
} // end namespace EightZip.
