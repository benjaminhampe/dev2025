set(LIB_NAME DarkSound)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    file(GLOB_RECURSE LIB_HEADERS ${LIB_HEADER_DIR}/*.h)

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE LIB_SOURCES_H ${LIB_SOURCE_DIR}/*.h)
    file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    set(LIB_SOURCES ${LIB_SOURCES_H} ${LIB_SOURCES_CPP})

    # set(LIB_HEADERS
        # ${LIB_HEADER_DIR}/DarkAudio.h
        # ${LIB_HEADER_DIR}/de/audio/DAC_LoopbackEngine_Miniaudio.h
        # ${LIB_HEADER_DIR}/de/audio/DAC_PlaybackEngine_RtAudio.h
        # ${LIB_HEADER_DIR}/de/audio/de_aligned_memory.h
        # ${LIB_HEADER_DIR}/de/audio/de_fft_pffft.h
        # ${LIB_HEADER_DIR}/de/audio/de_fft_generic.h
        # ${LIB_HEADER_DIR}/de/audio/IDspChainElement.h
    # )
    # set(LIB_SOURCES
        # ${LIB_SOURCE_DIR}/DarkAudio.cpp
        # ${LIB_SOURCE_DIR}/de/audio/DAC_LoopbackEngine_Miniaudio.cpp
        # ${LIB_SOURCE_DIR}/de/audio/DAC_PlaybackEngine_RtAudio.cpp
        # ${LIB_SOURCE_DIR}/de/audio/de_fft_pffft.cpp
        # ${LIB_SOURCE_DIR}/de/audio/de_fft_generic.cpp
        # ${LIB_SOURCE_DIR}/de/audio/de_kissfft.h
    # )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
      # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.qrc)
    endif()

    if (BUILD_SHARED_LIBS)
        message("${Blue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${Blue}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    if (MSVC)
    else()
        target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
        # target_link_libraries(${APP_NAME} PRIVATE stdc++fs)
        target_link_libraries(${LIB_NAME} PRIVATE pthread)
        target_link_libraries(${LIB_NAME} PRIVATE m)
    endif()

    if (WIN32)
        target_link_libraries(${LIB_NAME} PRIVATE comctl32 user32 gdi32 kernel32)
        target_link_libraries(${LIB_NAME} PRIVATE ole32 oleaut32)
    else()
        target_link_libraries(${LIB_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
    endif()

    # target_link_libraries(${LIB_NAME} PUBLIC DarkImage_2604)
    target_link_libraries(${LIB_NAME} PUBLIC DarkMP4)
    target_link_libraries(${LIB_NAME} PUBLIC de_alac)      # MP4 ALAC
    target_link_libraries(${LIB_NAME} PUBLIC de_faad2)     # MP4 AAC
    target_link_libraries(${LIB_NAME} PUBLIC de_opus)      # MP4 opus

endif()

# add_subdirectory(test)

