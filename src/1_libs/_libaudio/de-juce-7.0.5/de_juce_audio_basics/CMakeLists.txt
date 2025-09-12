set(LIB_NAME de_juce_audio_basics)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_audio_basics)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_audio_basics)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_audio_basics.h
      ${LIB_HEADER_DIR}/juce_audio_basics_config.h
      ${LIB_HEADER_DIR}/audio_play_head/juce_AudioPlayHead.h
      ${LIB_HEADER_DIR}/buffers/juce_AudioChannelSet.h
      ${LIB_HEADER_DIR}/buffers/juce_AudioDataConverters.h
      ${LIB_HEADER_DIR}/buffers/juce_AudioProcessLoadMeasurer.h
      ${LIB_HEADER_DIR}/buffers/juce_AudioSampleBuffer.h
      ${LIB_HEADER_DIR}/buffers/juce_FloatVectorOperations.h
      ${LIB_HEADER_DIR}/midi/juce_MidiBuffer.h
      ${LIB_HEADER_DIR}/midi/juce_MidiDataConcatenator.h
      ${LIB_HEADER_DIR}/midi/juce_MidiFile.h
      ${LIB_HEADER_DIR}/midi/juce_MidiKeyboardState.h
      ${LIB_HEADER_DIR}/midi/juce_MidiMessage.h
      ${LIB_HEADER_DIR}/midi/juce_MidiMessageSequence.h
      ${LIB_HEADER_DIR}/midi/juce_MidiRPN.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMP.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPacket.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPackets.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPConversion.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPConverters.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPDispatcher.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPFactory.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPIterator.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPMidi1ToBytestreamTranslator.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPMidi1ToMidi2DefaultTranslator.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPProtocols.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPReceiver.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPSysEx7.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPUtils.h
      ${LIB_HEADER_DIR}/midi/ump/juce_UMPView.h
      ${LIB_HEADER_DIR}/mpe/juce_MPEInstrument.h
      ${LIB_HEADER_DIR}/mpe/juce_MPEMessages.h
      ${LIB_HEADER_DIR}/mpe/juce_MPENote.h
      ${LIB_HEADER_DIR}/mpe/juce_MPESynthesiser.h
      ${LIB_HEADER_DIR}/mpe/juce_MPESynthesiserBase.h
      ${LIB_HEADER_DIR}/mpe/juce_MPESynthesiserVoice.h
      ${LIB_HEADER_DIR}/mpe/juce_MPEUtils.h
      ${LIB_HEADER_DIR}/mpe/juce_MPEValue.h
      ${LIB_HEADER_DIR}/mpe/juce_MPEZoneLayout.h
      # ${LIB_HEADER_DIR}/native/juce_mac_CoreAudioLayouts.h
      # ${LIB_HEADER_DIR}/native/juce_mac_CoreAudioTimeConversions.h
      ${LIB_HEADER_DIR}/sources/juce_AudioSource.h
      ${LIB_HEADER_DIR}/sources/juce_BufferingAudioSource.h
      ${LIB_HEADER_DIR}/sources/juce_ChannelRemappingAudioSource.h
      ${LIB_HEADER_DIR}/sources/juce_IIRFilterAudioSource.h
      ${LIB_HEADER_DIR}/sources/juce_MemoryAudioSource.h
      ${LIB_HEADER_DIR}/sources/juce_MixerAudioSource.h
      ${LIB_HEADER_DIR}/sources/juce_PositionableAudioSource.h
      ${LIB_HEADER_DIR}/sources/juce_ResamplingAudioSource.h
      ${LIB_HEADER_DIR}/sources/juce_ReverbAudioSource.h
      ${LIB_HEADER_DIR}/sources/juce_ToneGeneratorAudioSource.h
      ${LIB_HEADER_DIR}/synthesisers/juce_Synthesiser.h
      ${LIB_HEADER_DIR}/utilities/juce_ADSR.h
      ${LIB_HEADER_DIR}/utilities/juce_Decibels.h
      ${LIB_HEADER_DIR}/utilities/juce_GenericInterpolator.h
      ${LIB_HEADER_DIR}/utilities/juce_IIRFilter.h
      ${LIB_HEADER_DIR}/utilities/juce_Interpolators.h
      ${LIB_HEADER_DIR}/utilities/juce_Reverb.h
      ${LIB_HEADER_DIR}/utilities/juce_SmoothedValue.h
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/juce_audio_basics.cpp
      ${LIB_SOURCE_DIR}/audio_play_head/juce_AudioPlayHead.cpp
      ${LIB_SOURCE_DIR}/buffers/juce_AudioChannelSet.cpp
      ${LIB_SOURCE_DIR}/buffers/juce_AudioDataConverters.cpp
      ${LIB_SOURCE_DIR}/buffers/juce_AudioProcessLoadMeasurer.cpp
      ${LIB_SOURCE_DIR}/buffers/juce_FloatVectorOperations.cpp
      ${LIB_SOURCE_DIR}/midi/juce_MidiBuffer.cpp
      ${LIB_SOURCE_DIR}/midi/juce_MidiFile.cpp
      ${LIB_SOURCE_DIR}/midi/juce_MidiKeyboardState.cpp
      ${LIB_SOURCE_DIR}/midi/juce_MidiMessage.cpp
      ${LIB_SOURCE_DIR}/midi/juce_MidiMessageSequence.cpp
      ${LIB_SOURCE_DIR}/midi/juce_MidiRPN.cpp
      ${LIB_SOURCE_DIR}/midi/ump/juce_UMPIterator.cpp
      ${LIB_SOURCE_DIR}/midi/ump/juce_UMPMidi1ToMidi2DefaultTranslator.cpp
      ${LIB_SOURCE_DIR}/midi/ump/juce_UMPSysEx7.cpp
      ${LIB_SOURCE_DIR}/midi/ump/juce_UMPUtils.cpp
      ${LIB_SOURCE_DIR}/midi/ump/juce_UMPView.cpp
      ${LIB_SOURCE_DIR}/midi/ump/juce_UMP_test.cpp
      ${LIB_SOURCE_DIR}/mpe/juce_MPEInstrument.cpp
      ${LIB_SOURCE_DIR}/mpe/juce_MPEMessages.cpp
      ${LIB_SOURCE_DIR}/mpe/juce_MPENote.cpp
      ${LIB_SOURCE_DIR}/mpe/juce_MPESynthesiser.cpp
      ${LIB_SOURCE_DIR}/mpe/juce_MPESynthesiserBase.cpp
      ${LIB_SOURCE_DIR}/mpe/juce_MPESynthesiserVoice.cpp
      ${LIB_SOURCE_DIR}/mpe/juce_MPEUtils.cpp
      ${LIB_SOURCE_DIR}/mpe/juce_MPEValue.cpp
      ${LIB_SOURCE_DIR}/mpe/juce_MPEZoneLayout.cpp
      ${LIB_SOURCE_DIR}/sources/juce_BufferingAudioSource.cpp
      ${LIB_SOURCE_DIR}/sources/juce_ChannelRemappingAudioSource.cpp
      ${LIB_SOURCE_DIR}/sources/juce_IIRFilterAudioSource.cpp
      ${LIB_SOURCE_DIR}/sources/juce_MemoryAudioSource.cpp
      ${LIB_SOURCE_DIR}/sources/juce_MixerAudioSource.cpp
      ${LIB_SOURCE_DIR}/sources/juce_PositionableAudioSource.cpp
      ${LIB_SOURCE_DIR}/sources/juce_ResamplingAudioSource.cpp
      ${LIB_SOURCE_DIR}/sources/juce_ReverbAudioSource.cpp
      ${LIB_SOURCE_DIR}/sources/juce_ToneGeneratorAudioSource.cpp
      ${LIB_SOURCE_DIR}/synthesisers/juce_Synthesiser.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_ADSR_test.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_IIRFilter.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_Interpolators.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_LagrangeInterpolator.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_SmoothedValue.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_WindowedSincInterpolator.cpp
   )

   if (BUILD_SHARED)
      add_library(${LIB_NAME} SHARED "")
      message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
   else()
      add_library(${LIB_NAME} STATIC "")
      message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
   endif()

   target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

   target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
   target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
   target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

   target_link_libraries(${LIB_NAME} PUBLIC de_juce_core)

endif()

# add_subdirectory(test)

