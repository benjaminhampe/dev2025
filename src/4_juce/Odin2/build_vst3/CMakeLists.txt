set(LIB_NAME Odin2_vst3)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../src)
    set(APP_SOURCES
        ${APP_SOURCE_DIR}/assets/OdinAssets.h
        ${APP_SOURCE_DIR}/assets/OdinAssets.cpp
        ${APP_SOURCE_DIR}/AudioParameterConnections.h
        ${APP_SOURCE_DIR}/AudioValueTree.h
        ${APP_SOURCE_DIR}/AudioVarDeclarations.h
        ${APP_SOURCE_DIR}/ConfigFileManager.cpp
        ${APP_SOURCE_DIR}/ConfigFileManager.h
        ${APP_SOURCE_DIR}/GeneratedAudioValueCode.h
        ${APP_SOURCE_DIR}/GlobalIncludes.h
        ${APP_SOURCE_DIR}/OdinTreeListener.h
        ${APP_SOURCE_DIR}/PluginEditor.cpp
        ${APP_SOURCE_DIR}/PluginEditor.h
        ${APP_SOURCE_DIR}/PluginProcessor.cpp
        ${APP_SOURCE_DIR}/PluginProcessor.h
        ${APP_SOURCE_DIR}/PluginProcessorConstructor.cpp
        ${APP_SOURCE_DIR}/PluginProcessorMidi.cpp
        ${APP_SOURCE_DIR}/PluginProcessorMigratePatch.cpp
        ${APP_SOURCE_DIR}/PluginProcessorMisc.cpp
        ${APP_SOURCE_DIR}/PluginProcessorNonAudioParams.cpp
        ${APP_SOURCE_DIR}/PluginProcessorProcess.cpp
        ${APP_SOURCE_DIR}/PluginProcessorReadPatch.cpp
        ${APP_SOURCE_DIR}/PluginProcessorSetModPointers.cpp
        ${APP_SOURCE_DIR}/PluginProcessorValueChange.cpp
        ${APP_SOURCE_DIR}/ProcessorInitializerList.h
        ${APP_SOURCE_DIR}/Utilities.cpp
        ${APP_SOURCE_DIR}/Utilities.h
      # ${APP_SOURCE_DIR}/audio
        ${APP_SOURCE_DIR}/audio/ADSR.cpp
        ${APP_SOURCE_DIR}/audio/ADSR.h
        ${APP_SOURCE_DIR}/audio/Amplifier.cpp
        ${APP_SOURCE_DIR}/audio/Amplifier.h
        ${APP_SOURCE_DIR}/audio/ImpulseResponseCreator.cpp
        ${APP_SOURCE_DIR}/audio/ImpulseResponseCreator.h
        ${APP_SOURCE_DIR}/audio/OdinConstants.h
        ${APP_SOURCE_DIR}/audio/Voice.h
      # ${APP_SOURCE_DIR}/audio/Filters
        ${APP_SOURCE_DIR}/audio/Filters/BiquadAllpass.cpp
        ${APP_SOURCE_DIR}/audio/Filters/BiquadAllpass.h
        ${APP_SOURCE_DIR}/audio/Filters/BiquadEQ.cpp
        ${APP_SOURCE_DIR}/audio/Filters/BiquadEQ.h
        ${APP_SOURCE_DIR}/audio/Filters/BiquadFilter.cpp
        ${APP_SOURCE_DIR}/audio/Filters/BiquadFilter.h
        ${APP_SOURCE_DIR}/audio/Filters/BiquadResonator.cpp
        ${APP_SOURCE_DIR}/audio/Filters/BiquadResonator.h
        ${APP_SOURCE_DIR}/audio/Filters/CombFilter.cpp
        ${APP_SOURCE_DIR}/audio/Filters/CombFilter.h
        ${APP_SOURCE_DIR}/audio/Filters/DCBlockingFilter.h
        ${APP_SOURCE_DIR}/audio/Filters/DiodeFilter.cpp
        ${APP_SOURCE_DIR}/audio/Filters/DiodeFilter.h
        ${APP_SOURCE_DIR}/audio/Filters/FormantFilter.cpp
        ${APP_SOURCE_DIR}/audio/Filters/FormantFilter.h
        ${APP_SOURCE_DIR}/audio/Filters/Korg35Filter.cpp
        ${APP_SOURCE_DIR}/audio/Filters/Korg35Filter.h
        ${APP_SOURCE_DIR}/audio/Filters/LadderFilter.cpp
        ${APP_SOURCE_DIR}/audio/Filters/LadderFilter.h
        ${APP_SOURCE_DIR}/audio/Filters/OdinFilterBase.cpp
        ${APP_SOURCE_DIR}/audio/Filters/OdinFilterBase.h
        ${APP_SOURCE_DIR}/audio/Filters/SEMFilter12.cpp
        ${APP_SOURCE_DIR}/audio/Filters/SEMFilter12.h
        ${APP_SOURCE_DIR}/audio/Filters/VAOnePoleFilter.cpp
        ${APP_SOURCE_DIR}/audio/Filters/VAOnePoleFilter.h
      # ${APP_SOURCE_DIR}/audio/FX
        ${APP_SOURCE_DIR}/audio/FX/Bitcrusher.cpp
        ${APP_SOURCE_DIR}/audio/FX/Bitcrusher.h
        ${APP_SOURCE_DIR}/audio/FX/Chorus.cpp
        ${APP_SOURCE_DIR}/audio/FX/Chorus.h
        ${APP_SOURCE_DIR}/audio/FX/Delay.cpp
        ${APP_SOURCE_DIR}/audio/FX/Delay.h
        ${APP_SOURCE_DIR}/audio/FX/FeedbackDelayNetwork.cpp
        ${APP_SOURCE_DIR}/audio/FX/FeedbackDelayNetwork.h
        ${APP_SOURCE_DIR}/audio/FX/Flanger.cpp
        ${APP_SOURCE_DIR}/audio/FX/Flanger.h
        ${APP_SOURCE_DIR}/audio/FX/IntegerDelayLine.cpp
        ${APP_SOURCE_DIR}/audio/FX/IntegerDelayLine.h
        ${APP_SOURCE_DIR}/audio/FX/OversamplingDistortion.cpp
        ${APP_SOURCE_DIR}/audio/FX/OversamplingDistortion.h
        ${APP_SOURCE_DIR}/audio/FX/ParametricEQ.cpp
        ${APP_SOURCE_DIR}/audio/FX/ParametricEQ.h
        ${APP_SOURCE_DIR}/audio/FX/Phaser.cpp
        ${APP_SOURCE_DIR}/audio/FX/Phaser.h
        ${APP_SOURCE_DIR}/audio/FX/RingModulator.cpp
        ${APP_SOURCE_DIR}/audio/FX/RingModulator.h
        ${APP_SOURCE_DIR}/audio/FX/SurgeReverb.cpp
        ${APP_SOURCE_DIR}/audio/FX/SurgeReverb.h
        ${APP_SOURCE_DIR}/audio/FX/ZitaReverb.cpp
        ${APP_SOURCE_DIR}/audio/FX/ZitaReverb.h
      # ${APP_SOURCE_DIR}/audio/Oscillators
        ${APP_SOURCE_DIR}/audio/Oscillators/AnalogOscillator.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/AnalogOscillator.h
        ${APP_SOURCE_DIR}/audio/Oscillators/ChiptuneArpeggiator.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/ChiptuneArpeggiator.h
        ${APP_SOURCE_DIR}/audio/Oscillators/ChiptuneOscillator.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/ChiptuneOscillator.h
        ${APP_SOURCE_DIR}/audio/Oscillators/DriftGenerator.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/DriftGenerator.h
        ${APP_SOURCE_DIR}/audio/Oscillators/FMOscillator.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/FMOscillator.h
        ${APP_SOURCE_DIR}/audio/Oscillators/LFO.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/LFO.h
        ${APP_SOURCE_DIR}/audio/Oscillators/MultiOscillator.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/MultiOscillator.h
        ${APP_SOURCE_DIR}/audio/Oscillators/NoiseOscillator.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/NoiseOscillator.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Oscillator.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/Oscillator.h
        ${APP_SOURCE_DIR}/audio/Oscillators/PitchShiftTable.h
        ${APP_SOURCE_DIR}/audio/Oscillators/PMCarrierOsc.h
        ${APP_SOURCE_DIR}/audio/Oscillators/PMOscillator.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/PMOscillator.h
        ${APP_SOURCE_DIR}/audio/Oscillators/VectorOscillator.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/VectorOscillator.h
        ${APP_SOURCE_DIR}/audio/Oscillators/WavetableCoefficients.h
        ${APP_SOURCE_DIR}/audio/Oscillators/WavetableContainer.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/WavetableContainer.h
        ${APP_SOURCE_DIR}/audio/Oscillators/WavetableOsc1D.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/WavetableOsc1D.h
        ${APP_SOURCE_DIR}/audio/Oscillators/WavetableOsc2D.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/WavetableOsc2D.h
      # ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/WavetableMappings.h
      # ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive10.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive11.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive12.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive13.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive14.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive15.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive16.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive5.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive6.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive7.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive8.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Additive9.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_birds_0004.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_birds_0010.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_birds_0011.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_birds_0014.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_bitreduced_0002.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_bitreduced_0003.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_bitreduced_0006.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_bitreduced_0011.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_fmsynth_0011.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_fmsynth_0032.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_fmsynth_0034.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_fmsynth_0081.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0002.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0010.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0014.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0019.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0020.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0021.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0029.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0032.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0037.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0041.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0047.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0049.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0056.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0064.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0071.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/AKWF_hvoice_0093.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/BagPipe.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/BagPipeMutated1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/BagPipeMutated4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/BagPipeMutated5.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/BrokenSine1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/BrokenSine2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/BrokenSine3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/BrokenSine4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Cello1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Cello2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChipSaw.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChipSine.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneCustom1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneCustom2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneCustom3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneCustom4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneCustom5.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneCustom6.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneSquare12_5.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneSquare25.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneSquare50.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneSquare50Mutated1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneSquare50Mutated2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneSquare50Mutated3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneSquare50Mutated4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneSquare50Mutated5.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneSquare50Mutated6.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneSquare50Mutated7.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneSquare50Mutated8.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/ChiptuneTriangle.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/CTDiverging.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/CTHigh.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/CTHigh2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/CTHigh3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/CTRich.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/CTSoftTune.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/FatSaw.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/FatSawMutated1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/FatSawMutated2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/FatSawMutated3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/FatSawMutated4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/FatSawMutated5.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/FatSawMutated6.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/FatSawMutated7.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/FatSawMutated8.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Glass.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/GlassMutated1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/GlassMutated2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/GlassMutated3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics10.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics11.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics12.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics13.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics14.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics15.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics16.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics5.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics6.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics7.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics8.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Harmonics9.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegAah.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegAdd1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegAdd3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegAdd8.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegBags.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegBarbedWire.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegCello.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegHarm2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegMale.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegNoBass.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegOrgan.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegPiano.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegRip2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegSharp.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegToyBox.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LegTriQuad.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOPyramid12.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOPyramid4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOPyramid6.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOPyramid8.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOSpike.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOSquare12.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOSquare25.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOSquare50.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOStair12.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOStair3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOStair4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOStair6.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/LFOStair8.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Oboe1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Oboe2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Oboe3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Oboe4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Organ1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Organ2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Organ3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Organ4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/PerlinReplace1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/PerlinReplace2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/PerlinReplace3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/PerlinReplace4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/PerlinReplace5.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/PerlinReplace6.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/PerlinReplace7.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/PerlinReplace8.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Piano1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Piano2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Piano3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Piano4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Rectangular1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Rectangular2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Rectangular3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Rectangular4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Saw.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/SawMutated1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/SawMutated2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/SawMutated3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/SawMutated4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/SawMutated5.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/SawMutated6.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/SawMutated7.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/SawMutated8.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Sine.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Skyline1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Skyline2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Skyline3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Skyline4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Triangle.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Trumpet1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Trumpet2.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Trumpet3.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Trumpet4.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Violin1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Coefficients/Violin2.h
      # ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Tables
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Tables/LFOTableData.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Tables/LFOTableData.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Tables/WavetableData.1.h
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Tables/WavetableData.cpp
        ${APP_SOURCE_DIR}/audio/Oscillators/Wavetables/Tables/WavetableData.h
      # ${APP_SOURCE_DIR}/gui
        ${APP_SOURCE_DIR}/gui/ADSRComponent.cpp
        ${APP_SOURCE_DIR}/gui/ADSRComponent.h
        ${APP_SOURCE_DIR}/gui/AmpDistortionFlowComponent.cpp
        ${APP_SOURCE_DIR}/gui/AmpDistortionFlowComponent.h
        ${APP_SOURCE_DIR}/gui/ArpComponent.cpp
        ${APP_SOURCE_DIR}/gui/ArpComponent.h
        ${APP_SOURCE_DIR}/gui/BPMSelector.h
        ${APP_SOURCE_DIR}/gui/BrowserEntry.cpp
        ${APP_SOURCE_DIR}/gui/BrowserEntry.h
        ${APP_SOURCE_DIR}/gui/ChipdrawWindow.cpp
        ${APP_SOURCE_DIR}/gui/ChipdrawWindow.h
        ${APP_SOURCE_DIR}/gui/DelayComponent.cpp
        ${APP_SOURCE_DIR}/gui/DelayComponent.h
        ${APP_SOURCE_DIR}/gui/DragButton.h
        ${APP_SOURCE_DIR}/gui/DrawableSlider.cpp
        ${APP_SOURCE_DIR}/gui/DrawableSlider.h
        ${APP_SOURCE_DIR}/gui/FactoryPresetBinaryMapping.h
        ${APP_SOURCE_DIR}/gui/FactoryPresetNames.h
        ${APP_SOURCE_DIR}/gui/FilterComponent.cpp
        ${APP_SOURCE_DIR}/gui/FilterComponent.h
        ${APP_SOURCE_DIR}/gui/FixedTextGlassDropdown.cpp
        ${APP_SOURCE_DIR}/gui/FixedTextGlassDropdown.h
        ${APP_SOURCE_DIR}/gui/FormantSelector.h
        ${APP_SOURCE_DIR}/gui/FXButtonsSection.h
        ${APP_SOURCE_DIR}/gui/FXComponent.cpp
        ${APP_SOURCE_DIR}/gui/FXComponent.h
        ${APP_SOURCE_DIR}/gui/GlasDisplay.cpp
        ${APP_SOURCE_DIR}/gui/GlasDisplay.h
        ${APP_SOURCE_DIR}/gui/GlassDropdown.cpp
        ${APP_SOURCE_DIR}/gui/GlassDropdown.h
        ${APP_SOURCE_DIR}/gui/GuiDataEditor.cpp
        ${APP_SOURCE_DIR}/gui/GuiDataEditor.h
        ${APP_SOURCE_DIR}/gui/InputField.h
        ${APP_SOURCE_DIR}/gui/IntegerKnob.cpp
        ${APP_SOURCE_DIR}/gui/IntegerKnob.h
        ${APP_SOURCE_DIR}/gui/JsonGuiProvider.cpp
        ${APP_SOURCE_DIR}/gui/JsonGuiProvider.h
        ${APP_SOURCE_DIR}/gui/LeftRightButton.cpp
        ${APP_SOURCE_DIR}/gui/LeftRightButton.h
        ${APP_SOURCE_DIR}/gui/LFOComponent.cpp
        ${APP_SOURCE_DIR}/gui/LFOComponent.h
        ${APP_SOURCE_DIR}/gui/LFODisplayComponent.cpp
        ${APP_SOURCE_DIR}/gui/LFODisplayComponent.h
        ${APP_SOURCE_DIR}/gui/LFOPopupLookAndFeel.cpp
        ${APP_SOURCE_DIR}/gui/LFOPopupLookAndFeel.h
        ${APP_SOURCE_DIR}/gui/LFOSelectorComponent.cpp
        ${APP_SOURCE_DIR}/gui/LFOSelectorComponent.h
        ${APP_SOURCE_DIR}/gui/LiveConstrainer.cpp
        ${APP_SOURCE_DIR}/gui/LiveConstrainer.h
        ${APP_SOURCE_DIR}/gui/ModAmountComponent.cpp
        ${APP_SOURCE_DIR}/gui/ModAmountComponent.h
        ${APP_SOURCE_DIR}/gui/ModAmountComponentExtended.cpp
        ${APP_SOURCE_DIR}/gui/ModAmountComponentExtended.h
        ${APP_SOURCE_DIR}/gui/ModMatrix.cpp
        ${APP_SOURCE_DIR}/gui/ModMatrix.h
        ${APP_SOURCE_DIR}/gui/ModMatrixComponent.cpp
        ${APP_SOURCE_DIR}/gui/ModMatrixComponent.h
        ${APP_SOURCE_DIR}/gui/NumberSelector.cpp
        ${APP_SOURCE_DIR}/gui/NumberSelector.h
        ${APP_SOURCE_DIR}/gui/NumberSelectorWithText.cpp
        ${APP_SOURCE_DIR}/gui/NumberSelectorWithText.h
        ${APP_SOURCE_DIR}/gui/OdinArpeggiator.cpp
        ${APP_SOURCE_DIR}/gui/OdinArpeggiator.h
        ${APP_SOURCE_DIR}/gui/OdinButton.cpp
        ${APP_SOURCE_DIR}/gui/OdinButton.h
        ${APP_SOURCE_DIR}/gui/OdinControlAttachments.cpp
        ${APP_SOURCE_DIR}/gui/OdinControlAttachments.h
        ${APP_SOURCE_DIR}/gui/OdinControlBase.h
        ${APP_SOURCE_DIR}/gui/OdinFeels.h
        ${APP_SOURCE_DIR}/gui/OdinKnob.cpp
        ${APP_SOURCE_DIR}/gui/OdinKnob.h
        ${APP_SOURCE_DIR}/gui/OdinTooltipWindow.h
        ${APP_SOURCE_DIR}/gui/OscComponent.cpp
        ${APP_SOURCE_DIR}/gui/OscComponent.h
        ${APP_SOURCE_DIR}/gui/PatchBrowser.cpp
        ${APP_SOURCE_DIR}/gui/PatchBrowser.h
        ${APP_SOURCE_DIR}/gui/PatchBrowserScrollBar.cpp
        ${APP_SOURCE_DIR}/gui/PatchBrowserScrollBar.h
        ${APP_SOURCE_DIR}/gui/PatchBrowserSelector.cpp
        ${APP_SOURCE_DIR}/gui/PatchBrowserSelector.h
        ${APP_SOURCE_DIR}/gui/PhaserComponent.cpp
        ${APP_SOURCE_DIR}/gui/PhaserComponent.h
        ${APP_SOURCE_DIR}/gui/PitchWheel.h
        ${APP_SOURCE_DIR}/gui/RescaleProgressComponent.cpp
        ${APP_SOURCE_DIR}/gui/RescaleProgressComponent.h
        ${APP_SOURCE_DIR}/gui/ResizeDragger.cpp
        ${APP_SOURCE_DIR}/gui/ResizeDragger.h
        ${APP_SOURCE_DIR}/gui/ReverbComponent.cpp
        ${APP_SOURCE_DIR}/gui/ReverbComponent.h
        ${APP_SOURCE_DIR}/gui/SpecdrawDisplay.cpp
        ${APP_SOURCE_DIR}/gui/SpecdrawDisplay.h
        ${APP_SOURCE_DIR}/gui/SpectrumDisplay.cpp
        ${APP_SOURCE_DIR}/gui/SpectrumDisplay.h
        ${APP_SOURCE_DIR}/gui/SplineAdComponent.cpp
        ${APP_SOURCE_DIR}/gui/SplineAdComponent.h
        ${APP_SOURCE_DIR}/gui/StepComponent.cpp
        ${APP_SOURCE_DIR}/gui/StepComponent.h
        ${APP_SOURCE_DIR}/gui/StepLED.h
        ${APP_SOURCE_DIR}/gui/SyncTimeSelector.cpp
        ${APP_SOURCE_DIR}/gui/SyncTimeSelector.h
        ${APP_SOURCE_DIR}/gui/TextLabel.h
        ${APP_SOURCE_DIR}/gui/TitleButton.cpp
        ${APP_SOURCE_DIR}/gui/TitleButton.h
        ${APP_SOURCE_DIR}/gui/TooltipFeels.h
        ${APP_SOURCE_DIR}/gui/TuningComponent.cpp
        ${APP_SOURCE_DIR}/gui/TuningComponent.h
        ${APP_SOURCE_DIR}/gui/UIAssetManager.cpp
        ${APP_SOURCE_DIR}/gui/UIAssetManager.h
        ${APP_SOURCE_DIR}/gui/UIAssetsData.cpp
        ${APP_SOURCE_DIR}/gui/UIAssetsData.h
        ${APP_SOURCE_DIR}/gui/UIAssetsIndices.h
        ${APP_SOURCE_DIR}/gui/UIAssetsSize.cpp
        ${APP_SOURCE_DIR}/gui/UIAssetsSize.h
        ${APP_SOURCE_DIR}/gui/WavedrawDisplay.cpp
        ${APP_SOURCE_DIR}/gui/WavedrawDisplay.h
        ${APP_SOURCE_DIR}/gui/WaveformSelectorComponent.cpp
        ${APP_SOURCE_DIR}/gui/WaveformSelectorComponent.h
        ${APP_SOURCE_DIR}/gui/WavetableDisplay.cpp
        ${APP_SOURCE_DIR}/gui/WavetableDisplay.h
        ${APP_SOURCE_DIR}/gui/XYPadComponent.cpp
        ${APP_SOURCE_DIR}/gui/XYPadComponent.h
        ${APP_SOURCE_DIR}/gui/XYSectionComponent.cpp
        ${APP_SOURCE_DIR}/gui/XYSectionComponent.h
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    add_library(${LIB_NAME} SHARED "")
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL_BUILD=1)
    # else()
    # add_library(${LIB_NAME} STATIC "")
    # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL=0)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL_BUILD=0)
    # endif()

    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL=0)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DLL_BUILD=0)

    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)

    target_sources(${LIB_NAME} PRIVATE ${APP_SOURCES})

    # target_include_directories(${LIB_NAME} PRIVATE ${JUCE_HEADER_DIR})

    target_include_directories(${LIB_NAME} PRIVATE ${APP_SOURCE_DIR})

    # target_include_directories(${LIB_NAME} PRIVATE ${APP_SOURCE_DIR}/drivers)
    # target_include_directories(${LIB_NAME} PRIVATE ${APP_SOURCE_DIR}/vendor)

    # target_compile_definitions(${LIB_NAME} PUBLIC -DJucePlugin_Name="juce_vst2_plugin_001")

    # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)

    # target_compile_definitions(${LIB_NAME} PRIVATE -DHAVE_CONFIG_H=1)

    if(APPLE)
        target_compile_definitions(${LIB_NAME} PRIVATE ODIN_MAC=1)
    elseif(UNIX AND NOT APPLE)
        target_compile_definitions(${LIB_NAME} PRIVATE ODIN_LINUX=1)
    elseif(WIN32)
        target_compile_definitions(${LIB_NAME} PRIVATE ODIN_WIN=1)
    endif()
    
    if (MSVC)
    else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows) # disable console, also helps with win icon
        target_compile_options(${LIB_NAME} PUBLIC -Wno-multichar)
        
        target_compile_options(${LIB_NAME} PRIVATE -fconstexpr-depth=1024)
        target_compile_options(${LIB_NAME} PRIVATE -fbracket-depth=1024)
    endif()

    target_link_libraries(${LIB_NAME} PRIVATE de_juce_odin2_vst3)
    
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_audio_processors)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_gui_extra)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_cryptography)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_opengl)
    # target_link_libraries(${LIB_NAME} PRIVATE de_juce_video)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_dsp)
    target_link_libraries(${LIB_NAME} PRIVATE de_tuning_library)
    target_link_libraries(${LIB_NAME} PRIVATE de_nlohmann_json)

    set(APP_NAME ${LIB_NAME})
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

   # COPY MEDIA FILES relative to exe
   # file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
         # DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/QIrrlichtWidget2019 )

   # file(COPY ${CMAKE_CURRENT_LIST_DIR}/tux.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
   # # file(COPY ${CMAKE_CURRENT_LIST_DIR}/pcc_logo.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

   # file(COPY ../media/Odin2 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
    
   # copy theme resource files
   #ADD_CUSTOM_COMMAND( TARGET "${LIB_NAME}"
   #   POST_BUILD
   #   COMMAND "echo" "Copying theme.."
   #      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/themes" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes"
   #   )
endif()
