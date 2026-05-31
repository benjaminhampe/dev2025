set(APP_NAME Manta_1.1.0_x64_standalone)
project(${APP_NAME})

if (TARGET ${APP_NAME})
   message("${BoldYellow}[benni] HEADER_GUARD: ${APP_NAME}${ColourReset}")
else()
    
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/juce_audio_plugin_client.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/juce_audio_plugin_client_config.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_CheckSettingMacros.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_CreatePluginFilter.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_IncludeModuleHeaders.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_IncludeSystemHeaders.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_LinuxMessageThread.h
        ${LIB_HEADER_DIR}/juce_audio_plugin_client/utility/juce_WindowsHooks.h
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/juce_audio_plugin_client/juce_audio_plugin_client_Standalone.cpp
        ${LIB_SOURCE_DIR}/juce_audio_plugin_client/juce_audio_plugin_client_utils.cpp
        ${LIB_SOURCE_DIR}/juce_audio_plugin_client/Standalone/juce_StandaloneFilterApp.cpp
        ${LIB_SOURCE_DIR}/juce_audio_plugin_client/utility/juce_PluginUtilities.cpp
        # ${LIB_SOURCE_DIR}/VST/juce_VST_Wrapper.cpp
    )

    include(../sources.cmake) # PLG_SOURCES | PLG_SOURCE_DIR -> PLG = PLUGIN
    list(APPEND LIB_SOURCES ${PLG_SOURCES})
    
    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    message("${BoldGreen}[benni] APP: ${APP_NAME}${ColourReset}")
    add_executable(${APP_NAME} "")

   # if (BUILD_SHARED)
   #   add_library(${APP_NAME} SHARED "")
   #   message("${BoldBlue}[benni] DLL: ${APP_NAME}${ColourReset}")
      # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL=1)
      # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL_BUILD=1)
   # else()
      # add_library(${APP_NAME} STATIC "")
      # message("${BoldCyan}[benni] LIB: ${APP_NAME}${ColourReset}")
      # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL=0)
      # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL_BUILD=0)
   # endif()

   # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL=0)
   # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_DLL_BUILD=0)

   # target_compile_definitions(${APP_NAME} PUBLIC -DJUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)

   target_sources(${APP_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

   # target_include_directories(${APP_NAME} PRIVATE ${JUCE_HEADER_DIR})

    target_include_directories(${APP_NAME} PUBLIC ${PLG_SOURCE_DIR})
    target_include_directories(${APP_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${APP_NAME} PRIVATE ${LIB_SOURCE_DIR})
   
   # target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR}/Engine)
   # target_include_directories(${APP_NAME} PUBLIC ${APP_SOURCE_DIR}/Gui)

   # target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/drivers)
   # target_include_directories(${APP_NAME} PRIVATE ${APP_SOURCE_DIR}/vendor)

   # target_compile_definitions(${APP_NAME} PUBLIC -DJucePlugin_Name="juce_vst2_plugin_001")

   # target_compile_options(${APP_NAME} PRIVATE -Wno-unused-but-set-variable)

   # target_compile_definitions(${APP_NAME} PRIVATE -DHAVE_CONFIG_H=1)


    # if(APPLE)
        # target_compile_definitions(${APP_NAME} PRIVATE ODIN_MAC=1)
    # elseif(UNIX AND NOT APPLE)
        # target_compile_definitions(${APP_NAME} PRIVATE ODIN_LINUX=1)
    # elseif(WIN32)
        # target_compile_definitions(${APP_NAME} PRIVATE ODIN_WIN=1)
    # endif()

    # if (MSVC)
    # else()
        # target_link_options(${APP_NAME} PRIVATE -mwindows) # disable console, also helps with win icon
        # target_compile_options(${APP_NAME} PUBLIC -Wno-multichar)

        # target_compile_options(${APP_NAME} PRIVATE -fconstexpr-depth=1024)
        # target_compile_options(${APP_NAME} PRIVATE -fbracket-depth=1024)
    # endif()

    # target_link_libraries(${APP_NAME} PRIVATE Odin2_src_common)

    target_link_libraries(${APP_NAME} PRIVATE de_juce_core)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_dsp)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_gui_basics)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_gui_extra)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_audio_basics)
    # target_link_libraries(${APP_NAME} PRIVATE de_juce_audio_devices)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_audio_processors)
    target_link_libraries(${APP_NAME} PRIVATE de_juce_audio_utils)
    # target_link_libraries(${APP_NAME} PRIVATE de_juce_cryptography)
    # target_link_libraries(${APP_NAME} PRIVATE de_juce_opengl)
    # target_link_libraries(${APP_NAME} PRIVATE de_tuning_library)
    # target_link_libraries(${APP_NAME} PRIVATE de_nlohmann_json)

    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    # set_target_properties(${LIB_NAME} PROPERTIES
        # PREFIX ""
        # # SUFFIX ".vst3"
        # OUTPUT_NAME "Odin2_2.4.1_x64_standalone"
    # )
   # COPY MEDIA FILES relative to exe
   # file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
         # DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/QIrrlichtWidget2019 )

   # file(COPY ${CMAKE_CURRENT_LIST_DIR}/tux.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
   # # file(COPY ${CMAKE_CURRENT_LIST_DIR}/pcc_logo.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

   # file(COPY ../media/Odin2 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)

   # copy theme resource files
   #ADD_CUSTOM_COMMAND( TARGET "${APP_NAME}"
   #   POST_BUILD
   #   COMMAND "echo" "Copying theme.."
   #      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/themes" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes"
   #   )
endif()
