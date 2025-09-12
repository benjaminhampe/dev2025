set(LIB_NAME de_2504) # We dont like to clash with system libs.
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE LIB_HEADERS ${LIB_HEADER_DIR}/*.h)
    file(GLOB_RECURSE LIB_SOURCES_H   ${LIB_SOURCE_DIR}/*.h)
    file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    set(LIB_SOURCES ${LIB_SOURCES_H} ${LIB_SOURCES_CPP})
    # set(LIB_SOURCES
        # ${LIB_SOURCE_DIR}/vector/xmm_quantize_sub.c
    # )

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/de_resource.h")
        list(APPEND LIB_SOURCES "${CMAKE_CURRENT_LIST_DIR}/res/de_resource.rc")
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lame.rc ${LIB_SOURCE_DIR}/logoe.ico )
        set(DE_RESOURCE_RC "${CMAKE_CURRENT_LIST_DIR}/res/de_resource.rc" PARENT_SCOPE)
        # add_executable(myApp WIN32 main.cpp ${DE_RESOURCE_RC})
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}) # for lib user.
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}) # for lib src.
	
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_DE_RESOURCE_RC="${CMAKE_CURRENT_LIST_DIR}/res/de_resource.rc")
	

    if (WIN32)
        target_link_libraries(${LIB_NAME} PRIVATE comctl32 user32 gdi32 kernel32)
        target_link_libraries(${LIB_NAME} PRIVATE ole32 oleaut32)
    else()
        target_link_libraries(${LIB_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
    endif()

    if (MSVC)
    else()
        target_link_libraries(${LIB_NAME} PRIVATE pthread)
        target_link_libraries(${LIB_NAME} PRIVATE m)    
	endif()

    if (WIN32)
        target_link_libraries(${LIB_NAME} PRIVATE comctl32)
        target_link_libraries(${LIB_NAME} PRIVATE ole32)
        target_link_libraries(${LIB_NAME} PRIVATE oleaut32)    
        target_link_libraries(${LIB_NAME} PRIVATE glmf32)
        target_link_libraries(${LIB_NAME} PRIVATE opengl32)
        target_link_libraries(${LIB_NAME} PRIVATE dwmapi)
        target_link_libraries(${LIB_NAME} PRIVATE shell32)
        target_link_libraries(${LIB_NAME} PRIVATE user32)
        target_link_libraries(${LIB_NAME} PRIVATE gdi32)
        target_link_libraries(${LIB_NAME} PRIVATE kernel32)
        
        # target_link_libraries(${LIB_NAME} PUBLIC glu32)
        # target_link_libraries(${LIB_NAME} PUBLIC SetupAPI)
        # target_link_libraries(${LIB_NAME} PUBLIC wmcodecdspuuid)
        # target_link_libraries(${LIB_NAME} PUBLIC mfuuid)
        # target_link_libraries(${LIB_NAME} PUBLIC mf)
        # target_link_libraries(${LIB_NAME} PUBLIC mfplat)
        # target_link_libraries(${LIB_NAME} PUBLIC mf3216 mapi32 mapistub mdminst imm32)
        # target_link_libraries(${LIB_NAME} PUBLIC uuid)
        # target_link_libraries(${LIB_NAME} PUBLIC ks)
        # target_link_libraries(${LIB_NAME} PUBLIC ksuser)
        # target_link_libraries(${LIB_NAME} PUBLIC ksguid)
        target_link_libraries(${LIB_NAME} PUBLIC winmm)
        # target_link_libraries(${LIB_NAME} PUBLIC dsound)
        # target_link_libraries(${LIB_NAME} PUBLIC comctl32)
        # target_link_libraries(${LIB_NAME} PUBLIC oleaut32)
        # target_link_libraries(${LIB_NAME} PUBLIC ole32)
        # target_link_libraries(${LIB_NAME} PUBLIC advapi32)        
    else()
        target_link_libraries(${LIB_NAME} PRIVATE GL)
        target_link_libraries(${LIB_NAME} PRIVATE GLU)
        target_link_libraries(${LIB_NAME} PRIVATE X11)
        target_link_libraries(${LIB_NAME} PRIVATE Xxf86vm)
        target_link_libraries(${LIB_NAME} PRIVATE Xi)
        target_link_libraries(${LIB_NAME} PRIVATE Xcursor)
    endif()
    
    # target_link_libraries(${LIB_NAME} PRIVATE Qt6::Core)
    # target_link_libraries(${LIB_NAME} PRIVATE Qt6::Widgets)
    # target_link_libraries(${LIB_NAME} PRIVATE Qt6::OpenGL)
    # target_link_libraries(${LIB_NAME} PRIVATE Qt6::OpenGLWidgets)

    target_link_libraries(${LIB_NAME} PUBLIC de_glm)
    
    target_link_libraries(${LIB_NAME} PUBLIC de_glew)

    target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2)

    target_link_libraries(${LIB_NAME} PRIVATE de_webp)
	
    target_link_libraries(${LIB_NAME} PRIVATE de_zlib)
    target_link_libraries(${LIB_NAME} PRIVATE de_bzip2)
    target_link_libraries(${LIB_NAME} PRIVATE de_minizip)
    target_link_libraries(${LIB_NAME} PRIVATE de_unzip)
    target_link_libraries(${LIB_NAME} PRIVATE de_png)
    target_link_libraries(${LIB_NAME} PRIVATE de_jpeg8d)
    target_link_libraries(${LIB_NAME} PRIVATE de_tiff4)
    target_link_libraries(${LIB_NAME} PRIVATE de_gif5)
    target_link_libraries(${LIB_NAME} PRIVATE de_OpenEXR)

    target_link_libraries(${LIB_NAME} PUBLIC de_fontawesome)
    target_link_libraries(${LIB_NAME} PRIVATE de_freetype2)    

    # target_link_libraries(${LIB_NAME} PRIVATE de_assimp)
	
    # target_link_libraries(${LIB_NAME} PRIVATE de_rtaudio_601)
    # target_link_libraries(${LIB_NAME} PRIVATE de_sndfile)
    # target_link_libraries(${LIB_NAME} PRIVATE de_minimp3)
   # #target_link_libraries(${LIB_NAME} PRIVATE de_mpg123)
    # target_link_libraries(${LIB_NAME} PRIVATE de_soundtouch) # see GSoundTouch impl
    # target_link_libraries(${LIB_NAME} PRIVATE de_lame)    # Audio mp3 encoder
    # target_link_libraries(${LIB_NAME} PUBLIC de_kissfft) # What i use
    # target_link_libraries(${LIB_NAME} PUBLIC de_vst2sdk)
    # target_link_libraries(${LIB_NAME} PUBLIC de_rtmidi) # Need refactoring to make private.

    # target_link_libraries(${LIB_NAME} PRIVATE de_fftw3)   # Can be used
    # target_link_libraries(${LIB_NAME} PRIVATE de_ffmpeg)  # Audio + Video

    
    # file(COPY media/H3 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
	
endif()

add_subdirectory(test)
