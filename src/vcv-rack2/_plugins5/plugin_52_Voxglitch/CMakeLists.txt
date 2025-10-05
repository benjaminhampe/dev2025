set(LIB_NAME plugin_52_Voxglitch)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/Voxglitch_plugin.hpp
        ${LIB_SOURCE_DIR}/plugin.cpp
      # ${LIB_SOURCE_DIR}/modules
        ${LIB_SOURCE_DIR}/modules/ArpSeq.cpp
        ${LIB_SOURCE_DIR}/modules/autobreak.cpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio.cpp
        ${LIB_SOURCE_DIR}/modules/ByteBeat.cpp
        ${LIB_SOURCE_DIR}/modules/CueResearch.cpp
        ${LIB_SOURCE_DIR}/modules/DigitalProgrammer.cpp
        ${LIB_SOURCE_DIR}/modules/DigitalSequencer.cpp
        ${LIB_SOURCE_DIR}/modules/DigitalSequencerXP.cpp
        ${LIB_SOURCE_DIR}/modules/DrumRandomizer.cpp
        ${LIB_SOURCE_DIR}/modules/ghosts.cpp
        ${LIB_SOURCE_DIR}/modules/GlitchSequencer.cpp
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2.cpp
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2Expander.cpp
        ${LIB_SOURCE_DIR}/modules/GrainFx.cpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox.cpp
        ${LIB_SOURCE_DIR}/modules/GrooveBoxExpander.cpp
        ${LIB_SOURCE_DIR}/modules/Hazumi.cpp
        ${LIB_SOURCE_DIR}/modules/Looper.cpp
        ${LIB_SOURCE_DIR}/modules/NoteDetector.cpp
        ${LIB_SOURCE_DIR}/modules/OnePoint.cpp
        ${LIB_SOURCE_DIR}/modules/OneZero.cpp
        ${LIB_SOURCE_DIR}/modules/repeater.cpp
        ${LIB_SOURCE_DIR}/modules/Sampler16P.cpp
        ${LIB_SOURCE_DIR}/modules/SamplerX8.cpp
        ${LIB_SOURCE_DIR}/modules/Satanonaut.cpp
        ${LIB_SOURCE_DIR}/modules/SatanonautUnearthed.cpp
        ${LIB_SOURCE_DIR}/modules/VectorRotation.cpp
        ${LIB_SOURCE_DIR}/modules/wavbank.cpp
        ${LIB_SOURCE_DIR}/modules/wavbank_mc.cpp
        ${LIB_SOURCE_DIR}/modules/xy.cpp
      # ${LIB_SOURCE_DIR}/modules/ArpSeq
        ${LIB_SOURCE_DIR}/modules/ArpSeq/ArpSeq.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/ArpSequencerDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/ArpSeqWidget.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/ArpSeqWindow.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/ArpVoltageSequencerDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/CycleCounter.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/CycleCounters.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/defines.h
        ${LIB_SOURCE_DIR}/modules/ArpSeq/Page.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/RateModel.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/SegmentReadoutWidget.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/ShapeModel.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/SlewSlider.hpp
        ${LIB_SOURCE_DIR}/modules/ArpSeq/TabsWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/Autobreak
        ${LIB_SOURCE_DIR}/modules/Autobreak/Autobreak.hpp
        ${LIB_SOURCE_DIR}/modules/Autobreak/AutobreakLoadSample.hpp
        ${LIB_SOURCE_DIR}/modules/Autobreak/AutobreakWidget.hpp
        ${LIB_SOURCE_DIR}/modules/Autobreak/defines.h
      # ${LIB_SOURCE_DIR}/modules/AutobreakStudio
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/AutobreakGateSequencer.hpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/AutobreakMemory.hpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/AutobreakSequencer.hpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/AutobreakStudio.hpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/AutobreakStudioLoadFolder.hpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/AutobreakStudioLoadSample.hpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/AutobreakStudioWidget.hpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/AutobreakVoltageSequencer.hpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/defines.h
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/LcdTabsWidget.hpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/SequencerDisplayABS.hpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/VoltageSequencerDisplayABS.hpp
        ${LIB_SOURCE_DIR}/modules/AutobreakStudio/VoltageToggleSequencerDisplay.hpp
      # ${LIB_SOURCE_DIR}/modules/ByteBeat
        ${LIB_SOURCE_DIR}/modules/ByteBeat/ByteBeat.hpp
        ${LIB_SOURCE_DIR}/modules/ByteBeat/BytebeatSegmentReadoutWidget.hpp
        ${LIB_SOURCE_DIR}/modules/ByteBeat/ByteBeatWidget.hpp
        ${LIB_SOURCE_DIR}/modules/ByteBeat/defines.h
        ${LIB_SOURCE_DIR}/modules/ByteBeat/todo.txt
      # ${LIB_SOURCE_DIR}/modules/CueResearch
        ${LIB_SOURCE_DIR}/modules/CueResearch/CueResearch.hpp
        ${LIB_SOURCE_DIR}/modules/CueResearch/CueResearchLoadSample.hpp
        ${LIB_SOURCE_DIR}/modules/CueResearch/CueResearchWidget.hpp
        ${LIB_SOURCE_DIR}/modules/CueResearch/Marker.hpp
        ${LIB_SOURCE_DIR}/modules/CueResearch/ScrubState.hpp
        ${LIB_SOURCE_DIR}/modules/CueResearch/TrackModel.hpp
        ${LIB_SOURCE_DIR}/modules/CueResearch/TrackWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/DigitalProgrammer
        ${LIB_SOURCE_DIR}/modules/DigitalProgrammer/CopyPasteLabel.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalProgrammer/defines.h
        ${LIB_SOURCE_DIR}/modules/DigitalProgrammer/DigitalProgrammer.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalProgrammer/DigitalProgrammerWidget.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalProgrammer/DPBankButtonDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalProgrammer/DPSlider.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalProgrammer/DPSliderDisplay.hpp
      # ${LIB_SOURCE_DIR}/modules/DigitalSequencer
        ${LIB_SOURCE_DIR}/modules/DigitalSequencer/defines.h
        ${LIB_SOURCE_DIR}/modules/DigitalSequencer/DigitalSequencer.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalSequencer/DigitalSequencerWidget.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalSequencer/GateSequencerDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalSequencer/SequencerDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalSequencer/VoltageSequencerDisplay.hpp
      # ${LIB_SOURCE_DIR}/modules/DigitalSequencerXP
        ${LIB_SOURCE_DIR}/modules/DigitalSequencerXP/defines.h
        ${LIB_SOURCE_DIR}/modules/DigitalSequencerXP/DigitalSequencerXP.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalSequencerXP/DigitalSequencerXPWidget.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalSequencerXP/GateSequencerDisplayXP.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalSequencerXP/SequencerDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/DigitalSequencerXP/VoltageSequencerDisplayXP.hpp
      # ${LIB_SOURCE_DIR}/modules/DrumRandomizer
        ${LIB_SOURCE_DIR}/modules/DrumRandomizer/DrumRandomizer.hpp
        ${LIB_SOURCE_DIR}/modules/DrumRandomizer/DrumRandomizerReadoutWidget.hpp
        ${LIB_SOURCE_DIR}/modules/DrumRandomizer/DrumRandomizerWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/Ghosts
        ${LIB_SOURCE_DIR}/modules/Ghosts/defines.h
        ${LIB_SOURCE_DIR}/modules/Ghosts/Ghosts.hpp
        ${LIB_SOURCE_DIR}/modules/Ghosts/GhostsEx.hpp
        ${LIB_SOURCE_DIR}/modules/Ghosts/GhostsWidget.hpp
        ${LIB_SOURCE_DIR}/modules/Ghosts/LoadSample.hpp
      # ${LIB_SOURCE_DIR}/modules/GlitchSequencer
        ${LIB_SOURCE_DIR}/modules/GlitchSequencer/CellularAutomatonDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/GlitchSequencer/CellularAutomatonSequencer.hpp
        ${LIB_SOURCE_DIR}/modules/GlitchSequencer/defines.h
        ${LIB_SOURCE_DIR}/modules/GlitchSequencer/GlitchSequencer.hpp
        ${LIB_SOURCE_DIR}/modules/GlitchSequencer/GlitchSequencerWidget.hpp
        ${LIB_SOURCE_DIR}/modules/GlitchSequencer/LengthReadoutDisplay.hpp
      # ${LIB_SOURCE_DIR}/modules/GrainEngineMK2
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2/defines.h
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2/Grain.hpp
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2/GrainEngineMK2.hpp
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2/GrainEngineMK2LoadSample.hpp
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2/GrainEngineMK2PosDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2/GrainEngineMK2Widget.hpp
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2/GrainManager.hpp
      # ${LIB_SOURCE_DIR}/modules/GrainEngineMK2Expander
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2Expander/defines.h
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2Expander/GrainEngineMK2Expander.hpp
        ${LIB_SOURCE_DIR}/modules/GrainEngineMK2Expander/GrainEngineMK2ExpanderWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/GrainFx
        ${LIB_SOURCE_DIR}/modules/GrainFx/defines.h
        ${LIB_SOURCE_DIR}/modules/GrainFx/Grain.hpp
        ${LIB_SOURCE_DIR}/modules/GrainFx/GrainFx.hpp
        ${LIB_SOURCE_DIR}/modules/GrainFx/GrainFxCore.hpp
        ${LIB_SOURCE_DIR}/modules/GrainFx/GrainFxWidget.hpp
        ${LIB_SOURCE_DIR}/modules/GrainFx/SimpleTableOsc.hpp
      # ${LIB_SOURCE_DIR}/modules/GrooveBox
        ${LIB_SOURCE_DIR}/modules/GrooveBox/defines.h
        ${LIB_SOURCE_DIR}/modules/GrooveBox/GrooveBox.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/GrooveboxToExpanderMessage.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/GrooveBoxWidget.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/MemorySlot.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/ParameterLockSettings.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/Track.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/TrackModel.hpp
      # ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/GrooveboxLightSwitch.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/GrooveboxMemoryButton.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/GrooveboxParameterButton.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/GrooveboxSmallLight.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/GrooveboxSoftButton.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/GrooveboxStepButton.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/LCDColorScheme.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/LCDDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/LCDRatchetDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/LCDSampleDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/LCDTrackDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/ParameterKnob.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/RangeGrabbers.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBox/widgets/SequenceLengthWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/GrooveBoxExpander
        ${LIB_SOURCE_DIR}/modules/GrooveBoxExpander/defines.h
        ${LIB_SOURCE_DIR}/modules/GrooveBoxExpander/ExpanderToGrooveboxMessage.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBoxExpander/GrooveBoxExpander.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBoxExpander/GrooveBoxExpanderWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/GrooveBoxExpander/widgets
        ${LIB_SOURCE_DIR}/modules/GrooveBoxExpander/widgets/GrooveboxExpanderMuteButton.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBoxExpander/widgets/GrooveboxExpanderSoftButton.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBoxExpander/widgets/GrooveboxExpanderSoftButtonGreen.hpp
        ${LIB_SOURCE_DIR}/modules/GrooveBoxExpander/widgets/GrooveboxExpanderSoloButton.hpp
      # ${LIB_SOURCE_DIR}/modules/Hazumi
        ${LIB_SOURCE_DIR}/modules/Hazumi/defines.h
        ${LIB_SOURCE_DIR}/modules/Hazumi/Hazumi.hpp
        ${LIB_SOURCE_DIR}/modules/Hazumi/HazumiSequencer.hpp
        ${LIB_SOURCE_DIR}/modules/Hazumi/HazumiSequencerDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/Hazumi/HazumiWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/Looper
        ${LIB_SOURCE_DIR}/modules/Looper/defines.h
        ${LIB_SOURCE_DIR}/modules/Looper/Looper.hpp
        ${LIB_SOURCE_DIR}/modules/Looper/LooperLoadSample.hpp
        ${LIB_SOURCE_DIR}/modules/Looper/LooperWaveformDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/Looper/LooperWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/NoteDetector
        ${LIB_SOURCE_DIR}/modules/NoteDetector/NoteDetector.hpp
        ${LIB_SOURCE_DIR}/modules/NoteDetector/NoteDetectorWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/OnePoint
        ${LIB_SOURCE_DIR}/modules/OnePoint/OnePoint.hpp
        ${LIB_SOURCE_DIR}/modules/OnePoint/OnePointReadoutWidget.hpp
        ${LIB_SOURCE_DIR}/modules/OnePoint/OnePointWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/OneZero
        ${LIB_SOURCE_DIR}/modules/OneZero/defines.h
        ${LIB_SOURCE_DIR}/modules/OneZero/OneZero.hpp
        ${LIB_SOURCE_DIR}/modules/OneZero/OneZeroReadoutWidget.hpp
        ${LIB_SOURCE_DIR}/modules/OneZero/OneZeroWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/Repeater
        ${LIB_SOURCE_DIR}/modules/Repeater/defines.h
        ${LIB_SOURCE_DIR}/modules/Repeater/MenuItemLoadSample.hpp
        ${LIB_SOURCE_DIR}/modules/Repeater/Readout.hpp
        ${LIB_SOURCE_DIR}/modules/Repeater/Repeater.hpp
        ${LIB_SOURCE_DIR}/modules/Repeater/RepeaterWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/Sampler16P
        ${LIB_SOURCE_DIR}/modules/Sampler16P/defines.h
        ${LIB_SOURCE_DIR}/modules/Sampler16P/Sampler16P.hpp
        ${LIB_SOURCE_DIR}/modules/Sampler16P/Sampler16PLoadFolder.hpp
        ${LIB_SOURCE_DIR}/modules/Sampler16P/Sampler16PLoadSample.hpp
        ${LIB_SOURCE_DIR}/modules/Sampler16P/Sampler16PWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/SamplerX8
        ${LIB_SOURCE_DIR}/modules/SamplerX8/defines.h
        ${LIB_SOURCE_DIR}/modules/SamplerX8/SamplerX8.hpp
        ${LIB_SOURCE_DIR}/modules/SamplerX8/SamplerX8LoadFolder.hpp
        ${LIB_SOURCE_DIR}/modules/SamplerX8/SamplerX8LoadSample.hpp
        ${LIB_SOURCE_DIR}/modules/SamplerX8/SamplerX8Widget.hpp
      # ${LIB_SOURCE_DIR}/modules/Satanonaut
        ${LIB_SOURCE_DIR}/modules/Satanonaut/defines.h
        ${LIB_SOURCE_DIR}/modules/Satanonaut/Satanonaut.hpp
        ${LIB_SOURCE_DIR}/modules/Satanonaut/SatanonautAudioBuffer.hpp
        ${LIB_SOURCE_DIR}/modules/Satanonaut/SatanonautEffectReadout.hpp
        ${LIB_SOURCE_DIR}/modules/Satanonaut/SatanonautStereoAudioBuffer.hpp
        ${LIB_SOURCE_DIR}/modules/Satanonaut/SatanonautWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/SatanonautUnearthed
        ${LIB_SOURCE_DIR}/modules/SatanonautUnearthed/SatanonautUnearthed.hpp
        ${LIB_SOURCE_DIR}/modules/SatanonautUnearthed/SatanonautUnearthedWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/VectorRotation
        ${LIB_SOURCE_DIR}/modules/VectorRotation/VectorRotation.hpp
        ${LIB_SOURCE_DIR}/modules/VectorRotation/VectorRotationWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/WavBank
        ${LIB_SOURCE_DIR}/modules/WavBank/defines.h
        ${LIB_SOURCE_DIR}/modules/WavBank/MenuItemLoadBank.hpp
        ${LIB_SOURCE_DIR}/modules/WavBank/WavBank.hpp
        ${LIB_SOURCE_DIR}/modules/WavBank/WavBankReadout.hpp
        ${LIB_SOURCE_DIR}/modules/WavBank/WavBankWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/WavBankMC
        ${LIB_SOURCE_DIR}/modules/WavBankMC/defines.h
        ${LIB_SOURCE_DIR}/modules/WavBankMC/MenuItemLoadBankMC.hpp
        ${LIB_SOURCE_DIR}/modules/WavBankMC/WavBankMC.hpp
        ${LIB_SOURCE_DIR}/modules/WavBankMC/WavBankMCReadout.hpp
        ${LIB_SOURCE_DIR}/modules/WavBankMC/WavBankMCWidget.hpp
      # ${LIB_SOURCE_DIR}/modules/XY
        ${LIB_SOURCE_DIR}/modules/XY/defines.h
        ${LIB_SOURCE_DIR}/modules/XY/XY.hpp
        ${LIB_SOURCE_DIR}/modules/XY/XYDisplay.hpp
        ${LIB_SOURCE_DIR}/modules/XY/XYWidget.hpp
      # ${LIB_SOURCE_DIR}/vgLib-2.0
        ${LIB_SOURCE_DIR}/vgLib-2.0/AudioFile.h
        ${LIB_SOURCE_DIR}/vgLib-2.0/audio_buffer.hpp
      # ${LIB_SOURCE_DIR}/vgLib-2.0/ByteBeat.cpp # Duplicate symbol!
        ${LIB_SOURCE_DIR}/vgLib-2.0/common.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/constants.cpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/constants.h
        ${LIB_SOURCE_DIR}/vgLib-2.0/customParamQuantities.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/GrainEngineExpanderMessage.hpp
      # ${LIB_SOURCE_DIR}/vgLib-2.0/LCDMatrix
        ${LIB_SOURCE_DIR}/vgLib-2.0/LCDMatrix/font.h
        ${LIB_SOURCE_DIR}/vgLib-2.0/LCDMatrix/LCDMatrix.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/panelHelper.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/sample.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/SamplePlayer.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/sample_mc.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/tables.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/Theme.hpp
      # ${LIB_SOURCE_DIR}/vgLib-2.0/components
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/DigitalControl.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/DigitalHorizontalSlider.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/DigitalHSliderTooltip.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/DigitalRangeSelector.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/DigitalSelect.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/DigitalSequencerTooltip.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/DigitalSwitch.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/DigitalToggle.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/ImageWidget.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/PanelBackground.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/SmallLightBezel.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchComponents.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchKnobs.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchModule.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchModuleWidget.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchPanel.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchPorts.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchRoundShadow.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchSamplerModule.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchSamplerModuleWidget.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchScrews.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchSwitches.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchTooltip.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/components/VoxglitchWidget.hpp
      # ${LIB_SOURCE_DIR}/vgLib-2.0/dsp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/AdaptiveGainControl.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/ADSR.cpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/ADSR.h
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/ClockDivider.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/ClockModifier.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/DeclickFilter.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/FastSlewLimiter.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/Filter.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/MonoFadeOut.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/Quantizer.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/Random.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/SampleAndHold.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/SimpleDelay.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/SlewLimiter.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/StereoFadeIn.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/StereoFadeOut.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/StereoPan.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/StereoSmooth.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/dsp/Xfade.hpp
      # ${LIB_SOURCE_DIR}/vgLib-2.0/helpers
        ${LIB_SOURCE_DIR}/vgLib-2.0/helpers/JSON.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/helpers/NOTES.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/helpers/PRAMS.hpp
      # ${LIB_SOURCE_DIR}/vgLib-2.0/sequencer
        ${LIB_SOURCE_DIR}/vgLib-2.0/sequencer/ArpSequencer.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/sequencer/GateSequencer.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/sequencer/IndexSequencer.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/sequencer/IO.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/sequencer/Sequencer.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/sequencer/SequencerDisplayConfig.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/sequencer/VoltageSequencer.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/sequencer/VoltageSequencerHistory.hpp
      # ${LIB_SOURCE_DIR}/vgLib-2.0/widgets
        ${LIB_SOURCE_DIR}/vgLib-2.0/widgets/NoteReadoutWidget.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/widgets/SegmentReadoutWidget.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/widgets/WaveformModel.hpp
        ${LIB_SOURCE_DIR}/vgLib-2.0/widgets/WaveformWidget.hpp
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
        target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} STATIC "")
    # endif()

    # target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    # target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/modules)

    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

    # set_target_properties(plugin PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/plugin.def")

    # target_compile_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_compile_options(${LIB_NAME} PRIVATE -static-libgcc)
    # target_link_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_link_options(${LIB_NAME} PRIVATE -static-libgcc)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libRack2)
    else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows)

        # if (WIN32)
            # target_link_libraries(${LIB_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            # target_link_libraries(${LIB_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        # endif()
    endif()

    # if (CMAKE_BUILD_TYPE STREQUAL "Release")

        # --strip-debug : Removes only debugging symbols.
        # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        add_custom_command(
            TARGET ${LIB_NAME}
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${LIB_NAME}>
            COMMENT "Stripping debug symbols"
        )

    # endif()

    set(VCV_NAME "Voxglitch")
    add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # Create working directory "z_benni/Gemini"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}"
        # Move libGemini.dll to working directory Gemini and rename to plugin.dll
        COMMAND ${CMAKE_COMMAND} -E rename
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib${LIB_NAME}.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}/plugin.dll"
        # Compress directory Gemini to Gemini.tar.zst
        COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni"
                tar -cf "${VCV_NAME}.tar" "${VCV_NAME}"
        COMMAND zstd -o
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar.zst"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar"
        # Move Gemini.tar.zst to final destination and rename to .vcvplugin
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins"
        COMMAND ${CMAKE_COMMAND} -E rename
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar.zst"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins/${VCV_NAME}.vcvplugin"
    )

    file(COPY media/${VCV_NAME} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni)
endif()

# add_subdirectory(test)
