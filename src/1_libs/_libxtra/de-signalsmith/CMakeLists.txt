set(LIB_NAME de_signalsmith)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)
    # file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    # file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.cpp")

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/signalsmith-basics/analyser.h
		${LIB_HEADER_DIR}/signalsmith-basics/chorus.h
		${LIB_HEADER_DIR}/signalsmith-basics/crunch.h
		${LIB_HEADER_DIR}/signalsmith-basics/freq-shifter.h
		${LIB_HEADER_DIR}/signalsmith-basics/limiter.h
		${LIB_HEADER_DIR}/signalsmith-basics/reverb.h

		${LIB_HEADER_DIR}/signalsmith-dsp/common.h
		${LIB_HEADER_DIR}/signalsmith-dsp/curves.h
		${LIB_HEADER_DIR}/signalsmith-dsp/delay.h
		${LIB_HEADER_DIR}/signalsmith-dsp/envelopes.h
		${LIB_HEADER_DIR}/signalsmith-dsp/fft.h
		${LIB_HEADER_DIR}/signalsmith-dsp/filters.h
		${LIB_HEADER_DIR}/signalsmith-dsp/mix.h
		${LIB_HEADER_DIR}/signalsmith-dsp/perf.h
		${LIB_HEADER_DIR}/signalsmith-dsp/rates.h
		${LIB_HEADER_DIR}/signalsmith-dsp/spectral.h
		${LIB_HEADER_DIR}/signalsmith-dsp/windows.h

		${LIB_HEADER_DIR}/signalsmith-elliptic-blep/elliptic-blep.h

		${LIB_HEADER_DIR}/signalsmith-hilbert/hilbert.h
		${LIB_HEADER_DIR}/signalsmith-hilbert/plot.h
		${LIB_HEADER_DIR}/signalsmith-hilbert/simple-fft.h

		${LIB_HEADER_DIR}/signalsmith-linear/fft.h
		${LIB_HEADER_DIR}/signalsmith-linear/linear.h

		${LIB_HEADER_DIR}/signalsmith-linear/platform/fft-accelerate.h
		${LIB_HEADER_DIR}/signalsmith-linear/platform/fft-ipp.h
		${LIB_HEADER_DIR}/signalsmith-linear/platform/fft-pffft-double.h
		${LIB_HEADER_DIR}/signalsmith-linear/platform/fft-pffft.h
		${LIB_HEADER_DIR}/signalsmith-linear/platform/linear-accelerate.h
		${LIB_HEADER_DIR}/signalsmith-linear/stft.h

		${LIB_HEADER_DIR}/signalsmith-plot/heatmap.h
		${LIB_HEADER_DIR}/signalsmith-plot/plot.h

		${LIB_HEADER_DIR}/signalsmith-stretch/signalsmith-stretch.h

		${LIB_HEADER_DIR}/stfx/stfx-library.h
	)

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)	
	set(LIB_SOURCES 
		${LIB_SOURCE_DIR}/plot.cpp
	)

    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        # target_compile_definitions(${LIB_NAME} PUBLIC SIGNALSMITH_USE_PFFFT=1)
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
		
    target_compile_definitions(${LIB_NAME} PUBLIC SIGNALSMITH_USE_PFFFT=1)
        
    # target_compile_definitions(${LIB_NAME} PRIVATE PLUTOVG_BUILD=1)
    
    # target_compile_definitions(${LIB_NAME} PRIVATE PLUTOVG_DISABLE_FONT_FACE_CACHE_LOAD=1)
    
    #target_compile_definitions(${LIB_NAME} PUBLIC -D_IRR_STATIC_LIB_=1)
    # if (MSVC)
    # else()
    # target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
    # endif()
    # target_link_libraries(${LIB_NAME} PUBLIC de_irrlicht)
    # target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)
    # target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glm)
    # target_link_libraries(${LIB_NAME} PUBLIC de_glew)
    # #target_link_libraries(${LIB_NAME} PUBLIC de_glad)

    target_link_libraries(${LIB_NAME} PUBLIC m)
    target_link_libraries(${LIB_NAME} PUBLIC de_pffft)

    file(COPY media/signalsmith DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()

add_subdirectory(test)