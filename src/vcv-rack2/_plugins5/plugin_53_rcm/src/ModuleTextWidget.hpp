#pragma once

struct TextFieldModule {
	std::string text;
  bool dirty;

	json_t* dataToJson();
	void dataFromJson(json_t* rootJ);
};

struct TextFieldWidget : LedDisplayTextField {
	TextFieldModule *module = nullptr;

	void step() override;
	void onChange(const ChangeEvent& e) override;
	void setModule(TextFieldModule *module);
};
