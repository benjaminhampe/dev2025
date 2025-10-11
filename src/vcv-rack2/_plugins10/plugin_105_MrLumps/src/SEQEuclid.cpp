#include "a_plugin.hpp"

#include <string>
#include <memory>
#include "erBitData.hpp"


struct SEQEuclid : Module {
	enum ParamIds {
		BPM_PARAM,
		RESET_BUTTON,
		GATE_LENGTH_PARAM,
		PROB1_PARAM,
		PROB2_PARAM,
		PROB3_PARAM,
		PROB4_PARAM,
		FILL1_PARAM,
		FILL2_PARAM,
		FILL3_PARAM,
		FILL4_PARAM,
		LENGTH1_PARAM,
		LENGTH2_PARAM,
		LENGTH3_PARAM,
		LENGTH4_PARAM,
		JOG1_BUTTON,
		JOG2_BUTTON,
		JOG3_BUTTON,
		JOG4_BUTTON,
		NUM_PARAMS
	};
	enum InputIds {
		EXT_CLOCK_INPUT,
		RESET_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		GATE_OR_OUTPUT,
		TRIGGER_OR_OUTPUT,
		GATE1_OUTPUT,
		GATE2_OUTPUT,
		GATE3_OUTPUT,
		GATE4_OUTPUT,
		TRIGGER1_OUTPUT,
		TRIGGER2_OUTPUT,
		TRIGGER3_OUTPUT,
		TRIGGER4_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		GATES_LIGHT,
		NUM_LIGHTS
	};

	// LCG see numerical recipies and wikipedia
	// The std random engine seems inapropreate for this application
	// due to it's construction you'd need to fool with creating / destroying
	// distribution fuctions in the audio hot path, which seems unwise,
	struct Lcg {
		uint_fast32_t seed;

		explicit Lcg() {
			Lcg(738);
		}

		inline explicit Lcg(uint_fast32_t seed) : seed(seed) {
			int32();
		}

		inline uint_fast32_t int32() {
			return seed = (seed >> 1) ^ (-(signed int)(seed & 1u) & 0xD0000001u);
		}

		// float 0 - 1
		inline float flt()    { return static_cast<float>(2.32830643653869629E-10 * int32()); }
		inline uint8_t int8() { return static_cast<uint8_t>(int32()); }
		inline bool bit()     { return static_cast<bool>(int32() & (1 << 0)); }
	};

	// For accessing the patern data fill must be < length and > 0
	// if fill is >= length just output 1's
	// if fill is 0 output nothing
	struct Bank {
		int fill;
		int length;
		int currentStep;
		bool coinFlip;
		bool noteOn;
		dsp::SchmittTrigger jogTrigger;
		dsp::PulseGenerator gate;
		Lcg rng;


		Bank() {
			Reset();
		}

		void Reset() {
			fill = 0;
			length = 0;
			currentStep = 0;
			coinFlip = false;
			noteOn = false;
			//gate.pulseTime = 0.0f;
			//gate.time = 0.0f;
			gate.reset();
			rng.seed = 738;
		}

		// Given the current step, fill, length members and the given probablility
		// Is the note on or off?
		// If on set the gate
		void SetNote(const float p, const float glength) {
			noteOn = false;

			if (fill > 0) {
				// Flip coin
				if (p < 0.999f) {
					if (rng.flt() <= 1.0f - p) {
						coinFlip = true;
					}
				}
				// Normal operations
				if (coinFlip == false) {

					if (fill < length) {
						const patternBucket pattern_ref(&(bit_pattern_table[((fill * (SEQUENCE_MAX + 1)) + length)]));
						if (pattern_ref[currentStep]) {
							gate.trigger(glength);
							noteOn = true;
						}
					}  else if (coinFlip == false) {  // if fill > length output a gate
						gate.trigger(glength);
						noteOn = true;
					}
				}
			}
		}

		void AdvanceStep() {
			coinFlip = false;  // Make sure we clear any old random note off events
			currentStep++;
			if (currentStep + 1 > length) {
				currentStep = 0;
			}
		}
	};

	bool running = true;
	dsp::SchmittTrigger clockTrigger;  // for external clock
	dsp::SchmittTrigger resetTrigger;  // reset button

	Bank bank1;
	Bank bank2;
	Bank bank3;
	Bank bank4;

	double time = 0.0;
	double dTime = 1.0 / static_cast<double>(APP->engine->getSampleRate());
	int bpm = 120;
	double timerLength = 1.0 / (static_cast<double>(bpm) / 60.0);
	double timerTime = timerLength;
	int contrast = 0; // module context menu option, high contrast = 1

	SEQEuclid() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(BPM_PARAM, 30.0f, 256.0f, 120.0f, "BPM");
		getParamQuantity(BPM_PARAM)->snapEnabled = true;
		configParam(GATE_LENGTH_PARAM, 0.0f, 1.0f, 0.5f, "Gate length", "%", 0.f, 100.f);  // v2 default changed to 50% probability
		configParam(PROB1_PARAM, 0.0f, 1.0f, 1.0f, "CH1 Probability of sending beat", "%", 0.f, 100.f);  // v1 default changed to 100% probability
		configParam(PROB2_PARAM, 0.0f, 1.0f, 1.0f, "CH2 Probability of sending beat", "%", 0.f, 100.f);
		configParam(PROB3_PARAM, 0.0f, 1.0f, 1.0f, "CH3 Probability of sending beat", "%", 0.f, 100.f);
		configParam(PROB4_PARAM, 0.0f, 1.0f, 1.0f, "CH4 Probability of sending beat", "%", 0.f, 100.f);
		configParam(FILL1_PARAM, 0.0f, 256.0f, 0.0f, "CH1 Pattern fill", " beats");
		configParam(FILL2_PARAM, 0.0f, 256.0f, 0.0f, "CH2 Pattern fill", " beats");
		configParam(FILL3_PARAM, 0.0f, 256.0f, 0.0f, "CH3 Pattern fill", " beats");
		configParam(FILL4_PARAM, 0.0f, 256.0f, 0.0f, "CH4 Pattern fill", " beats");
		getParamQuantity(FILL1_PARAM)->snapEnabled = true;
		getParamQuantity(FILL2_PARAM)->snapEnabled = true;
		getParamQuantity(FILL3_PARAM)->snapEnabled = true;
		getParamQuantity(FILL4_PARAM)->snapEnabled = true;
		configParam(LENGTH1_PARAM, 0.0f, 256.0f, 0.0f, "CH1 Pattern length", " beats");
		configParam(LENGTH2_PARAM, 0.0f, 256.0f, 0.0f, "CH2 Pattern length", " beats");
		configParam(LENGTH3_PARAM, 0.0f, 256.0f, 0.0f, "CH3 Pattern length", " beats");
		configParam(LENGTH4_PARAM, 0.0f, 256.0f, 0.0f, "CH4 Pattern length", " beats");
		getParamQuantity(LENGTH1_PARAM)->snapEnabled = true;
		getParamQuantity(LENGTH2_PARAM)->snapEnabled = true;
		getParamQuantity(LENGTH3_PARAM)->snapEnabled = true;
		getParamQuantity(LENGTH4_PARAM)->snapEnabled = true;

		configButton(RESET_BUTTON, "Reset counters button");
		configButton(JOG1_BUTTON, "CH1 jog pattern forward one step button for offset patterns");
		configButton(JOG2_BUTTON, "CH2 jog pattern forward one step button for offset patterns");
		configButton(JOG3_BUTTON, "CH3 jog pattern forward one step button for offset patterns");
		configButton(JOG4_BUTTON, "CH4 jog pattern forward one step button for offset patterns");

		configLight(GATES_LIGHT, "Blinky gate duration");

		configInput(EXT_CLOCK_INPUT, "External clock trigger");
		configInput(RESET_INPUT, "Reset counters trigger");

		configOutput(GATE1_OUTPUT, "CH1 gate");
		configOutput(TRIGGER1_OUTPUT, "CH1 trigger");
		configOutput(GATE1_OUTPUT + 1, "CH2 gate");
		configOutput(TRIGGER1_OUTPUT + 1, "CH2 trigger");
		configOutput(GATE1_OUTPUT + 2, "CH3 gate");
		configOutput(TRIGGER1_OUTPUT + 2, "CH3 trigger");
		configOutput(GATE1_OUTPUT + 3, "CH4 gate");
		configOutput(TRIGGER1_OUTPUT + 3, "CH4 trigger");
		configOutput(GATE_OR_OUTPUT, "Summed gates from CH1 through CH4");
		configOutput(TRIGGER_OR_OUTPUT, "Summed triggers from CH1 through CH4");

	}

	void process(const ProcessArgs& args) override {
		//const float lightLambda = 0.075f;
		bool nextStep = false;

		// Do clock stuff
		if (running) {
			time += dTime;

			if (inputs[EXT_CLOCK_INPUT].isConnected()) {

				if (clockTrigger.process(inputs[EXT_CLOCK_INPUT].getVoltage())) {
					nextStep = true;
				}

			}  else {

				timerLength = 1.0 / (static_cast<double>(bpm) / 60.0);
				timerTime -= dTime;

				if (dTime > timerTime) {
					timerTime = 0.0;
				}

				if (timerTime <= 0.0) {
					nextStep = true;
					timerTime = timerLength;
				}

			}

			bank1.gate.process(dTime);
			bank2.gate.process(dTime);
			bank3.gate.process(dTime);
			bank4.gate.process(dTime);
		}

		// Deal with inputs and button presses

		// BPM param
		bpm = floor(params[BPM_PARAM].getValue());

		// Reset inputs
		if (resetTrigger.process(params[RESET_BUTTON].getValue() + inputs[RESET_INPUT].getVoltage())) {
			bank1.currentStep = 0;
			bank1.gate.reset();

			bank2.currentStep = 0;
			bank2.gate.reset();

			bank3.currentStep = 0;
			bank3.gate.reset();

			bank4.currentStep = 0;
			bank4.gate.reset();
		}

		// Bank controls

		// Fill and length
		bank1.fill   = floor(params[FILL1_PARAM].getValue());
		bank1.length = floor(params[LENGTH1_PARAM].getValue());

		bank2.fill   = floor(params[FILL2_PARAM].getValue());
		bank2.length = floor(params[LENGTH2_PARAM].getValue());

		bank3.fill   = floor(params[FILL3_PARAM].getValue());
		bank3.length = floor(params[LENGTH3_PARAM].getValue());

		bank4.fill   = floor(params[FILL4_PARAM].getValue());
		bank4.length = floor(params[LENGTH4_PARAM].getValue());

		// Jog button
		if (bank1.jogTrigger.process(params[JOG1_BUTTON].getValue())) {
			bank1.AdvanceStep();
		}
		if (bank2.jogTrigger.process(params[JOG2_BUTTON].getValue())) {
			bank2.AdvanceStep();
		}
		if (bank3.jogTrigger.process(params[JOG3_BUTTON].getValue())) {
			bank3.AdvanceStep();
		}
		if (bank4.jogTrigger.process(params[JOG4_BUTTON].getValue())) {
			bank4.AdvanceStep();
		}

		// Advance step
		if (nextStep) {
			bank1.AdvanceStep();
			bank2.AdvanceStep();
			bank3.AdvanceStep();
			bank4.AdvanceStep();
		}

		// Generate output

		// See if our notes are on this step
		if (nextStep) {
			bank1.SetNote(params[PROB1_PARAM].getValue(),
						timerLength * params[GATE_LENGTH_PARAM].getValue());
			bank2.SetNote(params[PROB2_PARAM].getValue(),
						timerLength * params[GATE_LENGTH_PARAM].getValue());
			bank3.SetNote(params[PROB3_PARAM].getValue(),
						timerLength * params[GATE_LENGTH_PARAM].getValue());
			bank4.SetNote(params[PROB4_PARAM].getValue(),
						timerLength * params[GATE_LENGTH_PARAM].getValue());
		}

		// Set output high if there's a note currently latched on
		// gate.process(0.0f) to get the current state without advancing time
		const float gate1 = (bank1.gate.process(0.0f)) ? 10.0f : 0.0f;
		const float gate2 = (bank2.gate.process(0.0f)) ? 10.0f : 0.0f;
		const float gate3 = (bank3.gate.process(0.0f)) ? 10.0f : 0.0f;
		const float gate4 = (bank4.gate.process(0.0f)) ? 10.0f : 0.0f;

		// blast out a trigger for new events
		const float trigger1 = (bank1.noteOn && nextStep) ? 10.0f : 0.0f;
		const float trigger2 = (bank2.noteOn && nextStep) ? 10.0f : 0.0f;
		const float trigger3 = (bank3.noteOn && nextStep) ? 10.0f : 0.0f;
		const float trigger4 = (bank4.noteOn && nextStep) ? 10.0f : 0.0f;

		// Setup summed outputs
		const float gateOr = (gate1 || gate2 || gate3 || gate4) ? 10.0f : 0.0f;
		const float triggerOr = (trigger1 || trigger2 || trigger3 || trigger4) ? 10.0f : 0.0f;

		// Send outputs out
		lights[GATES_LIGHT].setBrightness((gateOr >= 1.0f) ? 1.0f : 0.0f);

		outputs[GATE1_OUTPUT].setVoltage(gate1);
		outputs[GATE2_OUTPUT].setVoltage(gate2);
		outputs[GATE3_OUTPUT].setVoltage(gate3);
		outputs[GATE4_OUTPUT].setVoltage(gate4);

		outputs[TRIGGER1_OUTPUT].setVoltage(trigger1);
		outputs[TRIGGER2_OUTPUT].setVoltage(trigger2);
		outputs[TRIGGER3_OUTPUT].setVoltage(trigger3);
		outputs[TRIGGER4_OUTPUT].setVoltage(trigger4);

		outputs[GATE_OR_OUTPUT].setVoltage(gateOr);
		outputs[TRIGGER_OR_OUTPUT].setVoltage(triggerOr);
	}

	json_t *dataToJson() override {
		json_t *rootJ = json_object();
		json_t *contrastJ = json_integer((int)contrast);
		json_object_set_new(rootJ, "contrast", contrastJ);  // save contrast setting  
		return rootJ;
	}

	void dataFromJson(json_t *rootJ) override {
		json_t *contrastJ = json_object_get(rootJ, "contrast");
		if (contrastJ) {
			contrast = json_integer_value(contrastJ);  // retrieve contrast setting		
		}  
	}

	// Update dTime for SampleRateChange from Rack toolbar
	void onSampleRateChange() override {
		dTime = 1.0f / static_cast<double>(APP->engine->getSampleRate());
	}

	// Called via menu
	void onReset() override {
		time = 0.0;
		//dTime = 1.0 / static_cast<double>(args.sampleRate);
		dTime = 1.0 / static_cast<double>(APP->engine->getSampleRate());
		bpm = 120;
		timerLength = 1.0 / (static_cast<double>(bpm) / 60.0);
		timerTime = timerLength;

		bank1.Reset();
		bank2.Reset();
		bank3.Reset();
		bank4.Reset();
	}
};


struct SEQEuclidDisplayWidget : TransparentWidget {
	SEQEuclid *module;  // make sure we can see module-level variables, including contrast, from draw()
	int *value = NULL;

	SEQEuclidDisplayWidget() {
		//Don't store Font and Image references across multiple frames
	}

	void draw(const DrawArgs& args) override {
		if (!value) {
			return; // don't blow up the Plugin Manager
					// https://github.com/AScustomWorks/AS/blob/master/src/DelayPlusStereo.cpp
			}
		// LED Background
		int contrast = module ? module->contrast : 0;
		if (!contrast) {
			NVGcolor backgroundColor = nvgRGB(0x74, 0x44, 0x44);
			NVGcolor borderColor = nvgRGB(0x10, 0x10, 0x10);
			nvgBeginPath(args.vg);
			nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 5.0);
			nvgFillColor(args.vg, backgroundColor);
			nvgFill(args.vg);
			nvgStrokeWidth(args.vg, 1.0);
			nvgStrokeColor(args.vg, borderColor);
			nvgStroke(args.vg);
		}
	}

	void drawLayer(const DrawArgs& args, int layer) override {
		if (!value) {
			return; // don't blow up the Plugin Manager
			}
		if (layer == 1) {
			int contrast = module ? module->contrast : 0;

			//LCD backlight
			if (contrast) {
				NVGcolor backgroundColor = nvgRGB(0xc0, 0xc0, 0xc0);  // high contrast light background color
				NVGcolor borderColor = nvgRGB(0x17, 0x17, 0x17);  // high contrast dark border color

				nvgBeginPath(args.vg);
				nvgRoundedRect(args.vg, 0.0, 0.0, box.size.x, box.size.y, 5.0);
				nvgFillColor(args.vg, backgroundColor);
				nvgFill(args.vg);
				nvgStrokeWidth(args.vg, 3.0);  // high contrast thicker border
				nvgStrokeColor(args.vg, borderColor);
				nvgStroke(args.vg);
			}

			// Text
			std::shared_ptr<Font> font = APP->window->loadFont(asset::plugin(pluginInstance, "res/Segment7Standard.ttf"));
			if (font) {
				
				nvgFontSize(args.vg, 36);
				nvgFontFaceId(args.vg, font->handle);
				nvgTextLetterSpacing(args.vg, 2.5);

				std::string to_display = std::to_string(*value);
				Vec textPos = Vec(7.0f, 35.0f);

				NVGcolor textColor = contrast ? nvgRGB(0xc0, 0xc0, 0xc0) : nvgRGB(0xdf, 0xd2, 0x2c);
				nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
				nvgText(args.vg, textPos.x, textPos.y, "~~~", NULL);

				textColor = contrast ? nvgRGB(0xc0, 0xc0, 0xc0) : nvgRGB(0xda, 0xe9, 0x29);
				nvgFillColor(args.vg, nvgTransRGBA(textColor, 16));
				nvgText(args.vg, textPos.x, textPos.y, "\\\\\\", NULL);

				textColor = contrast ? nvgRGB(0x00, 0x00, 0x00) : nvgRGB(0xf0, 0x00, 0x00);
				nvgFillColor(args.vg, textColor);
				nvgText(args.vg, textPos.x, textPos.y, to_display.c_str(), NULL);
			}
		}
		Widget::drawLayer(args, layer);
	}
};

// Define and create module's context menu, options section, and High Contrast item
// Default is contrast = 0 for red digital panels
// High Contrast is contrast = 1 for black-on-white digital panels
// High Contrast option added to module to assist users reporting problems due to color blindness and vision issues

struct SEQEuclidSettingItem : MenuItem {  // define checkmark toggle for context menu
	uint8_t *setting = NULL;
	uint8_t offValue = 0;
	uint8_t onValue = 1;
	void onAction(const event::Action &e) override {
		// Toggle setting
		*setting = (*setting == onValue) ? offValue : onValue;
	}
	void step() override {
		rightText = (*setting == onValue) ? "✔" : "";
		MenuItem::step();
	}
};

struct SEQEuclidHighContrastItem : MenuItem {  // define High Contrast menu item
	SEQEuclid *seqeuclid;
	void onAction(const event::Action &e) override {
		seqeuclid->contrast = !seqeuclid->contrast;
	}
	void step() override {
		rightText = (seqeuclid->contrast) ? "✔" : "";
		MenuItem::step();
	}
};

struct SEQEuclidWidget : ModuleWidget {
	SEQEuclidWidget(SEQEuclid* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/SEQEuclid.svg")));

    	// Make sure it stays put

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		const float bankX[10] = { 8, 94, 134, 220, 258, 296, 324, 351 };
		const float bankY[7] = { 23, 72, 110, 164, 218, 272, 326 };

		// bpm display + control

		{
			SEQEuclidDisplayWidget *display = new SEQEuclidDisplayWidget();
			display->box.pos = Vec(bankX[0], bankY[0]);
			display->box.size = Vec(82, 42);
			if (module) {
				display->value = &module->bpm;
				display->module = module; // pass access to module-level variables, including contrast
			}
			addChild(display);
		}
		addParam(createParam<Davies1900hBlackKnob>(Vec(bankX[1], bankY[0]+3), module, SEQEuclid::BPM_PARAM));


		// Next row of stuff

		addInput(createInput<PJ301MPort>(Vec(bankX[0], bankY[1]), module, SEQEuclid::EXT_CLOCK_INPUT));
		addInput(createInput<PJ301MPort>(Vec(bankX[1], bankY[1]), module, SEQEuclid::RESET_INPUT));
		addParam(createParam<TL1105>(Vec(bankX[1]+24+4, bankY[1] + 4), module, SEQEuclid::RESET_BUTTON));
		addParam(createParam<Davies1900hBlackKnob>(Vec(bankX[5]-6, bankY[1] + 3), module, SEQEuclid::GATE_LENGTH_PARAM));

		// Row displays

		{
			SEQEuclidDisplayWidget *display = new SEQEuclidDisplayWidget();
			display->box.pos = Vec(bankX[0], bankY[2]);
			display->box.size = Vec(82, 42);
			if (module) {
				display->value = &module->bank1.fill;
				display->module = module; // pass access to module-level variables, including contrast
			}
			addChild(display);
		}
		{
			SEQEuclidDisplayWidget *display = new SEQEuclidDisplayWidget();
			display->box.pos = Vec(bankX[2], bankY[2]);
			display->box.size = Vec(82, 42);
			if (module) {
				display->value = &module->bank1.length;
				display->module = module; // pass access to module-level variables, including contrast
			}
			addChild(display);
		}

		{
			SEQEuclidDisplayWidget *display = new SEQEuclidDisplayWidget();
			display->box.pos = Vec(bankX[0], bankY[3]);
			display->box.size = Vec(82, 42);
			if (module) {
				display->value = &module->bank2.fill;
				display->module = module; // pass access to module-level variables, including contrast
			}
			addChild(display);
		}
		{
			SEQEuclidDisplayWidget *display = new SEQEuclidDisplayWidget();
			display->box.pos = Vec(bankX[2], bankY[3]);
			display->box.size = Vec(82, 42);
			if (module) {
				display->value = &module->bank2.length;
				display->module = module; // pass access to module-level variables, including contrast
			}
			addChild(display);
		}
		{
			SEQEuclidDisplayWidget *display = new SEQEuclidDisplayWidget();
			display->box.pos = Vec(bankX[0], bankY[4]);
			display->box.size = Vec(82, 42);
			if (module) {
				display->value = &module->bank3.fill;
				display->module = module; // pass access to module-level variables, including contrast
			}
			addChild(display);
		}
		{
			SEQEuclidDisplayWidget *display = new SEQEuclidDisplayWidget();
			display->box.pos = Vec(bankX[2], bankY[4]);
			display->box.size = Vec(82, 42);
			if (module) {
				display->value = &module->bank3.length;
				display->module = module; // pass access to module-level variables, including contrast
			}
			addChild(display);
		}
		{
			SEQEuclidDisplayWidget *display = new SEQEuclidDisplayWidget();
			display->box.pos = Vec(bankX[0], bankY[5]);
			display->box.size = Vec(82, 42);
			if (module) {
				display->value = &module->bank4.fill;
				display->module = module; // pass access to module-level variables, including contrast
			}
			addChild(display);
		}
		{
			SEQEuclidDisplayWidget *display = new SEQEuclidDisplayWidget();
			display->box.pos = Vec(bankX[2], bankY[5]);
			display->box.size = Vec(82, 42);
			if (module) {
				display->value = &module->bank4.length;
				display->module = module; // pass access to module-level variables, including contrast
			}
			addChild(display);
		}

		// Rows of bank controls

		for (int row = 0; row < 4; row++) {
			addParam(createParam<Davies1900hBlackKnob>(Vec(bankX[1], bankY[row + 2] + 3), module, SEQEuclid::FILL1_PARAM+row));
			addParam(createParam<Davies1900hBlackKnob>(Vec(bankX[3], bankY[row + 2] + 3), module, SEQEuclid::LENGTH1_PARAM+row));
			addParam(createParam<Davies1900hBlackKnob>(Vec(bankX[4], bankY[row + 2] + 3), module, SEQEuclid::PROB1_PARAM + row));
			addOutput(createOutput<PJ301MPort>(Vec(bankX[5], bankY[row + 2] + 9), module, SEQEuclid::GATE1_OUTPUT + row));
			addOutput(createOutput<PJ301MPort>(Vec(bankX[6], bankY[row + 2] + 9), module, SEQEuclid::TRIGGER1_OUTPUT + row));
			addParam(createParam<TL1105>(Vec(bankX[7], bankY[row + 2] + 13), module, SEQEuclid::JOG1_BUTTON + row));
		}

		// Final 2 outputs and output light

		addOutput(createOutput<PJ301MPort>(Vec(bankX[5], bankY[6] + 8), module, SEQEuclid::GATE_OR_OUTPUT));
		addOutput(createOutput<PJ301MPort>(Vec(bankX[6], bankY[6] + 8), module, SEQEuclid::TRIGGER_OR_OUTPUT));
		addChild(createLight<SmallLight<RedLight>>(Vec(bankX[7]+4, bankY[6] + 16), module, SEQEuclid::GATES_LIGHT));

	}

	void appendContextMenu(Menu *menu) override {
		SEQEuclid *seqeuclid = dynamic_cast<SEQEuclid*>(module);
		assert(seqeuclid);

		menu->addChild(construct<MenuLabel>());
		menu->addChild(construct<MenuLabel>(&MenuLabel::text, "Options"));	// add options section to menu
		menu->addChild(construct<SEQEuclidHighContrastItem>(&MenuItem::text, "High Contrast", &SEQEuclidHighContrastItem::seqeuclid, seqeuclid));
	}
};

Model* modelSEQEuclid = createModel<SEQEuclid, SEQEuclidWidget>("SEQE");