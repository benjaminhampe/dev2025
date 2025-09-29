#pragma once
#include <ui/common.hpp>
#include <ui/MenuEntry.hpp>


namespace rack {
namespace ui {


struct RACK_DLL_API MenuSeparator : MenuEntry {
	MenuSeparator();
	void draw(const DrawArgs& args) override;
};


} // namespace ui
} // namespace rack
