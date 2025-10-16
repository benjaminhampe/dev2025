#pragma once
#include <widget/OpaqueWidget.hpp>
#include <rack_quantity.hpp>
#include <ui/common.hpp>
#include <rack_context.hpp>


namespace rack {
namespace ui {


struct RACK_DLL_API Slider : widget::OpaqueWidget {
    /** Not owned. */
    Quantity* quantity = NULL;

    Slider();
    void draw(const DrawArgs& args) override;
    void onDragStart(const DragStartEvent& e) override;
    void onDragMove(const DragMoveEvent& e) override;
    void onDragEnd(const DragEndEvent& e) override;
    void onDoubleClick(const DoubleClickEvent& e) override;
};

struct RACK_DLL_API LEDLightSliderHorizontal : Slider {
    LEDLightSliderHorizontal() {
        box.size = math::Vec(120, 20); // horizontal dimensions
    }

    void draw(const DrawArgs& args) override {
        // Background
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillColor(args.vg, nvgRGB(30, 30, 30));
        nvgFill(args.vg);

        if (quantity)
        {
            float min = quantity->getMinValue();
            float max = quantity->getMaxValue();
            float val = quantity->getValue();

            float knobX = math::rescale(val, min, max, 0.0f, box.size.x);

            // Knob
            nvgBeginPath(args.vg);
            nvgCircle(args.vg, knobX, box.size.y / 2.0f, 6.0f);
            nvgFillColor(args.vg, nvgRGB(0, 255, 0)); // LED green
            nvgFill(args.vg);
        }
    }
};




} // namespace ui
} // namespace rack
