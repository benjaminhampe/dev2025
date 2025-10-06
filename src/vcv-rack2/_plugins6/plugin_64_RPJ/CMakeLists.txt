set(LIB_NAME plugin_64_RPJ)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    

	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/RPJ.cpp
        ${LIB_SOURCE_DIR}/RPJ.hpp    
        ${LIB_SOURCE_DIR}/Audio.cpp
        ${LIB_SOURCE_DIR}/Audio.h
        ${LIB_SOURCE_DIR}/AudioDelay.cpp
        ${LIB_SOURCE_DIR}/AudioDelay.h
        ${LIB_SOURCE_DIR}/AudioDetector.cpp
        ${LIB_SOURCE_DIR}/AudioDetector.h
        ${LIB_SOURCE_DIR}/AudioFilter.cpp
        ${LIB_SOURCE_DIR}/AudioFilter.hpp
        ${LIB_SOURCE_DIR}/BiQuad.cpp
        ${LIB_SOURCE_DIR}/BiQuad.hpp
        ${LIB_SOURCE_DIR}/BlindCurve.cpp
        ${LIB_SOURCE_DIR}/BlindCurve.hpp
        ${LIB_SOURCE_DIR}/Brave.cpp
        ${LIB_SOURCE_DIR}/Brave.hpp
        ${LIB_SOURCE_DIR}/CircularRide.cpp
        ${LIB_SOURCE_DIR}/CircularRide.hpp
        ${LIB_SOURCE_DIR}/Display.cpp
        ${LIB_SOURCE_DIR}/Display.hpp
        ${LIB_SOURCE_DIR}/DrillingHoles.cpp
        ${LIB_SOURCE_DIR}/DrillingHoles.hpp
        ${LIB_SOURCE_DIR}/DryLand.cpp
        ${LIB_SOURCE_DIR}/DryLand.hpp
        ${LIB_SOURCE_DIR}/Easter.cpp
        ${LIB_SOURCE_DIR}/Easter.hpp
        ${LIB_SOURCE_DIR}/EnvelopeFollower.cpp
        ${LIB_SOURCE_DIR}/EnvelopeFollower.hpp
        ${LIB_SOURCE_DIR}/Essence.cpp
        ${LIB_SOURCE_DIR}/Essence.hpp
        ${LIB_SOURCE_DIR}/Estonia.cpp
        ${LIB_SOURCE_DIR}/Estonia.hpp
        ${LIB_SOURCE_DIR}/Gaza.cpp
        ${LIB_SOURCE_DIR}/Gaza.hpp
        ${LIB_SOURCE_DIR}/Gazpacho.cpp
        ${LIB_SOURCE_DIR}/Gazpacho.hpp
        ${LIB_SOURCE_DIR}/Genie.cpp
        ${LIB_SOURCE_DIR}/Genie.hpp
        ${LIB_SOURCE_DIR}/GenieExpander.cpp
        ${LIB_SOURCE_DIR}/GenieExpander.hpp
        ${LIB_SOURCE_DIR}/Grendel.cpp
        ${LIB_SOURCE_DIR}/Grendel.hpp
        ${LIB_SOURCE_DIR}/LadyNina.cpp
        ${LIB_SOURCE_DIR}/LadyNina.hpp
        ${LIB_SOURCE_DIR}/Lavender.cpp
        ${LIB_SOURCE_DIR}/Lavender.hpp
        ${LIB_SOURCE_DIR}/Montreal.cpp
        ${LIB_SOURCE_DIR}/Montreal.hpp
        ${LIB_SOURCE_DIR}/PigeonPlink.cpp
        ${LIB_SOURCE_DIR}/PigeonPlink.hpp
        ${LIB_SOURCE_DIR}/SugarMice.cpp
        ${LIB_SOURCE_DIR}/SugarMice.hpp
        ${LIB_SOURCE_DIR}/TheWeb.cpp
        ${LIB_SOURCE_DIR}/TheWeb.hpp
        ${LIB_SOURCE_DIR}/TuxOn.cpp
        ${LIB_SOURCE_DIR}/TuxOn.hpp
        ${LIB_SOURCE_DIR}/WDF.cpp
        ${LIB_SOURCE_DIR}/WDF.hpp
        ${LIB_SOURCE_DIR}/ZVA.cpp
        ${LIB_SOURCE_DIR}/ZVA.hpp
        ${LIB_SOURCE_DIR}/composites/ADSR16.h
        ${LIB_SOURCE_DIR}/composites/IComposite.h
        ${LIB_SOURCE_DIR}/composites/WidgetComposite.h
        ${LIB_SOURCE_DIR}/ctrl/PopupMenuParamWidget.h
        ${LIB_SOURCE_DIR}/ctrl/RPJButtons.hpp
        ${LIB_SOURCE_DIR}/ctrl/RPJKnobs.hpp
        ${LIB_SOURCE_DIR}/ctrl/RPJLights.hpp
        ${LIB_SOURCE_DIR}/ctrl/RPJPorts.hpp
        ${LIB_SOURCE_DIR}/ctrl/RPJVumeters.hpp
        ${LIB_SOURCE_DIR}/ctrl/SqHelper.h
        ${LIB_SOURCE_DIR}/ctrl/SqMenuItem.h
        ${LIB_SOURCE_DIR}/ctrl/SqUI.h
        ${LIB_SOURCE_DIR}/ctrl/SqWidgets.h
        ${LIB_SOURCE_DIR}/ctrl/ToggleButton.h
        ${LIB_SOURCE_DIR}/dep/dr_flac.h
        ${LIB_SOURCE_DIR}/dep/dr_mp3.h
        ${LIB_SOURCE_DIR}/dep/dr_wav.h        
        ${LIB_SOURCE_DIR}/dsp/BiquadFilter.h
        ${LIB_SOURCE_DIR}/dsp/BiquadParams.h
        ${LIB_SOURCE_DIR}/dsp/BiquadState.h
        ${LIB_SOURCE_DIR}/dsp/ButterworthFilterDesigner.cpp
        ${LIB_SOURCE_DIR}/dsp/ButterworthFilterDesigner.h
        ${LIB_SOURCE_DIR}/dsp/WVCODsp.h
        ${LIB_SOURCE_DIR}/dsp/simd.h
        ${LIB_SOURCE_DIR}/dsp/SimdBlocks.h
        ${LIB_SOURCE_DIR}/dsp/DspFilter.cpp
        ${LIB_SOURCE_DIR}/dsp/DspFilter.h
        ${LIB_SOURCE_DIR}/dsp/SqMath.h
        ${LIB_SOURCE_DIR}/dsp/arm_intrinsics_sub.h
        ${LIB_SOURCE_DIR}/dsp/AudioMath.cpp
        ${LIB_SOURCE_DIR}/dsp/AudioMath.h
        ${LIB_SOURCE_DIR}/dsp/IIRDecimator.h
        ${LIB_SOURCE_DIR}/dsp/LookupTable.h
        ${LIB_SOURCE_DIR}/dsp/LookupTableFactory.h
        ${LIB_SOURCE_DIR}/dsp/ObjectCache.cpp
        ${LIB_SOURCE_DIR}/dsp/ObjectCache.h
        ${LIB_SOURCE_DIR}/dsp/asserts.h
        ${LIB_SOURCE_DIR}/dsp/Divider.h
        ${LIB_SOURCE_DIR}/dsp/SqLog.h        
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
        target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} STATIC "")
    # endif()

    # target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    # target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
	
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")
    
    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

    # set_target_properties(plugin PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/plugin.def")

    # target_compile_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_compile_options(${LIB_NAME} PRIVATE -static-libgcc)
    # target_link_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_link_options(${LIB_NAME} PRIVATE -static-libgcc)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libRack2)
    else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows)

        # if (WIN32)
            # target_link_libraries(${LIB_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            # target_link_libraries(${LIB_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        # endif()
    endif()

    # if (CMAKE_BUILD_TYPE STREQUAL "Release")

        # --strip-debug : Removes only debugging symbols.
        # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        add_custom_command(
            TARGET ${LIB_NAME}
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${LIB_NAME}>
            COMMENT "Stripping debug symbols"
        )

    # endif()

    set(VCV_NAME "RPJ")
    add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # Create working directory "z_benni/Gemini"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}"
        # Move libGemini.dll to working directory Gemini and rename to plugin.dll
        COMMAND ${CMAKE_COMMAND} -E rename
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib${LIB_NAME}.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}/plugin.dll"
        # Compress directory Gemini to Gemini.tar.zst
        COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni"
                tar -cf "${VCV_NAME}.tar" "${VCV_NAME}"
        COMMAND zstd -o
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar.zst"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar"
        # Move Gemini.tar.zst to final destination and rename to .vcvplugin
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins"
        COMMAND ${CMAKE_COMMAND} -E rename
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar.zst"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins/${VCV_NAME}.vcvplugin"
    )

    file(COPY media/${VCV_NAME} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni)
endif()

# add_subdirectory(test)
