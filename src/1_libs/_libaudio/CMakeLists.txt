add_subdirectory(de-freeverb)
add_subdirectory(de-kissfft)                # FFT
add_subdirectory(de-kissfft-cpp)                # FFT
add_subdirectory(de-celt-0.11)
add_subdirectory(_libsndfile)       # Import/Export WAV, FLAC, OGG, etc...
add_subdirectory(de-asiosdk-2.3)    # Steinberg ASIO driver, for RtAudio.
add_subdirectory(de-vst2sdk)        # Steinberg VSTi plugin importer.
add_subdirectory(de-fftw3-3.3.10-static)    # FFT
add_subdirectory(de-fftw3f-3.3.10-static)   # FFT
add_subdirectory(de-rtaudio-5.1.0)      # AudioHardwareBackend
add_subdirectory(de-rtaudio-6.0.1)      # AudioHardwareBackend
add_subdirectory(de-rtmidi-2.1.1)       # MIDI messages
add_subdirectory(de-soundtouch-1.9.2)   # Resampler + Pitch/Tempo change.
add_subdirectory(de-minimp3)        # MP3 importer
add_subdirectory(de-lame-3.100)     # MP3 exporter
add_subdirectory(de-taglib-1.11.1)  # MP3 id3 metadata, extract CoverArt.
add_subdirectory(de-samplerate-0.2.2)
add_subdirectory(de-portaudio-2015)
# add_subdirectory(de-portaudio-2021)
add_subdirectory(de-aften)
# add_subdirectory(de-jack2-1.9.22) # Many LinkerErrors in SharedMode
# add_subdirectory(de-jack2-shared)
add_subdirectory(de-vst3sdk-3.7.13)

# add_subdirectory(de-mpg123-1.23.8) # Got linker error for mpg123_length, dk why.
# add_subdirectory(de-mpg123-1.32.10) # Got linker error for mpg123_length, dk why.
# add_subdirectory(de-fmod-1.10)     # Broken? Not really for MinGW/GCC
# add_subdirectory(de-fmod-2.02.20)  # Broken? Not really for MinGW/GCC
# add_subdirectory(de-fmod-2.03.03)  # Broken? Not really for MinGW/GCC

# add_subdirectory(de-ffmpeg-4.3.2-win64-gcc-shared) # Import/Export MP4, AVI, MKV, etc...

# add_subdirectory(de-juce-7.0.5) # Big lib, only activate when needed.
