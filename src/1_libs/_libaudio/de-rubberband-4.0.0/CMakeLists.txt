set(LIB_NAME de_rubberband_400)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS       
        ${LIB_HEADER_DIR}/rubberband_400/rubberband-c.h
        ${LIB_HEADER_DIR}/rubberband_400/RubberBandLiveShifter.h
        ${LIB_HEADER_DIR}/rubberband_400/RubberBandStretcher.h
    )
    
    set(LIB_SOURCES       
        ${LIB_SOURCE_DIR}/common/Allocators.cpp
        ${LIB_SOURCE_DIR}/common/Allocators.h
        ${LIB_SOURCE_DIR}/common/BQResampler.cpp
        ${LIB_SOURCE_DIR}/common/BQResampler.h
        ${LIB_SOURCE_DIR}/common/FFT.cpp
        ${LIB_SOURCE_DIR}/common/FFT.h
        ${LIB_SOURCE_DIR}/common/FixedVector.h
        ${LIB_SOURCE_DIR}/common/HistogramFilter.h
        ${LIB_SOURCE_DIR}/common/Log.cpp
        ${LIB_SOURCE_DIR}/common/Log.h
        ${LIB_SOURCE_DIR}/common/mathmisc.cpp
        ${LIB_SOURCE_DIR}/common/mathmisc.h
        ${LIB_SOURCE_DIR}/common/MovingMedian.h
        ${LIB_SOURCE_DIR}/common/Profiler.cpp
        ${LIB_SOURCE_DIR}/common/Profiler.h
        ${LIB_SOURCE_DIR}/common/Resampler.cpp
        ${LIB_SOURCE_DIR}/common/Resampler.h
        ${LIB_SOURCE_DIR}/common/RingBuffer.h
        ${LIB_SOURCE_DIR}/common/SampleFilter.h
        ${LIB_SOURCE_DIR}/common/Scavenger.h
        ${LIB_SOURCE_DIR}/common/SingleThreadRingBuffer.h
        ${LIB_SOURCE_DIR}/common/StretchCalculator.cpp
        ${LIB_SOURCE_DIR}/common/StretchCalculator.h
        ${LIB_SOURCE_DIR}/common/sysutils.cpp
        ${LIB_SOURCE_DIR}/common/sysutils.h
        ${LIB_SOURCE_DIR}/common/Thread.cpp
        ${LIB_SOURCE_DIR}/common/Thread.h
        ${LIB_SOURCE_DIR}/common/VectorOps.h
        ${LIB_SOURCE_DIR}/common/VectorOpsComplex.cpp
        ${LIB_SOURCE_DIR}/common/VectorOpsComplex.h
        ${LIB_SOURCE_DIR}/common/Window.h

        ${LIB_SOURCE_DIR}/ext/float_cast/float_cast.h

        ${LIB_SOURCE_DIR}/ext/getopt/getopt.c
        ${LIB_SOURCE_DIR}/ext/getopt/getopt.h
        ${LIB_SOURCE_DIR}/ext/getopt/getopt_long.c
        ${LIB_SOURCE_DIR}/ext/getopt/unistd.h

        ${LIB_SOURCE_DIR}/ext/kissfft/kiss_fft.c
        ${LIB_SOURCE_DIR}/ext/kissfft/kiss_fft.h
        ${LIB_SOURCE_DIR}/ext/kissfft/kiss_fftr.c
        ${LIB_SOURCE_DIR}/ext/kissfft/kiss_fftr.h
        ${LIB_SOURCE_DIR}/ext/kissfft/kiss_fft_log.h
        ${LIB_SOURCE_DIR}/ext/kissfft/_kiss_fft_guts.h

        ${LIB_SOURCE_DIR}/ext/pommier/neon_mathfun.h
        ${LIB_SOURCE_DIR}/ext/pommier/sse_mathfun.h

        ${LIB_SOURCE_DIR}/ext/speex/resample.c
        ${LIB_SOURCE_DIR}/ext/speex/speex_resampler.h

        ${LIB_SOURCE_DIR}/faster/AudioCurveCalculator.cpp
        ${LIB_SOURCE_DIR}/faster/AudioCurveCalculator.h
        ${LIB_SOURCE_DIR}/faster/CompoundAudioCurve.cpp
        ${LIB_SOURCE_DIR}/faster/CompoundAudioCurve.h
        ${LIB_SOURCE_DIR}/faster/HighFrequencyAudioCurve.cpp
        ${LIB_SOURCE_DIR}/faster/HighFrequencyAudioCurve.h
        ${LIB_SOURCE_DIR}/faster/PercussiveAudioCurve.cpp
        ${LIB_SOURCE_DIR}/faster/PercussiveAudioCurve.h
        ${LIB_SOURCE_DIR}/faster/R2Stretcher.cpp
        ${LIB_SOURCE_DIR}/faster/R2Stretcher.h
        ${LIB_SOURCE_DIR}/faster/SilentAudioCurve.cpp
        ${LIB_SOURCE_DIR}/faster/SilentAudioCurve.h
        ${LIB_SOURCE_DIR}/faster/SincWindow.h
        ${LIB_SOURCE_DIR}/faster/StretcherChannelData.cpp
        ${LIB_SOURCE_DIR}/faster/StretcherChannelData.h
        ${LIB_SOURCE_DIR}/faster/StretcherProcess.cpp

        ${LIB_SOURCE_DIR}/finer/BinClassifier.h
        ${LIB_SOURCE_DIR}/finer/BinSegmenter.h
        ${LIB_SOURCE_DIR}/finer/Guide.h
        ${LIB_SOURCE_DIR}/finer/Peak.h
        ${LIB_SOURCE_DIR}/finer/PhaseAdvance.h
        ${LIB_SOURCE_DIR}/finer/R3LiveShifter.cpp
        ${LIB_SOURCE_DIR}/finer/R3LiveShifter.h
        ${LIB_SOURCE_DIR}/finer/R3Stretcher.cpp
        ${LIB_SOURCE_DIR}/finer/R3Stretcher.h

        ${LIB_SOURCE_DIR}/rubberband-c.cpp
        ${LIB_SOURCE_DIR}/RubberBandLiveShifter.cpp
        ${LIB_SOURCE_DIR}/RubberBandStretcher.cpp
    )

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    # if (BUILD_SHARED_LIBS)
       # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
       # add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/rubberband_400)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/SoundTouch_241)
    
    target_compile_definitions(${LIB_NAME} PUBLIC USE_BQRESAMPLER=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC USE_SPEEX=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC LACK_SINCOS=1 ) # needs nasm?
    # target_compile_definitions(${LIB_NAME} PUBLIC GETOPT_API=1)
    target_link_libraries(${LIB_NAME} PRIVATE de_kissfft)
    target_link_libraries(${LIB_NAME} PRIVATE de_speexdsp)
    
	if (MSVC)
        # target_compile_options(${LIB_NAME} PRIVATE /Z100)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libaudio)		
	else()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough
                              # -Wno-expansion-to-defined
                              # -Wno-unused-but-set-parameter )
        # target_link_libraries(${LIB_NAME} PUBLIC m)	
	endif()
endif()

# add_subdirectory(test)
