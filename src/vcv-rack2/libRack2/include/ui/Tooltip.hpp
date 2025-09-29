#pragma once
#include <widget/Widget.hpp>
#include <ui/common.hpp>


namespace rack {
namespace ui {


struct RACK_DLL_API Tooltip : widget::Widget {
	std::string text;

	void step() override;
	void draw(const DrawArgs& args) override;
};


} // namespace ui
} // namespace rack
