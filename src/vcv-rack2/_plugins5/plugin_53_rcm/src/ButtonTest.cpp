#include "rcm_plugin.hpp"
#include "BaseWidget.hpp"

using namespace std;
using namespace rack;


struct ButtonTest : BaseModule {
	enum ParamIds {
		SELECT_PRESET,
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};



	ButtonTest() : BaseModule() {
	}
	void step() override;

	int preset_num;
	
	json_t *dataToJson() override
	{
		return BaseModule::dataToJson();
	}
	void dataFromJson(json_t *root) override
	{
		BaseModule::dataFromJson(root);
	}

	// For more advanced Module features, read Rack's engine.hpp header file
	// - dataToJson, dataFromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

/////////step //////////////////////
void ButtonTest::step() 
{

};

struct ButtonTestWidget : BaseWidget
{
	TextField *textField;
	ButtonTestWidget(Module *module) : BaseWidget()
	{
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CV0to10.svg")));
	}
};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelButtonTest = createModel<ButtonTest, ButtonTestWidget>("rcm-ButtonTest");