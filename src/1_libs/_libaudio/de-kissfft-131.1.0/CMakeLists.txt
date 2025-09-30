set(LIB_NAME de_kissfft) # We dont like to clash with system libs.
project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/kissfft/kfc.h
        ${LIB_HEADER_DIR}/kissfft/kissfft.hh
        ${LIB_HEADER_DIR}/kissfft/kissfft_i32.hh
        ${LIB_HEADER_DIR}/kissfft/kiss_fft.h
        ${LIB_HEADER_DIR}/kissfft/kiss_fftnd.h
        ${LIB_HEADER_DIR}/kissfft/kiss_fftndr.h
        ${LIB_HEADER_DIR}/kissfft/kiss_fftr.h
        ${LIB_HEADER_DIR}/kissfft/kiss_fft_log.h
        ${LIB_HEADER_DIR}/kissfft/_kiss_fft_guts.h
	)

    set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/kfc.c
        ${LIB_SOURCE_DIR}/kiss_fft.c
        ${LIB_SOURCE_DIR}/kiss_fftnd.c
        ${LIB_SOURCE_DIR}/kiss_fftndr.c
        ${LIB_SOURCE_DIR}/kiss_fftr.c
	)

    if(WIN32)
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/kissfft)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_KISSFFT=1)
endif()   
