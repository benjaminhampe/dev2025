add_subdirectory(0_tools)
add_subdirectory(1_libs)

add_subdirectory(Polyline_fltk)

add_subdirectory(wav2mp3_fltk)
add_subdirectory(win32)
add_subdirectory(win32_OpenGL_Overlay)
add_subdirectory(win32_OpenGL_Overlay2)
add_subdirectory(win32_OpenGL_Overlay3)

add_subdirectory(AVX_Mandelbrot_de_glew)

if (BENNI_USE_DEMOS)
    add_subdirectory(2_demos)

    if (BENNI_USE_QT)
        add_subdirectory(VHS_qt6)
        add_subdirectory(AbentonTest1_qt6)
        add_subdirectory(AbentonTest2_qt6)
        add_subdirectory(AbentonTest3_qt6)
        add_subdirectory(AbentonTest4_qt6_nanovg)
    endif()

    add_subdirectory(RoundRectShader)
    add_subdirectory(RoundRectShader2)
    add_subdirectory(RoundRectShader3)
    add_subdirectory(RoundRectShader4)
    # add_subdirectory(stream_WASAPI)
endif()

if (BENNI_USE_JUCE)
    add_subdirectory(4_juce)
endif()

add_subdirectory(5_plugins)

if (BENNI_USE_QT)
    add_subdirectory(AbentonChain_qt6)
endif()

# add_subdirectory(H3_Siedler)
#add_subdirectory(Kinescope)
add_subdirectory(Minecraft_54k)


if (BENNI_USE_VCV)
    add_subdirectory(vcv-rack2)
endif()

# add_subdirectory(VST_benni) # Only for reference now

add_subdirectory(minimal_vst2x_host)


if (BENNI_USE_RAYLIB)
    add_subdirectory(raylib)
endif()


if (BENNI_USE_WHISPER)
    add_subdirectory(whisper.cpp)
endif()

# add_subdirectory(DirectML_test)
#
# add_subdirectory(minimal_vst3_host)
# add_subdirectory(AbletonLive64-qt6) # uses old RtAudio_5
# add_subdirectory(AudioPluginHost-qt6)
# add_subdirectory(AbletonLive64-qt6)
# add_subdirectory(AudioPluginHost-qt6)
# add_subdirectory(minimal_vst2x_host)
# add_subdirectory(BpmStudioLite-qt6)
# add_subdirectory(Raytracing)

#add_subdirectory(test_chrono_vs_benni)
# add_subdirectory(2_examples)
#add_subdirectory(3_extra)
#add_subdirectory(4_bad)
#add_subdirectory(Qt6_DesktopOpenGL)
#add_subdirectory(Qt6_Piano)
#add_subdirectory(de-openexr-3.3.2)
#add_subdirectory(libDarkGDK_2025)
#add_subdirectory(libDarkGDK_2025_examples)
#add_subdirectory(AbletonLive64-qt-v2)
# add_subdirectory(test_chrono_duration_cast)
# add_subdirectory(AudioAnimator3D_irr_sfml_fftw3)
# add_subdirectory(VSTi_fltk)
# add_subdirectory(Audi_Q7-irr)
# add_subdirectory(CommandLine-qt)
# add_subdirectory(hb_paint-win32)
# add_subdirectory(AbletonLive64-qt)
# add_subdirectory(AzimutProjection-wgl)
# add_subdirectory(adtflog2html)
# add_subdirectory(linegraph2d-qt)
# add_subdirectory(Siedler-server-qt)
# add_subdirectory(QCfg)
#add_subdirectory(DarkGDK_2025)
#add_subdirectory(irr_cgal_clip_hmap_2_poly)
# add_subdirectory(scigraph-qt)
