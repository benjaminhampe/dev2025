set(LIB_NAME de_soundtouch)
project(${LIB_NAME}-dev-1.9.2 CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/SoundTouch/BPMDetect.h
        ${LIB_HEADER_DIR}/SoundTouch/FIFOSampleBuffer.h
        ${LIB_HEADER_DIR}/SoundTouch/FIFOSamplePipe.h
        ${LIB_HEADER_DIR}/SoundTouch/SoundTouch.h
        ${LIB_HEADER_DIR}/SoundTouch/STTypes.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/AAFilter.cpp
        ${LIB_SOURCE_DIR}/AAFilter.h
        ${LIB_SOURCE_DIR}/BPMDetect.cpp
        ${LIB_SOURCE_DIR}/cpu_detect.h
        ${LIB_SOURCE_DIR}/cpu_detect_x86.cpp
        ${LIB_SOURCE_DIR}/FIFOSampleBuffer.cpp
        ${LIB_SOURCE_DIR}/FIRFilter.cpp
        ${LIB_SOURCE_DIR}/FIRFilter.h
        ${LIB_SOURCE_DIR}/InterpolateCubic.cpp
        ${LIB_SOURCE_DIR}/InterpolateCubic.h
        ${LIB_SOURCE_DIR}/InterpolateLinear.cpp
        ${LIB_SOURCE_DIR}/InterpolateLinear.h
        ${LIB_SOURCE_DIR}/InterpolateShannon.cpp
        ${LIB_SOURCE_DIR}/InterpolateShannon.h
        ${LIB_SOURCE_DIR}/mmx_optimized.cpp
        ${LIB_SOURCE_DIR}/PeakFinder.cpp
        ${LIB_SOURCE_DIR}/PeakFinder.h
        ${LIB_SOURCE_DIR}/RateTransposer.cpp
        ${LIB_SOURCE_DIR}/RateTransposer.h
        ${LIB_SOURCE_DIR}/SoundTouch.cpp
        ${LIB_SOURCE_DIR}/sse_optimized.cpp
        ${LIB_SOURCE_DIR}/TDStretch.cpp
        ${LIB_SOURCE_DIR}/TDStretch.h
    )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
       message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
       add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/SoundTouch)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC SOUNDTOUCH_FLOAT_SAMPLES=1 )
    # target_compile_definitions(${LIB_NAME} PUBLIC SOUNDTOUCH_INTEGER_SAMPLES=1 )
    # target_compile_definitions(${LIB_NAME} PUBLIC SOUNDTOUCH_ALLOW_X86_OPTIMIZATIONS=1 ) # needs nasm?
    
	if (MSVC)
        # target_compile_options(${LIB_NAME} PRIVATE /Z100)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libaudio)		
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough
                              # -Wno-expansion-to-defined
                              # -Wno-unused-but-set-parameter )
        target_link_libraries(${LIB_NAME} PUBLIC m)	
	endif()
endif()

add_subdirectory(test)
