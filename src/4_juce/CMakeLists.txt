if (BENNI_USE_JUCE)
    add_subdirectory(de-juce-7.0.5) # Big lib, only activate when needed.

    add_subdirectory(soundtouch-plugin)
    add_subdirectory(rubberband-plugin)    
    
    add_subdirectory(APMG_VersatileCompressor)
        
    if (BENNI_USE_JUCE_EXTRA)
        add_subdirectory(Odin2)
        add_subdirectory(Obxd)
        add_subdirectory(BasicOsc)
        add_subdirectory(Manta)
        # add_subdirectory(OPL-1.0.0)
        # add_subdirectory(Synister)
        # add_subdirectory(TapSynth)
    endif()
    
    if (BENNI_USE_JUCE_BESPOKE)
        add_subdirectory(BespokeSynth)
    endif()
endif()
