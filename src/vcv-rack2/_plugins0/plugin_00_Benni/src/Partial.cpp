#include "plugin.hpp"
#include <cmath>
#include <vector>

using namespace rack;

struct PartialController : Module {
    enum ParamIds {
        PARTIAL_PARAM,
        AMPLITUDE_PARAM,
        DETUNE_PARAM,
        LFO_AMPLITUDE_PARAM,
        LFO_DETUNE_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        VOCT_INPUT,
        AMPLITUDE_CV_INPUT,
        DETUNE_CV_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        SINE_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    std::vector<float> phases;

    PartialController() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(PARTIAL_PARAM, 1.f, 32.f, 1.f, "Partial Number");
        configParam(AMPLITUDE_PARAM, 0.f, 1.f, 0.5f, "Amplitude");
        configParam(DETUNE_PARAM, -100.f, 100.f, 0.f, "Detune (cent)");
        configParam(LFO_AMPLITUDE_PARAM, 0.f, 1.f, 0.f, "LFO Amplitude Depth");
        configParam(LFO_DETUNE_PARAM, 0.f, 100.f, 0.f, "LFO Detune Depth");
    }

    void process(const ProcessArgs& args) override
    {
        int channels = inputs[VOCT_INPUT].getChannels();
        outputs[SINE_OUTPUT].setChannels(channels);

        if ((int)phases.size() != channels)
            phases.resize(channels, 0.f);

        for (int ch = 0; ch < channels; ch++) {
            float sampleRate = args.sampleRate;

            // V/Oct
            float voct = inputs[VOCT_INPUT].getVoltage(ch);
            float baseFreq = 440.f * std::pow(2.f, voct - 4.f);

            // Partial
            int partial = (int)params[PARTIAL_PARAM].getValue();
            float freq = baseFreq * partial;

            // Detune
            float detuneCents = params[DETUNE_PARAM].getValue();
            if (inputs[DETUNE_CV_INPUT].isConnected())
                detuneCents += inputs[DETUNE_CV_INPUT].getVoltage(ch) * params[LFO_DETUNE_PARAM].getValue();
            freq *= std::pow(2.f, detuneCents / 1200.f);

            // Amplitude
            float amplitude = params[AMPLITUDE_PARAM].getValue();
            if (inputs[AMPLITUDE_CV_INPUT].isConnected())
                amplitude += inputs[AMPLITUDE_CV_INPUT].getVoltage(ch) * params[LFO_AMPLITUDE_PARAM].getValue();
            amplitude = clamp(amplitude, 0.f, 1.f);

            // Sinusgenerator
            phases[ch] += freq / sampleRate;
            if (phases[ch] >= 1.f)
                phases[ch] -= 1.f;
            float sine = std::sin(2.f * M_PI * phases[ch]);

            outputs[SINE_OUTPUT].setVoltage(5.f * sine * amplitude, ch);
        }
    }
};


struct PartialControllerWidget : ModuleWidget
{
    PartialControllerWidget(PartialController* module)
    {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Partial-panel.svg"),
                             asset::plugin(pluginInstance, "res/Partial-panel-dark.svg")));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 20)), module, PartialController::PARTIAL_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 40)), module, PartialController::AMPLITUDE_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 60)), module, PartialController::DETUNE_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 80)), module, PartialController::LFO_AMPLITUDE_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10, 100)), module, PartialController::LFO_DETUNE_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30, 20)), module, PartialController::VOCT_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30, 40)), module, PartialController::AMPLITUDE_CV_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30, 60)), module, PartialController::DETUNE_CV_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30, 80)), module, PartialController::SINE_OUTPUT));
    }

    void appendContextMenu(Menu* menu) override
    {
        PartialController* module = dynamic_cast<PartialController*>(this->module);
        assert(module);

        menu->addChild(new MenuSeparator());

        struct ContextMenuTextEdit : public ui::TextField
        {
            PartialController* m_module;

            ContextMenuTextEdit( PartialController* module )
                : m_module( module )
            {

            }

            void onChange(const ChangeEvent& event) override
            {
                try
                {
                    float val = std::stof(text);
                    val = clamp(val, 0.f, 1.f);
                    m_module->params[PartialController::AMPLITUDE_PARAM].setValue(val);
                }
                catch (...)
                {

                }
            };
        };

        struct AmplitudeEditItem : MenuItem
        {
            ContextMenuTextEdit* m_edtAmplitude;
            PartialController* module;

            AmplitudeEditItem(PartialController* m)
            {
                module = m;
                text = "Set Amplitude:";
            }

            Menu* createChildMenu() override
            {
                Menu* menu = new Menu;

                m_edtAmplitude = new ContextMenuTextEdit( module );
                m_edtAmplitude->box.size = Vec(60, 20);
                m_edtAmplitude->text = string::f("%.3f", module->params[PartialController::AMPLITUDE_PARAM].getValue());

                // textField->onChange = [=](const ChangeEvent& event)
                // {
                //     try {
                //         float val = std::stof(textField->text);
                //         val = clamp(val, 0.f, 1.f);
                //         module->params[PartialController::AMPLITUDE_PARAM].setValue(val);
                //     } catch (...) {}
                // };

                menu->addChild(m_edtAmplitude);
                return menu;
            }
        };

        menu->addChild(new AmplitudeEditItem(module));
    }
};


Model* modelPartial = createModel<PartialController, PartialControllerWidget>("Partial");



#ifdef FLOAT_8_SIMD_AVX2

void process(const ProcessArgs& args) override {
    int channels = inputs[VOCT_INPUT].getChannels();
    outputs[SINE_OUTPUT].setChannels(channels);

    if ((int)phases.size() != channels)
        phases.resize(channels, 0.f);

    float sampleRate = args.sampleRate;
    int partial = (int)params[PARTIAL_PARAM].getValue();
    float detuneParam = params[DETUNE_PARAM].getValue();
    float detuneDepth = params[LFO_DETUNE_PARAM].getValue();
    float ampParam = params[AMPLITUDE_PARAM].getValue();
    float ampDepth = params[LFO_AMPLITUDE_PARAM].getValue();

    for (int ch = 0; ch < channels; ch += 8) {
        int block = std::min(8, channels - ch);

        alignas(32) float voct[8] = {};
        alignas(32) float ampCV[8] = {};
        alignas(32) float detuneCV[8] = {};
        alignas(32) float phaseIn[8] = {};

        for (int i = 0; i < block; i++) {
            voct[i] = inputs[VOCT_INPUT].getVoltage(ch + i);
            ampCV[i] = inputs[AMPLITUDE_CV_INPUT].isConnected() ? inputs[AMPLITUDE_CV_INPUT].getVoltage(ch + i) : 0.f;
            detuneCV[i] = inputs[DETUNE_CV_INPUT].isConnected() ? inputs[DETUNE_CV_INPUT].getVoltage(ch + i) : 0.f;
            phaseIn[i] = phases[ch + i];
        }

        float8 voctVec(voct);
        float8 ampCVVec(ampCV);
        float8 detuneCVVec(detuneCV);
        float8 phaseVec(phaseIn);

        float8 baseFreq = float8(440.f) * pow(float8(2.f), voctVec - float8(4.f));
        float8 freq = baseFreq * float8((float)partial);

        float8 detune = float8(detuneParam) + detuneCVVec * float8(detuneDepth);
        freq *= pow(float8(2.f), detune / float8(1200.f));

        phaseVec += freq / float8(sampleRate);
        phaseVec -= phaseVec.floor(); // wrap to [0,1)

        float8 angle = phaseVec * float8(2.f * M_PI);
        float8 sine = angle.sin();

        float8 amp = float8(ampParam) + ampCVVec * float8(ampDepth);
        amp = amp.clamp(float8(0.f), float8(1.f));

        float8 out = sine * amp * float8(5.f);

        alignas(32) float result[8], newPhase[8];
        out.store(result);
        phaseVec.store(newPhase);

        for (int i = 0; i < block; i++) {
            outputs[SINE_OUTPUT].setVoltage(result[i], ch + i);
            phases[ch + i] = newPhase[i];
        }
    }
}









#endif






#if 0

#include <immintrin.h>

void process(const ProcessArgs& args) override {
    int channels = inputs[VOCT_INPUT].getChannels();
    outputs[SINE_OUTPUT].setChannels(channels);

    if ((int)phases.size() != channels)
        phases.resize(channels, 0.f);

    float sampleRate = args.sampleRate;
    int partial = (int)params[PARTIAL_PARAM].getValue();
    float detuneParam = params[DETUNE_PARAM].getValue();
    float detuneDepth = params[LFO_DETUNE_PARAM].getValue();
    float ampParam = params[AMPLITUDE_PARAM].getValue();
    float ampDepth = params[LFO_AMPLITUDE_PARAM].getValue();

    // AVX2 Verarbeitung in 8er-Blöcken
    for (int ch = 0; ch < channels; ch += 8) {
        int block = std::min(8, channels - ch);

        // Lade V/Oct
        float voct[8], ampCV[8], detuneCV[8], phase[8];
        for (int i = 0; i < block; i++) {
            voct[i] = inputs[VOCT_INPUT].getVoltage(ch + i);
            ampCV[i] = inputs[AMPLITUDE_CV_INPUT].isConnected() ? inputs[AMPLITUDE_CV_INPUT].getVoltage(ch + i) : 0.f;
            detuneCV[i] = inputs[DETUNE_CV_INPUT].isConnected() ? inputs[DETUNE_CV_INPUT].getVoltage(ch + i) : 0.f;
            phase[i] = phases[ch + i];
        }

        __m256 voctVec = _mm256_loadu_ps(voct);
        __m256 baseFreq = _mm256_mul_ps(_mm256_set1_ps(440.f), _mm256_pow_ps(_mm256_set1_ps(2.f), _mm256_sub_ps(voctVec, _mm256_set1_ps(4.f))));
        __m256 freq = _mm256_mul_ps(baseFreq, _mm256_set1_ps((float)partial));

        __m256 detune = _mm256_add_ps(_mm256_set1_ps(detuneParam), _mm256_mul_ps(_mm256_loadu_ps(detuneCV), _mm256_set1_ps(detuneDepth)));
        __m256 detuneFactor = _mm256_pow_ps(_mm256_set1_ps(2.f), _mm256_div_ps(detune, _mm256_set1_ps(1200.f)));
        freq = _mm256_mul_ps(freq, detuneFactor);

        __m256 delta = _mm256_div_ps(freq, _mm256_set1_ps(sampleRate));
        __m256 phaseVec = _mm256_add_ps(_mm256_loadu_ps(phase), delta);
        phaseVec = _mm256_sub_ps(phaseVec, _mm256_floor_ps(phaseVec)); // wrap

        __m256 angle = _mm256_mul_ps(phaseVec, _mm256_set1_ps(2.f * M_PI));
        __m256 sine = sin_approx_avx(angle); // eigene Approximation oder Sleef

        __m256 amp = _mm256_add_ps(_mm256_set1_ps(ampParam), _mm256_mul_ps(_mm256_loadu_ps(ampCV), _mm256_set1_ps(ampDepth)));
        amp = _mm256_max_ps(_mm256_set1_ps(0.f), _mm256_min_ps(amp, _mm256_set1_ps(1.f)));

        __m256 out = _mm256_mul_ps(sine, amp);
        out = _mm256_mul_ps(out, _mm256_set1_ps(5.f));

        float result[8], newPhase[8];
        _mm256_storeu_ps(result, out);
        _mm256_storeu_ps(newPhase, phaseVec);

        for (int i = 0; i < block; i++) {
            outputs[SINE_OUTPUT].setVoltage(result[i], ch + i);
            phases[ch + i] = newPhase[i];
        }
    }
}

#endif
