set(LIB_NAME de_soundtouch_241)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS       
        ${LIB_HEADER_DIR}/SoundTouch_241/BPMDetect.h
        ${LIB_HEADER_DIR}/SoundTouch_241/FIFOSampleBuffer.h
        ${LIB_HEADER_DIR}/SoundTouch_241/FIFOSamplePipe.h
        ${LIB_HEADER_DIR}/SoundTouch_241/SoundTouch.h
        ${LIB_HEADER_DIR}/SoundTouch_241/soundtouch_config.h
        ${LIB_HEADER_DIR}/SoundTouch_241/STTypes.h        
    )
    set(LIB_SOURCES       
        ${LIB_SOURCE_DIR}/SoundTouch_241/AAFilter.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/AAFilter.h
        ${LIB_SOURCE_DIR}/SoundTouch_241/BPMDetect.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/cpu_detect.h
        ${LIB_SOURCE_DIR}/SoundTouch_241/cpu_detect_x86.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/FIFOSampleBuffer.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/FIRFilter.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/FIRFilter.h
        ${LIB_SOURCE_DIR}/SoundTouch_241/InterpolateCubic.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/InterpolateCubic.h
        ${LIB_SOURCE_DIR}/SoundTouch_241/InterpolateLinear.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/InterpolateLinear.h
        ${LIB_SOURCE_DIR}/SoundTouch_241/InterpolateShannon.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/InterpolateShannon.h
        ${LIB_SOURCE_DIR}/SoundTouch_241/mmx_optimized.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/PeakFinder.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/PeakFinder.h
        ${LIB_SOURCE_DIR}/SoundTouch_241/RateTransposer.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/RateTransposer.h
        ${LIB_SOURCE_DIR}/SoundTouch_241/SoundTouch.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/sse_optimized.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/TDStretch.cpp
        ${LIB_SOURCE_DIR}/SoundTouch_241/TDStretch.h
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/SoundTouch_241)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/SoundTouch_241)
    
    # target_compile_definitions(${LIB_NAME} PUBLIC SOUNDTOUCH_FLOAT_SAMPLES=1 )
    # target_compile_definitions(${LIB_NAME} PUBLIC SOUNDTOUCH_INTEGER_SAMPLES=1 )
    # target_compile_definitions(${LIB_NAME} PUBLIC SOUNDTOUCH_ALLOW_X86_OPTIMIZATIONS=1 ) # needs nasm?
    
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
