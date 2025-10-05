#include "rcm_plugin.hpp"
#include "BaseWidget.hpp"
#include "ModuleTextWidget.hpp"

json_t* TextFieldModule::dataToJson() {
	json_t* rootJ = json_object();
	json_object_set_new(rootJ, "text", json_stringn(text.c_str(), text.size()));
	return rootJ;
}

void TextFieldModule::dataFromJson(json_t* rootJ) {
	json_t* textJ = json_object_get(rootJ, "text");
	if (textJ)
		text = json_string_value(textJ);
	dirty = true;
}

void TextFieldWidget::step() {
	TextField::step();
	if (module && module->dirty) {
		setText(module->text);
		module->dirty = false;
	}
}

void TextFieldWidget::onChange(const ChangeEvent& e) {
	if (module)
		module->text = text;
}

void TextFieldWidget::setModule(TextFieldModule *pmodule) {
	module = pmodule;

	if (module) {
		setText(module->text);
	}
}
