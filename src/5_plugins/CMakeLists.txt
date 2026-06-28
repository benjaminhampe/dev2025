add_subdirectory(Abenton_SineMachine5_vst2)
add_subdirectory(Sine_wgl_vst3)
add_subdirectory(Sine_fltk_vst3)

if (BENNI_USE_JUCE)
    # add_subdirectory(Product_001_Abenton_Gain_juce)
    # add_subdirectory(Product_002_Abenton_Spektrum3d_juce)
    add_subdirectory(Abenton_Spektrum3d_juce)
    # add_subdirectory(Abenton_Gain_juce)
    add_subdirectory(Abenton_DarthVaderVoice_juce)
    add_subdirectory(Abenton_TimeStretch_juce)

endif()
