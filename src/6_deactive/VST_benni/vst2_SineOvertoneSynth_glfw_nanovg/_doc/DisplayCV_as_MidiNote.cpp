#include "plugin.hpp"
using namespace rack;

#ifdef _MSC_VER
#define ALIGNED_FOR_X64 __declspec(align(8))
#define ALIGNED_FOR_SSE __declspec(align(16))
#define ALIGNED_FOR_AVX __declspec(align(32))
#else
#define ALIGNED_FOR_X64 alignas(8)
#define ALIGNED_FOR_SSE alignas(16)
#define ALIGNED_FOR_AVX alignas(32)
#endif

struct DispMidiNote : Module {
    enum ParamIds {
        // OFFSET_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        CV_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        // PITCH_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    ALIGNED_FOR_X64 float cvValues[16];
    ALIGNED_FOR_X64 float cvHistory[16];
    ALIGNED_FOR_X64 int activeChannel;

    DispMidiNote() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        //configParam(OFFSET_PARAM, -1.f, 1.f, 0.f, "Pre-offset", " semitones", 0.f, 12.f);
        configInput(CV_INPUT, "1V/octave pitch");
        //configOutput(PITCH_OUTPUT, "Pitch");
        //configBypass(PITCH_INPUT, PITCH_OUTPUT);

        std::memset(cvValues, 0, sizeof(cvValues));
        std::memset(cvHistory, 0, sizeof(cvHistory));
    }

    void process(const ProcessArgs& args) override
    {
        int channels = inputs[CV_INPUT].getChannels();

        // Reset
        // activeChannel = -1;
        std::memcpy(cvHistory, cvValues, sizeof(cvValues)); // save previous frame
        std::memset(cvValues, 0, sizeof(cvValues));

        // Set
        for (int i = 0; i < channels; i++)
        {
            cvValues[i] = inputs[CV_INPUT].getVoltage(i);
        }

        constexpr const float EPSILON = 10.0f * std::numeric_limits<float>::epsilon();

        // Determine
        int l_activeChannel = channels-1;
        for (int i = channels-1; i >= 0; i--)
        {
            if (cvValues[i] != 0.0f
                && std::abs(cvValues[i] - cvHistory[i]) > EPSILON)
            {
                l_activeChannel = i; // this channel changed
                break;
            }
        }

        activeChannel = l_activeChannel;
    }

    json_t* dataToJson() override {
        json_t* rootJ = json_object();
    /*
        json_t* enabledNotesJ = json_array();
        for (int i = 0; i < 12; i++)
        {
            json_array_insert_new(enabledNotesJ, i, json_boolean(enabledNotes[i]));
        }
        json_object_set_new(rootJ, "enabledNotes", enabledNotesJ);
    */
        return rootJ;
    }

    void dataFromJson(json_t* rootJ) override
    {
        /*
        json_t* enabledNotesJ = json_object_get(rootJ, "enabledNotes");

        if (enabledNotesJ) {
            for (int i = 0; i < 12; i++) {
                json_t* enabledNoteJ = json_array_get(enabledNotesJ, i);
                if (enabledNoteJ)
                    enabledNotes[i] = json_boolean_value(enabledNoteJ);
            }
        }
        updateRanges();
        */
    }
};


struct DispMidiNote_Widget : ModuleWidget
{
    std::string fontPath;
    std::shared_ptr<Font> font;

    DispMidiNote_Widget(DispMidiNote* module)
    {
        fontPath = asset::system("res/fonts/ShareTechMono-Regular.ttf");

        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Panel.svg"),
                             asset::plugin(pluginInstance, "res/Panel-dark.svg")));

        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ThemedScrew>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ThemedScrew>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        // addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.62, 80.551)), module, Quantizer::OFFSET_PARAM));

        addInput(createInputCentered<ThemedPJ301MPort>(mm2px(Vec(7.62, 96.859)), module, DispMidiNote::CV_INPUT));

        // addOutput(createOutputCentered<ThemedPJ301MPort>(mm2px(Vec(7.62, 113.115)), module, Quantizer::PITCH_OUTPUT));

        // QuantizerDisplay* quantizerDisplay = createWidget<QuantizerDisplay>(mm2px(Vec(0.0, 13.039)));
        // quantizerDisplay->box.size = mm2px(Vec(15.24, 55.88));
        // quantizerDisplay->setModule(module);
        // addChild(quantizerDisplay);
    }

    void appendContextMenu(Menu* menu) override
    {
        DispMidiNote* module = getModule<DispMidiNote>();

        menu->addChild(new MenuSeparator);

        menu->addChild(createMenuItem("Shift notes up", "", [=]() {
            // module->rotateNotes(1);
        }));
        menu->addChild(createMenuItem("Shift notes down", "", [=]() {
            // module->rotateNotes(-1);
        }));
    }

    struct DecomposeCV_Result
    {
        int midiNote;
        int octave;
        int semitone;
        float centDetune;

        const char* noteStr() const
        {
            static const char* noteNames[12] = {
                "C", "C♯", "D", "D♯", "E", "F", "F♯", "G", "G♯", "A", "A♯", "B"
            };

            return noteNames[(semitone % 12)];
        }
    };

    static DecomposeCV_Result decomposeCV(const float cv)
    {
        // Convert CV to MIDI note (floating point)
        float midiFloat = cv * 12.0f;

        // Round to nearest MIDI note
        int midiNote = static_cast<int>(std::round(midiFloat));

        // Extract octave (can be negative)
        int octave = midiNote / 12;

        int semitone = midiNote - (octave * 12);
        while (semitone < 0)
        {
            semitone += 12;
        }

        // Cent detuning: difference between actual pitch and rounded MIDI note
        float centDetune = (midiFloat - midiNote) * 100.0f;

        return { midiNote, octave, semitone, centDetune };
    }

    void draw(const DrawArgs& args) override
    {
        ModuleWidget::draw(args);

        //RK_FATAL("\n");
        if (!font)
        {
            font = APP->window->loadFont(fontPath);
        }

        nvgFontFaceId(args.vg, font->handle);
        nvgTextAlign(args.vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        //nvgTextLetterSpacing(args.vg, -1);

        nvgFontSize(args.vg, 16);
        nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 0xFF));

        const auto module = dynamic_cast<const DispMidiNote*>(this->module);

        if (module && module->inputs.size() > DispMidiNote::CV_INPUT)
        {
            const int channels = module->inputs[DispMidiNote::CV_INPUT].getChannels();
            std::ostringstream o;

            float x = 3;
            float y = 20;
            if (module->activeChannel > -1 && module->activeChannel < channels)
            {
                float cv = module->cvValues[module->activeChannel];
                auto ds = decomposeCV(cv);

                o.str("");
                o << ds.noteStr() << ds.octave;

                nvgText(args.vg, x, y, o.str().c_str(), NULL);
            }
            y += 30;

            nvgFontSize(args.vg, 8);
            nvgFillColor(args.vg, nvgRGBA(0, 0, 0, 0xFF));

            for (int i = 0; i < channels; ++i)
            {
                float cv = module->cvValues[i];
                auto ds = decomposeCV(cv);

                o.str("");
                o << ds.noteStr() << ds.octave;
                nvgText(args.vg, x, y, o.str().c_str(), NULL);
                y += 12;
            }
        }

    }
};


Model* modelDisplayCV_as_MidiNote = createModel<DispMidiNote, DispMidiNote_Widget>("DisplayCV_as_MidiNote");
