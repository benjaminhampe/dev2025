#pragma once
#include <app/common.hpp>
#include <app/Knob.hpp>


namespace rack {
namespace app {


struct RACK_DLL_API SliderKnob : Knob {
	SliderKnob();

	void onHover(const HoverEvent& e) override;
	void onButton(const ButtonEvent& e) override;
};


} // namespace app
} // namespace rack
