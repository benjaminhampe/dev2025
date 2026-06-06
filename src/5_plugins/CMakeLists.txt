add_subdirectory(Abenton_SineMachine5_vst2)

if (BENNI_USE_JUCE)
    # add_subdirectory(Product_001_Abenton_Gain_juce)
    # add_subdirectory(Product_002_Abenton_Spektrum3d_juce)
    add_subdirectory(Abenton_Spektrum3d_juce)
    # add_subdirectory(Abenton_Gain_juce)

endif()
