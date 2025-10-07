set(LIB_NAME de_airwindows) # Several VST effects in one plugin
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/airwindows/AirWinBaseClass.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/AirWinBaseClass_pluginRegistry.cpp
        ${LIB_SOURCE_DIR}/ADClip7.cpp
        ${LIB_SOURCE_DIR}/ADClip7.h
        ${LIB_SOURCE_DIR}/ADClip7Proc.cpp
        ${LIB_SOURCE_DIR}/Air.cpp
        ${LIB_SOURCE_DIR}/Air.h
        ${LIB_SOURCE_DIR}/AirProc.cpp
        ${LIB_SOURCE_DIR}/Apicolypse.cpp
        ${LIB_SOURCE_DIR}/Apicolypse.h
        ${LIB_SOURCE_DIR}/ApicolypseProc.cpp
        ${LIB_SOURCE_DIR}/BassDrive.cpp
        ${LIB_SOURCE_DIR}/BassDrive.h
        ${LIB_SOURCE_DIR}/BassDriveProc.cpp
        ${LIB_SOURCE_DIR}/BitGlitter.cpp
        ${LIB_SOURCE_DIR}/BitGlitter.h
        ${LIB_SOURCE_DIR}/BitGlitterProc.cpp
        ${LIB_SOURCE_DIR}/BlockParty.cpp
        ${LIB_SOURCE_DIR}/BlockParty.h
        ${LIB_SOURCE_DIR}/BlockPartyProc.cpp
        ${LIB_SOURCE_DIR}/BrightAmbience2.cpp
        ${LIB_SOURCE_DIR}/BrightAmbience2.h
        ${LIB_SOURCE_DIR}/BrightAmbience2Proc.cpp
        ${LIB_SOURCE_DIR}/BussColors4.cpp
        ${LIB_SOURCE_DIR}/BussColors4.h
        ${LIB_SOURCE_DIR}/BussColors4Proc.cpp
        ${LIB_SOURCE_DIR}/ButterComp2.cpp
        ${LIB_SOURCE_DIR}/ButterComp2.h
        ${LIB_SOURCE_DIR}/ButterComp2Proc.cpp
        ${LIB_SOURCE_DIR}/Cabs.cpp
        ${LIB_SOURCE_DIR}/Cabs.h
        ${LIB_SOURCE_DIR}/CabsProc.cpp
        ${LIB_SOURCE_DIR}/Capacitor.cpp
        ${LIB_SOURCE_DIR}/Capacitor.h
        ${LIB_SOURCE_DIR}/CapacitorProc.cpp
        ${LIB_SOURCE_DIR}/Chamber.cpp
        ${LIB_SOURCE_DIR}/ChamberProc.cpp
        ${LIB_SOURCE_DIR}/ChromeOxide.cpp
        ${LIB_SOURCE_DIR}/ChromeOxideProc.cpp
        ${LIB_SOURCE_DIR}/Cojones.cpp
        ${LIB_SOURCE_DIR}/Cojones.h
        ${LIB_SOURCE_DIR}/CojonesProc.cpp
        ${LIB_SOURCE_DIR}/Compresaturator.cpp
        ${LIB_SOURCE_DIR}/Compresaturator.h
        ${LIB_SOURCE_DIR}/CompresaturatorProc.cpp
        ${LIB_SOURCE_DIR}/CrunchyGrooveWear.cpp
        ${LIB_SOURCE_DIR}/CrunchyGrooveWear.h
        ${LIB_SOURCE_DIR}/CrunchyGrooveWearProc.cpp
        ${LIB_SOURCE_DIR}/DeBess.cpp
        ${LIB_SOURCE_DIR}/DeBess.h
        ${LIB_SOURCE_DIR}/DeBessProc.cpp
        ${LIB_SOURCE_DIR}/DeEss.cpp
        ${LIB_SOURCE_DIR}/DeEss.h
        ${LIB_SOURCE_DIR}/DeEssProc.cpp
        ${LIB_SOURCE_DIR}/DeRez2.cpp
        ${LIB_SOURCE_DIR}/DeRez2.h
        ${LIB_SOURCE_DIR}/DeRez2Proc.cpp
        ${LIB_SOURCE_DIR}/DeckWrecka.cpp
        ${LIB_SOURCE_DIR}/DeckWrecka.h
        ${LIB_SOURCE_DIR}/DeckWreckaProc.cpp
        ${LIB_SOURCE_DIR}/Density.cpp
        ${LIB_SOURCE_DIR}/Density.h
        ${LIB_SOURCE_DIR}/DensityProc.cpp
        ${LIB_SOURCE_DIR}/Drive.cpp
        ${LIB_SOURCE_DIR}/Drive.h
        ${LIB_SOURCE_DIR}/DriveProc.cpp
        ${LIB_SOURCE_DIR}/DrumSlam.cpp
        ${LIB_SOURCE_DIR}/DrumSlam.h
        ${LIB_SOURCE_DIR}/DrumSlamProc.cpp
        ${LIB_SOURCE_DIR}/DubSub.cpp
        ${LIB_SOURCE_DIR}/DubSubProc.cpp
        ${LIB_SOURCE_DIR}/DubCenter.cpp
        ${LIB_SOURCE_DIR}/DubCenterProc.cpp
        ${LIB_SOURCE_DIR}/DustBunny.cpp
        ${LIB_SOURCE_DIR}/DustBunny.h
        ${LIB_SOURCE_DIR}/DustBunnyProc.cpp
        ${LIB_SOURCE_DIR}/FireAmp.cpp
        ${LIB_SOURCE_DIR}/FireAmpProc.cpp
        ${LIB_SOURCE_DIR}/Focus.cpp
        ${LIB_SOURCE_DIR}/Focus.h
        ${LIB_SOURCE_DIR}/FocusProc.cpp
        ${LIB_SOURCE_DIR}/Fracture.cpp
        ${LIB_SOURCE_DIR}/Fracture.h
        ${LIB_SOURCE_DIR}/FractureProc.cpp
        ${LIB_SOURCE_DIR}/Galactic.cpp
        ${LIB_SOURCE_DIR}/Galactic.h
        ${LIB_SOURCE_DIR}/GalacticProc.cpp
        ${LIB_SOURCE_DIR}/GlitchShifter.cpp
        ${LIB_SOURCE_DIR}/GlitchShifterProc.cpp
        ${LIB_SOURCE_DIR}/GrooveWear.cpp
        ${LIB_SOURCE_DIR}/GrooveWear.h
        ${LIB_SOURCE_DIR}/GrooveWearProc.cpp
        ${LIB_SOURCE_DIR}/HardVacuum.cpp
        ${LIB_SOURCE_DIR}/HardVacuum.h
        ${LIB_SOURCE_DIR}/HardVacuumProc.cpp
        ${LIB_SOURCE_DIR}/Hombre.cpp
        ${LIB_SOURCE_DIR}/Hombre.h
        ${LIB_SOURCE_DIR}/HombreProc.cpp
        ${LIB_SOURCE_DIR}/Infinity.cpp
        ${LIB_SOURCE_DIR}/Infinity.h
        ${LIB_SOURCE_DIR}/InfinityProc.cpp
        ${LIB_SOURCE_DIR}/IronOxide5.cpp
        ${LIB_SOURCE_DIR}/IronOxide5.h
        ${LIB_SOURCE_DIR}/IronOxide5Proc.cpp
        ${LIB_SOURCE_DIR}/Logical4.cpp
        ${LIB_SOURCE_DIR}/Logical4.h
        ${LIB_SOURCE_DIR}/Logical4Proc.cpp
        ${LIB_SOURCE_DIR}/Loud.cpp
        ${LIB_SOURCE_DIR}/Loud.h
        ${LIB_SOURCE_DIR}/LoudProc.cpp
        ${LIB_SOURCE_DIR}/Mackity.cpp
        ${LIB_SOURCE_DIR}/Mackity.h
        ${LIB_SOURCE_DIR}/MackityProc.cpp
        ${LIB_SOURCE_DIR}/MackEQ.cpp
        ${LIB_SOURCE_DIR}/MackEQ.h
        ${LIB_SOURCE_DIR}/MackEQProc.cpp
        ${LIB_SOURCE_DIR}/MatrixVerb.cpp
        ${LIB_SOURCE_DIR}/MatrixVerb.h
        ${LIB_SOURCE_DIR}/MatrixVerbProc.cpp
        ${LIB_SOURCE_DIR}/Melt.cpp
        ${LIB_SOURCE_DIR}/Melt.h
        ${LIB_SOURCE_DIR}/MeltProc.cpp
        ${LIB_SOURCE_DIR}/Mojo.cpp
        ${LIB_SOURCE_DIR}/Mojo.h
        ${LIB_SOURCE_DIR}/MojoProc.cpp
        ${LIB_SOURCE_DIR}/NCSeventeen.cpp
        ${LIB_SOURCE_DIR}/NCSeventeen.h
        ${LIB_SOURCE_DIR}/NCSeventeenProc.cpp
        ${LIB_SOURCE_DIR}/Noise.cpp
        ${LIB_SOURCE_DIR}/Noise.h
        ${LIB_SOURCE_DIR}/NoiseProc.cpp
        ${LIB_SOURCE_DIR}/NonlinearSpace.cpp
        ${LIB_SOURCE_DIR}/NonlinearSpaceProc.cpp
        ${LIB_SOURCE_DIR}/OneCornerClip.cpp
        ${LIB_SOURCE_DIR}/OneCornerClip.h
        ${LIB_SOURCE_DIR}/OneCornerClipProc.cpp
        ${LIB_SOURCE_DIR}/Pafnuty.cpp
        ${LIB_SOURCE_DIR}/PafnutyProc.cpp
        ${LIB_SOURCE_DIR}/PocketVerbs.cpp
        ${LIB_SOURCE_DIR}/PocketVerbs.h
        ${LIB_SOURCE_DIR}/PocketVerbsProc.cpp
        ${LIB_SOURCE_DIR}/Point.cpp
        ${LIB_SOURCE_DIR}/Point.h
        ${LIB_SOURCE_DIR}/PointProc.cpp
        ${LIB_SOURCE_DIR}/Pop.cpp
        ${LIB_SOURCE_DIR}/Pop.h
        ${LIB_SOURCE_DIR}/PopProc.cpp
        ${LIB_SOURCE_DIR}/PowerSag.cpp
        ${LIB_SOURCE_DIR}/PowerSagProc.cpp
        ${LIB_SOURCE_DIR}/Pressure4.cpp
        ${LIB_SOURCE_DIR}/Pressure4.h
        ${LIB_SOURCE_DIR}/Pressure4Proc.cpp
        ${LIB_SOURCE_DIR}/PyeWacket.cpp
        ${LIB_SOURCE_DIR}/PyeWacket.h
        ${LIB_SOURCE_DIR}/PyeWacketProc.cpp
        ${LIB_SOURCE_DIR}/SingleEndedTriode.cpp
        ${LIB_SOURCE_DIR}/SingleEndedTriode.h
        ${LIB_SOURCE_DIR}/SingleEndedTriodeProc.cpp
        ${LIB_SOURCE_DIR}/Slew.cpp
        ${LIB_SOURCE_DIR}/Slew.h
        ${LIB_SOURCE_DIR}/Slew2.cpp
        ${LIB_SOURCE_DIR}/Slew2.h
        ${LIB_SOURCE_DIR}/Slew2Proc.cpp
        ${LIB_SOURCE_DIR}/SlewProc.cpp
        ${LIB_SOURCE_DIR}/Spiral2.cpp
        ${LIB_SOURCE_DIR}/Spiral2.h
        ${LIB_SOURCE_DIR}/Spiral2Proc.cpp
        ${LIB_SOURCE_DIR}/StarChild.cpp
        ${LIB_SOURCE_DIR}/StarChild.h
        ${LIB_SOURCE_DIR}/StarChildProc.cpp
        ${LIB_SOURCE_DIR}/Surge.cpp
        ${LIB_SOURCE_DIR}/Surge.h
        ${LIB_SOURCE_DIR}/SurgeProc.cpp
        ${LIB_SOURCE_DIR}/TapeDust.cpp
        ${LIB_SOURCE_DIR}/TapeDustProc.cpp
        ${LIB_SOURCE_DIR}/ToTape6.cpp
        ${LIB_SOURCE_DIR}/ToTape6.h
        ${LIB_SOURCE_DIR}/ToTape6Proc.cpp
        ${LIB_SOURCE_DIR}/ToVinyl4.cpp
        ${LIB_SOURCE_DIR}/ToVinyl4Proc.cpp
        ${LIB_SOURCE_DIR}/TripleSpread.cpp
        ${LIB_SOURCE_DIR}/TripleSpread.h
        ${LIB_SOURCE_DIR}/TripleSpreadProc.cpp
        ${LIB_SOURCE_DIR}/UnBox.cpp
        ${LIB_SOURCE_DIR}/UnBox.h
        ${LIB_SOURCE_DIR}/UnBoxProc.cpp
        ${LIB_SOURCE_DIR}/Verbity.cpp
        ${LIB_SOURCE_DIR}/Verbity.h
        ${LIB_SOURCE_DIR}/VerbityProc.cpp
        ${LIB_SOURCE_DIR}/VariMu.cpp
        ${LIB_SOURCE_DIR}/VariMu.h
        ${LIB_SOURCE_DIR}/VariMuProc.cpp
        ${LIB_SOURCE_DIR}/VoiceOfTheStarship.cpp
        ${LIB_SOURCE_DIR}/VoiceOfTheStarship.h
        ${LIB_SOURCE_DIR}/VoiceOfTheStarshipProc.cpp
        ${LIB_SOURCE_DIR}/YBandpass.cpp
        ${LIB_SOURCE_DIR}/YBandpass.h
        ${LIB_SOURCE_DIR}/YBandpassProc.cpp
        ${LIB_SOURCE_DIR}/YHighpass.cpp
        ${LIB_SOURCE_DIR}/YHighpass.h
        ${LIB_SOURCE_DIR}/YHighpassProc.cpp
        ${LIB_SOURCE_DIR}/YLowpass.cpp
        ${LIB_SOURCE_DIR}/YLowpass.h
        ${LIB_SOURCE_DIR}/YLowpassProc.cpp
        ${LIB_SOURCE_DIR}/YNotch.cpp
        ${LIB_SOURCE_DIR}/YNotch.h
        ${LIB_SOURCE_DIR}/YNotchProc.cpp
        ${LIB_SOURCE_DIR}/ZBandpass2.cpp
        ${LIB_SOURCE_DIR}/ZBandpass2.h
        ${LIB_SOURCE_DIR}/ZBandpass2Proc.cpp
        ${LIB_SOURCE_DIR}/ZHighpass2.cpp
        ${LIB_SOURCE_DIR}/ZHighpass2.h
        ${LIB_SOURCE_DIR}/ZHighpass2Proc.cpp
        ${LIB_SOURCE_DIR}/ZLowpass2.cpp
        ${LIB_SOURCE_DIR}/ZLowpass2.h
        ${LIB_SOURCE_DIR}/ZLowpass2Proc.cpp
        ${LIB_SOURCE_DIR}/ZNotch2.cpp
        ${LIB_SOURCE_DIR}/ZNotch2.h
        ${LIB_SOURCE_DIR}/ZNotch2Proc.cpp
    )

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    # if (BUILD_SHARED)
    #     message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    #     add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBAIRWINDOWS=1)
    target_compile_definitions(${LIB_NAME} PUBLIC SURGE_INCLUDE_AIRWINDOWS=1)

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/airwindows)

    target_compile_definitions(${LIB_NAME} PRIVATE
        AudioEffectX=AirWinBaseClass
        VstPlugCategory=long
        $<$<BOOL:${WIN32}>:_USE_MATH_DEFINES>
    )

    # target_link_libraries(${LIB_NAME} PRIVATE de_glad_430)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libaudio)
    else()
        # target_link_libraries(${LIB_NAME} PUBLIC m)
        # target_link_libraries(${LIB_NAME} PUBLIC pthread)
    endif()

    # if (WIN32)
        # target_compile_definitions(${LIB_NAME} PUBLIC _WIN32_WINNT=0x0602)
        # target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_ASIO__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_DS__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __WINDOWS_WASAPI__=1)
        # target_link_libraries(${LIB_NAME} PUBLIC mfplat mf mfuuid wmcodecdspuuid)
        # #target_link_libraries(${LIB_NAME} PUBLIC mf3216 mapi32 mapistub mdminst imm32)
        # target_link_libraries(${LIB_NAME} PUBLIC dsound winmm ksuser uuid) # ks ksguid
        # target_link_libraries(${LIB_NAME} PUBLIC advapi32 ole32 oleaut32 comctl32 kernel32 user32)
        # target_link_libraries(${LIB_NAME} PUBLIC de_asiosdk)
    # else()
        # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_ALSA__=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC __UNIX_JACK__=1)
        # # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_PULSE__=1)
        # # target_compile_definitions(${LIB_NAME} PUBLIC __LINUX_OSS__=1)
        # target_link_libraries(${LIB_NAME} PUBLIC asound)
        # target_link_libraries(${LIB_NAME} PUBLIC jack)
        # # target_link_libraries(${LIB_NAME} PUBLIC de_sndfile)
    # endif()



endif()

# add_subdirectory(test)
