set(APP_NAME polyphone_qt6)
project(${APP_NAME}-2.5.1 LANGUAGES CXX)

if (TARGET ${APP_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

	# ////////////////////
	# ====	  Qt6     ====
	# ////////////////////	
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    find_package(Qt6Core)
    find_package(Qt6Widgets)
	find_package(Qt6PrintSupport)
	find_package(Qt6Svg)
	find_package(Qt6Network)
    # find_package(Qt6OpenGL)
    # find_package(Qt6OpenGLWidgets)
	find_package(Qt6LinguistTools)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets PrintSupport Svg Network LinguistTools)
    # find_package(Qt6 COMPONENTS OpenGL OpenGLWidgets)
    include_directories(SYSTEM ${Qt6Widgets_INCLUDE_DIRS})
    add_definitions(${Qt6Widgets_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt6Widgets_EXECUTABLE_COMPILE_FLAGS}")

	# ////////////////////////////////////////////////////////////////////////
	# ====	 Sources:  .h, .cpp, .ui, .ts -> .qm -> .qrc, .qrc, .rc     ====
	# ////////////////////////////////////////////////////////////////////////
    set(APP_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE APP_SOURCES_H "${APP_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE APP_SOURCES_CPP "${APP_SOURCE_DIR}/*.cpp")
	file(GLOB_RECURSE APP_SOURCES_UI "${APP_SOURCE_DIR}/*.ui")
	file(GLOB_RECURSE APP_SOURCES_TS "${APP_SOURCE_DIR}/translation/*.ts")	
    set(APP_SOURCES 
		${APP_SOURCES_H} 
		${APP_SOURCES_CPP} 
		${APP_SOURCES_UI}
		${APP_SOURCES_TS})

    if (WIN32)
		set(APP_RC_DIR ${CMAKE_CURRENT_LIST_DIR}/res)	
        enable_language(RC)
        list(APPEND APP_SOURCES ${APP_RC_DIR}/polyphone.h)
        list(APPEND APP_SOURCES ${APP_RC_DIR}/polyphone.rc)
	    list(APPEND APP_SOURCES ${APP_RC_DIR}/polyphone.qrc)
    endif()

	# Step 1: Find all .ts files
	#         file(GLOB APP_SOURCES_TS "${CMAKE_SOURCE_DIR}/translations/*.ts")
	# Step 2: Compile .ts → .qm
	#		  qt_add_translations(APP_SOURCES_QM ${APP_SOURCES_TS})
	# Step 3: Generate .qrc file dynamically
	#		  set(QRC_PATH "${CMAKE_BINARY_DIR}/translations.qrc")
	#         set(QRC_CONTENT "<RCC>\n  <qresource prefix=\"/translations\">\n")
	#         foreach(qm ${APP_SOURCES_QM})
	# 	          # Make path relative to source dir for qrc
	# 	          file(RELATIVE_PATH REL_QM ${CMAKE_SOURCE_DIR} ${qm})
	# 	          string(APPEND QRC_CONTENT "    <file>${REL_QM}</file>\n")
	#         endforeach()
	#         string(APPEND QRC_CONTENT "  </qresource>\n</RCC>\n")
	#         file(WRITE ${QRC_PATH} "${QRC_CONTENT}")
	# Step 4: Embed translations into app
	#         qt_add_resources(TRANSLATION_RES ${QRC_PATH})	
	# Step 5: Add executable and link resources
	# 		  add_executable(MyApp main.cpp ${TRANSLATION_RES} )
	#         list(APPEND APP_SOURCES ${TRANSLATION_RES})
	
	# # Step 1: Find all .ts files
	# file(GLOB_RECURSE APP_SOURCES_TS "${APP_SOURCE_DIR}/translation/*.ts")
	
	# # Step 2: Compile .ts → .qm
	# qt_add_translations(APP_SOURCES_QM ${APP_SOURCES_TS})
	
	# # Step 3: Generate .qrc file dynamically
	# set(QRC_PATH "${CMAKE_BINARY_DIR}/translations.qrc")
	# set(QRC_CONTENT "<RCC>\n  <qresource prefix=\"/translations\">\n")
	# foreach(qm ${APP_SOURCES_QM})
		# # Make path relative to source dir for qrc
		# file(RELATIVE_PATH REL_QM ${CMAKE_SOURCE_DIR} ${qm})
		# string(APPEND QRC_CONTENT "    <file>${REL_QM}</file>\n")
	# endforeach()
	# string(APPEND QRC_CONTENT "  </qresource>\n</RCC>\n")
	# file(WRITE ${QRC_PATH} "${QRC_CONTENT}")
	
	# # Step 4: Embed translations into app
	# qt_add_resources(TRANSLATION_RES ${QRC_PATH})
	
	# # Step 5: Add executable and link resources
	# list(APPEND APP_SOURCES ${TRANSLATION_RES})
	
	# ////////////////////////////
	# ====	 Application     ====
	# ////////////////////////////
    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} ${APP_SOURCES})

    target_include_directories(${APP_NAME} PRIVATE
        ${APP_SOURCE_DIR}
        ${APP_SOURCE_DIR}/_lib
        ${APP_SOURCE_DIR}/_lib/qtsingleapplication
        ${APP_SOURCE_DIR}/_lib/_option_sfarklib
        ${APP_SOURCE_DIR}/_lib/_option_stk
        ${APP_SOURCE_DIR}/clavier
        ${APP_SOURCE_DIR}/context
        ${APP_SOURCE_DIR}/context/interface
        ${APP_SOURCE_DIR}/core
        ${APP_SOURCE_DIR}/core/input
        ${APP_SOURCE_DIR}/core/output
        ${APP_SOURCE_DIR}/core/model
        ${APP_SOURCE_DIR}/core/sample
        ${APP_SOURCE_DIR}/core/types
        ${APP_SOURCE_DIR}/dialogs
        ${APP_SOURCE_DIR}/editor
        ${APP_SOURCE_DIR}/editor/footers
        ${APP_SOURCE_DIR}/editor/graphics
        ${APP_SOURCE_DIR}/editor/modulator
        ${APP_SOURCE_DIR}/editor/overview
        ${APP_SOURCE_DIR}/editor/tools
        ${APP_SOURCE_DIR}/editor/widgets
        ${APP_SOURCE_DIR}/editor/tree
        ${APP_SOURCE_DIR}/extensions
        ${APP_SOURCE_DIR}/mainwindow
        ${APP_SOURCE_DIR}/player
        ${APP_SOURCE_DIR}/repository
        ${APP_SOURCE_DIR}/repository/browser
        ${APP_SOURCE_DIR}/repository/daily
        ${APP_SOURCE_DIR}/repository/widgets
        ${APP_SOURCE_DIR}/repository/user
        ${APP_SOURCE_DIR}/repository/soundfont
        ${APP_SOURCE_DIR}/repository/soundfont/viewer
        ${APP_SOURCE_DIR}/repository/soundfont/editor
        ${APP_SOURCE_DIR}/sound_engine
        ${APP_SOURCE_DIR}/sound_engine/elements
    )

    #target_compile_definitions(${APP_NAME} PRIVATE USE_LOCAL_RTAUDIO=1)
    #target_compile_definitions(${APP_NAME} PRIVATE USE_LOCAL_RTMIDI=1)
    target_compile_definitions(${APP_NAME} PRIVATE USE_LOCAL_STK=1)
    #target_compile_definitions(${APP_NAME} PRIVATE USE_WOLFSSL=1)
    target_compile_definitions(${APP_NAME} PRIVATE RT_AUDIO_5_2=1)
    #target_compile_definitions(${APP_NAME} PRIVATE NO_SF2_REPOSITORY=1)
    #target_compile_definitions(${APP_NAME} PRIVATE NO_SF2_CREATION=1)
    target_compile_definitions(${APP_NAME} PRIVATE SFTOOLS_NOXML=1)
    target_compile_definitions(${APP_NAME} PRIVATE SOFT_VERSION="2.5.1")
    target_compile_definitions(${APP_NAME} PRIVATE IDENTIFIER="")
    target_compile_definitions(${APP_NAME} PRIVATE CURRENT_YEAR="2025")

    target_link_libraries(${APP_NAME} PRIVATE Qt6::Core)
    target_link_libraries(${APP_NAME} PRIVATE Qt6::Widgets)
	target_link_libraries(${APP_NAME} PRIVATE Qt6::PrintSupport)
	target_link_libraries(${APP_NAME} PRIVATE Qt6::Svg)
	target_link_libraries(${APP_NAME} PRIVATE Qt6::Network)
    # target_link_libraries(${APP_NAME} PRIVATE Qt6::OpenGL)
    # target_link_libraries(${APP_NAME} PRIVATE Qt6::OpenGLWidgets)

    if (WIN32)
        target_link_libraries(${APP_NAME} PUBLIC ws2_32)
    endif()

    if (MSVC)
    else()
        target_link_options(${APP_NAME} PRIVATE -mwindows)
        
        # target_compile_options(${APP_NAME} PRIVATE -Werror=maybe-uninitialized)

        # target_link_options(${APP_NAME} PRIVATE -mwindows)

		target_link_options(${APP_NAME} PRIVATE -ffloat-store)

        # target_compile_options(${APP_NAME} PRIVATE -Werror=maybe-uninitialized)
        target_compile_options(${APP_NAME} PRIVATE -Wno-macro-redefined)
        # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-command-line-argument)

        # target_link_libraries(${APP_NAME} PRIVATE stdc++fs)
        target_link_libraries(${APP_NAME} PRIVATE pthread)
        target_link_libraries(${APP_NAME} PRIVATE m)
    endif()
	
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    target_link_libraries(${APP_NAME} PRIVATE de_openssl_static)
	target_link_libraries(${APP_NAME} PRIVATE de_zlib)
    # target_link_libraries(${APP_NAME} PRIVATE de_ogg)
    # target_link_libraries(${APP_NAME} PRIVATE de_vorbis)
    # target_link_libraries(${APP_NAME} PRIVATE de_vorbisfile)
	# target_link_libraries(${APP_NAME} PRIVATE de_vorbisenc)
	# target_link_libraries(${APP_NAME} PRIVATE de_flac)
	target_link_libraries(${APP_NAME} PRIVATE de_sndfile)
	target_link_libraries(${APP_NAME} PRIVATE de_ogg)
    target_link_libraries(${APP_NAME} PRIVATE de_rtaudio)
    target_link_libraries(${APP_NAME} PRIVATE de_rtmidi)
    # target_link_libraries(${APP_NAME} PRIVATE de_vst2sdk)
	# target_link_libraries(${APP_NAME} PRIVATE de_2504)

    # target_link_libraries(${APP_NAME} PRIVATE de_glm)
    # target_link_libraries(${APP_NAME} PRIVATE de_glew)

    # target_link_libraries(${APP_NAME} PRIVATE de_tinyxml2)

    # target_link_libraries(${APP_NAME} PRIVATE de_webp)

    # target_link_libraries(${APP_NAME} PRIVATE de_zlib)
    # target_link_libraries(${APP_NAME} PRIVATE de_bzip2)
    # target_link_libraries(${APP_NAME} PRIVATE de_minizip)
    # target_link_libraries(${APP_NAME} PRIVATE de_unzip)
    # target_link_libraries(${APP_NAME} PRIVATE de_png)
    # target_link_libraries(${APP_NAME} PRIVATE de_jpeg8d)
    # target_link_libraries(${APP_NAME} PRIVATE de_tiff4)
    # target_link_libraries(${APP_NAME} PRIVATE de_gif5)

    # target_link_libraries(${APP_NAME} PRIVATE de_OpenEXR)

    # target_link_libraries(${APP_NAME} PRIVATE de_fontawesome)
    # target_link_libraries(${APP_NAME} PRIVATE de_freetype2)    

    # target_link_libraries(${APP_NAME} PRIVATE de_rtaudio)
    # target_link_libraries(${APP_NAME} PRIVATE de_rtmidi)
    # target_link_libraries(${APP_NAME} PRIVATE de_sndfile)
    # target_link_libraries(${APP_NAME} PRIVATE de_minimp3)
   # #target_link_libraries(${APP_NAME} PRIVATE de_mpg123)
    # target_link_libraries(${APP_NAME} PRIVATE de_kissfft) # What i use
    # target_link_libraries(${APP_NAME} PRIVATE de_soundtouch) # see GSoundTouch impl

    # # target_link_libraries(${APP_NAME} PRIVATE de_vst2sdk)
    # # target_link_libraries(${APP_NAME} PRIVATE de_fftw3)   # Can be used
    # # target_link_libraries(${APP_NAME} PRIVATE de_ffmpeg)  # Audio + Video
    # # target_link_libraries(${APP_NAME} PRIVATE de_lame)    # Audio mp3 encoder
	

    # if (WIN32)
        # target_link_libraries(${APP_NAME} PRIVATE winmm)
		
        # target_link_libraries(${APP_NAME} PRIVATE comctl32)
        # target_link_libraries(${APP_NAME} PRIVATE user32)
        # target_link_libraries(${APP_NAME} PRIVATE gdi32)
        # target_link_libraries(${APP_NAME} PRIVATE kernel32)
        # target_link_libraries(${APP_NAME} PRIVATE ole32)
        # target_link_libraries(${APP_NAME} PRIVATE oleaut32)    
        # target_link_libraries(${APP_NAME} PRIVATE glmf32)
        # target_link_libraries(${APP_NAME} PRIVATE opengl32)
        # target_link_libraries(${APP_NAME} PRIVATE dwmapi)
        # target_link_libraries(${APP_NAME} PRIVATE shell32)

        # # target_link_libraries(${APP_NAME} PUBLIC glu32)
        # # target_link_libraries(${APP_NAME} PUBLIC SetupAPI)
        # target_link_libraries(${APP_NAME} PUBLIC wmcodecdspuuid)
        # target_link_libraries(${APP_NAME} PUBLIC mfuuid)
        # target_link_libraries(${APP_NAME} PUBLIC mf)
        # target_link_libraries(${APP_NAME} PUBLIC mfplat)
        # # target_link_libraries(${APP_NAME} PUBLIC mf3216 mapi32 mapistub mdminst imm32)
        # # target_link_libraries(${APP_NAME} PUBLIC uuid)
        # target_link_libraries(${APP_NAME} PUBLIC ks)
        # target_link_libraries(${APP_NAME} PUBLIC ksuser)
        # target_link_libraries(${APP_NAME} PUBLIC ksguid)
        
        # # target_link_libraries(${APP_NAME} PUBLIC dsound)
        # # target_link_libraries(${APP_NAME} PUBLIC comctl32)
        # # target_link_libraries(${APP_NAME} PUBLIC oleaut32)
        # # target_link_libraries(${APP_NAME} PUBLIC ole32)
        # # target_link_libraries(${APP_NAME} PUBLIC advapi32)        
    # else()
        # target_link_libraries(${APP_NAME} PRIVATE GL)
        # target_link_libraries(${APP_NAME} PRIVATE GLU)
        # target_link_libraries(${APP_NAME} PRIVATE X11)
        # target_link_libraries(${APP_NAME} PRIVATE Xxf86vm)
        # target_link_libraries(${APP_NAME} PRIVATE Xi)
        # target_link_libraries(${APP_NAME} PRIVATE Xcursor)
    # endif()
    
    
    # file(COPY media/H4 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)

    # ===========================
    # Recommended Optimizations:
    # ===========================
    # g++ -O3 -mtune=native -mavx2 -flto -o myprogram myprogram.cpp

    # ===========================
    # Extreme Optimizations: (loses compatibility to older CPU's)
    # ===========================
    # g++ -O3 -march=native -mavx2 -o myprogram_fast myprogram.cpp

    if (CMAKE_BUILD_TYPE STREQUAL "Release")

        message(STATUS "Building in Release mode, strip symbols")

        # 1. Stripping Debug Symbols from an Executable
        #  objcopy --strip-debug myprogram
        #   - Removes debugging symbols but keeps function names.
        #
        # 2. Creating a Separate Debug Symbol File
        # If you want to keep debugging symbols in a separate file for later use:
        #  objcopy --only-keep-debug myprogram myprogram.debug
        #  objcopy --strip-debug myprogram
        #  objcopy --add-gnu-debuglink=myprogram.debug myprogram

        # --strip-debug : Removes only debugging symbols.
        # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        # add_custom_command(
        #     TARGET ${APP_NAME}
        #     POST_BUILD
        #     COMMAND ${CMAKE_OBJCOPY} --strip-debug $<TARGET_FILE:${APP_NAME}>
        #     COMMENT "Stripping debug symbols"
        # )

    endif()
endif()
