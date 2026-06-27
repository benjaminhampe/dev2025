add_subdirectory(_libcommon)
add_subdirectory(_libarchive)
add_subdirectory(_libimage)
add_subdirectory(_libgpu)
add_subdirectory(_libextra)
add_subdirectory(_libaudio)
add_subdirectory(_libassimp)
add_subdirectory(_libcad)
add_subdirectory(_libnetwork)
add_subdirectory(_libdatabase)


# add_subdirectory(DarkImage-25.04)
# add_subdirectory(DarkCoverArt-25.04)
# add_subdirectory(DarkMidi-25.04)
# add_subdirectory(DarkSIMD-25.04)
# add_subdirectory(DarkAudio)

add_subdirectory(DarkImage-26.04)   # Abenton Live64 Instrument
add_subdirectory(DarkGPU-26.04)     # Abenton Live64 Instrument
add_subdirectory(DarkMP4)           # Abenton Live64 Instrument -> import YouTube AAC audio from MP4 files

add_subdirectory(file2bin)

if (BENNI_USE_OPENCV)
    add_subdirectory(de-opencv-4.13.0)
endif()

if (BENNI_USE_QT)
	add_subdirectory(de-QtAdvancedDockingSystem-4.4.1)
endif()



# add_subdirectory(DarkMesh)

# add_subdirectory(_libphysics)
# add_subdirectory(de-clara)
# add_subdirectory(de-ode-0.16.5)

# Images
# Graphics

# Geometry
# Physics
# Boost
# CGAL
# OpenSCAD

# GUI
# FLTK
# wxWidgets3
# Enet
# Ginac
# sqlite3
# Audio,
# ffmpeg, libavcodec, etc..
# RtMidi, RtAudio
# Steinberg ASIO Treiber, Steinberg VSTi2.4 Audio Plugin SDK.
# Taglib
# Fmod1, Fmod2
# FFTW3, FFTW3f, KissFFT
# Juce ( 6 full Juce Apps + their respective VST2 Plugins in _example folder )
