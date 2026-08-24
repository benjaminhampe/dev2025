#pragma once
#include "Base.h"

class DynamicLayout : public Fl_Group {
public:
    DynamicLayout(int X, int Y, int W, int H)
        : Fl_Group(X, Y, W, H)
    {
        end(); // wichtig
    }

    void resize(int X, int Y, int W, int H) override {
        Fl_Group::resize(X, Y, W, H);

        // Layout-Regeln
        child(0)->resize(10, 10, W - 20, 30);
        child(1)->resize(10, 50, W - 20, 30);
        child(2)->resize(10, 90, W - 20, H - 100);
    }
};
