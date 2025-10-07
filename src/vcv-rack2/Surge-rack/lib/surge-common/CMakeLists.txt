set(LIB_NAME de_surge_common)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # ${LIB_HEADER_DIR}/LuaSupport.h
    # ${LIB_SOURCE_DIR}/LuaSupport.cpp

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/DebugHelpers.h
        ${LIB_HEADER_DIR}/FilterConfiguration.h
        ${LIB_HEADER_DIR}/FxPresetAndClipboardManager.h
        ${LIB_HEADER_DIR}/globals.h
        ${LIB_HEADER_DIR}/MemoryPool.h
        ${LIB_HEADER_DIR}/ModulationSource.h
        ${LIB_HEADER_DIR}/ModulatorPresetManager.h
        ${LIB_HEADER_DIR}/Parameter.h
        ${LIB_HEADER_DIR}/PatchDB.h
        ${LIB_HEADER_DIR}/PatchFileHeaderStructs.h
        ${LIB_HEADER_DIR}/resource.h
        ${LIB_HEADER_DIR}/SkinColors.h
        ${LIB_HEADER_DIR}/SkinFonts.h
        ${LIB_HEADER_DIR}/SkinModel.h
        ${LIB_HEADER_DIR}/StringOps.h
        ${LIB_HEADER_DIR}/SurgeMemoryPools.h
        ${LIB_HEADER_DIR}/SurgeParamConfig.h
        ${LIB_HEADER_DIR}/SurgeStorage.h
        ${LIB_HEADER_DIR}/SurgeSynthesizer.h
        ${LIB_HEADER_DIR}/UnitConversions.h
        ${LIB_HEADER_DIR}/UserDefaults.h
        ${LIB_HEADER_DIR}/version.h
        # dsp
        ${LIB_HEADER_DIR}/dsp/CXOR.h
        ${LIB_HEADER_DIR}/dsp/DSPExternalAdapterUtils.h
        ${LIB_HEADER_DIR}/dsp/Effect.h
        ${LIB_HEADER_DIR}/dsp/Oscillator.h
        ${LIB_HEADER_DIR}/dsp/QuadFilterChain.h
        ${LIB_HEADER_DIR}/dsp/SurgeVoice.h
        ${LIB_HEADER_DIR}/dsp/SurgeVoiceState.h
        ${LIB_HEADER_DIR}/dsp/Wavetable.h
        ${LIB_HEADER_DIR}/dsp/WavetableScriptEvaluator.h
        # dsp/effects
        ${LIB_HEADER_DIR}/dsp/effects/AudioInputEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/BBDEnsembleEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/BonsaiEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/ChorusEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/ChorusEffectImpl.h
        ${LIB_HEADER_DIR}/dsp/effects/CombulatorEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/ConditionerEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/DelayEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/DistortionEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/FlangerEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/FrequencyShifterEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/GraphicEQ11BandEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/ModControl.h
        ${LIB_HEADER_DIR}/dsp/effects/MSToolEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/NimbusEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/ParametricEQ3BandEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/PhaserEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/ResonatorEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/Reverb1Effect.h
        ${LIB_HEADER_DIR}/dsp/effects/Reverb2Effect.h
        ${LIB_HEADER_DIR}/dsp/effects/RingModulatorEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/RotarySpeakerEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/SurgeSSTFXAdapter.h
        ${LIB_HEADER_DIR}/dsp/effects/TreemonsterEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/VocoderEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/WaveShaperEffect.h
        # dsp/effects/airwindows
        ${LIB_HEADER_DIR}/dsp/effects/airwindows/AirWindowsEffect.h
        # dsp/effects/chowdsp
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/CHOWEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/ExciterEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/NeuronEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/SpringReverbEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/TapeEffect.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/bbd_utils/BBDCompander.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/bbd_utils/BBDDelayLine.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/bbd_utils/BBDFilterBank.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/bbd_utils/BBDNonlin.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/exciter/LevelDetector.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/BilinearUtils.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/chowdsp_DelayInterpolation.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/chowdsp_DelayLine.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/chowdsp_IIR.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/FIRFIlter.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/omega.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/Oversampling.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/Shelf.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/SmoothedValue.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/StateVariableFilter.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/wdf.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared/wdf_sse.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/spring_reverb/ReflectionNetwork.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/spring_reverb/SchroederAllpass.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/spring_reverb/SpringReverbProc.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/tape/ChewDropout.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/tape/ChewProcessor.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/tape/DegradeFilter.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/tape/DegradeNoise.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/tape/DegradeProcessor.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/tape/HysteresisOps.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/tape/HysteresisProcessing.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/tape/HysteresisProcessor.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/tape/LossFilter.h
        ${LIB_HEADER_DIR}/dsp/effects/chowdsp/tape/ToneControl.h
        # dsp/filters
        ${LIB_HEADER_DIR}/dsp/filters/AllpassFilter.h
        ${LIB_HEADER_DIR}/dsp/filters/BiquadFilter.h
        ${LIB_HEADER_DIR}/dsp/filters/VectorizedSVFilter.h
        # dsp/modulators
        ${LIB_HEADER_DIR}/dsp/modulators/ADSRModulationSource.h
        ${LIB_HEADER_DIR}/dsp/modulators/FormulaModulationHelper.h
        ${LIB_HEADER_DIR}/dsp/modulators/LFOModulationSource.h
        ${LIB_HEADER_DIR}/dsp/modulators/MSEGModulationHelper.h
        # dsp/oscillators
        ${LIB_HEADER_DIR}/dsp/oscillators/AliasOscillator.h
        ${LIB_HEADER_DIR}/dsp/oscillators/AudioInputOscillator.h
        ${LIB_HEADER_DIR}/dsp/oscillators/ClassicOscillator.h
        ${LIB_HEADER_DIR}/dsp/oscillators/FM2Oscillator.h
        ${LIB_HEADER_DIR}/dsp/oscillators/FM3Oscillator.h
        ${LIB_HEADER_DIR}/dsp/oscillators/ModernOscillator.h
        ${LIB_HEADER_DIR}/dsp/oscillators/OscillatorBase.h
        ${LIB_HEADER_DIR}/dsp/oscillators/OscillatorCommonFunctions.h
        ${LIB_HEADER_DIR}/dsp/oscillators/SampleAndHoldOscillator.h
        ${LIB_HEADER_DIR}/dsp/oscillators/SineOscillator.h
        ${LIB_HEADER_DIR}/dsp/oscillators/StringOscillator.h
        ${LIB_HEADER_DIR}/dsp/oscillators/TwistOscillator.h
        ${LIB_HEADER_DIR}/dsp/oscillators/WavetableOscillator.h
        ${LIB_HEADER_DIR}/dsp/oscillators/WindowOscillator.h
        # dsp/utilities
        ${LIB_HEADER_DIR}/dsp/utilities/DSPUtils.h
        ${LIB_HEADER_DIR}/dsp/utilities/SSEComplex.h
        ${LIB_HEADER_DIR}/dsp/utilities/SSESincDelayLine.h
        # dsp/vembertech
        ${LIB_HEADER_DIR}/dsp/vembertech/basic_dsp.h
        ${LIB_HEADER_DIR}/dsp/vembertech/lipol.h
        ${LIB_HEADER_DIR}/dsp/vembertech/portable_intrinsics.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/DebugHelpers.cpp
        ${LIB_SOURCE_DIR}/FxPresetAndClipboardManager.cpp
        ${LIB_SOURCE_DIR}/ModulationSource.cpp
        ${LIB_SOURCE_DIR}/ModulatorPresetManager.cpp
        ${LIB_SOURCE_DIR}/Parameter.cpp
        ${LIB_SOURCE_DIR}/PatchDB.cpp
        ${LIB_SOURCE_DIR}/PatchDBQueryParser.cpp
        ${LIB_SOURCE_DIR}/SkinColors.cpp
        ${LIB_SOURCE_DIR}/SkinFonts.cpp
        ${LIB_SOURCE_DIR}/SkinModel.cpp
        ${LIB_SOURCE_DIR}/SkinModelImpl.cpp
        ${LIB_SOURCE_DIR}/SurgePatch.cpp
        ${LIB_SOURCE_DIR}/SurgeStorage.cpp
        ${LIB_SOURCE_DIR}/SurgeSynthesizer.cpp
        ${LIB_SOURCE_DIR}/SurgeSynthesizerIO.cpp
        ${LIB_SOURCE_DIR}/UserDefaults.cpp
        ${LIB_SOURCE_DIR}/version.cpp
        ${LIB_SOURCE_DIR}/WAVFileSupport.cpp
        # dsp
        ${LIB_SOURCE_DIR}/dsp/DSPExternalAdapterUtils.cpp
        ${LIB_SOURCE_DIR}/dsp/Effect.cpp
        ${LIB_SOURCE_DIR}/dsp/Oscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/QuadFilterChain.cpp
        ${LIB_SOURCE_DIR}/dsp/SurgeVoice.cpp
        ${LIB_SOURCE_DIR}/dsp/Wavetable.cpp
        ${LIB_SOURCE_DIR}/dsp/WavetableScriptEvaluator.cpp
        # dsp/effects
        ${LIB_SOURCE_DIR}/dsp/effects/AudioInputEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/BBDEnsembleEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/BonsaiEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/CombulatorEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/ConditionerEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/DelayEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/DistortionEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/FlangerEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/FrequencyShifterEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/GraphicEQ11BandEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/MSToolEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/NimbusEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/ParametricEQ3BandEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/PhaserEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/ResonatorEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/Reverb1Effect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/Reverb2Effect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/RingModulatorEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/RotarySpeakerEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/TreemonsterEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/VocoderEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/WaveShaperEffect.cpp
        # dsp/effects/airwindows
        ${LIB_SOURCE_DIR}/dsp/effects/airwindows/AirWindowsEffect.cpp
        # dsp/effects/chowdsp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/CHOWEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/ExciterEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/NeuronEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/SpringReverbEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/TapeEffect.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/bbd_utils/BBDDelayLine.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/exciter/LevelDetector.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/shared/chowdsp_DelayLine.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/shared/StateVariableFilter.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/spring_reverb/SpringReverbProc.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/tape/ChewProcessor.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/tape/DegradeProcessor.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/tape/HysteresisProcessing.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/tape/HysteresisProcessor.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/tape/LossFilter.cpp
        ${LIB_SOURCE_DIR}/dsp/effects/chowdsp/tape/ToneControl.cpp
        # dsp/filters
        ${LIB_SOURCE_DIR}/dsp/filters/VectorizedSVFilter.cpp
        # dsp/modulators
        ${LIB_SOURCE_DIR}/dsp/modulators/FormulaModulationHelper.cpp
        ${LIB_SOURCE_DIR}/dsp/modulators/LFOModulationSource.cpp
        ${LIB_SOURCE_DIR}/dsp/modulators/MSEGModulationHelper.cpp
        # dsp/oscillators
        ${LIB_SOURCE_DIR}/dsp/oscillators/AliasOscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/oscillators/AudioInputOscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/oscillators/ClassicOscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/oscillators/FM2Oscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/oscillators/FM3Oscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/oscillators/ModernOscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/oscillators/SampleAndHoldOscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/oscillators/SineOscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/oscillators/StringOscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/oscillators/TwistOscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/oscillators/WavetableOscillator.cpp
        ${LIB_SOURCE_DIR}/dsp/oscillators/WindowOscillator.cpp
    )

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    # if (BUILD_SHARED)
    #     message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    #     add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBSSTSTRNATCMP=1)
    target_compile_definitions(${LIB_NAME} PUBLIC SURGE_COMPILE_BLOCK_SIZE=64)
    # target_compile_definitions(${LIB_NAME} PUBLIC SURGE_INCLUDE_AIRWINDOWS=1)
    target_compile_definitions(${LIB_NAME} PUBLIC SURGE_SKIP_ODDSOUND_MTS=1)

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/effects)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/effects/airwindows)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/effects/chowdsp)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/effects/chowdsp/bbd_utils)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/effects/chowdsp/exciter)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/effects/chowdsp/shared)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/effects/chowdsp/spring_reverb)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/effects/chowdsp/tape)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/filters)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/modulators)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/oscillators)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/utilities)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/dsp/vembertech)

    target_link_libraries(${LIB_NAME} PUBLIC de_sst_basic_blocks)
    target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml)
    target_link_libraries(${LIB_NAME} PUBLIC de_taocpp_pegtl)
    target_link_libraries(${LIB_NAME} PUBLIC de_sqlite3)
    target_link_libraries(${LIB_NAME} PUBLIC de_sst_cpputils)
    target_link_libraries(${LIB_NAME} PUBLIC de_tuning_library)
    target_link_libraries(${LIB_NAME} PUBLIC de_sst_plugininfra)
    target_link_libraries(${LIB_NAME} PUBLIC de_sst_filters)
    target_link_libraries(${LIB_NAME} PUBLIC de_sst_waveshapers)
    target_link_libraries(${LIB_NAME} PUBLIC de_sst_strnatcmp)
    target_link_libraries(${LIB_NAME} PUBLIC de_sst_effects)
    target_link_libraries(${LIB_NAME} PUBLIC de_airwindows)
    target_link_libraries(${LIB_NAME} PUBLIC de_samplerate)
    target_link_libraries(${LIB_NAME} PUBLIC de_eurorack)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libaudio)
    else()

        # target_link_libraries(${LIB_NAME} PUBLIC pthread)
    endif()

    # if (WIN32)
        # target_compile_definitions(${LIB_NAME} PUBLIC _WIN32_WINNT=0x0602)
        # target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_ASIO__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_DS__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_WASAPI__=1)
        # target_link_libraries(${LIB_NAME} PUBLIC mfplat mf mfuuid wmcodecdspuuid)
        # #target_link_libraries(${LIB_NAME} PUBLIC mf3216 mapi32 mapistub mdminst imm32)
        # target_link_libraries(${LIB_NAME} PUBLIC dsound winmm ksuser uuid) # ks ksguid
        # target_link_libraries(${LIB_NAME} PUBLIC advapi32 ole32 oleaut32 comctl32 kernel32 user32)
        # target_link_libraries(${LIB_NAME} PUBLIC de_asiosdk)
    # else()
        # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_ALSA__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __UNIX_JACK__=1)
        # # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_PULSE__=1)
        # # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_OSS__=1)
        # target_link_libraries(${LIB_NAME} PUBLIC asound)
        # target_link_libraries(${LIB_NAME} PUBLIC jack)
        # # target_link_libraries(${LIB_NAME} PUBLIC de_sndfile)
    # endif()



endif()

# add_subdirectory(test)
