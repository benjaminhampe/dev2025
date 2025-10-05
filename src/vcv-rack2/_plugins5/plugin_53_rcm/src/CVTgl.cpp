#include "rcm_plugin.hpp"
#include "BaseWidget.hpp"
#include "ModuleTextWidget.hpp"

struct CVTglModule : BaseModule {
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

	CVTglModule() : BaseModule() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configParam(CVTglModule::BUTTON_PARAM, 0.0, 1.0, 0.0);
	}

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

	void step() override;

};

void CVTglModule::step() {
	outputs[CV_OUTPUT].setChannels(1);
	outputs[CV_OUTPUT].setVoltage(params[BUTTON_PARAM].value * 10.f);
}

struct CKSSWhite : SvgSwitch {
	CKSSWhite() {
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSS_0_White.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CKSS_1_White.svg")));
	}
};

struct CVTglModuleWidget : BaseWidget {
	TextFieldWidget *textField;

	CVTglModuleWidget(CVTglModule *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CVTgl.svg")));

		addParam(createParam<CKSSWhite>(Vec(31, 172), module, CVTglModule::BUTTON_PARAM));

		addOutput(createOutput<PJ301MPort>(Vec(26, 331), module, CVTglModule::CV_OUTPUT));

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
Model *modelCVTglModule = createModel<CVTglModule, CVTglModuleWidget>("rcm-CVTgl");
