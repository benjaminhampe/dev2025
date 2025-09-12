set(LIB_NAME de_vst2sdk)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/vst2.x)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/pluginterfaces/vst2.x/aeffect.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst2.x/aeffectx.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst2.x/vstfxstore.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst2.x/aeffeditor.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst2.x/audioeffect.h
        ${LIB_HEADER_DIR}/pluginterfaces/vst2.x/audioeffectx.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/audioeffect.cpp
        ${LIB_SOURCE_DIR}/audioeffectx.cpp
        # ${LIB_SOURCE_DIR}/public.sdk/source/vst2.x/vstplugmain.cpp
    )

    if(WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
    endif()
    
endif()


