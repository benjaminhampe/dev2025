set(LIB_NAME de_juce_audio_processors)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_audio_processors)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_audio_processors)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_audio_processors.h
      ${LIB_HEADER_DIR}/juce_audio_processors_config.h
      ${LIB_HEADER_DIR}/format/juce_AudioPluginFormat.h
      ${LIB_HEADER_DIR}/format/juce_AudioPluginFormatManager.h
      # ${LIB_HEADER_DIR}/format_types/juce_ARACommon.h
      # ${LIB_HEADER_DIR}/format_types/juce_ARAHosting.h
      # ${LIB_HEADER_DIR}/format_types/juce_AudioUnitPluginFormat.h
      # ${LIB_HEADER_DIR}/format_types/juce_AU_Shared.h
      # ${LIB_HEADER_DIR}/format_types/juce_LADSPAPluginFormat.h
      # ${LIB_HEADER_DIR}/format_types/juce_LV2Common.h
      # ${LIB_HEADER_DIR}/format_types/juce_LV2PluginFormat.h
      # ${LIB_HEADER_DIR}/format_types/juce_LV2Resources.h
      ${LIB_HEADER_DIR}/format_types/juce_VST3Common.h
      ${LIB_HEADER_DIR}/format_types/juce_VST3Headers.h
      ${LIB_HEADER_DIR}/format_types/juce_VST3PluginFormat.h
      ${LIB_HEADER_DIR}/format_types/juce_LegacyAudioParameter.h
      ${LIB_HEADER_DIR}/format_types/juce_VSTCommon.h
      ${LIB_HEADER_DIR}/format_types/juce_VSTMidiEventList.h
      ${LIB_HEADER_DIR}/format_types/juce_VSTPluginFormat.h
      ${LIB_HEADER_DIR}/processors/juce_callOnMessageThread.h
      ${LIB_HEADER_DIR}/processors/juce_AudioPluginInstance.h
      ${LIB_HEADER_DIR}/processors/juce_AudioProcessor.h
      ${LIB_HEADER_DIR}/processors/juce_AudioProcessorEditor.h
      ${LIB_HEADER_DIR}/processors/juce_AudioProcessorEditorHostContext.h
      ${LIB_HEADER_DIR}/processors/juce_AudioProcessorGraph.h
      ${LIB_HEADER_DIR}/processors/juce_AudioProcessorListener.h
      ${LIB_HEADER_DIR}/processors/juce_AudioProcessorParameter.h
      ${LIB_HEADER_DIR}/processors/juce_AudioProcessorParameterGroup.h
      ${LIB_HEADER_DIR}/processors/juce_GenericAudioProcessorEditor.h
      ${LIB_HEADER_DIR}/processors/juce_HostedAudioProcessorParameter.h
      ${LIB_HEADER_DIR}/processors/juce_PluginDescription.h
      ${LIB_HEADER_DIR}/scanning/juce_KnownPluginList.h
      ${LIB_HEADER_DIR}/scanning/juce_PluginDirectoryScanner.h
      ${LIB_HEADER_DIR}/scanning/juce_PluginListComponent.h
      # ${LIB_HEADER_DIR}/utilities/ARA/juce_ARADebug.h
      # ${LIB_HEADER_DIR}/utilities/ARA/juce_ARADocumentController.h
      # ${LIB_HEADER_DIR}/utilities/ARA/juce_ARAModelObjects.h
      # ${LIB_HEADER_DIR}/utilities/ARA/juce_ARAPlugInInstanceRoles.h
      # ${LIB_HEADER_DIR}/utilities/ARA/juce_ARA_utils.h
      # ${LIB_HEADER_DIR}/utilities/ARA/juce_AudioProcessor_ARAExtensions.h
      ${LIB_HEADER_DIR}/utilities/juce_AudioParameterBool.h
      ${LIB_HEADER_DIR}/utilities/juce_AudioParameterChoice.h
      ${LIB_HEADER_DIR}/utilities/juce_AudioParameterFloat.h
      ${LIB_HEADER_DIR}/utilities/juce_AudioParameterInt.h
      ${LIB_HEADER_DIR}/utilities/juce_AudioProcessorParameterWithID.h
      ${LIB_HEADER_DIR}/utilities/juce_AudioProcessorValueTreeState.h
      ${LIB_HEADER_DIR}/utilities/juce_ExtensionsVisitor.h
      ${LIB_HEADER_DIR}/utilities/juce_FlagCache.h
      ${LIB_HEADER_DIR}/utilities/juce_NativeScaleFactorNotifier.h
      ${LIB_HEADER_DIR}/utilities/juce_ParameterAttachments.h
      ${LIB_HEADER_DIR}/utilities/juce_PluginHostType.h
      ${LIB_HEADER_DIR}/utilities/juce_RangedAudioParameter.h
      ${LIB_HEADER_DIR}/utilities/juce_VST3ClientExtensions.h
      ${LIB_HEADER_DIR}/utilities/juce_VSTCallbackHandler.h
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/format/juce_AudioPluginFormat.cpp
      ${LIB_SOURCE_DIR}/format/juce_AudioPluginFormatManager.cpp
      # ${LIB_SOURCE_DIR}/format_types/juce_ARACommon.cpp
      # ${LIB_SOURCE_DIR}/format_types/juce_ARAHosting.cpp
      # ${LIB_SOURCE_DIR}/format_types/juce_AudioUnitPluginFormat.mm
      # ${LIB_SOURCE_DIR}/format_types/juce_LADSPAPluginFormat.cpp
      ${LIB_SOURCE_DIR}/format_types/juce_LegacyAudioParameter.cpp
      # ${LIB_SOURCE_DIR}/format_types/juce_LV2PluginFormat.cpp
      # ${LIB_SOURCE_DIR}/format_types/juce_LV2PluginFormat_test.cpp
      # ${LIB_SOURCE_DIR}/format_types/juce_LV2SupportLibs.cpp
      ${LIB_SOURCE_DIR}/format_types/juce_VST3PluginFormat.cpp
      # ${LIB_SOURCE_DIR}/format_types/juce_VST3PluginFormat_test.cpp
      ${LIB_SOURCE_DIR}/format_types/juce_VSTPluginFormat.cpp
      ${LIB_SOURCE_DIR}/juce_audio_processors.cpp
      # ${LIB_SOURCE_DIR}/juce_audio_processors_ara.cpp
      # ${LIB_SOURCE_DIR}/juce_audio_processors_lv2_libs.cpp
      ${LIB_SOURCE_DIR}/processors/juce_AudioPluginInstance.cpp
      ${LIB_SOURCE_DIR}/processors/juce_AudioProcessor.cpp
      ${LIB_SOURCE_DIR}/processors/juce_AudioProcessorEditor.cpp
      ${LIB_SOURCE_DIR}/processors/juce_AudioProcessorGraph.cpp
      ${LIB_SOURCE_DIR}/processors/juce_AudioProcessorParameterGroup.cpp
      ${LIB_SOURCE_DIR}/processors/juce_GenericAudioProcessorEditor.cpp
      ${LIB_SOURCE_DIR}/processors/juce_PluginDescription.cpp
      ${LIB_SOURCE_DIR}/scanning/juce_KnownPluginList.cpp
      ${LIB_SOURCE_DIR}/scanning/juce_PluginDirectoryScanner.cpp
      ${LIB_SOURCE_DIR}/scanning/juce_PluginListComponent.cpp
      # ${LIB_SOURCE_DIR}/utilities/ARA/juce_ARADocumentController.cpp
      # ${LIB_SOURCE_DIR}/utilities/ARA/juce_ARADocumentControllerCommon.cpp
      # ${LIB_SOURCE_DIR}/utilities/ARA/juce_ARAModelObjects.cpp
      # ${LIB_SOURCE_DIR}/utilities/ARA/juce_ARAPlugInInstanceRoles.cpp
      # ${LIB_SOURCE_DIR}/utilities/ARA/juce_ARA_utils.cpp
      # ${LIB_SOURCE_DIR}/utilities/ARA/juce_AudioProcessor_ARAExtensions.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_AudioParameterBool.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_AudioParameterChoice.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_AudioParameterFloat.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_AudioParameterInt.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_AudioProcessorParameterWithID.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_AudioProcessorValueTreeState.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_NativeScaleFactorNotifier.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_ParameterAttachments.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_PluginHostType.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_RangedAudioParameter.cpp
      ${LIB_SOURCE_DIR}/utilities/juce_VSTCallbackHandler.cpp
   )

   if (BUILD_SHARED)
      add_library(${LIB_NAME} SHARED "")
      message("${BoldGreen}[benni] DLL: ${LIB_NAME}${ColourReset}")
   else()
      add_library(${LIB_NAME} STATIC "")
      message("${BoldGreen}[benni] LIB: ${LIB_NAME}${ColourReset}")
   endif()

   target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

   target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
   target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
   target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/..)

   # target_compile_definitions(${LIB_NAME} PUBLIC -D_UNICODE=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DUNICODE=1)

   target_compile_definitions(${LIB_NAME} PUBLIC USE_PCRE=0) # lv2 sdk
   target_compile_definitions(${LIB_NAME} PUBLIC JUCE_VST3HEADERS_INCLUDE_HEADERS_ONLY=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC JUCE_PLUGINHOST_VST=0)
   # target_compile_definitions(${LIB_NAME} PUBLIC JUCE_PLUGINHOST_AU=0)
   # target_compile_definitions(${LIB_NAME} PUBLIC JUCE_WEB_BROWSER=0)

   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_basics)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_gui_extra)

   # target_link_libraries(${LIB_NAME} PRIVATE de_asiosdk)
   target_link_libraries(${LIB_NAME} PUBLIC de_vst2sdk)
   target_link_libraries(${LIB_NAME} PUBLIC de_vst3sdk)

endif()

# add_subdirectory(test)

