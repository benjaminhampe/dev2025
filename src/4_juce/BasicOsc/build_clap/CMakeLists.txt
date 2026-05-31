set(LIB_NAME BasicOsc_1.0.2_clap)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/juce_audio_plugin_client.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/juce_audio_plugin_client_config.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_CheckSettingMacros.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_CreatePluginFilter.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_IncludeModuleHeaders.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_IncludeSystemHeaders.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_LinuxMessageThread.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_WindowsHooks.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/CLAP/clap-juce-extensions.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/CLAP/clap-juce-wrapper.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/CLAP/clap-juce-entry.h
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/juce_audio_plugin_client/utility/juce_PluginUtilities.cpp
      # ${LIB_SOURCE_DIR}/juce_audio_plugin_client/CLAP/clap-juce-mac.mm
        ${LIB_SOURCE_DIR}/juce_audio_plugin_client/CLAP/clap-juce-extensions.cpp
        ${LIB_SOURCE_DIR}/juce_audio_plugin_client/CLAP/clap-juce-wrapper.cpp
        ${LIB_SOURCE_DIR}/juce_audio_plugin_client/CLAP/clap-juce-entry.cpp
	)

    include(../sources.cmake) # PLG_SOURCES | PLG_SOURCE_DIR
    list(APPEND LIB_SOURCES ${PLG_SOURCES})
    
    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    add_library(${LIB_NAME} SHARED "")

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${PLG_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC CLAP_JUCE_EXTENSIONS=1)
    target_compile_definitions(${LIB_NAME} PUBLIC JUCE_CLAP=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC JUCE_BUILD_VST3=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC JUCE_PLUGINHOST_VST3=1)
    
    # set_target_properties(${LIB_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    # target_link_options(${LIB_NAME} PRIVATE "-Wl,--export-all-symbols")

    # target_compile_options(${LIB_NAME} PRIVATE -fno-builtin)
    # target_compile_options(${LIB_NAME} PRIVATE -fno-stack-protector)
    # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)

    # target_compile_definitions(${LIB_NAME} PUBLIC -DJucePlugin_Name="juce_vst2_plugin_001")
    # target_compile_definitions(${LIB_NAME} PRIVATE -DHAVE_CONFIG_H=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -D_UNICODE=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DUNICODE=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_FLAC=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_OGGVORBIS=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CDBURNER=0)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CDREADER=0)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DJUCE_USE_CAMERA=0) # need #include <dshow.h><qedit.h><dshowasf.h>

    target_link_libraries(${LIB_NAME} PRIVATE de_juce_core)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_dsp)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_audio_basics)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_audio_processors)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_audio_utils)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_gui_basics)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_gui_extra)
    # target_link_libraries(${LIB_NAME} PRIVATE de_juce_opengl)
    target_link_libraries(${LIB_NAME} PUBLIC de_clap)
    target_link_libraries(${LIB_NAME} PUBLIC de_clap_helpers)
    # target_link_libraries(${LIB_NAME} PRIVATE de_glm)
    # target_link_libraries(${LIB_NAME} PRIVATE de_glew)
    # target_link_libraries(${LIB_NAME} PRIVATE de_tuning_library)
    # target_link_libraries(${LIB_NAME} PRIVATE de_nlohmann_json)
    
    # OpenMP
    # target_compile_options(${LIB_NAME} PRIVATE -fopenmp)
    # target_link_options(${LIB_NAME} PRIVATE -fopenmp)

    # if(APPLE)
        # target_compile_definitions(${LIB_NAME} PRIVATE ODIN_MAC=1)
    # elseif(UNIX AND NOT APPLE)
        # target_compile_definitions(${LIB_NAME} PRIVATE ODIN_LINUX=1)
    # elseif(WIN32)
        # target_compile_definitions(${LIB_NAME} PRIVATE ODIN_WIN=1)
    # endif()
    
    # if (MSVC)
    # else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows) # disable console, also helps with win icon
        # target_compile_options(${LIB_NAME} PUBLIC -Wno-multichar)
        # target_compile_options(${LIB_NAME} PRIVATE -fconstexpr-depth=1024)
        # target_compile_options(${LIB_NAME} PRIVATE -fbracket-depth=1024)
    # endif()
    
    set(APP_NAME ${LIB_NAME})
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    set_target_properties(${LIB_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".clap"
        OUTPUT_NAME "BasicOsc_1.0.2_x64"
        ARCHIVE_OUTPUT_NAME "BasicOsc_1.0.2_x64_clap"
    )

   # COPY MEDIA FILES relative to exe
   # file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
         # DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/QIrrlichtWidget2019 )

   # file(COPY ${CMAKE_CURRENT_LIST_DIR}/tux.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
   # # file(COPY ${CMAKE_CURRENT_LIST_DIR}/pcc_logo.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

   # file(COPY ../media/Odin2 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
    
   # copy theme resource files
   #ADD_CUSTOM_COMMAND( TARGET "${LIB_NAME}"
   #   POST_BUILD
   #   COMMAND "echo" "Copying theme.."
   #      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/themes" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes"
   #   )
endif()

add_subdirectory(test)