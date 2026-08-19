set(LIB_NAME de_sfml_audio_310)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)
    # file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    # file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.cpp")

    # set(LUNASVG_VERSION_MAJOR 3)
    # set(LUNASVG_VERSION_MINOR 1)
    # set(LUNASVG_VERSION_MICRO 0)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/SFML/Audio/AudioResource.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/Export.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/InputSoundFile.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/Listener.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/Music.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/OutputSoundFile.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/PlaybackDevice.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/Sound.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/SoundBuffer.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/SoundBufferRecorder.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/SoundChannel.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/SoundFileFactory.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/SoundFileFactory.inl
        ${LIB_HEADER_DIR}/SFML/Audio/SoundFileReader.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/SoundFileWriter.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/SoundRecorder.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/SoundSource.hpp
        ${LIB_HEADER_DIR}/SFML/Audio/SoundStream.hpp
        ${LIB_HEADER_DIR}/SFML/Audio.hpp
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/SFML/Audio/AudioDevice.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/AudioDevice.hpp
        ${LIB_SOURCE_DIR}/SFML/Audio/AudioResource.cpp
        # ${LIB_SOURCE_DIR}/SFML/Audio/CMakeLists.txt
        # ${LIB_SOURCE_DIR}/SFML/Audio/Dependencies.cmake.in
        ${LIB_SOURCE_DIR}/SFML/Audio/InputSoundFile.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/Listener.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/Miniaudio.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/MiniaudioUtils.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/MiniaudioUtils.hpp
        ${LIB_SOURCE_DIR}/SFML/Audio/Music.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/OutputSoundFile.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/PlaybackDevice.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/Sound.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundBuffer.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundBufferRecorder.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileFactory.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileReaderFlac.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileReaderFlac.hpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileReaderMp3.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileReaderMp3.hpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileReaderOgg.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileReaderOgg.hpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileReaderWav.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileReaderWav.hpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileWriterFlac.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileWriterFlac.hpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileWriterOgg.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileWriterOgg.hpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileWriterWav.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundFileWriterWav.hpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundRecorder.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundSource.cpp
        ${LIB_SOURCE_DIR}/SFML/Audio/SoundStream.cpp
    )

    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        target_compile_definitions(${LIB_NAME} PUBLIC SFML_STATIC=1)
    endif()

    target_compile_definitions(${LIB_NAME} PRIVATE SFML_AUDIO_EXPORTS=1)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/extlibs)

    # target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)

    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_system_310)
    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_window_310)
    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_graphics_310)
    target_link_libraries(${LIB_NAME} PUBLIC de_sfml_extlibs)
    # target_link_libraries(${LIB_NAME} PUBLIC de_miniaudio)
    target_link_libraries(${LIB_NAME} PUBLIC de_flac)
    target_link_libraries(${LIB_NAME} PUBLIC de_ogg)
    target_link_libraries(${LIB_NAME} PUBLIC de_vorbis)

endif()

add_subdirectory(test)
