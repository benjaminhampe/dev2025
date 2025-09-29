set(LIB_NAME de_juce_audio_utils)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_audio_utils)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_audio_utils)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_audio_utils.h
      ${LIB_HEADER_DIR}/juce_audio_utils_config.h
      ${LIB_HEADER_DIR}/audio_cd/juce_AudioCDBurner.h
      ${LIB_HEADER_DIR}/audio_cd/juce_AudioCDReader.h
      ${LIB_HEADER_DIR}/gui/juce_AudioAppComponent.h
      ${LIB_HEADER_DIR}/gui/juce_AudioDeviceSelectorComponent.h
      ${LIB_HEADER_DIR}/gui/juce_AudioThumbnail.h
      ${LIB_HEADER_DIR}/gui/juce_AudioThumbnailBase.h
      ${LIB_HEADER_DIR}/gui/juce_AudioThumbnailCache.h
      ${LIB_HEADER_DIR}/gui/juce_AudioVisualiserComponent.h
      ${LIB_HEADER_DIR}/gui/juce_BluetoothMidiDevicePairingDialogue.h
      ${LIB_HEADER_DIR}/gui/juce_KeyboardComponentBase.h
      ${LIB_HEADER_DIR}/gui/juce_MidiKeyboardComponent.h
      ${LIB_HEADER_DIR}/gui/juce_MPEKeyboardComponent.h
      ${LIB_HEADER_DIR}/players/juce_AudioProcessorPlayer.h
      ${LIB_HEADER_DIR}/players/juce_SoundPlayer.h     
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/juce_audio_utils.cpp
      ${LIB_SOURCE_DIR}/audio_cd/juce_AudioCDReader.cpp
      ${LIB_SOURCE_DIR}/gui/juce_AudioAppComponent.cpp
      ${LIB_SOURCE_DIR}/gui/juce_AudioDeviceSelectorComponent.cpp
      ${LIB_SOURCE_DIR}/gui/juce_AudioThumbnail.cpp
      ${LIB_SOURCE_DIR}/gui/juce_AudioThumbnailCache.cpp
      ${LIB_SOURCE_DIR}/gui/juce_AudioVisualiserComponent.cpp
      ${LIB_SOURCE_DIR}/gui/juce_KeyboardComponentBase.cpp
      ${LIB_SOURCE_DIR}/gui/juce_MidiKeyboardComponent.cpp
      ${LIB_SOURCE_DIR}/gui/juce_MPEKeyboardComponent.cpp
      # ${LIB_SOURCE_DIR}/native/juce_android_BluetoothMidiDevicePairingDialogue.cpp
      # ${LIB_SOURCE_DIR}/native/juce_ios_BluetoothMidiDevicePairingDialogue.mm
      # ${LIB_SOURCE_DIR}/native/juce_linux_AudioCDReader.cpp
      # ${LIB_SOURCE_DIR}/native/juce_linux_BluetoothMidiDevicePairingDialogue.cpp
      # ${LIB_SOURCE_DIR}/native/juce_mac_AudioCDBurner.mm
      # ${LIB_SOURCE_DIR}/native/juce_mac_AudioCDReader.mm
      # ${LIB_SOURCE_DIR}/native/juce_mac_BluetoothMidiDevicePairingDialogue.mm
      ${LIB_SOURCE_DIR}/native/juce_win32_AudioCDBurner.h
      ${LIB_SOURCE_DIR}/native/juce_win32_AudioCDBurner.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_AudioCDReader.h
      ${LIB_SOURCE_DIR}/native/juce_win32_AudioCDReader.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_BluetoothMidiDevicePairingDialogue.h
      ${LIB_SOURCE_DIR}/native/juce_win32_BluetoothMidiDevicePairingDialogue.cpp
      ${LIB_SOURCE_DIR}/players/juce_AudioProcessorPlayer.cpp
      ${LIB_SOURCE_DIR}/players/juce_SoundPlayer.cpp     
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
   target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/..)

   # target_compile_definitions(${LIB_NAME} PUBLIC -D_UNICODE=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DUNICODE=1)

   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_FLAC=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_OGGVORBIS=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CDBURNER=0)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CDREADER=0)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CAMERA=0) # need #include <dshow.h><qedit.h><dshowasf.h>

   target_link_libraries(${LIB_NAME} PUBLIC de_juce_gui_basics)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_formats)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_devices)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_processors)


endif()

# add_subdirectory(test)

