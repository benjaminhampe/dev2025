#include "rcm_plugin.hpp"
#include "BaseWidget.hpp"
#include "ModuleTextWidget.hpp"

struct CVMmtModule : BaseModule {
	enum ParamIds {
		BUTTON_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		CV_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	TextFieldModule textField;

	CVMmtModule() : BaseModule() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(BUTTON_PARAM, 0.0, 10.0, 0.0);
	}
	void step() override;

	json_t* dataToJson() override {
		json_t* rootJ = BaseModule::dataToJson();
		if (!rootJ) {
			rootJ = json_object();
		}
		json_object_set_new(rootJ, "textfield", textField.dataToJson());
		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		BaseModule::dataFromJson(rootJ);
		json_t* textfieldJ = json_object_get(rootJ, "textfield");
		if (textfieldJ) {
			textField.dataFromJson(textfieldJ);
		}
	}
};


void CVMmtModule::step() {
	outputs[CV_OUTPUT].setChannels(1);
	outputs[CV_OUTPUT].setVoltage(params[BUTTON_PARAM].value);
}

struct PB61303White : SvgSwitch {
	PB61303White() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/PB61303White.svg")));
	}
};

struct CVMmtModuleWidget : BaseWidget {
	TextFieldWidget *textField;

	CVMmtModuleWidget(CVMmtModule *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CVMmt.svg")));

		auto pbswitch = createParam<PB61303White>(Vec(10, 156.23), module, CVMmtModule::BUTTON_PARAM);
		pbswitch->momentary = true;
		addParam(pbswitch);

		addOutput(createOutput<PJ301MPort>(Vec(26, 331), module, CVMmtModule::CV_OUTPUT));

		textField = createWidget<TextFieldWidget>(Vec(7.5, 38.0));
		textField->box.size = Vec(60.0, 80.0);
		textField->multiline = true;
		((LedDisplayTextField*)textField)->color = componentlibrary::SCHEME_WHITE;
		if (module) {
			textField->setModule(&module->textField);
		}
		addChild(textField);

		initColourChange(Rect(Vec(10, 10), Vec(50, 13)), module, 0.754f, 1.f, 0.58f);
	}

};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelCVMmtModule = createModel<CVMmtModule, CVMmtModuleWidget>("rcm-CVMmt");
