#pragma once

#include "bogaudio.hpp"

extern Model* modelTest;

//#define LPF 1
// #define LPFNOISE 1
// #define SINE 1
// #define SQUARE 1
// #define SAW 1
// #define SATSAW 1
// #define TRIANGLE 1
// #define SAMPLED_TRIANGLE 1
// #define SINEBANK 1
// #define OVERSAMPLING 1
// #define OVERSAMPLED_BL 1
// #define ANTIALIASING 1
// #define DECIMATORS 1
// #define INTERPOLATOR 1
// #define FM 1
// #define PM 1
// #define FEEDBACK_PM 1
// #define EG 1
// #define TABLES 1
// #define SLEW 1
// #define RMS 1
// #define FASTRMS 1
// #define RAVG 1
// #define SATURATOR 1
// #define BROWNIAN 1
// #define INTEGRATOR 1
// #define RANDOMWALK 1
// #define DCBLOCKER 1
// #define LFO_SMOOTHER 1
#define STEPPED_RANDOM 1

#include "bogaudio_dsp_pitch.hpp"
#ifdef LPF
#include "bogaudio_dsp_filters_filter.hpp"
#elif LPFNOISE
#include "bogaudio_dsp_filters_filter.hpp"
#include "bogaudio_dsp_noise.hpp"
#elif SINE
#include "bogaudio_dsp_oscillator.hpp"
#elif SQUARE
#include "bogaudio_dsp_oscillator.hpp"
#elif SAW
#include "bogaudio_dsp_oscillator.hpp"
#elif SATSAW
#include "bogaudio_dsp_oscillator.hpp"
#elif TRIANGLE
#include "bogaudio_dsp_oscillator.hpp"
#elif SAMPLED_TRIANGLE
#include "bogaudio_dsp_oscillator.hpp"
#elif SINEBANK
#include "bogaudio_dsp_oscillator.hpp"
#elif OVERSAMPLING
#include "bogaudio_dsp_oscillator.hpp"
#include "bogaudio_dsp_decimator.hpp" // rack
#include "bogaudio_dsp_filters_resample.hpp"
#define OVERSAMPLEN 16
#elif OVERSAMPLED_BL
#include "bogaudio_dsp_oscillator.hpp"
#include "bogaudio_dsp_filters_resample.hpp"
#elif ANTIALIASING
#include "bogaudio_dsp_oscillator.hpp"
#include "bogaudio_dsp_decimator.hpp" // rack
#include "bogaudio_dsp_filters_resample.hpp"
#elif DECIMATORS
#include "bogaudio_dsp_oscillator.hpp"
#include "bogaudio_dsp_filters_resample.hpp"
#include "bogaudio_dsp_decimator.hpp" // rack
#elif INTERPOLATOR
#include "bogaudio_dsp_oscillator.hpp"
#include "bogaudio_dsp_filters_resample.hpp"
#elif FM
#include "bogaudio_dsp_oscillator.hpp"
#elif PM
#include "bogaudio_dsp_oscillator.hpp"
#elif FEEDBACK_PM
#include "bogaudio_dsp_oscillator.hpp"
#elif EG
#include "bogaudio_dsp_envelope.hpp"
#elif TABLES
#include "bogaudio_dsp_oscillator.hpp"
#elif SLEW
#include "bogaudio_dsp_signal.hpp"
#elif RMS
#include "bogaudio_dsp_signal.hpp"
#include "bogaudio_dsp_filters_utility.hpp"
#elif FASTRMS
#include "bogaudio_dsp_signal.hpp"
#include "bogaudio_dsp_filters_utility.hpp"
#elif RAVG
#include "bogaudio_dsp_signal.hpp"
#elif SATURATOR
#include "bogaudio_dsp_oscillator.hpp"
#include "bogaudio_dsp_signal.hpp"
#elif BROWNIAN
#include "bogaudio_dsp_noise.hpp"
#elif INTEGRATOR
#include "bogaudio_dsp_noise.hpp"
#elif RANDOMWALK
#include "bogaudio_dsp_noise.hpp"
#elif DCBLOCKER
#include "bogaudio_dsp_filters_experiments.hpp"
#elif LFO_SMOOTHER
#include "bogaudio_dsp_signal.hpp"
#include "bogaudio_dsp_pitch.hpp"
#include "lfo_base.hpp"
#elif STEPPED_RANDOM
#include "bogaudio_dsp_oscillator.hpp"
#include "bogaudio_dsp_noise.hpp"
#else
#error what
#endif

using namespace bogaudio::dsp;

namespace bogaudio {

struct Test : BGModule {
	enum ParamsIds {
		PARAM1_PARAM,
		PARAM2_PARAM,
		PARAM3_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV1_INPUT,
		CV2_INPUT,
		CV3_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

#ifdef LPF
	LowPassFilter _lpf;
#elif LPFNOISE
	WhiteNoiseGenerator _noise;
	LowPassFilter _lpf;
#elif SINE
	SineOscillator _sine;
	SineTable _table;
	TablePhasor _sine2;
#elif SQUARE
	SquareOscillator _square;
	BandLimitedSquareOscillator _square2;
#elif SAW
	SawOscillator _saw;
	BandLimitedSawOscillator _saw2;
#elif SATSAW
	SaturatingSawOscillator _saw;
	BandLimitedSawOscillator _saw2;
#elif TRIANGLE
	TriangleOscillator _triangle;
#elif SAMPLED_TRIANGLE
	TriangleOscillator _triangle;
	TriangleOscillator _triangle2;
	int _sampleSteps = 1 << 20;
	int _sampleStep = 0;
	float _sample = 0.0f;
#elif SINEBANK
	SineBankOscillator _sineBank;
#elif OVERSAMPLING
	SawOscillator _saw1;
	SawOscillator _saw2;
	LowPassFilter _lpf;
	LowPassFilter _lpf2;
	rack::Decimator<OVERSAMPLEN, OVERSAMPLEN> _rackDecimator;
#elif OVERSAMPLED_BL
	BandLimitedSawOscillator _saw1;
	BandLimitedSawOscillator _saw2;
	LowPassFilter _lpf;
#elif ANTIALIASING
	#define OVERSAMPLEN 8
	Phasor _phasor;
	Phasor _oversampledPhasor;
	BandLimitedSawOscillator _saw;
	BandLimitedSquareOscillator _square;
	bogaudio::dsp::LPFDecimator _sawDecimator;
	bogaudio::dsp::LPFDecimator _squareDecimator;
	rack::Decimator<OVERSAMPLEN, OVERSAMPLEN> _sawRackDecimator;
	rack::Decimator<OVERSAMPLEN, OVERSAMPLEN> _squareRackDecimator;
#elif DECIMATORS
	#define OVERSAMPLEN 8
	#define STAGES 4
	BandLimitedSawOscillator _saw;
	bogaudio::dsp::CICDecimator _cicDecimator;
	bogaudio::dsp::LPFDecimator _lpfDecimator;
	rack::Decimator<OVERSAMPLEN, OVERSAMPLEN> _rackDecimator;
#elif INTERPOLATOR
	#define FACTOR 8
	#define STAGES 4
	BandLimitedSawOscillator _saw;
	bogaudio::dsp::CICDecimator _decimator;
	bogaudio::dsp::CICInterpolator _interpolator;
	int _steps;
	float _rawSamples[FACTOR] {};
	float _processedSamples[FACTOR] {};
#elif FM
	float _baseHz = 0.0f;
	float _ratio = 0.0f;
	float _index = 0.0f;
	float _sampleRate = 0.0f;
	SineTableOscillator _modulator;
	SineTableOscillator _carrier;
	SineTableOscillator _modulator2;
	SineTableOscillator _carrier2;
#elif PM
	SineTableOscillator _modulator;
	SineTableOscillator _carrier;
#elif FEEDBACK_PM
	SineTableOscillator _carrier;
	float _feedbackSample = 0.0f;
#elif EG
	ADSR _envelope;
#elif TABLES
	SineTableOscillator _sine;
	TablePhasor _table;
#elif SLEW
	bogaudio::dsp::SlewLimiter _slew;
	ShapedSlewLimiter _slew2;
#elif RMS
	RootMeanSquare _rms;
	PucketteEnvelopeFollower _pef;
#elif FASTRMS
	PureRootMeanSquare _pure;
	FastRootMeanSquare _fast;
#elif RAVG
	RunningAverage _average;
	Trigger _reset;
#elif SATURATOR
	Saturator _saturator;
#elif BROWNIAN
	WhiteNoiseGenerator _noise1;
	GaussianNoiseGenerator _noise2;
	LowPassFilter _filter1;
	LowPassFilter _filter2;
	float _last1 = 0.0f;
	float _last2 = 0.0f;
#elif INTEGRATOR
	WhiteNoiseGenerator _noise;
	Integrator _integrator;
#elif RANDOMWALK
	RandomWalk _walk1;
	RandomWalk _walk2;
#elif DCBLOCKER
	DCBlocker _filter;
#elif LFO_SMOOTHER
	LFOBase::Smoother _smoother;
#elif STEPPED_RANDOM
	PositiveZeroCrossing _trigger;
	SteppedRandomOscillator _stepped;
	SteppedRandomOscillator::phase_t _lastPhase = 0;
	WhiteNoiseGenerator _noise;
	float _lastNoise = 0.0f;
#endif

	Test()
#if SINE
	: _table(12)
	, _sine2(_table)
#elif DECIMATORS
	: _cicDecimator(STAGES)
#elif INTERPOLATOR
	: _decimator(STAGES)
	, _interpolator(STAGES)
#elif TABLES
	: _table(StaticBlepTable::table(), 44100.0, 1000.0)
#elif RAVG
	: _average(APP->engine->getSampleRate(), 1.0f, 1000.0f)
#endif
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(PARAM1_PARAM, 0.0f, 1.0f, 0.5f, "param1");
		configParam(PARAM2_PARAM, 0.0f, 1.0f, 0.5f, "param2");
		configParam(PARAM3_PARAM, 0.0f, 1.0f, 0.5f, "param3");

#ifdef SINE
		_table.generate();
		_sine2.setPhase(M_PI);

#elif SAW
		_saw2.setPhase(M_PI);

#elif SATSAW
		_saw2.setPhase(M_PI);

#elif SAMPLED_TRIANGLE
		_triangle2.setPhase(M_PI);

#elif SINEBANK
		const float baseAmplitude = 5.0;
		switch (5) {
			case 1: {
				// saw
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, i, baseAmplitude / (float)i);
				}
				_sineBank.syncToPhase(M_PI);
				break;
			}

			case 2: {
				// square
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)i : 0.0);
				}
				break;
			}

			case 3: {
				// triangle
				if (false) {
					for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
						_sineBank.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)(i * i) : 0.0);
					}
					_sineBank.syncToPhase(M_PI / 2.0);
				}
				else {
					_sineBank.setPartial(1, 1.0f, baseAmplitude);
					for (int i = 2, n = _sineBank.partialCount(); i < n; ++i) {
						float k = 2*i - 1;
						_sineBank.setPartial(i, k, powf(-1.0f, k) * baseAmplitude/(k * k));
					}
				}
				break;
			}

			case 4: {
				// saw-square
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, i, i % 2 == 1 ? baseAmplitude / (float)i : baseAmplitude / (float)(2 * i));
				}
				break;
			}

			case 5: {
				// ?
				float factor = 0.717;
				float factor2 = factor;
				float multiple = 1.0;
				for (int i = 1, n = _sineBank.partialCount(); i <= n; ++i) {
					_sineBank.setPartial(i, multiple, baseAmplitude / multiple);
					multiple += i % 2 == 1 ? factor : factor2;
				}
				break;
			}
		}

#elif OVERSAMPLED_BL
		_saw2.setPhase(M_PI);
#endif
	}

	void reset() override;
	void processAll(const ProcessArgs& args) override;
	float oscillatorPitch(float max = 10000.0);
	float oscillatorPitch2(float max = 10000.0);
	float ratio2();
	float index3();
};

} // namespace bogaudio
