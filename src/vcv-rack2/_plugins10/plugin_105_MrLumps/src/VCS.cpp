#include "a_plugin.hpp"


//1x8 Voltage Controlled Switch (VCS1)
struct VCS1 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		TRIGGER_INPUT,
		SIGNAL_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		SWITCHED_OUTPUT,
		NUM_OUTPUTS = SWITCHED_OUTPUT + 8
	};
	enum LightIds {
		OUTPUT_LIGHTS,
		NUM_LIGHTS = OUTPUT_LIGHTS + 8
	};

	dsp::SchmittTrigger inputTrigger;
	int currentSwitch;

	VCS1() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configLight(OUTPUT_LIGHTS, "OUT 1");
		configLight(OUTPUT_LIGHTS + 1, "OUT 2");
		configLight(OUTPUT_LIGHTS + 2, "OUT 3");
		configLight(OUTPUT_LIGHTS + 3, "OUT 4");
		configLight(OUTPUT_LIGHTS + 4, "OUT 5");
		configLight(OUTPUT_LIGHTS + 5, "OUT 6");
		configLight(OUTPUT_LIGHTS + 6, "OUT 7");
		configLight(OUTPUT_LIGHTS + 7, "OUT 8");

		configInput(TRIGGER_INPUT, "Trigger");
		configInput(SIGNAL_INPUT, "Signal");

		configOutput(SWITCHED_OUTPUT, "OUT 1");
		configOutput(SWITCHED_OUTPUT + 1, "OUT 2");
		configOutput(SWITCHED_OUTPUT + 2, "OUT 3");
		configOutput(SWITCHED_OUTPUT + 3, "OUT 4");
		configOutput(SWITCHED_OUTPUT + 4, "OUT 5");
		configOutput(SWITCHED_OUTPUT + 5, "OUT 6");
		configOutput(SWITCHED_OUTPUT + 6, "OUT 7");
		configOutput(SWITCHED_OUTPUT + 7, "OUT 8");

		currentSwitch = 0;
		lights[OUTPUT_LIGHTS + currentSwitch].setBrightness(10.0f);
	}

	//Lets try simplest 1st
	//Just start at 0 and walk around the outputs on triggers
	void process(const ProcessArgs& args) override {

		if (inputs[TRIGGER_INPUT].isConnected()) {
			// External clock
			if (inputTrigger.process(inputs[TRIGGER_INPUT].getVoltage())) {
				//Switch ports to the next active port

				//Search forward from the current switch
				for (int i = currentSwitch+1; i < NUM_OUTPUTS; i++) {
					if (outputs[i].isConnected()) {
						lights[OUTPUT_LIGHTS + currentSwitch].setBrightness(0.0f);
						outputs[currentSwitch].setVoltage(0.0f);
						lights[OUTPUT_LIGHTS + i].setBrightness(10.0f);
						currentSwitch = i;
						goto OUTPUT;
					}
				}
				//Else wrap around
				for (int i = 0; i < currentSwitch; i++) {
					if (outputs[i].isConnected()) {
						lights[OUTPUT_LIGHTS + currentSwitch].setBrightness(0.0f);
						outputs[currentSwitch].setVoltage(0.0f);
						lights[OUTPUT_LIGHTS + i].setBrightness(10.0f);
						currentSwitch = i;
						goto OUTPUT;
					}
				}
				//Okay we have nothing to switch to
			}
		}

	OUTPUT:

		//We have signal so send output
		if (inputs[SIGNAL_INPUT].isConnected()) {
			if (outputs[currentSwitch].isConnected()) {
				outputs[currentSwitch].setVoltage(inputs[SIGNAL_INPUT].getVoltage());
			}
		}

	}

	// Called via menu
	void onReset() override {
		for (int c = 0; c < 8; c++) {
			lights[OUTPUT_LIGHTS + c].setBrightness(0.0f);
			outputs[c].setVoltage(0.0f);
		}
		currentSwitch = 0;
		lights[OUTPUT_LIGHTS + currentSwitch].setBrightness(10.0f);
	}
};


struct VCS1Widget : ModuleWidget {
	VCS1Widget(VCS1* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCS1x8.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		const float bankX[8] = { 4, 31, 4, 31, 4, 31, 4, 31 };
		const float bankY[8] = { 112, 112, 179, 179, 246, 246, 313, 313 };

		//Trigger input
		addInput(createInput<PJ3410Port>(Vec(29, 23), module, VCS1::TRIGGER_INPUT));

		//Signal input
		addInput(createInput<PJ3410Port>(Vec(29, 57), module, VCS1::SIGNAL_INPUT));

		//Switched ouputs + lights
		for (int outputs = 0; outputs < 8; outputs++) {
			addChild(createLight<SmallLight<RedLight>>(Vec(bankX[outputs] + 9, bankY[outputs] - 12), module, VCS1::OUTPUT_LIGHTS + outputs));
			addOutput(createOutput<PJ301MPort>(Vec(bankX[outputs], bankY[outputs]), module, VCS1::SWITCHED_OUTPUT + outputs));
		}
	}
};


//2x4 Voltage Controlled Switch (VCS2)
struct VCS2 : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		TRIGGER_INPUT,
		SIGNAL_INPUT_L,
		SIGNAL_INPUT_R,
		NUM_INPUTS
	};
	enum OutputIds {
		SWITCHED_OUTPUT_L,
		SWITCHED_OUTPUT_L2,
		SWITCHED_OUTPUT_L3,
		SWITCHED_OUTPUT_L4,
		SWITCHED_OUTPUT_R,
		SWITCHED_OUTPUT_R2,
		SWITCHED_OUTPUT_R3,
		SWITCHED_OUTPUT_R4,
		NUM_OUTPUTS
	};
	enum LightIds {
		OUTPUT_LIGHTS,
		NUM_LIGHTS = OUTPUT_LIGHTS + 8
	};

	dsp::SchmittTrigger inputTrigger;
	int currentSwitchL;
	int currentSwitchR;

	VCS2() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

		configLight(OUTPUT_LIGHTS, "LEFT OUT 1");
		configLight(OUTPUT_LIGHTS + 1, "LEFT OUT 2");
		configLight(OUTPUT_LIGHTS + 2, "LEFT OUT 3");
		configLight(OUTPUT_LIGHTS + 3, "LEFT OUT 4");
		configLight(OUTPUT_LIGHTS + 4, "RIGHT OUT 1");
		configLight(OUTPUT_LIGHTS + 5, "RIGHT OUT 2");
		configLight(OUTPUT_LIGHTS + 6, "RIGHT OUT 3");
		configLight(OUTPUT_LIGHTS + 7, "RIGHT OUT 4");

		configInput(TRIGGER_INPUT, "Trigger");
		configInput(SIGNAL_INPUT_L, "Left signal");
		configInput(SIGNAL_INPUT_R, "Right signal");

		configOutput(SWITCHED_OUTPUT_L, "LEFT OUT 1");
		configOutput(SWITCHED_OUTPUT_L + 1, "LEFT OUT 2");
		configOutput(SWITCHED_OUTPUT_L + 2, "LEFT OUT 3");
		configOutput(SWITCHED_OUTPUT_L + 3, "LEFT OUT 4");
		configOutput(SWITCHED_OUTPUT_R, "RIGHT OUT 1");
		configOutput(SWITCHED_OUTPUT_R + 1, "RIGHT OUT 2");
		configOutput(SWITCHED_OUTPUT_R + 2, "RIGHT OUT 3");
		configOutput(SWITCHED_OUTPUT_R + 3, "RIGHT OUT 4");

		currentSwitchL = 0;
		currentSwitchR = SWITCHED_OUTPUT_R;
		lights[OUTPUT_LIGHTS + currentSwitchL].setBrightness(10.0f);
		lights[OUTPUT_LIGHTS + currentSwitchR].setBrightness(10.0f);	
	}

	void process(const ProcessArgs& args) override {

		if (inputs[TRIGGER_INPUT].isConnected()) {
			// External clock
			if (inputTrigger.process(inputs[TRIGGER_INPUT].getVoltage())) {
				//Switch ports to the next active port

				// L Bank
				{
					//Search forward from the current switch
					for (int i = currentSwitchL + 1; i < NUM_OUTPUTS/2; i++) {
						if (outputs[i].isConnected()) {
							lights[OUTPUT_LIGHTS + currentSwitchL].setBrightness(0.0f);
							outputs[currentSwitchL].setVoltage(0.0f);
							lights[OUTPUT_LIGHTS + i].setBrightness(10.0f);
							currentSwitchL = i;
							goto NEXTSIDE;
						}
					}
					//Else wrap around
					for (int i = SWITCHED_OUTPUT_L; i < currentSwitchL; i++) {
						if (outputs[i].isConnected()) {
							lights[OUTPUT_LIGHTS + currentSwitchL].setBrightness(0.0f);
							outputs[currentSwitchL].setVoltage(0.0f);
							lights[OUTPUT_LIGHTS + i].setBrightness(10.0f);
							currentSwitchL = i;
							goto NEXTSIDE;
						}
					}
					//Okay we have nothing to switch to, next bank
				}
			NEXTSIDE:
				// R Bank
				{
					//Search forward from the current switch
					for (int i = currentSwitchR + 1; i < NUM_OUTPUTS; i++) {
						if (outputs[i].isConnected()) {
							lights[OUTPUT_LIGHTS + currentSwitchR].setBrightness(0.0f);
							outputs[currentSwitchR].setVoltage(0.0f);
							lights[OUTPUT_LIGHTS + i].setBrightness(10.0f);
							currentSwitchR = i;
							goto OUTPUT;
						}
					}
					//Else wrap around
					for (int i = SWITCHED_OUTPUT_R; i < currentSwitchR; i++) {
						if (outputs[i].isConnected()) {
							lights[OUTPUT_LIGHTS + currentSwitchR].setBrightness(0.0f);
							outputs[currentSwitchR].setVoltage(0.0f);
							lights[OUTPUT_LIGHTS + i].setBrightness(10.0f);
							currentSwitchR = i;
							goto OUTPUT;
						}
					}
					//Okay we have nothing to switch to
				}
			}
		}

	OUTPUT:

		//We have signal so send output
		if (inputs[SIGNAL_INPUT_L].isConnected()) {
			if (outputs[currentSwitchL].isConnected()) {
				outputs[currentSwitchL].setVoltage(inputs[SIGNAL_INPUT_L].getVoltage());
			}
		}

		if (inputs[SIGNAL_INPUT_R].isConnected()) {
			if (outputs[currentSwitchR].isConnected()) {
				outputs[currentSwitchR].setVoltage(inputs[SIGNAL_INPUT_R].getVoltage());
			}
		}

	}

	// Called via menu
	void onReset() override {
		for (int c = 0; c < 4; c++) {
			lights[OUTPUT_LIGHTS + c].setBrightness(0.0f);   // Left
			lights[OUTPUT_LIGHTS + c+4].setBrightness(0.0f); // Right	
		}
		outputs[currentSwitchL].setVoltage(0.0f);
		outputs[currentSwitchR].setVoltage(0.0f);
		currentSwitchL = 0;
		currentSwitchR = SWITCHED_OUTPUT_R;
		lights[OUTPUT_LIGHTS + currentSwitchL].setBrightness(10.0f);
		lights[OUTPUT_LIGHTS + currentSwitchR].setBrightness(10.0f);	
	}
};

struct VCS2Widget : ModuleWidget {
	VCS2Widget(VCS2* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/VCS2x4.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		const float bankX[2] = { 4, 31 };
		const float bankY[4] = { 112, 179, 246, 313 };

		//Trigger input
		addInput(createInput<PJ3410Port>(Vec(29, 23), module, VCS2::TRIGGER_INPUT));

		//Signal inputs
		addInput(createInput<PJ3410Port>(Vec(0, 52), module, VCS2::SIGNAL_INPUT_L));
		addInput(createInput<PJ3410Port>(Vec(29, 52), module, VCS2::SIGNAL_INPUT_R));

		//Switched ouputs + lights
		for (int outputs = 0; outputs < 4; outputs++) {
			addChild(createLight<SmallLight<RedLight>>(Vec(bankX[0] + 9, bankY[outputs] - 12), module, VCS2::OUTPUT_LIGHTS + outputs));
			addOutput(createOutput<PJ301MPort>(Vec(bankX[0], bankY[outputs]), module, VCS2::SWITCHED_OUTPUT_L + outputs));

			addChild(createLight<SmallLight<RedLight>>(Vec(bankX[1] + 9, bankY[outputs] - 12), module, VCS2::OUTPUT_LIGHTS + outputs+4));
			addOutput(createOutput<PJ301MPort>(Vec(bankX[1], bankY[outputs]), module, VCS2::SWITCHED_OUTPUT_R + outputs));
		}
	}
};

Model* modelVCS1 = createModel<VCS1, VCS1Widget>("VCS1");
Model* modelVCS2 = createModel<VCS2, VCS2Widget>("VCS2");