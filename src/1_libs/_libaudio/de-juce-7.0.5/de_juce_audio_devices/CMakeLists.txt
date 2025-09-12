set(LIB_NAME de_juce_audio_devices)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_audio_devices)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_audio_devices)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_audio_devices.h
      ${LIB_HEADER_DIR}/juce_audio_devices_config.h
      ${LIB_HEADER_DIR}/audio_io/juce_AudioDeviceManager.h
      ${LIB_HEADER_DIR}/audio_io/juce_AudioIODevice.h
      ${LIB_HEADER_DIR}/audio_io/juce_AudioIODeviceType.h
      ${LIB_HEADER_DIR}/audio_io/juce_SystemAudioVolume.h
      ${LIB_HEADER_DIR}/audio_io/juce_SampleRateHelpers.h
      ${LIB_HEADER_DIR}/midi_io/juce_MidiDevices.h
      ${LIB_HEADER_DIR}/midi_io/juce_MidiMessageCollector.h
      ${LIB_HEADER_DIR}/midi_io/ump/juce_UMPBytestreamInputHandler.h
      ${LIB_HEADER_DIR}/midi_io/ump/juce_UMPU32InputHandler.h
      # ${LIB_HEADER_DIR}/native
      ${LIB_HEADER_DIR}/sources/juce_AudioSourcePlayer.h
      ${LIB_HEADER_DIR}/sources/juce_AudioTransportSource.h
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/juce_audio_devices.cpp
      ${LIB_SOURCE_DIR}/audio_io/juce_AudioDeviceManager.cpp
      ${LIB_SOURCE_DIR}/audio_io/juce_AudioIODevice.cpp
      ${LIB_SOURCE_DIR}/audio_io/juce_AudioIODeviceType.cpp
      ${LIB_SOURCE_DIR}/audio_io/juce_SampleRateHelpers.cpp
      ${LIB_SOURCE_DIR}/midi_io/juce_MidiDevices.cpp
      ${LIB_SOURCE_DIR}/midi_io/juce_MidiMessageCollector.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32__audio_devices.h
      ${LIB_SOURCE_DIR}/native/juce_win32_ASIO.h
      ${LIB_SOURCE_DIR}/native/juce_win32_ASIO.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_DirectSound.h
      ${LIB_SOURCE_DIR}/native/juce_win32_DirectSound.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_WASAPI.h
      ${LIB_SOURCE_DIR}/native/juce_win32_WASAPI.cpp
      ${LIB_SOURCE_DIR}/native/juce_win32_Midi.h
      ${LIB_SOURCE_DIR}/native/juce_win32_Midi.cpp
      ${LIB_SOURCE_DIR}/sources/juce_AudioSourcePlayer.cpp
      ${LIB_SOURCE_DIR}/sources/juce_AudioTransportSource.cpp
   )

   if (BUILD_SHARED)
      add_library(${LIB_NAME} SHARED "")
      message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
   else()
      add_library(${LIB_NAME} STATIC "")
      message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
   endif()

   target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

   target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
   target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
   target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/..)

   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_WINDOWS=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_ASIO=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_WASAPI=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_DIRECTSOUND=1)

   target_link_libraries(${LIB_NAME} PUBLIC de_juce_events)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_basics)
   target_link_libraries(${LIB_NAME} PUBLIC de_juce_graphics)

   # if (WIN32)  # on Windows:
      # target_link_libraries(${LIB_NAME} PUBLIC comctl32 user32 gdi32 kernel32)
      # target_link_libraries(${LIB_NAME} PUBLIC mfplat mf mfuuid wmcodecdspuuid)
      # target_link_libraries(${LIB_NAME} PUBLIC mf3216 mapi32 mapistub mdminst imm32)
      # target_link_libraries(${LIB_NAME} PUBLIC dxgi dsound winmm ksguid ksuser ks uuid)
      # target_link_libraries(${LIB_NAME} PUBLIC advapi32 ole32 oleaut32 comctl32 kernel32 user32 m)
      # target_link_libraries(${LIB_NAME} PUBLIC de_asiosdk)
      # target_link_libraries(${LIB_NAME} PUBLIC wsock32 ws2_32) # freeaddrinfo
      # target_link_libraries(${LIB_NAME} PUBLIC msvfw32 vfw32) # DrawDibDraw
      # target_link_libraries(${LIB_NAME} PUBLIC version)
      # target_link_libraries(${LIB_NAME} PUBLIC wininet)
      # target_link_libraries(${LIB_NAME} PUBLIC shlwapi) # PathStripToRootW
   # else()      # on Linux:
      # target_link_libraries(${LIB_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
      # target_compile_definitions(${LIB_NAME} PUBLIC -D__LINUX_ALSA__=1)
      # target_link_libraries(${LIB_NAME} PUBLIC asound)
      # target_compile_definitions(${LIB_NAME} PUBLIC -D__UNIX_JACK__=1)
      # target_link_libraries(${LIB_NAME} PUBLIC jack)
   # endif()

   target_link_libraries(${LIB_NAME} PRIVATE de_asiosdk)    # Audio Steinberg ASIO SDK
   # target_link_libraries(${LIB_NAME} PRIVATE de_vst2sdk)    # Audio Steinberg VST SDK

endif()

# add_subdirectory(test)

