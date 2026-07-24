#include "Resample_signalsmith.h"

#if 0

#include <signalsmith-stretch/signalsmith-stretch.h>

namespace de {
namespace sound {

#if 0
    timeFactor = args.flag<double>("time", "Time-scaling factor (e.g. 2 is twice as slow)", timeFactor);
        }
        bool fixedPhase = false, purePV = false, stretchPhase = false, singleTimeObservation = false;
        double pitchWeight = 1, timeWeight = 2, channelWeight = 1, maxWeight = 1;
        double zeroPadding = 2;
        double gain = 1;
        if (mode != Mode::resample) {
            if (mode == Mode::paulStretch) blockMs = 120;
            if (mode == Mode::phaseVocoder) blockMs = 120;
            if (mode == Mode::hybridPhase) blockMs = 120;
            blockMs = args.flag<double>("block", "Block length (ms)", blockMs);
            if (mode == Mode::overlapAdd) overlapFactor = 1.5;
            if (mode == Mode::wsola) overlapFactor = 2;
            overlapFactor = args.flag<double>("overlap", "Overlap factor for blocks", overlapFactor);
            if (mode == Mode::wsola) searchMs = args.flag<double>("search", "WSOLA search duration (ms)", searchMs);
            if (mode == Mode::spectralCut) fixedPhase = args.hasFlag("fixed-phase", "Don't phase-shift segments to match the previous block");
            if (mode == Mode::phaseVocoder) {
                purePV = args.hasFlag("pure", "Pure phase-vocoder (no transient/energy stuff)");
            }
            if (mode == Mode::phaseVocoder || mode == Mode::vasePhocoder) {
                gain = args.flag<double>("gain", "Gain factor (amplitude), default 1", 1);
            }
            if (mode == Mode::vasePhocoder) stretchPhase = args.hasFlag("stretch-phase", "Stretch phase (trigonmetrically) instead of using longer strides");
            if (mode == Mode::spectralCut || mode == Mode::phaseVocoder || mode == Mode::paulStretch || mode == Mode::vasePhocoder || mode == Mode::hybridPhase) {
                zeroPadding = args.flag<double>("zero-padding", "Zero-padding factor for spectral processing", zeroPadding);
            }
            if (mode == Mode::hybridPhase) {
                pitchWeight = args.flag<double>("pitch-weight", "Weighting for pitch-based phase information", pitchWeight);
                timeWeight = args.flag<double>("time-weight", "Weighting for timing-based phase information", timeWeight);
                channelWeight = args.flag<double>("channel-weight", "Weighting for inter-channel phase information", channelWeight);
                maxWeight = args.flag<double>("max-weight", "Additional weighting for the strongest phase predictor", maxWeight);
                if (mode == Mode::hybridPhase) {
                    singleTimeObservation = args.hasFlag("single-vertical", "Use a single vertical (timing) observation, instead of combining multiple");
                }
            }
            trimLatency = args.hasFlag("trim", "Trim edges to remove processing latency");
        }
        args.errorExit();

        if (!inputWav.read(inputFile)) args.errorExit(inputWav.result.reason);
        outputWav.channels = inputWav.channels;
        outputWav.sampleRate = inputWav.sampleRate;

        int blockSamples = int(blockMs*0.001*inputWav.sampleRate + 0.5);
        int intervalSamples = int(blockSamples/overlapFactor);
        int searchSamples = int(searchMs*0.001*inputWav.sampleRate + 0.5);

        if (mode == Mode::resample) {
            outputWav = inputWav; // Just copy the waveform
            outputWav.sampleRate *= freqFactor;
        } else if (mode == Mode::overlapAdd) {
            OverlapAddStretch stretch;
            stretch.configure(inputWav.channels, blockSamples, intervalSamples);
            stretch.setTimeFactor(timeFactor*freqFactor);
            processBlocks(stretch, timeFactor*freqFactor);
            outputWav.sampleRate *= freqFactor;
        } else if (mode == Mode::wsola) {
            WsolaStretch stretch;
            stretch.configure(inputWav.channels, blockSamples, intervalSamples, searchSamples);
            stretch.setTimeFactor(timeFactor*freqFactor);
            processBlocks(stretch, timeFactor*freqFactor);
            outputWav.sampleRate *= freqFactor;
        } else if (mode == Mode::spectralCut) {
            SpectralCutStretch stretch(fixedPhase);
            stretch.configure(inputWav.channels, blockSamples, intervalSamples);
            stretch.setTimeFactor(timeFactor);
            stretch.setFreqFactor(freqFactor);
            processBlocks(stretch, timeFactor);
        } else if (mode == Mode::phaseVocoder) {
            PhaseVocoderStretch stretch(purePV);
            stretch.configure(inputWav.channels, blockSamples, intervalSamples, zeroPadding);
            stretch.gain = gain;
            stretch.setTimeFactor(timeFactor*freqFactor);
            processBlocks(stretch, timeFactor*freqFactor);
            outputWav.sampleRate *= freqFactor;
        } else if (mode == Mode::paulStretch) {
            PaulStretch stretch;
            stretch.configure(inputWav.channels, blockSamples, intervalSamples, zeroPadding);
            stretch.setTimeFactor(timeFactor*freqFactor);
            processBlocks(stretch, timeFactor*freqFactor);
            outputWav.sampleRate *= freqFactor;
        } else if (mode == Mode::vasePhocoder) {
            VasePhocoderStretch stretch(stretchPhase);
            stretch.configure(inputWav.channels, blockSamples, intervalSamples, zeroPadding);
            stretch.gain = gain;
            stretch.setTimeFactor(timeFactor);
            stretch.setFreqFactor(freqFactor);
            processBlocks(stretch, timeFactor);
        } else if (mode == Mode::hybridPhase) {
            HybridPhaseStretch stretch(!singleTimeObservation, pitchWeight, timeWeight, channelWeight, maxWeight);
            stretch.configure(inputWav.channels, blockSamples, intervalSamples, zeroPadding);
            stretch.setTimeFactor(timeFactor);
            stretch.setFreqFactor(freqFactor);
            processBlocks(stretch, timeFactor);
        }
#endif

bool resample_44100_to_48000_signalsmith(Sound & sound)
{
    if (sound.m_sampleRate == 48000)
    {
        return true; // Nothing todo
    }

    if (sound.m_sampleRate != 44100)
    {
        DE_WARN("Not really suited for big sampleRate differences")
    }

    const double ratio = double(sound.m_sampleRate) / 48000.0;   // ≈ 0.91875

    // Signalsmith Stretch initialisieren
    signalsmith::stretch::SignalSmithStretch<float> stretcher;
    stretcher.setChannels(input.channels);
    stretcher.setRatio(ratio);

    const int inFrames = input.samples.size() / input.channels;
    const int outFrames = int(inFrames * ratio) + 8;

    output.samples.resize(outFrames * input.channels);

    stretcher.processInterleaved(
        input.samples.data(), inFrames,
        output.samples.data(), outFrames
    );

    return output;
}

} // end namespace sound.
} // end namespace de.

#endif
