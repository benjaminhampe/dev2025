set(LIB_NAME AS_DarkAudio)
project(lib-${LIB_NAME}-1.2-dev CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    #add_subdirectory(lib)

    # ::: Headers
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/DarkAudio.hpp
        # ${LIB_HEADER_DIR}/vlc.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/Buffer.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/BufferIO.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/BufferList.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/BufferListF32.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/BufferQueue.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/BufferQueueTest.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/FloatBuffer.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/FloatBufferList.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/io/BufferIO_FFMPEG.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/io/BufferIO_LAME.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/io/BufferIO_SND.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/io/BufferIO_WAV.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/io/DecoderFFMPEG.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/io/de_libsndfile.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/io/Wav.hpp
        ${LIB_HEADER_DIR}/de/audio/buffer/SampleType.hpp
        ${LIB_HEADER_DIR}/de/audio/dsp/ADSR.hpp
        ${LIB_HEADER_DIR}/de/audio/dsp/DspCrossfader.h
        ${LIB_HEADER_DIR}/de/audio/dsp/DspLevelMeter.h
        ${LIB_HEADER_DIR}/de/audio/dsp/DspLowPass_IIR_Biquad.hpp
        ${LIB_HEADER_DIR}/de/audio/dsp/DspMixer.h
        ${LIB_HEADER_DIR}/de/audio/dsp/DspPan.h
        ${LIB_HEADER_DIR}/de/audio/dsp/DspSoundTouch.h
        ${LIB_HEADER_DIR}/de/audio/dsp/fft2.hpp
        ${LIB_HEADER_DIR}/de/audio/dsp/IDspChainElement.hpp
        ${LIB_HEADER_DIR}/de/audio/dsp/MultiOSC.hpp
        ${LIB_HEADER_DIR}/de/audio/dsp/old/Delay.hpp
        ${LIB_HEADER_DIR}/de/audio/dsp/old/Gain.hpp.txt
        ${LIB_HEADER_DIR}/de/audio/dsp/old/LowPass3.hpp
        ${LIB_HEADER_DIR}/de/audio/dsp/old/_dsp_readme.txt
        ${LIB_HEADER_DIR}/de/audio/dsp/old/_ISynth.hpp.txt
        ${LIB_HEADER_DIR}/de/audio/dsp/OSC_Sine.hpp
        ${LIB_HEADER_DIR}/de/audio/dsp/ShiftBuffer.hpp
        ${LIB_HEADER_DIR}/de/audio/engine/EngineRtAudioStream.hpp
        ${LIB_HEADER_DIR}/de/audio/engine/MixerStream.hpp
        ${LIB_HEADER_DIR}/de/audio/engine/RtUtils.hpp
        ${LIB_HEADER_DIR}/de/audio/music/AudioQueue.hpp
        ${LIB_HEADER_DIR}/de/audio/music/MusicFileStream.hpp
        ${LIB_HEADER_DIR}/de/audio/music/MusicPlayer.hpp
        ${LIB_HEADER_DIR}/de/ffmpeg/File.hpp
        ${LIB_HEADER_DIR}/de/ffmpeg/FilePayload.hpp
        ${LIB_HEADER_DIR}/de/ffmpeg/GetCoverArt.hpp
        ${LIB_HEADER_DIR}/de/ffmpeg/ResamplingFile.hpp
        ${LIB_HEADER_DIR}/de/ffmpeg/Utils.hpp
        ${LIB_HEADER_DIR}/de/midi/InputStream.hpp
        ${LIB_HEADER_DIR}/de/midi/MidiFile.hpp
        ${LIB_HEADER_DIR}/de/midi/MidiParser.hpp
        ${LIB_HEADER_DIR}/de/midi/MidiParserTest.hpp
        ${LIB_HEADER_DIR}/de/midi/MidiTools.hpp
        ${LIB_HEADER_DIR}/de/video/VideoDecoderFFMPEG.hpp
    )

    # ::: Sources
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/de/audio/buffer/Buffer.cpp
        ${LIB_SOURCE_DIR}/de/audio/buffer/BufferQueue.cpp
        ${LIB_SOURCE_DIR}/de/audio/buffer/BufferQueueTest.cpp
        ${LIB_SOURCE_DIR}/de/audio/buffer/FloatBuffer.cpp
        ${LIB_SOURCE_DIR}/de/audio/buffer/io/BufferIO.cpp
        ${LIB_SOURCE_DIR}/de/audio/buffer/io/BufferIO_FFMPEG.cpp
        ${LIB_SOURCE_DIR}/de/audio/buffer/io/BufferIO_LAME.cpp
        ${LIB_SOURCE_DIR}/de/audio/buffer/io/BufferIO_SND.cpp
        ${LIB_SOURCE_DIR}/de/audio/buffer/io/BufferIO_WAV.cpp
        ${LIB_SOURCE_DIR}/de/audio/buffer/io/DecoderFFMPEG.cpp
        ${LIB_SOURCE_DIR}/de/audio/buffer/io/Wav.cpp
        ${LIB_SOURCE_DIR}/de/audio/buffer/SampleType.cpp
        ${LIB_SOURCE_DIR}/de/audio/dsp/ADSR.cpp
        ${LIB_SOURCE_DIR}/de/audio/dsp/DspLevelMeter.cpp
        ${LIB_SOURCE_DIR}/de/audio/dsp/DspMixer.cpp
        ${LIB_SOURCE_DIR}/de/audio/dsp/DspSoundTouch.cpp
        ${LIB_SOURCE_DIR}/de/audio/dsp/MultiOSC.cpp
        ${LIB_SOURCE_DIR}/de/audio/dsp/old/Delay.cpp
        ${LIB_SOURCE_DIR}/de/audio/dsp/old/LowPass3.cpp
        ${LIB_SOURCE_DIR}/de/audio/dsp/ShiftBuffer.cpp
        ${LIB_SOURCE_DIR}/de/audio/engine/EngineRtAudioStream.cpp
        ${LIB_SOURCE_DIR}/de/audio/engine/MixerStream.cpp
        ${LIB_SOURCE_DIR}/de/audio/engine/RtUtils.cpp
        ${LIB_SOURCE_DIR}/de/audio/music/AudioQueue.cpp
        ${LIB_SOURCE_DIR}/de/audio/music/MusicFileStream.cpp
        ${LIB_SOURCE_DIR}/de/audio/music/MusicPlayer.cpp
        ${LIB_SOURCE_DIR}/de/ffmpeg/File.cpp
        ${LIB_SOURCE_DIR}/de/ffmpeg/GetCoverArt.cpp
        ${LIB_SOURCE_DIR}/de/ffmpeg/ResamplingFile.cpp
        ${LIB_SOURCE_DIR}/de/ffmpeg/Utils.cpp
        ${LIB_SOURCE_DIR}/de/midi/MidiFile.cpp
        ${LIB_SOURCE_DIR}/de/midi/MidiParser.cpp
        ${LIB_SOURCE_DIR}/de/midi/MidiParserTest.cpp
        ${LIB_SOURCE_DIR}/de/midi/MidiTools.cpp
        ${LIB_SOURCE_DIR}/de/video/VideoDecoderFFMPEG.cpp
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})
	
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    
    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-deprecated-declarations)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-strict-aliasing)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-comment)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-variable)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-sign-compare)
    endif()
    
    target_link_libraries(${LIB_NAME} PUBLIC de_2501)
    target_link_libraries(${LIB_NAME} PUBLIC de_rtaudio_601)
    target_link_libraries(${LIB_NAME} PUBLIC de_rtmidi)
    target_link_libraries(${LIB_NAME} PUBLIC de_fftw3)
    target_link_libraries(${LIB_NAME} PUBLIC de_kissfft_cpp)
    target_link_libraries(${LIB_NAME} PUBLIC de_soundtouch)
    target_link_libraries(${LIB_NAME} PUBLIC de_lame)
    target_link_libraries(${LIB_NAME} PUBLIC de_ffmpeg)

endif()

# add_subdirectory(test)

