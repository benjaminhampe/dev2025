set(LIB_NAME Abenton_Spectrum3d_x64_vst2)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()

    set(PLG_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../src)
	set(PLG_SOURCES 
        ${PLG_SOURCE_DIR}/JucePluginConfig.h
		${PLG_SOURCE_DIR}/JuceHeader.h
		${PLG_SOURCE_DIR}/MyComponent.h
        ${PLG_SOURCE_DIR}/MyComponent.cpp
		${PLG_SOURCE_DIR}/MyEditor.h
		${PLG_SOURCE_DIR}/MyEditor.cpp
		${PLG_SOURCE_DIR}/MyProcessor.h
		${PLG_SOURCE_DIR}/MyProcessor.cpp
	)
    
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
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/juce_audio_plugin_client/utility/juce_PluginUtilities.cpp
        ${LIB_SOURCE_DIR}/juce_audio_plugin_client/VST/juce_VST_Wrapper.cpp
    )    
   
    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    add_library(${LIB_NAME} SHARED "")
    message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")

    target_sources(${LIB_NAME} PRIVATE ${LIB_SOURCES} ${PLG_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${PLG_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    
    # target_compile_definitions(${LIB_NAME} PUBLIC -D_UNICODE=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DUNICODE=1)
    
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_core)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_opengl)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_gui_basics)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_gui_extra)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_audio_basics)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_audio_devices)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_audio_processors)
    target_link_libraries(${LIB_NAME} PRIVATE de_juce_audio_utils)
    target_link_libraries(${LIB_NAME} PRIVATE de_glm)
    target_link_libraries(${LIB_NAME} PRIVATE de_glew)

    # # OpenMP
    # target_compile_options(${LIB_NAME} PRIVATE -fopenmp)
    # target_link_options(${LIB_NAME} PRIVATE -fopenmp)

    set(APP_NAME ${LIB_NAME})
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)

    set_target_properties(${LIB_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".dll"
        OUTPUT_NAME "Abenton_Spectrum3d_1.0.0_x64_vst2"
    )

    # COPY MEDIA FILES relative to exe
    # file( COPY        ${CMAKE_CURRENT_LIST_DIR}/media/tux.png
    # DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/QIrrlichtWidget2019 )

    # file(COPY ${CMAKE_CURRENT_LIST_DIR}/tux.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    # # file(COPY ${CMAKE_CURRENT_LIST_DIR}/pcc_logo.png DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

    # copy theme resource files
    #ADD_CUSTOM_COMMAND( TARGET "${LIB_NAME}"
    #   POST_BUILD
    #   COMMAND "echo" "Copying theme.."
    #      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/themes" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/themes"
    #   )
endif()
