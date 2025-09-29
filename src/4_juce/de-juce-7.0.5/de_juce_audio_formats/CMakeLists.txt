set(LIB_NAME de_juce_audio_formats)
project(${LIB_NAME}-7.0.5-dev)

if (TARGET ${LIB_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
   #add_subdirectory(lib)

   set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/juce_audio_formats)
   set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/juce_audio_formats)

#   file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
#   file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
#   set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

#   file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
#   file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
#   set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

   set(LIB_HEADERS
      ${LIB_HEADER_DIR}/juce_audio_formats.h
      ${LIB_HEADER_DIR}/juce_audio_formats_config.h
      ${LIB_HEADER_DIR}/codecs/juce_AiffAudioFormat.h
      ${LIB_HEADER_DIR}/codecs/juce_CoreAudioFormat.h
      ${LIB_HEADER_DIR}/codecs/juce_FlacAudioFormat.h
      ${LIB_HEADER_DIR}/codecs/juce_LAMEEncoderAudioFormat.h
      ${LIB_HEADER_DIR}/codecs/juce_MP3AudioFormat.h
      ${LIB_HEADER_DIR}/codecs/juce_OggVorbisAudioFormat.h
      ${LIB_HEADER_DIR}/codecs/juce_WavAudioFormat.h
      ${LIB_HEADER_DIR}/codecs/juce_WindowsMediaAudioFormat.h
      # ${LIB_HEADER_DIR}/format/juce_ARAAudioReaders.h
      ${LIB_HEADER_DIR}/format/juce_AudioFormat.h
      ${LIB_HEADER_DIR}/format/juce_AudioFormatManager.h
      ${LIB_HEADER_DIR}/format/juce_AudioFormatReader.h
      ${LIB_HEADER_DIR}/format/juce_AudioFormatReaderSource.h
      ${LIB_HEADER_DIR}/format/juce_AudioFormatWriter.h
      ${LIB_HEADER_DIR}/format/juce_AudioSubsectionReader.h
      ${LIB_HEADER_DIR}/format/juce_BufferingAudioFormatReader.h
      ${LIB_HEADER_DIR}/format/juce_MemoryMappedAudioFormatReader.h
      ${LIB_HEADER_DIR}/sampler/juce_Sampler.h
   )

   set(LIB_SOURCES
      ${LIB_SOURCE_DIR}/juce_audio_formats.cpp
      ${LIB_SOURCE_DIR}/codecs/juce_AiffAudioFormat.cpp
      ${LIB_SOURCE_DIR}/codecs/juce_CoreAudioFormat.cpp
      ${LIB_SOURCE_DIR}/codecs/juce_FlacAudioFormat.cpp
      ${LIB_SOURCE_DIR}/codecs/juce_LAMEEncoderAudioFormat.cpp
      ${LIB_SOURCE_DIR}/codecs/juce_MP3AudioFormat.cpp
      ${LIB_SOURCE_DIR}/codecs/juce_OggVorbisAudioFormat.cpp
      ${LIB_SOURCE_DIR}/codecs/juce_WavAudioFormat.cpp
      ${LIB_SOURCE_DIR}/codecs/juce_WindowsMediaAudioFormat.cpp
      # ${LIB_SOURCE_DIR}/format/juce_ARAAudioReaders.cpp
      ${LIB_SOURCE_DIR}/format/juce_AudioFormat.cpp
      ${LIB_SOURCE_DIR}/format/juce_AudioFormatManager.cpp
      ${LIB_SOURCE_DIR}/format/juce_AudioFormatReader.cpp
      ${LIB_SOURCE_DIR}/format/juce_AudioFormatReaderSource.cpp
      ${LIB_SOURCE_DIR}/format/juce_AudioFormatWriter.cpp
      ${LIB_SOURCE_DIR}/format/juce_AudioSubsectionReader.cpp
      ${LIB_SOURCE_DIR}/format/juce_BufferingAudioFormatReader.cpp
      ${LIB_SOURCE_DIR}/sampler/juce_Sampler.cpp     
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

   # target_compile_definitions(${LIB_NAME} PUBLIC -D_UNICODE=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DUNICODE=1)

   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_FLAC=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_OGGVORBIS=1)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CDBURNER=0)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CDREADER=0)
   # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CAMERA=0) # need #include <dshow.h><qedit.h><dshowasf.h>

   target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_basics)
   # target_link_libraries(${LIB_NAME} PUBLIC de_juce_audio_processors) # ARAAudioSource
   
   target_link_libraries(${LIB_NAME} PRIVATE de_ogg)
   target_link_libraries(${LIB_NAME} PRIVATE de_flac)
   target_link_libraries(${LIB_NAME} PRIVATE de_opus)
   target_link_libraries(${LIB_NAME} PRIVATE de_theora)
   target_link_libraries(${LIB_NAME} PRIVATE de_vorbis)
   target_link_libraries(${LIB_NAME} PRIVATE de_sndfile)
   target_link_libraries(${LIB_NAME} PRIVATE de_lame)

endif()

# add_subdirectory(test)

