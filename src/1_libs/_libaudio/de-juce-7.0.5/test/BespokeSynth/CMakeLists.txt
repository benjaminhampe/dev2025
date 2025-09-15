set(APP_NAME BespokeSynth)
project(${APP_NAME})

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    add_subdirectory(lib)

    math(EXPR BESPOKE_BITNESS "${CMAKE_SIZEOF_VOID_P} * 8" OUTPUT_FORMAT DECIMAL)
    message(STATUS "Targeting ${BESPOKE_BITNESS} bit configuration")

    # find_package (Python 3.6 COMPONENTS Interpreter Development REQUIRED)
    # message(STATUS "Python version: ${Python_VERSION}")
    # message(STATUS "Python executable: ${Python_EXECUTABLE}")
    # message(STATUS "Python library: ${Python_LIBRARIES}")

    # Global build parameters. Optional features are configurable via additional
    # BESPOKE_* variables defined in their respective subdirs.
    option(BESPOKE_NIGHTLY "Automated build from github" OFF)
    option(BESPOKE_PORTABLE "Create a self-contained build that ships all dependencies" ON)
    # set(BESPOKE_JUCE_LOCATION "${CMAKE_SOURCE_DIR}/libs/JUCE" CACHE STRING "Path to JUCE library source tree")
    # set(BESPOKE_PYTHON_ROOT "" CACHE STRING "Python search path. Override as needed.")
    # set(BESPOKE_VST2_SDK_LOCATION "" CACHE STRING "Steinberg VST2 SDK directory for non-FOSS builds")
    # set(BESPOKE_ASIO_SDK_LOCATION "" CACHE STRING "Steinberg ASIO SDK directory for non-FOSS builds")
    # set(BESPOKE_SPACEMOUSE_SDK_LOCATION "" CACHE STRING "3Dxware SpaceMouse SDK directory for non-FOSS builds")
    option(BESPOKE_SYSTEM_JUCE "Use system installation of juce" ON)
    option(BESPOKE_DEVENDORED_SYSTEM_JUCE "Use system libraries when using system installation of juce" ON)
    option(BESPOKE_SYSTEM_PYBIND11 "Use a system installation of pybind11" ON)
    option(BESPOKE_SYSTEM_JSONCPP "Use system-wide installation of jsoncpp" ON)
    option(BESPOKE_SYSTEM_TUNING_LIBRARY "Use system installation of tuning-library" ON)
    option(BESPOKE_SYSTEM_ABLETONLINK "Use system installation of abletonlink" ON)
    option(BESPOKE_USE_ASAN "Build with ASAN" OFF)

    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(APP_SOURCES 
        ${APP_SOURCE_DIR}/AbletonDeviceShared.h
        ${APP_SOURCE_DIR}/AbletonLink.cpp
        ${APP_SOURCE_DIR}/AbletonLink.h
        ${APP_SOURCE_DIR}/AbletonMoveControl.cpp
        ${APP_SOURCE_DIR}/AbletonMoveControl.h
        ${APP_SOURCE_DIR}/AbletonMoveLCD.cpp
        ${APP_SOURCE_DIR}/AbletonMoveLCD.h
        ${APP_SOURCE_DIR}/Acciaccatura.cpp
        ${APP_SOURCE_DIR}/Acciaccatura.h
        ${APP_SOURCE_DIR}/ADSR.cpp
        ${APP_SOURCE_DIR}/ADSR.h
        ${APP_SOURCE_DIR}/ADSRDisplay.cpp
        ${APP_SOURCE_DIR}/ADSRDisplay.h
        ${APP_SOURCE_DIR}/Amplifier.cpp
        ${APP_SOURCE_DIR}/Amplifier.h
        ${APP_SOURCE_DIR}/Arpeggiator.cpp
        ${APP_SOURCE_DIR}/Arpeggiator.h
        ${APP_SOURCE_DIR}/ArrangementController.cpp
        ${APP_SOURCE_DIR}/ArrangementController.h
        ${APP_SOURCE_DIR}/AudioLevelToCV.cpp
        ${APP_SOURCE_DIR}/AudioLevelToCV.h
        ${APP_SOURCE_DIR}/AudioMeter.cpp
        ${APP_SOURCE_DIR}/AudioMeter.h
        ${APP_SOURCE_DIR}/AudioRouter.cpp
        ${APP_SOURCE_DIR}/AudioRouter.h
        ${APP_SOURCE_DIR}/AudioSend.cpp
        ${APP_SOURCE_DIR}/AudioSend.h
        ${APP_SOURCE_DIR}/AudioSplitter.cpp
        ${APP_SOURCE_DIR}/AudioSplitter.h
        ${APP_SOURCE_DIR}/AudioToCV.cpp
        ${APP_SOURCE_DIR}/AudioToCV.h
        ${APP_SOURCE_DIR}/AudioToPulse.cpp
        ${APP_SOURCE_DIR}/AudioToPulse.h
        ${APP_SOURCE_DIR}/Autotalent.cpp
        ${APP_SOURCE_DIR}/Autotalent.h
        ${APP_SOURCE_DIR}/BandVocoder.cpp
        ${APP_SOURCE_DIR}/BandVocoder.h
        ${APP_SOURCE_DIR}/BassLineSequencer.cpp
        ${APP_SOURCE_DIR}/BassLineSequencer.h
        ${APP_SOURCE_DIR}/BeatBloks.cpp
        ${APP_SOURCE_DIR}/BeatBloks.h
        ${APP_SOURCE_DIR}/Beats.cpp
        ${APP_SOURCE_DIR}/Beats.h
        ${APP_SOURCE_DIR}/BiquadFilter.cpp
        ${APP_SOURCE_DIR}/BiquadFilter.h
        ${APP_SOURCE_DIR}/BiquadFilterEffect.cpp
        ${APP_SOURCE_DIR}/BiquadFilterEffect.h
        ${APP_SOURCE_DIR}/BitcrushEffect.cpp
        ${APP_SOURCE_DIR}/BitcrushEffect.h
        ${APP_SOURCE_DIR}/BoundsToPulse.cpp
        ${APP_SOURCE_DIR}/BoundsToPulse.h
        ${APP_SOURCE_DIR}/BufferShuffler.cpp
        ${APP_SOURCE_DIR}/BufferShuffler.h
        ${APP_SOURCE_DIR}/ButterworthFilterEffect.cpp
        ${APP_SOURCE_DIR}/ButterworthFilterEffect.h
        ${APP_SOURCE_DIR}/Canvas.cpp
        ${APP_SOURCE_DIR}/Canvas.h
        ${APP_SOURCE_DIR}/CanvasControls.cpp
        ${APP_SOURCE_DIR}/CanvasControls.h
        ${APP_SOURCE_DIR}/CanvasElement.cpp
        ${APP_SOURCE_DIR}/CanvasElement.h
        ${APP_SOURCE_DIR}/CanvasScrollbar.cpp
        ${APP_SOURCE_DIR}/CanvasScrollbar.h
        ${APP_SOURCE_DIR}/CanvasTimeline.cpp
        ${APP_SOURCE_DIR}/CanvasTimeline.h
        ${APP_SOURCE_DIR}/Capo.cpp
        ${APP_SOURCE_DIR}/Capo.h
        ${APP_SOURCE_DIR}/ChannelBuffer.cpp
        ${APP_SOURCE_DIR}/ChannelBuffer.h
        ${APP_SOURCE_DIR}/ChaosEngine.cpp
        ${APP_SOURCE_DIR}/ChaosEngine.h
        ${APP_SOURCE_DIR}/Checkbox.cpp
        ${APP_SOURCE_DIR}/Checkbox.h
        ${APP_SOURCE_DIR}/Chord.cpp
        ${APP_SOURCE_DIR}/Chord.h
        ${APP_SOURCE_DIR}/ChordDatabase.cpp
        ${APP_SOURCE_DIR}/ChordDatabase.h
        ${APP_SOURCE_DIR}/ChordDisplayer.cpp
        ${APP_SOURCE_DIR}/ChordDisplayer.h
        ${APP_SOURCE_DIR}/Chorder.cpp
        ${APP_SOURCE_DIR}/Chorder.h
        ${APP_SOURCE_DIR}/ChordHolder.cpp
        ${APP_SOURCE_DIR}/ChordHolder.h
        ${APP_SOURCE_DIR}/CircleSequencer.cpp
        ${APP_SOURCE_DIR}/CircleSequencer.h
        ${APP_SOURCE_DIR}/ClickButton.cpp
        ${APP_SOURCE_DIR}/ClickButton.h
        ${APP_SOURCE_DIR}/ClipArranger.cpp
        ${APP_SOURCE_DIR}/ClipArranger.h
        ${APP_SOURCE_DIR}/ClipLauncher.cpp
        ${APP_SOURCE_DIR}/ClipLauncher.h
        ${APP_SOURCE_DIR}/CodeEntry.cpp
        ${APP_SOURCE_DIR}/CodeEntry.h
        ${APP_SOURCE_DIR}/ComboGridController.cpp
        ${APP_SOURCE_DIR}/ComboGridController.h
        ${APP_SOURCE_DIR}/CommentDisplay.cpp
        ${APP_SOURCE_DIR}/CommentDisplay.h
        ${APP_SOURCE_DIR}/Compressor.cpp
        ${APP_SOURCE_DIR}/Compressor.h
        ${APP_SOURCE_DIR}/ControlInterface.cpp
        ${APP_SOURCE_DIR}/ControlInterface.h
        ${APP_SOURCE_DIR}/ControllingSong.cpp
        ${APP_SOURCE_DIR}/ControllingSong.h
        ${APP_SOURCE_DIR}/ControlRecorder.cpp
        ${APP_SOURCE_DIR}/ControlRecorder.h
        ${APP_SOURCE_DIR}/ControlSequencer.cpp
        ${APP_SOURCE_DIR}/ControlSequencer.h
        ${APP_SOURCE_DIR}/ControlTactileFeedback.cpp
        ${APP_SOURCE_DIR}/ControlTactileFeedback.h
        ${APP_SOURCE_DIR}/Curve.cpp
        ${APP_SOURCE_DIR}/Curve.h
        ${APP_SOURCE_DIR}/CurveLooper.cpp
        ${APP_SOURCE_DIR}/CurveLooper.h
        ${APP_SOURCE_DIR}/DataProvider.cpp
        ${APP_SOURCE_DIR}/DataProvider.h
        ${APP_SOURCE_DIR}/DCOffset.cpp
        ${APP_SOURCE_DIR}/DCOffset.h
        ${APP_SOURCE_DIR}/DCRemoverEffect.cpp
        ${APP_SOURCE_DIR}/DCRemoverEffect.h
        ${APP_SOURCE_DIR}/DebugAudioSource.cpp
        ${APP_SOURCE_DIR}/DebugAudioSource.h
        ${APP_SOURCE_DIR}/DelayEffect.cpp
        ${APP_SOURCE_DIR}/DelayEffect.h
        ${APP_SOURCE_DIR}/DistortionEffect.cpp
        ${APP_SOURCE_DIR}/DistortionEffect.h
        ${APP_SOURCE_DIR}/DotGrid.cpp
        ${APP_SOURCE_DIR}/DotGrid.h
        ${APP_SOURCE_DIR}/DotSequencer.cpp
        ${APP_SOURCE_DIR}/DotSequencer.h
        ${APP_SOURCE_DIR}/DropdownList.cpp
        ${APP_SOURCE_DIR}/DropdownList.h
        ${APP_SOURCE_DIR}/DrumPlayer.cpp
        ${APP_SOURCE_DIR}/DrumPlayer.h
        ${APP_SOURCE_DIR}/DrumSynth.cpp
        ${APP_SOURCE_DIR}/DrumSynth.h
        ${APP_SOURCE_DIR}/EffectChain.cpp
        ${APP_SOURCE_DIR}/EffectChain.h
        ${APP_SOURCE_DIR}/EffectFactory.cpp
        ${APP_SOURCE_DIR}/EffectFactory.h
        ${APP_SOURCE_DIR}/EnvelopeEditor.cpp
        ${APP_SOURCE_DIR}/EnvelopeEditor.h
        ${APP_SOURCE_DIR}/EnvelopeModulator.cpp
        ${APP_SOURCE_DIR}/EnvelopeModulator.h
        ${APP_SOURCE_DIR}/EnvOscillator.cpp
        ${APP_SOURCE_DIR}/EnvOscillator.h
        ${APP_SOURCE_DIR}/EQEffect.cpp
        ${APP_SOURCE_DIR}/EQEffect.h
        ${APP_SOURCE_DIR}/EQModule.cpp
        ${APP_SOURCE_DIR}/EQModule.h
        ${APP_SOURCE_DIR}/EuclideanSequencer.cpp
        ${APP_SOURCE_DIR}/EuclideanSequencer.h
        ${APP_SOURCE_DIR}/EventCanvas.cpp
        ${APP_SOURCE_DIR}/EventCanvas.h
        ${APP_SOURCE_DIR}/FeedbackModule.cpp
        ${APP_SOURCE_DIR}/FeedbackModule.h
        ${APP_SOURCE_DIR}/FFT.cpp
        ${APP_SOURCE_DIR}/FFT.h
        ${APP_SOURCE_DIR}/FFTtoAdditive.cpp
        ${APP_SOURCE_DIR}/FFTtoAdditive.h
        ${APP_SOURCE_DIR}/FileStream.cpp
        ${APP_SOURCE_DIR}/FileStream.h
        ${APP_SOURCE_DIR}/FillSaveDropdown.h
        ${APP_SOURCE_DIR}/FilterButterworth24db.cpp
        ${APP_SOURCE_DIR}/FilterButterworth24db.h
        ${APP_SOURCE_DIR}/FilterViz.cpp
        ${APP_SOURCE_DIR}/FilterViz.h
        ${APP_SOURCE_DIR}/FloatSliderLFOControl.cpp
        ${APP_SOURCE_DIR}/FloatSliderLFOControl.h
        ${APP_SOURCE_DIR}/FMSynth.cpp
        ${APP_SOURCE_DIR}/FMSynth.h
        ${APP_SOURCE_DIR}/FMVoice.cpp
        ${APP_SOURCE_DIR}/FMVoice.h
        ${APP_SOURCE_DIR}/FollowingSong.cpp
        ${APP_SOURCE_DIR}/FollowingSong.h
        ${APP_SOURCE_DIR}/FormantFilterEffect.cpp
        ${APP_SOURCE_DIR}/FormantFilterEffect.h
        ${APP_SOURCE_DIR}/FourOnTheFloor.cpp
        ${APP_SOURCE_DIR}/FourOnTheFloor.h
        ${APP_SOURCE_DIR}/FreeverbEffect.cpp
        ${APP_SOURCE_DIR}/FreeverbEffect.h
        ${APP_SOURCE_DIR}/FreqDelay.cpp
        ${APP_SOURCE_DIR}/FreqDelay.h
        ${APP_SOURCE_DIR}/FreqDomainBoilerplate.cpp
        ${APP_SOURCE_DIR}/FreqDomainBoilerplate.h
        ${APP_SOURCE_DIR}/FubbleModule.cpp
        ${APP_SOURCE_DIR}/FubbleModule.h
        ${APP_SOURCE_DIR}/GainStageEffect.cpp
        ${APP_SOURCE_DIR}/GainStageEffect.h
        ${APP_SOURCE_DIR}/GateEffect.cpp
        ${APP_SOURCE_DIR}/GateEffect.h
        ${APP_SOURCE_DIR}/GlobalControls.cpp
        ${APP_SOURCE_DIR}/GlobalControls.h
        ${APP_SOURCE_DIR}/Granulator.cpp
        ${APP_SOURCE_DIR}/Granulator.h
        ${APP_SOURCE_DIR}/GridController.cpp
        ${APP_SOURCE_DIR}/GridController.h
        ${APP_SOURCE_DIR}/GridModule.cpp
        ${APP_SOURCE_DIR}/GridModule.h
        ${APP_SOURCE_DIR}/GridSliders.cpp
        ${APP_SOURCE_DIR}/GridSliders.h
        ${APP_SOURCE_DIR}/GroupControl.cpp
        ${APP_SOURCE_DIR}/GroupControl.h
        ${APP_SOURCE_DIR}/HelpDisplay.cpp
        ${APP_SOURCE_DIR}/HelpDisplay.h
        ${APP_SOURCE_DIR}/IAudioEffect.h
        ${APP_SOURCE_DIR}/IAudioPoller.h
        ${APP_SOURCE_DIR}/IAudioProcessor.cpp
        ${APP_SOURCE_DIR}/IAudioProcessor.h
        ${APP_SOURCE_DIR}/IAudioReceiver.cpp
        ${APP_SOURCE_DIR}/IAudioReceiver.h
        ${APP_SOURCE_DIR}/IAudioSource.cpp
        ${APP_SOURCE_DIR}/IAudioSource.h
        ${APP_SOURCE_DIR}/IClickable.cpp
        ${APP_SOURCE_DIR}/IClickable.h
        ${APP_SOURCE_DIR}/IDrawableModule.cpp
        ${APP_SOURCE_DIR}/IDrawableModule.h
        ${APP_SOURCE_DIR}/IDrivableSequencer.h
        ${APP_SOURCE_DIR}/IInputRecordable.h
        ${APP_SOURCE_DIR}/IMidiVoice.h
        ${APP_SOURCE_DIR}/IModulator.cpp
        ${APP_SOURCE_DIR}/IModulator.h
        ${APP_SOURCE_DIR}/INonstandardController.h
        ${APP_SOURCE_DIR}/INoteReceiver.cpp
        ${APP_SOURCE_DIR}/INoteReceiver.h
        ${APP_SOURCE_DIR}/INoteSource.cpp
        ${APP_SOURCE_DIR}/INoteSource.h
        ${APP_SOURCE_DIR}/InputChannel.cpp
        ${APP_SOURCE_DIR}/InputChannel.h
        ${APP_SOURCE_DIR}/Inverter.cpp
        ${APP_SOURCE_DIR}/Inverter.h
        ${APP_SOURCE_DIR}/IPatchable.h
        ${APP_SOURCE_DIR}/IPollable.h
        ${APP_SOURCE_DIR}/IPulseReceiver.cpp
        ${APP_SOURCE_DIR}/IPulseReceiver.h
        ${APP_SOURCE_DIR}/IUIControl.cpp
        ${APP_SOURCE_DIR}/IUIControl.h
        ${APP_SOURCE_DIR}/IVoiceParams.h
        ${APP_SOURCE_DIR}/JumpBlender.cpp
        ${APP_SOURCE_DIR}/JumpBlender.h
        ${APP_SOURCE_DIR}/KarplusStrong.cpp
        ${APP_SOURCE_DIR}/KarplusStrong.h
        ${APP_SOURCE_DIR}/KarplusStrongVoice.cpp
        ${APP_SOURCE_DIR}/KarplusStrongVoice.h
        ${APP_SOURCE_DIR}/KeyboardDisplay.cpp
        ${APP_SOURCE_DIR}/KeyboardDisplay.h
        ${APP_SOURCE_DIR}/Kicker.cpp
        ${APP_SOURCE_DIR}/Kicker.h
        ${APP_SOURCE_DIR}/KompleteKontrol.cpp
        ${APP_SOURCE_DIR}/KompleteKontrol.h
        ${APP_SOURCE_DIR}/LabelDisplay.cpp
        ${APP_SOURCE_DIR}/LabelDisplay.h
        ${APP_SOURCE_DIR}/LatencyCalculator.cpp
        ${APP_SOURCE_DIR}/LatencyCalculator.h
        ${APP_SOURCE_DIR}/LaunchpadInterpreter.cpp
        ${APP_SOURCE_DIR}/LaunchpadInterpreter.h
        ${APP_SOURCE_DIR}/LaunchpadKeyboard.cpp
        ${APP_SOURCE_DIR}/LaunchpadKeyboard.h
        ${APP_SOURCE_DIR}/LaunchpadNoteDisplayer.cpp
        ${APP_SOURCE_DIR}/LaunchpadNoteDisplayer.h
        ${APP_SOURCE_DIR}/LevelMeterDisplay.cpp
        ${APP_SOURCE_DIR}/LevelMeterDisplay.h
        ${APP_SOURCE_DIR}/LFO.cpp
        ${APP_SOURCE_DIR}/LFO.h
        ${APP_SOURCE_DIR}/LFOController.cpp
        ${APP_SOURCE_DIR}/LFOController.h
        ${APP_SOURCE_DIR}/LinkwitzRileyFilter.cpp
        ${APP_SOURCE_DIR}/LinkwitzRileyFilter.h
        ${APP_SOURCE_DIR}/LinnstrumentControl.cpp
        ${APP_SOURCE_DIR}/LinnstrumentControl.h
        ${APP_SOURCE_DIR}/Lissajous.cpp
        ${APP_SOURCE_DIR}/Lissajous.h
        ${APP_SOURCE_DIR}/LiveGranulator.cpp
        ${APP_SOURCE_DIR}/LiveGranulator.h
        ${APP_SOURCE_DIR}/LocationZoomer.cpp
        ${APP_SOURCE_DIR}/LocationZoomer.h
        ${APP_SOURCE_DIR}/LockFreeQueue.h
        ${APP_SOURCE_DIR}/Looper.cpp
        ${APP_SOURCE_DIR}/Looper.h
        ${APP_SOURCE_DIR}/LooperGranulator.cpp
        ${APP_SOURCE_DIR}/LooperGranulator.h
        ${APP_SOURCE_DIR}/LooperRecorder.cpp
        ${APP_SOURCE_DIR}/LooperRecorder.h
        ${APP_SOURCE_DIR}/LoopStorer.cpp
        ${APP_SOURCE_DIR}/LoopStorer.h
        ${APP_SOURCE_DIR}/M185Sequencer.cpp
        ${APP_SOURCE_DIR}/M185Sequencer.h
        ${APP_SOURCE_DIR}/MacroSlider.cpp
        ${APP_SOURCE_DIR}/MacroSlider.h
        ${APP_SOURCE_DIR}/Main.cpp
        ${APP_SOURCE_DIR}/MainComponent.cpp
        ${APP_SOURCE_DIR}/MathUtils.cpp
        ${APP_SOURCE_DIR}/MathUtils.h
        ${APP_SOURCE_DIR}/Metronome.cpp
        ${APP_SOURCE_DIR}/Metronome.h
        ${APP_SOURCE_DIR}/MidiCapturer.cpp
        ${APP_SOURCE_DIR}/MidiCapturer.h
        ${APP_SOURCE_DIR}/MidiClockIn.cpp
        ${APP_SOURCE_DIR}/MidiClockIn.h
        ${APP_SOURCE_DIR}/MidiClockOut.cpp
        ${APP_SOURCE_DIR}/MidiClockOut.h
        ${APP_SOURCE_DIR}/MidiControlChange.cpp
        ${APP_SOURCE_DIR}/MidiControlChange.h
        ${APP_SOURCE_DIR}/MidiController.cpp
        ${APP_SOURCE_DIR}/MidiController.h
        ${APP_SOURCE_DIR}/MidiDevice.cpp
        ${APP_SOURCE_DIR}/MidiDevice.h
        ${APP_SOURCE_DIR}/MidiOutput.cpp
        ${APP_SOURCE_DIR}/MidiOutput.h
        ${APP_SOURCE_DIR}/MidiReader.cpp
        ${APP_SOURCE_DIR}/MidiReader.h
        ${APP_SOURCE_DIR}/Minimap.cpp
        ${APP_SOURCE_DIR}/Minimap.h
        ${APP_SOURCE_DIR}/ModularSynth.cpp
        ${APP_SOURCE_DIR}/ModularSynth.h
        ${APP_SOURCE_DIR}/ModulationChain.cpp
        ${APP_SOURCE_DIR}/ModulationChain.h
        ${APP_SOURCE_DIR}/ModulationVisualizer.cpp
        ${APP_SOURCE_DIR}/ModulationVisualizer.h
        ${APP_SOURCE_DIR}/ModulatorAccum.cpp
        ${APP_SOURCE_DIR}/ModulatorAccum.h
        ${APP_SOURCE_DIR}/ModulatorAdd.cpp
        ${APP_SOURCE_DIR}/ModulatorAdd.h
        ${APP_SOURCE_DIR}/ModulatorAddCentered.cpp
        ${APP_SOURCE_DIR}/ModulatorAddCentered.h
        ${APP_SOURCE_DIR}/ModulatorCurve.cpp
        ${APP_SOURCE_DIR}/ModulatorCurve.h
        ${APP_SOURCE_DIR}/ModulatorExpression.cpp
        ${APP_SOURCE_DIR}/ModulatorExpression.h
        ${APP_SOURCE_DIR}/ModulatorGravity.cpp
        ${APP_SOURCE_DIR}/ModulatorGravity.h
        ${APP_SOURCE_DIR}/ModulatorMult.cpp
        ${APP_SOURCE_DIR}/ModulatorMult.h
        ${APP_SOURCE_DIR}/ModulatorSmoother.cpp
        ${APP_SOURCE_DIR}/ModulatorSmoother.h
        ${APP_SOURCE_DIR}/ModulatorSubtract.cpp
        ${APP_SOURCE_DIR}/ModulatorSubtract.h
        ${APP_SOURCE_DIR}/ModulatorWander.cpp
        ${APP_SOURCE_DIR}/ModulatorWander.h
        ${APP_SOURCE_DIR}/ModuleContainer.cpp
        ${APP_SOURCE_DIR}/ModuleContainer.h
        ${APP_SOURCE_DIR}/ModuleFactory.cpp
        ${APP_SOURCE_DIR}/ModuleFactory.h
        ${APP_SOURCE_DIR}/ModuleSaveData.cpp
        ${APP_SOURCE_DIR}/ModuleSaveData.h
        ${APP_SOURCE_DIR}/ModuleSaveDataPanel.cpp
        ${APP_SOURCE_DIR}/ModuleSaveDataPanel.h
        ${APP_SOURCE_DIR}/ModWheel.cpp
        ${APP_SOURCE_DIR}/ModWheel.h
        ${APP_SOURCE_DIR}/ModWheelToCV.cpp
        ${APP_SOURCE_DIR}/ModWheelToCV.h
        ${APP_SOURCE_DIR}/ModwheelToPressure.cpp
        ${APP_SOURCE_DIR}/ModwheelToPressure.h
        ${APP_SOURCE_DIR}/ModwheelToVibrato.cpp
        ${APP_SOURCE_DIR}/ModwheelToVibrato.h
        ${APP_SOURCE_DIR}/Monome.cpp
        ${APP_SOURCE_DIR}/Monome.h
        ${APP_SOURCE_DIR}/Monophonify.cpp
        ${APP_SOURCE_DIR}/Monophonify.h
        ${APP_SOURCE_DIR}/MPESmoother.cpp
        ${APP_SOURCE_DIR}/MPESmoother.h
        ${APP_SOURCE_DIR}/MPETweaker.cpp
        ${APP_SOURCE_DIR}/MPETweaker.h
        ${APP_SOURCE_DIR}/MultibandCompressor.cpp
        ${APP_SOURCE_DIR}/MultibandCompressor.h
        ${APP_SOURCE_DIR}/MultiBandTracker.cpp
        ${APP_SOURCE_DIR}/MultiBandTracker.h
        ${APP_SOURCE_DIR}/MultitapDelay.cpp
        ${APP_SOURCE_DIR}/MultitapDelay.h
        ${APP_SOURCE_DIR}/MultitrackRecorder.cpp
        ${APP_SOURCE_DIR}/MultitrackRecorder.h
        ${APP_SOURCE_DIR}/Muter.cpp
        ${APP_SOURCE_DIR}/Muter.h
        ${APP_SOURCE_DIR}/NamedMutex.cpp
        ${APP_SOURCE_DIR}/NamedMutex.h
        ${APP_SOURCE_DIR}/Neighborhooder.cpp
        ${APP_SOURCE_DIR}/Neighborhooder.h
        ${APP_SOURCE_DIR}/NoiseEffect.cpp
        ${APP_SOURCE_DIR}/NoiseEffect.h
        ${APP_SOURCE_DIR}/NoteCanvas.cpp
        ${APP_SOURCE_DIR}/NoteCanvas.h
        ${APP_SOURCE_DIR}/NoteChainNode.cpp
        ${APP_SOURCE_DIR}/NoteChainNode.h
        ${APP_SOURCE_DIR}/NoteChance.cpp
        ${APP_SOURCE_DIR}/NoteChance.h
        ${APP_SOURCE_DIR}/NoteCounter.cpp
        ${APP_SOURCE_DIR}/NoteCounter.h
        ${APP_SOURCE_DIR}/NoteCreator.cpp
        ${APP_SOURCE_DIR}/NoteCreator.h
        ${APP_SOURCE_DIR}/NoteDelayer.cpp
        ${APP_SOURCE_DIR}/NoteDelayer.h
        ${APP_SOURCE_DIR}/NoteDisplayer.cpp
        ${APP_SOURCE_DIR}/NoteDisplayer.h
        ${APP_SOURCE_DIR}/NoteEcho.cpp
        ${APP_SOURCE_DIR}/NoteEcho.h
        ${APP_SOURCE_DIR}/NoteEffectBase.h
        ${APP_SOURCE_DIR}/NoteExpressionRouter.cpp
        ${APP_SOURCE_DIR}/NoteExpressionRouter.h
        ${APP_SOURCE_DIR}/NoteFilter.cpp
        ${APP_SOURCE_DIR}/NoteFilter.h
        ${APP_SOURCE_DIR}/NoteFlusher.cpp
        ${APP_SOURCE_DIR}/NoteFlusher.h
        ${APP_SOURCE_DIR}/NoteGate.cpp
        ${APP_SOURCE_DIR}/NoteGate.h
        ${APP_SOURCE_DIR}/NoteHocket.cpp
        ${APP_SOURCE_DIR}/NoteHocket.h
        ${APP_SOURCE_DIR}/NoteHumanizer.cpp
        ${APP_SOURCE_DIR}/NoteHumanizer.h
        ${APP_SOURCE_DIR}/NoteLatch.cpp
        ${APP_SOURCE_DIR}/NoteLatch.h
        ${APP_SOURCE_DIR}/NoteLooper.cpp
        ${APP_SOURCE_DIR}/NoteLooper.h
        ${APP_SOURCE_DIR}/NoteOctaver.cpp
        ${APP_SOURCE_DIR}/NoteOctaver.h
        ${APP_SOURCE_DIR}/NoteOutputQueue.cpp
        ${APP_SOURCE_DIR}/NoteOutputQueue.h
        ${APP_SOURCE_DIR}/NotePanAlternator.cpp
        ${APP_SOURCE_DIR}/NotePanAlternator.h
        ${APP_SOURCE_DIR}/NotePanner.cpp
        ${APP_SOURCE_DIR}/NotePanner.h
        ${APP_SOURCE_DIR}/NotePanRandom.cpp
        ${APP_SOURCE_DIR}/NotePanRandom.h
        ${APP_SOURCE_DIR}/NoteQuantizer.cpp
        ${APP_SOURCE_DIR}/NoteQuantizer.h
        ${APP_SOURCE_DIR}/NoteRangeFilter.cpp
        ${APP_SOURCE_DIR}/NoteRangeFilter.h
        ${APP_SOURCE_DIR}/NoteRatchet.cpp
        ${APP_SOURCE_DIR}/NoteRatchet.h
        ${APP_SOURCE_DIR}/NoteRouter.cpp
        ${APP_SOURCE_DIR}/NoteRouter.h
        ${APP_SOURCE_DIR}/NoteSinger.cpp
        ${APP_SOURCE_DIR}/NoteSinger.h
        ${APP_SOURCE_DIR}/NoteSorter.cpp
        ${APP_SOURCE_DIR}/NoteSorter.h
        ${APP_SOURCE_DIR}/NoteStepper.cpp
        ${APP_SOURCE_DIR}/NoteStepper.h
        ${APP_SOURCE_DIR}/NoteStepSequencer.cpp
        ${APP_SOURCE_DIR}/NoteStepSequencer.h
        ${APP_SOURCE_DIR}/NoteStreamDisplay.cpp
        ${APP_SOURCE_DIR}/NoteStreamDisplay.h
        ${APP_SOURCE_DIR}/NoteStrummer.cpp
        ${APP_SOURCE_DIR}/NoteStrummer.h
        ${APP_SOURCE_DIR}/NoteSustain.cpp
        ${APP_SOURCE_DIR}/NoteSustain.h
        ${APP_SOURCE_DIR}/NoteTable.cpp
        ${APP_SOURCE_DIR}/NoteTable.h
        ${APP_SOURCE_DIR}/NoteToFreq.cpp
        ${APP_SOURCE_DIR}/NoteToFreq.h
        ${APP_SOURCE_DIR}/NoteToggle.cpp
        ${APP_SOURCE_DIR}/NoteToggle.h
        ${APP_SOURCE_DIR}/NoteToMs.cpp
        ${APP_SOURCE_DIR}/NoteToMs.h
        ${APP_SOURCE_DIR}/NoteToPulse.cpp
        ${APP_SOURCE_DIR}/NoteToPulse.h
        ${APP_SOURCE_DIR}/NoteTransformer.cpp
        ${APP_SOURCE_DIR}/NoteTransformer.h
        ${APP_SOURCE_DIR}/NoteVibrato.cpp
        ${APP_SOURCE_DIR}/NoteVibrato.h
        ${APP_SOURCE_DIR}/ofxJSONElement.cpp
        ${APP_SOURCE_DIR}/ofxJSONElement.h
        ${APP_SOURCE_DIR}/OpenFrameworksPort.cpp
        ${APP_SOURCE_DIR}/OpenFrameworksPort.h
        ${APP_SOURCE_DIR}/OscController.cpp
        ${APP_SOURCE_DIR}/OscController.h
        ${APP_SOURCE_DIR}/Oscillator.cpp
        ${APP_SOURCE_DIR}/Oscillator.h
        ${APP_SOURCE_DIR}/OSCOutput.cpp
        ${APP_SOURCE_DIR}/OSCOutput.h
        ${APP_SOURCE_DIR}/OutputChannel.cpp
        ${APP_SOURCE_DIR}/OutputChannel.h
        ${APP_SOURCE_DIR}/PanicButton.cpp
        ${APP_SOURCE_DIR}/PanicButton.h
        ${APP_SOURCE_DIR}/Panner.cpp
        ${APP_SOURCE_DIR}/Panner.h
        ${APP_SOURCE_DIR}/PatchCable.cpp
        ${APP_SOURCE_DIR}/PatchCable.h
        ${APP_SOURCE_DIR}/PatchCableSource.cpp
        ${APP_SOURCE_DIR}/PatchCableSource.h
        ${APP_SOURCE_DIR}/PeakTracker.cpp
        ${APP_SOURCE_DIR}/PeakTracker.h
        ${APP_SOURCE_DIR}/PerformanceTimer.cpp
        ${APP_SOURCE_DIR}/PerformanceTimer.h
        ${APP_SOURCE_DIR}/PerlinNoise.cpp
        ${APP_SOURCE_DIR}/PerlinNoise.h
        ${APP_SOURCE_DIR}/PitchBender.cpp
        ${APP_SOURCE_DIR}/PitchBender.h
        ${APP_SOURCE_DIR}/PitchChorus.cpp
        ${APP_SOURCE_DIR}/PitchChorus.h
        ${APP_SOURCE_DIR}/PitchDetector.cpp
        ${APP_SOURCE_DIR}/PitchDetector.h
        ${APP_SOURCE_DIR}/PitchDive.cpp
        ${APP_SOURCE_DIR}/PitchDive.h
        ${APP_SOURCE_DIR}/PitchPanner.cpp
        ${APP_SOURCE_DIR}/PitchPanner.h
        ${APP_SOURCE_DIR}/PitchRemap.cpp
        ${APP_SOURCE_DIR}/PitchRemap.h
        ${APP_SOURCE_DIR}/PitchSetter.cpp
        ${APP_SOURCE_DIR}/PitchSetter.h
        ${APP_SOURCE_DIR}/PitchShiftEffect.cpp
        ${APP_SOURCE_DIR}/PitchShiftEffect.h
        ${APP_SOURCE_DIR}/PitchShifter.cpp
        ${APP_SOURCE_DIR}/PitchShifter.h
        ${APP_SOURCE_DIR}/PitchToCV.cpp
        ${APP_SOURCE_DIR}/PitchToCV.h
        ${APP_SOURCE_DIR}/PitchToSpeed.cpp
        ${APP_SOURCE_DIR}/PitchToSpeed.h
        ${APP_SOURCE_DIR}/PitchToValue.cpp
        ${APP_SOURCE_DIR}/PitchToValue.h
        ${APP_SOURCE_DIR}/PlaySequencer.cpp
        ${APP_SOURCE_DIR}/PlaySequencer.h
        ${APP_SOURCE_DIR}/PolyphonyMgr.cpp
        ${APP_SOURCE_DIR}/PolyphonyMgr.h
        ${APP_SOURCE_DIR}/Polyrhythms.cpp
        ${APP_SOURCE_DIR}/Polyrhythms.h
        ${APP_SOURCE_DIR}/Prefab.cpp
        ${APP_SOURCE_DIR}/Prefab.h
        ${APP_SOURCE_DIR}/Pressure.cpp
        ${APP_SOURCE_DIR}/Pressure.h
        ${APP_SOURCE_DIR}/PressureToCV.cpp
        ${APP_SOURCE_DIR}/PressureToCV.h
        ${APP_SOURCE_DIR}/PressureToModwheel.cpp
        ${APP_SOURCE_DIR}/PressureToModwheel.h
        ${APP_SOURCE_DIR}/PressureToVibrato.cpp
        ${APP_SOURCE_DIR}/PressureToVibrato.h
        ${APP_SOURCE_DIR}/PreviousNote.cpp
        ${APP_SOURCE_DIR}/PreviousNote.h
        ${APP_SOURCE_DIR}/Producer.cpp
        ${APP_SOURCE_DIR}/Producer.h
        ${APP_SOURCE_DIR}/Profiler.cpp
        ${APP_SOURCE_DIR}/Profiler.h
        ${APP_SOURCE_DIR}/PSMoveController.cpp
        ${APP_SOURCE_DIR}/PSMoveController.h
        ${APP_SOURCE_DIR}/PSMoveMgr.cpp
        ${APP_SOURCE_DIR}/PSMoveMgr.h
        ${APP_SOURCE_DIR}/PulseButton.cpp
        ${APP_SOURCE_DIR}/PulseButton.h
        ${APP_SOURCE_DIR}/PulseChance.cpp
        ${APP_SOURCE_DIR}/PulseChance.h
        ${APP_SOURCE_DIR}/PulseDelayer.cpp
        ${APP_SOURCE_DIR}/PulseDelayer.h
        ${APP_SOURCE_DIR}/PulseDisplayer.cpp
        ${APP_SOURCE_DIR}/PulseDisplayer.h
        ${APP_SOURCE_DIR}/PulseFlag.cpp
        ${APP_SOURCE_DIR}/PulseFlag.h
        ${APP_SOURCE_DIR}/PulseGate.cpp
        ${APP_SOURCE_DIR}/PulseGate.h
        ${APP_SOURCE_DIR}/PulseHocket.cpp
        ${APP_SOURCE_DIR}/PulseHocket.h
        ${APP_SOURCE_DIR}/PulseLimit.cpp
        ${APP_SOURCE_DIR}/PulseLimit.h
        ${APP_SOURCE_DIR}/Pulser.cpp
        ${APP_SOURCE_DIR}/Pulser.h
        ${APP_SOURCE_DIR}/PulseRouter.cpp
        ${APP_SOURCE_DIR}/PulseRouter.h
        ${APP_SOURCE_DIR}/PulseSequence.cpp
        ${APP_SOURCE_DIR}/PulseSequence.h
        ${APP_SOURCE_DIR}/PulseTrain.cpp
        ${APP_SOURCE_DIR}/PulseTrain.h
        ${APP_SOURCE_DIR}/Pumper.cpp
        ${APP_SOURCE_DIR}/Pumper.h
        ${APP_SOURCE_DIR}/Push2Control.cpp
        ${APP_SOURCE_DIR}/Push2Control.h
        ${APP_SOURCE_DIR}/QuickSpawnMenu.cpp
        ${APP_SOURCE_DIR}/QuickSpawnMenu.h
        ${APP_SOURCE_DIR}/QwertyController.cpp
        ${APP_SOURCE_DIR}/QwertyController.h
        ${APP_SOURCE_DIR}/QwertyToPitchMapping.cpp
        ${APP_SOURCE_DIR}/QwertyToPitchMapping.h
        ${APP_SOURCE_DIR}/RadioButton.cpp
        ${APP_SOURCE_DIR}/RadioButton.h
        ${APP_SOURCE_DIR}/RadioSequencer.cpp
        ${APP_SOURCE_DIR}/RadioSequencer.h
        ${APP_SOURCE_DIR}/Ramp.cpp
        ${APP_SOURCE_DIR}/Ramp.h
        ${APP_SOURCE_DIR}/Ramper.cpp
        ${APP_SOURCE_DIR}/Ramper.h
        ${APP_SOURCE_DIR}/RandomNoteGenerator.cpp
        ${APP_SOURCE_DIR}/RandomNoteGenerator.h
        ${APP_SOURCE_DIR}/Razor.cpp
        ${APP_SOURCE_DIR}/Razor.h
        ${APP_SOURCE_DIR}/Rewriter.cpp
        ${APP_SOURCE_DIR}/Rewriter.h
        ${APP_SOURCE_DIR}/RhythmSequencer.cpp
        ${APP_SOURCE_DIR}/RhythmSequencer.h
        ${APP_SOURCE_DIR}/RingModulator.cpp
        ${APP_SOURCE_DIR}/RingModulator.h
        ${APP_SOURCE_DIR}/RollingBuffer.cpp
        ${APP_SOURCE_DIR}/RollingBuffer.h
        ${APP_SOURCE_DIR}/Sample.cpp
        ${APP_SOURCE_DIR}/Sample.h
        ${APP_SOURCE_DIR}/SampleBrowser.cpp
        ${APP_SOURCE_DIR}/SampleBrowser.h
        ${APP_SOURCE_DIR}/SampleCanvas.cpp
        ${APP_SOURCE_DIR}/SampleCanvas.h
        ${APP_SOURCE_DIR}/SampleCapturer.cpp
        ${APP_SOURCE_DIR}/SampleCapturer.h
        ${APP_SOURCE_DIR}/SampleDrawer.cpp
        ${APP_SOURCE_DIR}/SampleDrawer.h
        ${APP_SOURCE_DIR}/SampleFinder.cpp
        ${APP_SOURCE_DIR}/SampleFinder.h
        ${APP_SOURCE_DIR}/SampleLayerer.cpp
        ${APP_SOURCE_DIR}/SampleLayerer.h
        ${APP_SOURCE_DIR}/SamplePlayer.cpp
        ${APP_SOURCE_DIR}/SamplePlayer.h
        ${APP_SOURCE_DIR}/Sampler.cpp
        ${APP_SOURCE_DIR}/Sampler.h
        ${APP_SOURCE_DIR}/SamplerGrid.cpp
        ${APP_SOURCE_DIR}/SamplerGrid.h
        ${APP_SOURCE_DIR}/SampleVoice.cpp
        ${APP_SOURCE_DIR}/SampleVoice.h
        ${APP_SOURCE_DIR}/SaveStateLoader.cpp
        ${APP_SOURCE_DIR}/SaveStateLoader.h
        ${APP_SOURCE_DIR}/Scale.cpp
        ${APP_SOURCE_DIR}/Scale.h
        ${APP_SOURCE_DIR}/ScaleDegree.cpp
        ${APP_SOURCE_DIR}/ScaleDegree.h
        ${APP_SOURCE_DIR}/ScaleDetect.cpp
        ${APP_SOURCE_DIR}/ScaleDetect.h
        ${APP_SOURCE_DIR}/ScriptModule.cpp
        ${APP_SOURCE_DIR}/ScriptModule.h
        ${APP_SOURCE_DIR}/ScriptModule_PythonInterface.i
        ${APP_SOURCE_DIR}/ScriptStatus.cpp
        ${APP_SOURCE_DIR}/ScriptStatus.h
        ${APP_SOURCE_DIR}/SeaOfGrain.cpp
        ${APP_SOURCE_DIR}/SeaOfGrain.h
        ${APP_SOURCE_DIR}/Selector.cpp
        ${APP_SOURCE_DIR}/Selector.h
        ${APP_SOURCE_DIR}/SignalClamp.cpp
        ${APP_SOURCE_DIR}/SignalClamp.h
        ${APP_SOURCE_DIR}/SignalGenerator.cpp
        ${APP_SOURCE_DIR}/SignalGenerator.h
        ${APP_SOURCE_DIR}/SingleOscillator.cpp
        ${APP_SOURCE_DIR}/SingleOscillator.h
        ${APP_SOURCE_DIR}/SingleOscillatorVoice.cpp
        ${APP_SOURCE_DIR}/SingleOscillatorVoice.h
        ${APP_SOURCE_DIR}/Slider.cpp
        ${APP_SOURCE_DIR}/Slider.h
        ${APP_SOURCE_DIR}/SliderSequencer.cpp
        ${APP_SOURCE_DIR}/SliderSequencer.h
        ${APP_SOURCE_DIR}/SlowLayers.cpp
        ${APP_SOURCE_DIR}/SlowLayers.h
        ${APP_SOURCE_DIR}/Snapshots.cpp
        ${APP_SOURCE_DIR}/Snapshots.h
        ${APP_SOURCE_DIR}/SongBuilder.cpp
        ${APP_SOURCE_DIR}/SongBuilder.h
        ${APP_SOURCE_DIR}/SpaceMouseControl.cpp
        ${APP_SOURCE_DIR}/SpaceMouseControl.h
        ${APP_SOURCE_DIR}/SpectralDisplay.cpp
        ${APP_SOURCE_DIR}/SpectralDisplay.h
        ${APP_SOURCE_DIR}/Splitter.cpp
        ${APP_SOURCE_DIR}/Splitter.h
        ${APP_SOURCE_DIR}/StepSequencer.cpp
        ${APP_SOURCE_DIR}/StepSequencer.h
        ${APP_SOURCE_DIR}/StereoRotation.cpp
        ${APP_SOURCE_DIR}/StereoRotation.h
        ${APP_SOURCE_DIR}/Stutter.cpp
        ${APP_SOURCE_DIR}/Stutter.h
        ${APP_SOURCE_DIR}/StutterControl.cpp
        ${APP_SOURCE_DIR}/StutterControl.h
        ${APP_SOURCE_DIR}/SustainPedal.cpp
        ${APP_SOURCE_DIR}/SustainPedal.h
        ${APP_SOURCE_DIR}/SwitchAndRamp.cpp
        ${APP_SOURCE_DIR}/SwitchAndRamp.h
        ${APP_SOURCE_DIR}/SynthGlobals.cpp
        ${APP_SOURCE_DIR}/SynthGlobals.h
        ${APP_SOURCE_DIR}/TakeRecorder.cpp
        ${APP_SOURCE_DIR}/TakeRecorder.h
        ${APP_SOURCE_DIR}/TextEntry.cpp
        ${APP_SOURCE_DIR}/TextEntry.h
        ${APP_SOURCE_DIR}/TimelineControl.cpp
        ${APP_SOURCE_DIR}/TimelineControl.h
        ${APP_SOURCE_DIR}/TimerDisplay.cpp
        ${APP_SOURCE_DIR}/TimerDisplay.h
        ${APP_SOURCE_DIR}/TitleBar.cpp
        ${APP_SOURCE_DIR}/TitleBar.h
        ${APP_SOURCE_DIR}/TrackOrganizer.cpp
        ${APP_SOURCE_DIR}/TrackOrganizer.h
        ${APP_SOURCE_DIR}/Transport.cpp
        ${APP_SOURCE_DIR}/Transport.h
        ${APP_SOURCE_DIR}/TransposeFrom.cpp
        ${APP_SOURCE_DIR}/TransposeFrom.h
        ${APP_SOURCE_DIR}/TremoloEffect.cpp
        ${APP_SOURCE_DIR}/TremoloEffect.h
        ${APP_SOURCE_DIR}/TriggerDetector.cpp
        ${APP_SOURCE_DIR}/TriggerDetector.h
        ${APP_SOURCE_DIR}/UIControlMacros.h
        ${APP_SOURCE_DIR}/UIGrid.cpp
        ${APP_SOURCE_DIR}/UIGrid.h
        ${APP_SOURCE_DIR}/UnstableModWheel.cpp
        ${APP_SOURCE_DIR}/UnstableModWheel.h
        ${APP_SOURCE_DIR}/UnstablePitch.cpp
        ${APP_SOURCE_DIR}/UnstablePitch.h
        ${APP_SOURCE_DIR}/UnstablePressure.cpp
        ${APP_SOURCE_DIR}/UnstablePressure.h
        ${APP_SOURCE_DIR}/UserData.cpp
        ${APP_SOURCE_DIR}/UserData.h
        ${APP_SOURCE_DIR}/UserPrefs.cpp
        ${APP_SOURCE_DIR}/UserPrefs.h
        ${APP_SOURCE_DIR}/UserPrefsEditor.cpp
        ${APP_SOURCE_DIR}/UserPrefsEditor.h
        ${APP_SOURCE_DIR}/ValueSetter.cpp
        ${APP_SOURCE_DIR}/ValueSetter.h
        ${APP_SOURCE_DIR}/ValueStream.cpp
        ${APP_SOURCE_DIR}/ValueStream.h
        ${APP_SOURCE_DIR}/VelocityCurve.cpp
        ${APP_SOURCE_DIR}/VelocityCurve.h
        ${APP_SOURCE_DIR}/VelocityScaler.cpp
        ${APP_SOURCE_DIR}/VelocityScaler.h
        ${APP_SOURCE_DIR}/VelocitySetter.cpp
        ${APP_SOURCE_DIR}/VelocitySetter.h
        ${APP_SOURCE_DIR}/VelocityStepSequencer.cpp
        ${APP_SOURCE_DIR}/VelocityStepSequencer.h
        ${APP_SOURCE_DIR}/VelocityToChance.cpp
        ${APP_SOURCE_DIR}/VelocityToChance.h
        ${APP_SOURCE_DIR}/VelocityToCV.cpp
        ${APP_SOURCE_DIR}/VelocityToCV.h
        ${APP_SOURCE_DIR}/VersionInfo.cpp
        ${APP_SOURCE_DIR}/VersionInfo.h
        ${APP_SOURCE_DIR}/VersionInfoBld.cpp
        ${APP_SOURCE_DIR}/VinylTempoControl.cpp
        ${APP_SOURCE_DIR}/VinylTempoControl.h
        ${APP_SOURCE_DIR}/Vocoder.cpp
        ${APP_SOURCE_DIR}/Vocoder.h
        ${APP_SOURCE_DIR}/VocoderCarrierInput.cpp
        ${APP_SOURCE_DIR}/VocoderCarrierInput.h
        ${APP_SOURCE_DIR}/VoiceManager.cpp
        ${APP_SOURCE_DIR}/VoiceManager.h
        ${APP_SOURCE_DIR}/VoiceSetter.cpp
        ${APP_SOURCE_DIR}/VoiceSetter.h
        ${APP_SOURCE_DIR}/VolcaBeatsControl.cpp
        ${APP_SOURCE_DIR}/VolcaBeatsControl.h
        ${APP_SOURCE_DIR}/VSTPlayhead.cpp
        ${APP_SOURCE_DIR}/VSTPlayhead.h
        ${APP_SOURCE_DIR}/VSTPlugin.cpp
        ${APP_SOURCE_DIR}/VSTPlugin.h
        ${APP_SOURCE_DIR}/VSTScanner.cpp
        ${APP_SOURCE_DIR}/VSTScanner.h
        ${APP_SOURCE_DIR}/VSTWindow.cpp
        ${APP_SOURCE_DIR}/VSTWindow.h
        ${APP_SOURCE_DIR}/WaveformViewer.cpp
        ${APP_SOURCE_DIR}/WaveformViewer.h
        ${APP_SOURCE_DIR}/Waveshaper.cpp
        ${APP_SOURCE_DIR}/Waveshaper.h
        ${APP_SOURCE_DIR}/WhiteKeys.cpp
        ${APP_SOURCE_DIR}/WhiteKeys.h
        ${APP_SOURCE_DIR}/WindowCloseListener.h
        ${APP_SOURCE_DIR}/Xoshiro256ss.h

        # ${APP_SOURCE_DIR}/CFMessaging/KontrolKommunicator.cpp
        # ${APP_SOURCE_DIR}/CFMessaging/KontrolKommunicator.h
        # ${APP_SOURCE_DIR}/CFMessaging/ListenPort.cpp
        # ${APP_SOURCE_DIR}/CFMessaging/ListenPort.h
        # ${APP_SOURCE_DIR}/CFMessaging/NIMessage.cpp
        # ${APP_SOURCE_DIR}/CFMessaging/NIMessage.h
        # ${APP_SOURCE_DIR}/CFMessaging/SendPort.cpp
        # ${APP_SOURCE_DIR}/CFMessaging/SendPort.h
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
	  # list(APPEND APP_SOURCES "${APP_RC_DIR}/resource.qrc")
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    if (MINGW)
         set_source_files_properties( ${APP_SOURCE_DIR}/NoteExpressionRouter.cpp
                                      ${APP_SOURCE_DIR}/ModulatorExpression.cpp
                                      ${APP_SOURCE_DIR}/SynthGlobals.cpp
                                      ${APP_SOURCE_DIR}/Waveshaper.cpp
                                      PROPERTIES COMPILE_FLAGS "-Wa,-mbig-obj")
    endif()
	 
    target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR})

    # if JUCE is devendored, these libraries need to be linked against
    # target_link_libraries(${APP_NAME} PRIVATE FLAC)
    # target_link_libraries(${APP_NAME} PRIVATE ogg)
    # target_link_libraries(${APP_NAME} PRIVATE vorbis)
    # target_link_libraries(${APP_NAME} PRIVATE vorbisenc)
    # target_link_libraries(${APP_NAME} PRIVATE vorbisfile)

    # target_link_libraries(${APP_NAME} PRIVATE ${Python_LIBRARIES})
    target_link_libraries(${APP_NAME} PRIVATE dbghelp)
	
    target_link_libraries(${APP_NAME} PRIVATE de_jsoncpp)
    target_link_libraries(${APP_NAME} PRIVATE de_freeverb)
    target_link_libraries(${APP_NAME} PRIVATE de_nanovg)
    # target_link_libraries(${APP_NAME} PRIVATE de_psmove)
    # target_link_libraries(${APP_NAME} PRIVATE de_pybind11)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_audio_basics)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_audio_devices)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_audio_formats)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_audio_processors)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_cryptography)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_gui_basics)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_opengl)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_osc)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_dsp)
    # target_link_libraries(${APP_NAME} PRIVATE de_bespoke_ableton_link)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_ableton_push2)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_boost_asio_standalone)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_catch)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_catch2)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_exprtk)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_leathers)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_libusb)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_oddsound_mts_esp)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_readerwriterqueue)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_scalable_font2)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_tuning_library)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_xwax)
    # target_link_libraries(${APP_NAME} PRIVATE de_pybind11)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_psmove)
    target_link_libraries(${APP_NAME} PRIVATE de_bespoke_3dxware)


    if (WIN32)
        target_compile_definitions(${APP_NAME} PRIVATE BESPOKE_WINDOWS=1)
        target_compile_definitions(${APP_NAME} PRIVATE BESPOKE_SPACEMOUSE_SUPPORT=1)
        target_link_libraries(${APP_NAME} PRIVATE dwmapi)
        target_link_libraries(${APP_NAME} PRIVATE comctl32 user32 gdi32 kernel32 shell32)
        target_link_libraries(${APP_NAME} PRIVATE ole32 oleaut32)

        if (MINGW)
            target_link_libraries(${APP_NAME} PRIVATE iphlpapi)
        endif()
    else()
        target_link_libraries(${APP_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
    endif()

    if (MSVC)
    else()
        target_link_options(${APP_NAME} PRIVATE -mwindows)
    endif()
	
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    
    # if (CMAKE_BUILD_TYPE STREQUAL "Release")
        # message(STATUS "Building in Release mode, strip symbols")
        # --strip-debug : Removes only debugging symbols.
        # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        # add_custom_command(
            # TARGET ${APP_NAME}
            # POST_BUILD
            # COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            # COMMENT "Stripping debug symbols"
        # )
    # endif()

    file(COPY media/BespokeSynth DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
