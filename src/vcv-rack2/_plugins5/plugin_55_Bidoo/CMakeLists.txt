set(LIB_NAME plugin_55_Bidoo)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    add_subdirectory(lib)

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/bidoo_plugin.cpp
        ${LIB_SOURCE_DIR}/bidoo_plugin.hpp
        ${LIB_SOURCE_DIR}/ACNE.cpp
        ${LIB_SOURCE_DIR}/ANTN.cpp
        ${LIB_SOURCE_DIR}/BAFIS.cpp
        ${LIB_SOURCE_DIR}/BANCAU.cpp
        ${LIB_SOURCE_DIR}/BAR.cpp
        ${LIB_SOURCE_DIR}/BidooComponents.hpp
        ${LIB_SOURCE_DIR}/BISTROT.cpp
        ${LIB_SOURCE_DIR}/BORDL.cpp
        ${LIB_SOURCE_DIR}/CANARD.cpp
        ${LIB_SOURCE_DIR}/CHUTE.cpp
        ${LIB_SOURCE_DIR}/DFUZE.cpp
        ${LIB_SOURCE_DIR}/DIKTAT.cpp
        ${LIB_SOURCE_DIR}/DILEMO.cpp
        ${LIB_SOURCE_DIR}/DTROY.cpp
        ${LIB_SOURCE_DIR}/DUKE.cpp
        ${LIB_SOURCE_DIR}/EDSAROS.cpp
        ${LIB_SOURCE_DIR}/EMILE.cpp
        ${LIB_SOURCE_DIR}/ENCORE.cpp
        ${LIB_SOURCE_DIR}/ENCOREExpander.cpp
        ${LIB_SOURCE_DIR}/FLAME.cpp
        ${LIB_SOURCE_DIR}/FORK.cpp
        ${LIB_SOURCE_DIR}/FREIN.cpp
        ${LIB_SOURCE_DIR}/HCTIP.cpp
        ${LIB_SOURCE_DIR}/HUITRE.cpp
        ${LIB_SOURCE_DIR}/LAMBDA.cpp
        ${LIB_SOURCE_DIR}/LATE.cpp
        ${LIB_SOURCE_DIR}/LIMBO.cpp
        ${LIB_SOURCE_DIR}/LIMONADE.cpp
        ${LIB_SOURCE_DIR}/LOURDE.cpp
        ${LIB_SOURCE_DIR}/MAGMA.cpp
        ${LIB_SOURCE_DIR}/MINIBAR.cpp
        ${LIB_SOURCE_DIR}/MOIRE.cpp
        ${LIB_SOURCE_DIR}/MS.cpp
        ${LIB_SOURCE_DIR}/MU.cpp
        ${LIB_SOURCE_DIR}/OAI.cpp
        ${LIB_SOURCE_DIR}/OUAIVE.cpp
        ${LIB_SOURCE_DIR}/PERCO.cpp
        ${LIB_SOURCE_DIR}/PILOT.cpp
        ${LIB_SOURCE_DIR}/POUPRE.cpp
        ${LIB_SOURCE_DIR}/RABBIT.cpp
        ${LIB_SOURCE_DIR}/RATEAU.cpp
        ${LIB_SOURCE_DIR}/REI.cpp
        ${LIB_SOURCE_DIR}/SIGMA.cpp
        ${LIB_SOURCE_DIR}/SPORE.cpp
        ${LIB_SOURCE_DIR}/TIARE.cpp
        ${LIB_SOURCE_DIR}/TOCANTE.cpp
        ${LIB_SOURCE_DIR}/VOID_.cpp
        ${LIB_SOURCE_DIR}/ZINC.cpp
        ${LIB_SOURCE_DIR}/ZOUMAI.cpp
        ${LIB_SOURCE_DIR}/ZOUMAIExpander.cpp
        ${LIB_SOURCE_DIR}/dep/AudioFile/AudioFile.h
        ${LIB_SOURCE_DIR}/dep/dr_wav/dr_wav.h
        ${LIB_SOURCE_DIR}/dep/filters/biquad.cpp
        ${LIB_SOURCE_DIR}/dep/filters/biquad.h
        ${LIB_SOURCE_DIR}/dep/filters/fftanalysis.h
        ${LIB_SOURCE_DIR}/dep/filters/fftfilter.h
        ${LIB_SOURCE_DIR}/dep/filters/fftsynth.h
        ${LIB_SOURCE_DIR}/dep/filters/pitchshifter.h
        ${LIB_SOURCE_DIR}/dep/freeverb/allpass.cpp
        ${LIB_SOURCE_DIR}/dep/freeverb/allpass.hpp
        ${LIB_SOURCE_DIR}/dep/freeverb/comb.cpp
        ${LIB_SOURCE_DIR}/dep/freeverb/comb.hpp
        ${LIB_SOURCE_DIR}/dep/freeverb/revmodel.cpp
        ${LIB_SOURCE_DIR}/dep/freeverb/revmodel.hpp
        ${LIB_SOURCE_DIR}/dep/freeverb/tuning.hh
        # ${LIB_SOURCE_DIR}/dep/gverb/include/gverb.h
        # ${LIB_SOURCE_DIR}/dep/gverb/include/gverbdsp.h
        # ${LIB_SOURCE_DIR}/dep/gverb/include/ladspa-util.h
        # ${LIB_SOURCE_DIR}/dep/gverb/src/gverb.c
        # ${LIB_SOURCE_DIR}/dep/gverb/src/gverbdsp.c
        ${LIB_SOURCE_DIR}/dep/lodepng/lodepng.cpp
        ${LIB_SOURCE_DIR}/dep/lodepng/lodepng.h
        ${LIB_SOURCE_DIR}/dep/lodepng/lodepng_util.cpp
        ${LIB_SOURCE_DIR}/dep/lodepng/lodepng_util.h
        ${LIB_SOURCE_DIR}/dep/lodepng/pngdetail.cpp
        # ${LIB_SOURCE_DIR}/dep/minimp3/minimp3.h
        ${LIB_SOURCE_DIR}/dep/osc/wtOsc.h
        # ${LIB_SOURCE_DIR}/dep/pffft/fftpack.c
        # ${LIB_SOURCE_DIR}/dep/pffft/fftpack.h
        # ${LIB_SOURCE_DIR}/dep/pffft/pffft.c
        # ${LIB_SOURCE_DIR}/dep/pffft/pffft.h
        ${LIB_SOURCE_DIR}/dep/quantizer.cpp
        ${LIB_SOURCE_DIR}/dep/quantizer.hpp
        ${LIB_SOURCE_DIR}/dep/resampler/BaseVoiceState.cpp
        ${LIB_SOURCE_DIR}/dep/resampler/BaseVoiceState.h
        ${LIB_SOURCE_DIR}/dep/resampler/def.h
        ${LIB_SOURCE_DIR}/dep/resampler/Downsampler2Flt.cpp
        ${LIB_SOURCE_DIR}/dep/resampler/Downsampler2Flt.h
        ${LIB_SOURCE_DIR}/dep/resampler/Downsampler2Flt.hpp
        ${LIB_SOURCE_DIR}/dep/resampler/Fixed3232.h
        ${LIB_SOURCE_DIR}/dep/resampler/fnc.h
        ${LIB_SOURCE_DIR}/dep/resampler/fnc.hpp
        ${LIB_SOURCE_DIR}/dep/resampler/Int16.h
        ${LIB_SOURCE_DIR}/dep/resampler/Int32.h
        ${LIB_SOURCE_DIR}/dep/resampler/Int64.h
        ${LIB_SOURCE_DIR}/dep/resampler/InterpFlt.h
        ${LIB_SOURCE_DIR}/dep/resampler/InterpFlt.hpp
        ${LIB_SOURCE_DIR}/dep/resampler/InterpFltPhase.h
        ${LIB_SOURCE_DIR}/dep/resampler/InterpFltPhase.hpp
        ${LIB_SOURCE_DIR}/dep/resampler/InterpPack.cpp
        ${LIB_SOURCE_DIR}/dep/resampler/InterpPack.h
#       ${LIB_SOURCE_DIR}/dep/resampler/main.cpp
        ${LIB_SOURCE_DIR}/dep/resampler/MipMapFlt.cpp
        ${LIB_SOURCE_DIR}/dep/resampler/MipMapFlt.h
        ${LIB_SOURCE_DIR}/dep/resampler/MipMapFlt.hpp
        ${LIB_SOURCE_DIR}/dep/resampler/ResamplerFlt.cpp
        ${LIB_SOURCE_DIR}/dep/resampler/ResamplerFlt.h
        ${LIB_SOURCE_DIR}/dep/resampler/StopWatch.cpp
        ${LIB_SOURCE_DIR}/dep/resampler/StopWatch.h
        ${LIB_SOURCE_DIR}/dep/resampler/StopWatch.hpp
        ${LIB_SOURCE_DIR}/dep/resampler/UInt32.h
        ${LIB_SOURCE_DIR}/dep/waves.cpp
        ${LIB_SOURCE_DIR}/dep/waves.hpp
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
    target_link_libraries(${LIB_NAME} PRIVATE de_curl_shared)
    target_link_libraries(${LIB_NAME} PRIVATE de_gverb)
    target_link_libraries(${LIB_NAME} PRIVATE de_pffft)
    target_link_libraries(${LIB_NAME} PRIVATE de_minimp3)
    target_link_libraries(${LIB_NAME} PRIVATE pthread)

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

    set(VCV_NAME "Bidoo")
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
