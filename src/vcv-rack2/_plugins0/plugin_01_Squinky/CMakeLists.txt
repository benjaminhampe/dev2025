set(LIB_NAME plugin_01_Squinky)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/composites/ADSR16.h
        ${LIB_SOURCE_DIR}/composites/ADSR4.h
        ${LIB_SOURCE_DIR}/composites/ADSRSampler.h
        ${LIB_SOURCE_DIR}/composites/Basic.h
        ${LIB_SOURCE_DIR}/composites/Blank.h
        ${LIB_SOURCE_DIR}/composites/CH10.h
        ${LIB_SOURCE_DIR}/composites/ChaosKitty.h
        ${LIB_SOURCE_DIR}/composites/CHB.h
        ${LIB_SOURCE_DIR}/composites/CHBg.h
        ${LIB_SOURCE_DIR}/composites/ColoredNoise.h
        ${LIB_SOURCE_DIR}/composites/Compressor.h
        ${LIB_SOURCE_DIR}/composites/Compressor2.h
        ${LIB_SOURCE_DIR}/composites/CompressorParamHolder.h
        ${LIB_SOURCE_DIR}/composites/daveguide.h
        ${LIB_SOURCE_DIR}/composites/DividerX.h
        ${LIB_SOURCE_DIR}/composites/DrumTrigger.h
        ${LIB_SOURCE_DIR}/composites/EV3.h
        ${LIB_SOURCE_DIR}/composites/F2.h
        ${LIB_SOURCE_DIR}/composites/F2_Poly.h
        ${LIB_SOURCE_DIR}/composites/F4.h
        ${LIB_SOURCE_DIR}/composites/Filt.h
        ${LIB_SOURCE_DIR}/composites/FrequencyShifter.h
        ${LIB_SOURCE_DIR}/composites/FunVCOComposite.h
        ${LIB_SOURCE_DIR}/composites/GMR.h
        ${LIB_SOURCE_DIR}/composites/GMR2.h
        ${LIB_SOURCE_DIR}/composites/Gray.h
        ${LIB_SOURCE_DIR}/composites/IComposite.h
        ${LIB_SOURCE_DIR}/composites/KSComposite.h
        ${LIB_SOURCE_DIR}/composites/LadderFilterBank.h
        ${LIB_SOURCE_DIR}/composites/LFN.h
        ${LIB_SOURCE_DIR}/composites/LFNB.h
        ${LIB_SOURCE_DIR}/composites/Mix4.h
        ${LIB_SOURCE_DIR}/composites/Mix8.h
        ${LIB_SOURCE_DIR}/composites/MixHelper.h
        ${LIB_SOURCE_DIR}/composites/MixM.h
        ${LIB_SOURCE_DIR}/composites/mixpolyhelper.h
        ${LIB_SOURCE_DIR}/composites/MixStereo.h
        ${LIB_SOURCE_DIR}/composites/Samp.h
        ${LIB_SOURCE_DIR}/composites/Seq.h
        ${LIB_SOURCE_DIR}/composites/Seq4.h
        ${LIB_SOURCE_DIR}/composites/Shaper.h
        ${LIB_SOURCE_DIR}/composites/Sines.h
        ${LIB_SOURCE_DIR}/composites/Slew4.h
        ${LIB_SOURCE_DIR}/composites/SqPort.h
        ${LIB_SOURCE_DIR}/composites/Sub.h
        ${LIB_SOURCE_DIR}/composites/Super.h
        ${LIB_SOURCE_DIR}/composites/SuperDsp.h
        # ${LIB_SOURCE_DIR}/composites/TestComposite.h
        ${LIB_SOURCE_DIR}/composites/Tremolo.h
        ${LIB_SOURCE_DIR}/composites/VocalAnimator.h
        ${LIB_SOURCE_DIR}/composites/VocalFilter.h
        ${LIB_SOURCE_DIR}/composites/WidgetComposite.h
        ${LIB_SOURCE_DIR}/composites/WVCO.h

        ${LIB_SOURCE_DIR}/dsp/fft/FFT.cpp
        ${LIB_SOURCE_DIR}/dsp/fft/FFT.h
        ${LIB_SOURCE_DIR}/dsp/fft/FFTCrossFader.cpp
        ${LIB_SOURCE_DIR}/dsp/fft/FFTCrossFader.h
        ${LIB_SOURCE_DIR}/dsp/fft/FFTData.cpp
        ${LIB_SOURCE_DIR}/dsp/fft/FFTData.h
        ${LIB_SOURCE_DIR}/dsp/fft/FFTUtils.cpp
        ${LIB_SOURCE_DIR}/dsp/fft/FFTUtils.h
        ${LIB_SOURCE_DIR}/dsp/fft/OnsetDetector.cpp
        ${LIB_SOURCE_DIR}/dsp/fft/OnsetDetector.h

        ${LIB_SOURCE_DIR}/dsp/filters/BiquadFilter.h
        ${LIB_SOURCE_DIR}/dsp/filters/BiquadParams.h
        ${LIB_SOURCE_DIR}/dsp/filters/BiquadState.h
        ${LIB_SOURCE_DIR}/dsp/filters/ButterworthFilterDesigner.cpp
        ${LIB_SOURCE_DIR}/dsp/filters/ButterworthFilterDesigner.h
        ${LIB_SOURCE_DIR}/dsp/filters/ButterworthLookup.h
        ${LIB_SOURCE_DIR}/dsp/filters/DCBlocker.h
        ${LIB_SOURCE_DIR}/dsp/filters/FormantTables2.cpp
        ${LIB_SOURCE_DIR}/dsp/filters/FormantTables2.h
        ${LIB_SOURCE_DIR}/dsp/filters/GraphicEq.h
        ${LIB_SOURCE_DIR}/dsp/filters/HilbertFilterDesigner.cpp
        ${LIB_SOURCE_DIR}/dsp/filters/HilbertFilterDesigner.h
        ${LIB_SOURCE_DIR}/dsp/filters/LadderFilter.h
        ${LIB_SOURCE_DIR}/dsp/filters/LowpassFilter.h
        ${LIB_SOURCE_DIR}/dsp/filters/MultiLag.h
        ${LIB_SOURCE_DIR}/dsp/filters/MultiLag2.h
        ${LIB_SOURCE_DIR}/dsp/filters/SmoothedHPF.h
        ${LIB_SOURCE_DIR}/dsp/filters/StateVariable4P.h
        ${LIB_SOURCE_DIR}/dsp/filters/StateVariable4PHP.h
        ${LIB_SOURCE_DIR}/dsp/filters/StateVariableFilter.h
        ${LIB_SOURCE_DIR}/dsp/filters/StateVariableFilter2.h
        ${LIB_SOURCE_DIR}/dsp/filters/TrapezoidalLowpass.h

        ${LIB_SOURCE_DIR}/dsp/generators/BasicVCO.h
        ${LIB_SOURCE_DIR}/dsp/generators/Chaos.h
        ${LIB_SOURCE_DIR}/dsp/generators/minBlepGenerator.cpp
        ${LIB_SOURCE_DIR}/dsp/generators/MinBLEPVCO.h
        ${LIB_SOURCE_DIR}/dsp/generators/MultiModOsc.h
        ${LIB_SOURCE_DIR}/dsp/generators/SawOscillator.h
        ${LIB_SOURCE_DIR}/dsp/generators/SinesVCO.h
        ${LIB_SOURCE_DIR}/dsp/generators/SinOscillator.h
        ${LIB_SOURCE_DIR}/dsp/generators/SubVCO.h
        ${LIB_SOURCE_DIR}/dsp/generators/WVCODsp.h

        ${LIB_SOURCE_DIR}/dsp/samp/CompiledInstrument.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/CompiledInstrument.h
        ${LIB_SOURCE_DIR}/dsp/samp/CompiledRegion.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/CompiledRegion.h
        ${LIB_SOURCE_DIR}/dsp/samp/CubicInterpolator.h
        ${LIB_SOURCE_DIR}/dsp/samp/dr_wav.h
        ${LIB_SOURCE_DIR}/dsp/samp/FilePath.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/FilePath.h
        ${LIB_SOURCE_DIR}/dsp/samp/FixedPointAccumulator.h
        ${LIB_SOURCE_DIR}/dsp/samp/FlacReader.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/FlacReader.h
        ${LIB_SOURCE_DIR}/dsp/samp/HeadingTracker.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/HeadingTracker.h
        ${LIB_SOURCE_DIR}/dsp/samp/InstrumentInfo.h
        ${LIB_SOURCE_DIR}/dsp/samp/LexContext.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/LexContext.h
        ${LIB_SOURCE_DIR}/dsp/samp/RegionPool.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/RegionPool.h
        ${LIB_SOURCE_DIR}/dsp/samp/Sampler4vx.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/Sampler4vx.h
        ${LIB_SOURCE_DIR}/dsp/samp/SamplerErrorContext.h
        ${LIB_SOURCE_DIR}/dsp/samp/SamplerPlayback.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/SamplerPlayback.h
        ${LIB_SOURCE_DIR}/dsp/samp/SamplerSchema.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/SamplerSchema.h
        ${LIB_SOURCE_DIR}/dsp/samp/SamplerSharedState.h
        ${LIB_SOURCE_DIR}/dsp/samp/SInstrument.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/SInstrument.h
        ${LIB_SOURCE_DIR}/dsp/samp/SLex.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/SLex.h
        ${LIB_SOURCE_DIR}/dsp/samp/SParse.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/SParse.h
        ${LIB_SOURCE_DIR}/dsp/samp/Streamer.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/Streamer.h
        ${LIB_SOURCE_DIR}/dsp/samp/WaveLoader.cpp
        ${LIB_SOURCE_DIR}/dsp/samp/WaveLoader.h
        ${LIB_SOURCE_DIR}/dsp/samp/WaveLoaders.cpp

        ${LIB_SOURCE_DIR}/dsp/simd.h
        ${LIB_SOURCE_DIR}/dsp/SimdBlocks.h

        ${LIB_SOURCE_DIR}/dsp/third-party/falco/DspFilter.cpp
        ${LIB_SOURCE_DIR}/dsp/third-party/falco/DspFilter.h

        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/export.h

        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/FLAC/assert.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/FLAC/export.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/FLAC/format.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/FLAC/ordinals.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/FLAC/stream_decoder.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/format.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/ordinals.h

        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private/bitmath.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private/bitreader.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private/cpu.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private/crc.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private/fixed.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private/float.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private/format.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private/lpc.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private/macros.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private/md5.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private/memory.h

        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/protected/stream_decoder.h

        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/share/alloc.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/share/compat.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/share/endswap.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/share/windows_unicode_filenames.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/share/win_utf8_io.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/stream_decoder.h

        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/src/bitreader.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/src/cpu.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/src/crc.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/src/fixed.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/src/format.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/src/lpc.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/src/md5.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/src/memory.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/src/stream_decoder.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/flac/src/windows_unicode_filenames.c

        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/.hgignore
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/.hgtags
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/.hg_archival.txt
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/CHANGELOG
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/COPYING
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/kissfft.hh
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/kiss_fft.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/kiss_fft.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/Makefile
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/README
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/README.simd
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/benchfftw.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/benchkiss.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/compfft.py
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/doit.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/fastfir.py
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/fft.py
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/Makefile
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/mk_test.py
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/pstats.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/pstats.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/tailscrap.m
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/testcpp.cc
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/testkiss.py
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/test_real.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/test_vs_dft.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/twotonetest.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/TIPS
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/fftutil.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kfc.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kfc.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fastfir.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftnd.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftnd.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftndr.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftndr.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftr.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftr.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/Makefile
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/psdpng.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/_kiss_fft_guts.h

        ${LIB_SOURCE_DIR}/dsp/third-party/midifile/Binasc.cpp
        ${LIB_SOURCE_DIR}/dsp/third-party/midifile/Binasc.h
        ${LIB_SOURCE_DIR}/dsp/third-party/midifile/MidiEvent.cpp
        ${LIB_SOURCE_DIR}/dsp/third-party/midifile/MidiEvent.h
        ${LIB_SOURCE_DIR}/dsp/third-party/midifile/MidiEventList.cpp
        ${LIB_SOURCE_DIR}/dsp/third-party/midifile/MidiEventList.h
        ${LIB_SOURCE_DIR}/dsp/third-party/midifile/MidiFile.cpp
        ${LIB_SOURCE_DIR}/dsp/third-party/midifile/MidiFile.h
        ${LIB_SOURCE_DIR}/dsp/third-party/midifile/MidiMessage.cpp
        ${LIB_SOURCE_DIR}/dsp/third-party/midifile/MidiMessage.h

        ${LIB_SOURCE_DIR}/dsp/third-party/src/FunVCO.h
        ${LIB_SOURCE_DIR}/dsp/third-party/src/FunVCO3.h
        ${LIB_SOURCE_DIR}/dsp/third-party/src/minblep.cpp
        ${LIB_SOURCE_DIR}/dsp/third-party/src/SqMath.h

        ${LIB_SOURCE_DIR}/dsp/utils/ACDetector.h
        ${LIB_SOURCE_DIR}/dsp/utils/arm_intrinsics_sub.h
        ${LIB_SOURCE_DIR}/dsp/utils/AsymRampShaper.h
        ${LIB_SOURCE_DIR}/dsp/utils/AsymWaveShaper.cpp
        ${LIB_SOURCE_DIR}/dsp/utils/AsymWaveShaper.h
        ${LIB_SOURCE_DIR}/dsp/utils/AudioMath.cpp
        ${LIB_SOURCE_DIR}/dsp/utils/AudioMath.h
        ${LIB_SOURCE_DIR}/dsp/utils/AudioMath_4.h
        ${LIB_SOURCE_DIR}/dsp/utils/AutioMath_4.cpp
        ${LIB_SOURCE_DIR}/dsp/utils/ClockRecovery.h
        ${LIB_SOURCE_DIR}/dsp/utils/Cmprsr.cpp
        ${LIB_SOURCE_DIR}/dsp/utils/Cmprsr.h
        ${LIB_SOURCE_DIR}/dsp/utils/CompCurves.cpp
        ${LIB_SOURCE_DIR}/dsp/utils/CompCurves.h
        ${LIB_SOURCE_DIR}/dsp/utils/Decimator.h
        ${LIB_SOURCE_DIR}/dsp/utils/fVec.h
        ${LIB_SOURCE_DIR}/dsp/utils/GateDelay.h
        ${LIB_SOURCE_DIR}/dsp/utils/IIRDecimator.h
        ${LIB_SOURCE_DIR}/dsp/utils/IIRUpsampler.h
        ${LIB_SOURCE_DIR}/dsp/utils/Limiter.h
        ${LIB_SOURCE_DIR}/dsp/utils/LookupTable.h
        ${LIB_SOURCE_DIR}/dsp/utils/LookupTableFactory.h
        ${LIB_SOURCE_DIR}/dsp/utils/NonUniformLookupTable.h
        ${LIB_SOURCE_DIR}/dsp/utils/ObjectCache.cpp
        ${LIB_SOURCE_DIR}/dsp/utils/ObjectCache.h
        ${LIB_SOURCE_DIR}/dsp/utils/OscSmoother.h
        ${LIB_SOURCE_DIR}/dsp/utils/poly.h
        ${LIB_SOURCE_DIR}/dsp/utils/SimpleQuantizer.cpp
        ${LIB_SOURCE_DIR}/dsp/utils/SimpleQuantizer.h
        ${LIB_SOURCE_DIR}/dsp/utils/SplineRenderer.cpp
        ${LIB_SOURCE_DIR}/dsp/utils/SplineRenderer.h
        ${LIB_SOURCE_DIR}/dsp/utils/SqSchmidtTrigger.h

        ${LIB_SOURCE_DIR}/midi/controller/AuditionLocker.h
        ${LIB_SOURCE_DIR}/midi/controller/IMidiPlayerHost.h
        ${LIB_SOURCE_DIR}/midi/controller/MakeEmptyTrackCommand4.cpp
        ${LIB_SOURCE_DIR}/midi/controller/MakeEmptyTrackCommand4.h
        ${LIB_SOURCE_DIR}/midi/controller/MidiAudition.h
        ${LIB_SOURCE_DIR}/midi/controller/MidiEditor.cpp
        ${LIB_SOURCE_DIR}/midi/controller/MidiEditor.h
        ${LIB_SOURCE_DIR}/midi/controller/MidiEditorTab.cpp
        ${LIB_SOURCE_DIR}/midi/controller/MidiFileProxy.cpp
        ${LIB_SOURCE_DIR}/midi/controller/MidiFileProxy.h
        ${LIB_SOURCE_DIR}/midi/controller/MidiKeyboardHandler.cpp
        ${LIB_SOURCE_DIR}/midi/controller/MidiKeyboardHandler.h
        ${LIB_SOURCE_DIR}/midi/controller/MidiLock.cpp
        ${LIB_SOURCE_DIR}/midi/controller/MidiLock.h
        ${LIB_SOURCE_DIR}/midi/controller/MidiPlayer2.cpp
        ${LIB_SOURCE_DIR}/midi/controller/MidiPlayer2.h
        ${LIB_SOURCE_DIR}/midi/controller/MidiPlayer4.cpp
        ${LIB_SOURCE_DIR}/midi/controller/MidiPlayer4.h
        ${LIB_SOURCE_DIR}/midi/controller/MidiTrackPlayer.cpp
        ${LIB_SOURCE_DIR}/midi/controller/MidiTrackPlayer.h
        ${LIB_SOURCE_DIR}/midi/controller/MidiVoice.cpp
        ${LIB_SOURCE_DIR}/midi/controller/MidiVoice.h
        ${LIB_SOURCE_DIR}/midi/controller/MidiVoiceAssigner.cpp
        ${LIB_SOURCE_DIR}/midi/controller/MidiVoiceAssigner.h
        ${LIB_SOURCE_DIR}/midi/controller/NewSongDataCommand.h
        ${LIB_SOURCE_DIR}/midi/controller/NewSongDataDataCommand.cpp
        ${LIB_SOURCE_DIR}/midi/controller/ReplaceDataCommand.cpp
        ${LIB_SOURCE_DIR}/midi/controller/ReplaceDataCommand.h
        ${LIB_SOURCE_DIR}/midi/controller/SeqClock.h
        ${LIB_SOURCE_DIR}/midi/controller/SeqClock2.h
        ${LIB_SOURCE_DIR}/midi/controller/SqCommand.h
        ${LIB_SOURCE_DIR}/midi/controller/StepRecordInput.h
        ${LIB_SOURCE_DIR}/midi/controller/UndoRedoStack.cpp
        ${LIB_SOURCE_DIR}/midi/controller/UndoRedoStack.h

        ${LIB_SOURCE_DIR}/midi/model/ISeqSettings.h
        ${LIB_SOURCE_DIR}/midi/model/MidiSelectionModel.cpp
        ${LIB_SOURCE_DIR}/midi/model/MidiSelectionModel.h
        ${LIB_SOURCE_DIR}/midi/model/MidiSequencer.cpp
        ${LIB_SOURCE_DIR}/midi/model/MidiSequencer.h
        ${LIB_SOURCE_DIR}/midi/model/MidiSequencer4.cpp
        ${LIB_SOURCE_DIR}/midi/model/MidiSequencer4.h
        ${LIB_SOURCE_DIR}/midi/model/MidiSong.cpp
        ${LIB_SOURCE_DIR}/midi/model/MidiSong.h
        ${LIB_SOURCE_DIR}/midi/model/MidiSong4.cpp
        ${LIB_SOURCE_DIR}/midi/model/MidiSong4.h
        ${LIB_SOURCE_DIR}/midi/model/MidiTrack.cpp
        ${LIB_SOURCE_DIR}/midi/model/MidiTrack.h
        ${LIB_SOURCE_DIR}/midi/model/MidiTrack4Options.h
        ${LIB_SOURCE_DIR}/midi/model/PitchUtils.h
        ${LIB_SOURCE_DIR}/midi/model/Scale.cpp
        ${LIB_SOURCE_DIR}/midi/model/Scale.h
        ${LIB_SOURCE_DIR}/midi/model/ScaleRelativeNote.cpp
        ${LIB_SOURCE_DIR}/midi/model/ScaleRelativeNote.h
        ${LIB_SOURCE_DIR}/midi/model/SqClipboard.cpp
        ${LIB_SOURCE_DIR}/midi/model/SqClipboard.h
        ${LIB_SOURCE_DIR}/midi/model/SqMidiEvent.h
        ${LIB_SOURCE_DIR}/midi/model/SqRemoteEditor.cpp
        ${LIB_SOURCE_DIR}/midi/model/SqRemoteEditor.h
        ${LIB_SOURCE_DIR}/midi/model/TimeUtils.h
        ${LIB_SOURCE_DIR}/midi/model/Triad.cpp
        ${LIB_SOURCE_DIR}/midi/model/Triad.h

        ${LIB_SOURCE_DIR}/midi/view/MidiEditorContext.cpp
        ${LIB_SOURCE_DIR}/midi/view/MidiEditorContext.h
        ${LIB_SOURCE_DIR}/midi/view/NoteScreenScale.cpp
        ${LIB_SOURCE_DIR}/midi/view/NoteScreenScale.h

        # ${LIB_SOURCE_DIR}/pboost/pboost.cpp

        ${LIB_SOURCE_DIR}/sqsrc/clock/ClockMult.cpp
        ${LIB_SOURCE_DIR}/sqsrc/clock/ClockMult.h
        ${LIB_SOURCE_DIR}/sqsrc/clock/GenerativeTriggerGenerator.h
        ${LIB_SOURCE_DIR}/sqsrc/clock/GenerativeTriggerGenerator2.h
        ${LIB_SOURCE_DIR}/sqsrc/clock/TriggerSequencer.h

        ${LIB_SOURCE_DIR}/sqsrc/delay/FractionalDelay.cpp
        ${LIB_SOURCE_DIR}/sqsrc/delay/FractionalDelay.h

        ${LIB_SOURCE_DIR}/sqsrc/grammar/StochasticGrammar.h
        ${LIB_SOURCE_DIR}/sqsrc/grammar/StochasticGrammar2.cpp
        ${LIB_SOURCE_DIR}/sqsrc/grammar/StochasticGrammar2.h
        ${LIB_SOURCE_DIR}/sqsrc/grammar/StochasticNote.cpp
        ${LIB_SOURCE_DIR}/sqsrc/grammar/StochasticNote.h
        ${LIB_SOURCE_DIR}/sqsrc/grammar/StochasticProductionRule.cpp
        ${LIB_SOURCE_DIR}/sqsrc/grammar/StochasticProductionRule.h

        ${LIB_SOURCE_DIR}/sqsrc/thread/ThreadClient.cpp
        ${LIB_SOURCE_DIR}/sqsrc/thread/ThreadClient.h
        ${LIB_SOURCE_DIR}/sqsrc/thread/ThreadPriority.h
        ${LIB_SOURCE_DIR}/sqsrc/thread/ThreadServer.cpp
        ${LIB_SOURCE_DIR}/sqsrc/thread/ThreadServer.h
        ${LIB_SOURCE_DIR}/sqsrc/thread/ThreadSharedState.cpp
        ${LIB_SOURCE_DIR}/sqsrc/thread/ThreadSharedState.h

        ${LIB_SOURCE_DIR}/sqsrc/util/asserts.h
        ${LIB_SOURCE_DIR}/sqsrc/util/AtomicRingBuffer.h
        ${LIB_SOURCE_DIR}/sqsrc/util/CommChannels.h
        ${LIB_SOURCE_DIR}/sqsrc/util/Constants.h
        ${LIB_SOURCE_DIR}/sqsrc/util/Divider.h
        ${LIB_SOURCE_DIR}/sqsrc/util/DrawTimer.h
        ${LIB_SOURCE_DIR}/sqsrc/util/GateTrigger.h
        ${LIB_SOURCE_DIR}/sqsrc/util/InteropClipboard.cpp
        ${LIB_SOURCE_DIR}/sqsrc/util/InteropClipboard.h
        ${LIB_SOURCE_DIR}/sqsrc/util/ManagedPool.h
        ${LIB_SOURCE_DIR}/sqsrc/util/OneShot.h
        ${LIB_SOURCE_DIR}/sqsrc/util/PeakDetector.h
        ${LIB_SOURCE_DIR}/sqsrc/util/RingBuffer.h
        ${LIB_SOURCE_DIR}/sqsrc/util/SchmidtTrigger.h
        ${LIB_SOURCE_DIR}/sqsrc/util/SqTime.h
        ${LIB_SOURCE_DIR}/sqsrc/util/TriggerOutput.h

        ${LIB_SOURCE_DIR}/src/BasicModule.cpp
        ${LIB_SOURCE_DIR}/src/BlankModule.cpp
        ${LIB_SOURCE_DIR}/src/BootyModule.cpp
        ${LIB_SOURCE_DIR}/src/C2Json.h
        ${LIB_SOURCE_DIR}/src/CH10Module.cpp
        ${LIB_SOURCE_DIR}/src/ChaosKittyModule.cpp
        ${LIB_SOURCE_DIR}/src/CHBgModule.cpp
        ${LIB_SOURCE_DIR}/src/CHBModule.cpp
        ${LIB_SOURCE_DIR}/src/CHBPanelManager.h
        ${LIB_SOURCE_DIR}/src/ColoredNoiseModule.cpp
        ${LIB_SOURCE_DIR}/src/Comp2TextUtil.h
        ${LIB_SOURCE_DIR}/src/CompressorModule.cpp
        ${LIB_SOURCE_DIR}/src/CompressorModule2.cpp
        ${LIB_SOURCE_DIR}/src/CompressorTooltips.h
        ${LIB_SOURCE_DIR}/src/DGModule.cpp
        ${LIB_SOURCE_DIR}/src/DividerXModule.cpp
        ${LIB_SOURCE_DIR}/src/DrumTriggerModule.cpp
        ${LIB_SOURCE_DIR}/src/EV3Module.cpp
        ${LIB_SOURCE_DIR}/src/EVModule.cpp
        ${LIB_SOURCE_DIR}/src/F2Module.cpp
        ${LIB_SOURCE_DIR}/src/F4Module.cpp
        ${LIB_SOURCE_DIR}/src/FiltModule.cpp
        ${LIB_SOURCE_DIR}/src/FunVModule.cpp
        ${LIB_SOURCE_DIR}/src/GMRModule.cpp
        ${LIB_SOURCE_DIR}/src/GrayModule.cpp
        ${LIB_SOURCE_DIR}/src/KSModule.cpp
        ${LIB_SOURCE_DIR}/src/LFNBModule.cpp
        ${LIB_SOURCE_DIR}/src/LFNModule.cpp
        ${LIB_SOURCE_DIR}/src/Mix4Module.cpp
        ${LIB_SOURCE_DIR}/src/Mix8Module.cpp
        ${LIB_SOURCE_DIR}/src/MixerModule.h
        ${LIB_SOURCE_DIR}/src/MixMModule.cpp
        ${LIB_SOURCE_DIR}/src/MixStereoModule.cpp
        ${LIB_SOURCE_DIR}/src/MultiVUMeter.h
        ${LIB_SOURCE_DIR}/src/NoiseDrawer.h
        ${LIB_SOURCE_DIR}/src/SampModule.cpp
        ${LIB_SOURCE_DIR}/src/Sequencer4Module.cpp
        ${LIB_SOURCE_DIR}/src/Sequencer4Module.h
        ${LIB_SOURCE_DIR}/src/Sequencer4Widget.h
        ${LIB_SOURCE_DIR}/src/SequencerModule.cpp
        ${LIB_SOURCE_DIR}/src/SequencerModule.h
        ${LIB_SOURCE_DIR}/src/ShaperModule.cpp
        ${LIB_SOURCE_DIR}/src/SinesModule.cpp
        ${LIB_SOURCE_DIR}/src/Slew4Module.cpp
        ${LIB_SOURCE_DIR}/src/Squinky.cpp
        ${LIB_SOURCE_DIR}/src/Squinky.hpp
        ${LIB_SOURCE_DIR}/src/SubModule.cpp
        ${LIB_SOURCE_DIR}/src/SuperModule.cpp
        ${LIB_SOURCE_DIR}/src/TestModule.cpp
        ${LIB_SOURCE_DIR}/src/ThreadBoost.cpp
        ${LIB_SOURCE_DIR}/src/TremoloModule.cpp
        ${LIB_SOURCE_DIR}/src/VocalFilterModule.cpp
        ${LIB_SOURCE_DIR}/src/VocalModule.cpp
        ${LIB_SOURCE_DIR}/src/WVCOModule.cpp

        ${LIB_SOURCE_DIR}/src/ctrl/PopupMenuParamWidget.h
        ${LIB_SOURCE_DIR}/src/ctrl/SemitoneDisplay.h
        ${LIB_SOURCE_DIR}/src/ctrl/SqHelper.h
        ${LIB_SOURCE_DIR}/src/ctrl/SqMenuItem.h
        ${LIB_SOURCE_DIR}/src/ctrl/SqToggleLED.h
        ${LIB_SOURCE_DIR}/src/ctrl/SqTooltips.h
        ${LIB_SOURCE_DIR}/src/ctrl/SqUI.h
        ${LIB_SOURCE_DIR}/src/ctrl/SqVuMeter.h
        ${LIB_SOURCE_DIR}/src/ctrl/SqWidgets.h
        ${LIB_SOURCE_DIR}/src/ctrl/SubMenuParamCtrl.h
        ${LIB_SOURCE_DIR}/src/ctrl/TextDisplay.h
        ${LIB_SOURCE_DIR}/src/ctrl/ToggleButton.h
        ${LIB_SOURCE_DIR}/src/ctrl/ToggleButtonV1.h
        ${LIB_SOURCE_DIR}/src/ctrl/ToggleManager2.h
        ${LIB_SOURCE_DIR}/src/ctrl/WaveformSwitch.h

        ${LIB_SOURCE_DIR}/src/grammar/FakeScreen.h
        ${LIB_SOURCE_DIR}/src/grammar/GMRMainScreen.cpp
        ${LIB_SOURCE_DIR}/src/grammar/GMRMainScreen.h
        ${LIB_SOURCE_DIR}/src/grammar/GMRScreenHolder.cpp
        ${LIB_SOURCE_DIR}/src/grammar/GMRScreenHolder.h
        ${LIB_SOURCE_DIR}/src/grammar/GMRSerialization.cpp
        ${LIB_SOURCE_DIR}/src/grammar/GMRSerialization.h
        ${LIB_SOURCE_DIR}/src/grammar/GMRTabbedHeader.cpp
        ${LIB_SOURCE_DIR}/src/grammar/GMRTabbedHeader.h
        ${LIB_SOURCE_DIR}/src/grammar/GrammarRulePanel.cpp
        ${LIB_SOURCE_DIR}/src/grammar/GrammarRulePanel.h
        ${LIB_SOURCE_DIR}/src/grammar/ProductionRuleEditor.cpp
        ${LIB_SOURCE_DIR}/src/grammar/ProductionRuleEditor.h
        ${LIB_SOURCE_DIR}/src/grammar/RuleRowEditor.cpp
        ${LIB_SOURCE_DIR}/src/grammar/RuleRowEditor.h
        ${LIB_SOURCE_DIR}/src/grammar/TextUtils.cpp
        ${LIB_SOURCE_DIR}/src/grammar/TextUtils.h

        ${LIB_SOURCE_DIR}/src/kbd/ActionContext.h
        ${LIB_SOURCE_DIR}/src/kbd/Actions.cpp
        ${LIB_SOURCE_DIR}/src/kbd/Actions.h
        ${LIB_SOURCE_DIR}/src/kbd/KbdManager.cpp
        ${LIB_SOURCE_DIR}/src/kbd/KbdManager.h
        ${LIB_SOURCE_DIR}/src/kbd/KeyMapping.cpp
        ${LIB_SOURCE_DIR}/src/kbd/KeyMapping.h
        ${LIB_SOURCE_DIR}/src/kbd/SqKey.cpp
        ${LIB_SOURCE_DIR}/src/kbd/SqKey.h
        ${LIB_SOURCE_DIR}/src/kbd/StepRecorder.cpp
        ${LIB_SOURCE_DIR}/src/kbd/StepRecorder.h

        ${LIB_SOURCE_DIR}/src/seq/AboveNoteGrid.cpp
        ${LIB_SOURCE_DIR}/src/seq/AboveNoteGrid.h
        ${LIB_SOURCE_DIR}/src/seq/ClockFinder.cpp
        ${LIB_SOURCE_DIR}/src/seq/ClockFinder.h
        ${LIB_SOURCE_DIR}/src/seq/InputControls.cpp
        ${LIB_SOURCE_DIR}/src/seq/InputControls.h
        ${LIB_SOURCE_DIR}/src/seq/InputScreen.cpp
        ${LIB_SOURCE_DIR}/src/seq/InputScreen.h
        ${LIB_SOURCE_DIR}/src/seq/InputScreenManager.cpp
        ${LIB_SOURCE_DIR}/src/seq/InputScreenManager.h
        ${LIB_SOURCE_DIR}/src/seq/InteropClipboard.cpp
        ${LIB_SOURCE_DIR}/src/seq/MouseManager.cpp
        ${LIB_SOURCE_DIR}/src/seq/MouseManager.h
        ${LIB_SOURCE_DIR}/src/seq/NoteDisplay.cpp
        ${LIB_SOURCE_DIR}/src/seq/NoteDisplay.h
        ${LIB_SOURCE_DIR}/src/seq/NoteDragger.cpp
        ${LIB_SOURCE_DIR}/src/seq/NoteDragger.h
        ${LIB_SOURCE_DIR}/src/seq/PitchInputWidget.cpp
        ${LIB_SOURCE_DIR}/src/seq/PitchInputWidget.h
        ${LIB_SOURCE_DIR}/src/seq/S4Button.cpp
        ${LIB_SOURCE_DIR}/src/seq/S4Button.h
        ${LIB_SOURCE_DIR}/src/seq/S4ButtonGrid.cpp
        ${LIB_SOURCE_DIR}/src/seq/S4ButtonGrid.h
        ${LIB_SOURCE_DIR}/src/seq/SeqSettings.cpp
        ${LIB_SOURCE_DIR}/src/seq/SeqSettings.h
        ${LIB_SOURCE_DIR}/src/seq/SequencerSerializer.cpp
        ${LIB_SOURCE_DIR}/src/seq/SequencerSerializer.h
        ${LIB_SOURCE_DIR}/src/seq/SqGfx.cpp
        ${LIB_SOURCE_DIR}/src/seq/SqGfx.h
        ${LIB_SOURCE_DIR}/src/seq/UIPrefs.cpp
        ${LIB_SOURCE_DIR}/src/seq/UIPrefs.h
        ${LIB_SOURCE_DIR}/src/seq/UndoBridge.cpp
        ${LIB_SOURCE_DIR}/src/seq/XformScreens.cpp
        ${LIB_SOURCE_DIR}/src/seq/XformScreens.h

        ${LIB_SOURCE_DIR}/src/third-party/IMWidgets.cpp
        ${LIB_SOURCE_DIR}/src/third-party/IMWidgets.hpp

        ${LIB_SOURCE_DIR}/ssehack/x86intrin.h

        ${LIB_SOURCE_DIR}/util/FilteredIterator.h
        ${LIB_SOURCE_DIR}/util/RandomRange.h
        ${LIB_SOURCE_DIR}/util/SqLog.cpp
        ${LIB_SOURCE_DIR}/util/SqLog.h
        ${LIB_SOURCE_DIR}/util/SqStream.h

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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/composites)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/fft)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/filters)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/generators)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/samp)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/falco)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/FLAC)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/flac/include)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/private)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/protected)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/flac/include/share)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/flac/src)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/midifile)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/third-party/src)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/dsp/utils)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/midi/controller)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/midi/model)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/midi/view)
    # ${LIB_SOURCE_DIR}/pboost/pboost.cpp
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/sqsrc/clock)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/sqsrc/delay)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/sqsrc/grammar)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/sqsrc/thread)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/sqsrc/util)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/src)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/src/ctrl)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/src/grammar)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/src/kbd)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/src/seq)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/src/third-party)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/ssehack)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/util)
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/.hgignore
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/.hgtags
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/.hg_archival.txt
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/CHANGELOG
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/COPYING
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/kissfft.hh
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/kiss_fft.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/kiss_fft.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/Makefile
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/README
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/README.simd
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/benchfftw.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/benchkiss.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/compfft.py
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/doit.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/fastfir.py
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/fft.py
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/Makefile
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/mk_test.py
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/pstats.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/pstats.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/tailscrap.m
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/testcpp.cc
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/testkiss.py
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/test_real.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/test_vs_dft.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/test/twotonetest.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/TIPS
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/fftutil.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kfc.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kfc.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fastfir.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftnd.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftnd.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftndr.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftndr.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftr.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/kiss_fftr.h
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/Makefile
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/tools/psdpng.c
        # ${LIB_SOURCE_DIR}/dsp/third-party/kiss_fft130/_kiss_fft_guts.h

    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _ASSERT=1)
    target_compile_definitions(${LIB_NAME} PRIVATE __PLUGIN=1)
    target_compile_definitions(${LIB_NAME} PRIVATE __V1x=1)
    target_compile_definitions(${LIB_NAME} PRIVATE _SEQ=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    target_link_libraries(${LIB_NAME} PRIVATE de_kissfft)
    target_link_libraries(${LIB_NAME} PRIVATE de_flac)
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

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

    # set_target_properties(${LIB_NAME} PROPERTIES
    #     OUTPUT_NAME "Fundamental-2.6.2-llvm-win-x64"
    #     SUFFIX ".dll"
    # )

    set(VCV_NAME "Squinky")
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

    # add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E copy
            # "$<TARGET_FILE:${LIB_NAME}>"
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins/myplugin.vcvplugin"
    # )

    # add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E copy
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/myplugin.dll"
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcvplugins/myplugin.vcvplugin"
    # )

    # target_link_libraries(${LIB_NAME} PUBLIC de_rack2_nanovg)

endif()

# add_subdirectory(test)
