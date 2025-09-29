#pragma once
#include <ui/common.hpp>
#include <ui/Button.hpp>


namespace rack {
namespace ui {


/** Button with a dropdown icon on its right.
*/
struct RACK_DLL_API ChoiceButton : Button {
	void draw(const DrawArgs& args) override;
};


} // namespace ui
} // namespace rack
