set(LIB_NAME de_BasicOsc_vst2)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_audio_plugin_client)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_audio_plugin_client)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_audio_plugin_client.h
      ${LIB_HEADER_DIR}/juce_audio_plugin_client_config.h
      # ${LIB_HEADER_DIR}/Standalone/juce_StandaloneFilterWindow.h
      ${LIB_HEADER_DIR}/utility/juce_CheckSettingMacros.h
      ${LIB_HEADER_DIR}/utility/juce_CreatePluginFilter.h
      ${LIB_HEADER_DIR}/utility/juce_IncludeModuleHeaders.h
      ${LIB_HEADER_DIR}/utility/juce_IncludeSystemHeaders.h
      ${LIB_HEADER_DIR}/utility/juce_LinuxMessageThread.h
      ${LIB_HEADER_DIR}/utility/juce_WindowsHooks.h
   )

   set(LIB_SOURCES
      # ${LIB_SOURCE_DIR}/juce_audio_plugin_client_Standalone.cpp
      ${LIB_SOURCE_DIR}/juce_audio_plugin_client_utils.cpp
      # ${LIB_SOURCE_DIR}/Standalone/juce_StandaloneFilterApp.cpp
      ${LIB_SOURCE_DIR}/utility/juce_PluginUtilities.cpp
      ${LIB_SOURCE_DIR}/VST/juce_VST_Wrapper.cpp
   )

#   if (BUILD_SHARED)
#      add_library(${LIB_NAME} SHARED "")
#      message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
#   else()
      add_library(${LIB_NAME} STATIC "")
      message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
#   endif()

   target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

   target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
   target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
   target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/..)

   # target_compile_definitions(${LIB_NAME} PUBLIC -D_UNICODE=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DUNICODE=1)

   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_FLAC=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_OGGVORBIS=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CDBURNER=0)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CDREADER=0)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CAMERA=0) # need #include <dshow.h><qedit.h><dshowasf.h>

   target_link_libraries(${LIB_NAME} PUBLIC de_juce_dsp)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_gui_basics)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_basics)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_devices)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_processors)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_gui_extra)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_utils)


endif()

# add_subdirectory(test)

