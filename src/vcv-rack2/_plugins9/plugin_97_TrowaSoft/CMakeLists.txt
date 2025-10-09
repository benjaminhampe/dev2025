set(LIB_NAME plugin_97_TrowaSoft)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    

	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/Features.hpp
        ${LIB_SOURCE_DIR}/Module_multiOscillator.cpp
        ${LIB_SOURCE_DIR}/Module_multiOscillator.hpp
        ${LIB_SOURCE_DIR}/Module_multiScope.cpp
        ${LIB_SOURCE_DIR}/Module_multiScope.hpp
        ${LIB_SOURCE_DIR}/Module_multiSeq.cpp
        ${LIB_SOURCE_DIR}/Module_multiSeq.hpp
        ${LIB_SOURCE_DIR}/Module_oscCV.cpp
        ${LIB_SOURCE_DIR}/Module_oscCV.hpp
        ${LIB_SOURCE_DIR}/Module_oscCVExpander.cpp
        ${LIB_SOURCE_DIR}/Module_oscCVExpander.hpp
        ${LIB_SOURCE_DIR}/Module_polyGen.cpp
        ${LIB_SOURCE_DIR}/Module_polyGen.hpp
        ${LIB_SOURCE_DIR}/Module_trigSeq.cpp
        ${LIB_SOURCE_DIR}/Module_trigSeq.hpp
        ${LIB_SOURCE_DIR}/Module_voltSeq.cpp
        ${LIB_SOURCE_DIR}/Module_voltSeq.hpp
        ${LIB_SOURCE_DIR}/trowaSoft.cpp
        ${LIB_SOURCE_DIR}/trowaSoft.hpp
        ${LIB_SOURCE_DIR}/trowaSoftCLights.hpp
        ${LIB_SOURCE_DIR}/trowaSoftComponents.cpp
        ${LIB_SOURCE_DIR}/trowaSoftComponents.hpp
        ${LIB_SOURCE_DIR}/trowaSoftUtilities.cpp
        ${LIB_SOURCE_DIR}/trowaSoftUtilities.hpp
        ${LIB_SOURCE_DIR}/TSBlank.cpp
        ${LIB_SOURCE_DIR}/TSBlank.hpp
        ${LIB_SOURCE_DIR}/TSColors.hpp
        ${LIB_SOURCE_DIR}/TSExternalControlMessage.hpp
        ${LIB_SOURCE_DIR}/TSModuleBase.hpp
        ${LIB_SOURCE_DIR}/TSOSCBaseListener.hpp
        ${LIB_SOURCE_DIR}/TSOSCCommon.cpp
        ${LIB_SOURCE_DIR}/TSOSCCommon.hpp
        ${LIB_SOURCE_DIR}/TSOSCCommunicator.cpp
        ${LIB_SOURCE_DIR}/TSOSCCommunicator.hpp
        ${LIB_SOURCE_DIR}/TSOSCConfigWidget.cpp
        ${LIB_SOURCE_DIR}/TSOSCConfigWidget.hpp
        ${LIB_SOURCE_DIR}/TSOSCCV_Common.cpp
        ${LIB_SOURCE_DIR}/TSOSCCV_Common.hpp
        ${LIB_SOURCE_DIR}/TSOSCCV_RxConnector.cpp
        ${LIB_SOURCE_DIR}/TSOSCCV_RxConnector.hpp
        ${LIB_SOURCE_DIR}/TSOSCSequencerListener.cpp
        ${LIB_SOURCE_DIR}/TSOSCSequencerListener.hpp
        ${LIB_SOURCE_DIR}/TSOSCSequencerOutputMessages.hpp
        ${LIB_SOURCE_DIR}/TSParamQuantity.hpp
        ${LIB_SOURCE_DIR}/TSParamTextField.cpp
        ${LIB_SOURCE_DIR}/TSParamTextField.hpp
        ${LIB_SOURCE_DIR}/TSScopeBase.cpp
        ${LIB_SOURCE_DIR}/TSScopeBase.hpp
        ${LIB_SOURCE_DIR}/TSSequencerModuleBase.cpp
        ${LIB_SOURCE_DIR}/TSSequencerModuleBase.hpp
        ${LIB_SOURCE_DIR}/TSSequencerWidgetBase.cpp
        ${LIB_SOURCE_DIR}/TSSequencerWidgetBase.hpp
        ${LIB_SOURCE_DIR}/TSSModuleWidgetBase.hpp
        ${LIB_SOURCE_DIR}/TSTempoBPM.cpp
        ${LIB_SOURCE_DIR}/TSTempoBPM.hpp
        ${LIB_SOURCE_DIR}/TSTextField.cpp
        ${LIB_SOURCE_DIR}/TSTextField.hpp
        ${LIB_SOURCE_DIR}/Widget_multiOscillator.cpp
        ${LIB_SOURCE_DIR}/Widget_multiOscillator.hpp
        ${LIB_SOURCE_DIR}/Widget_multiScope.cpp
        ${LIB_SOURCE_DIR}/Widget_multiScope.hpp
        ${LIB_SOURCE_DIR}/Widget_oscCV.cpp
        ${LIB_SOURCE_DIR}/Widget_oscCV.hpp
        ${LIB_SOURCE_DIR}/Widget_oscCVExpander.cpp
        ${LIB_SOURCE_DIR}/Widget_oscCVExpander.hpp
        ${LIB_SOURCE_DIR}/Widget_polyGen.cpp
        ${LIB_SOURCE_DIR}/Widget_polyGen.hpp
        ${LIB_SOURCE_DIR}/Widget_trigSeq64.cpp
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
        target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} STATIC "")
    # endif()

    # target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    # target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
	
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")
    
    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    target_link_libraries(${LIB_NAME} PRIVATE de_oscpack)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

    # set_target_properties(plugin PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/plugin.def")

    # target_compile_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_compile_options(${LIB_NAME} PRIVATE -static-libgcc)
    # target_link_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_link_options(${LIB_NAME} PRIVATE -static-libgcc)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libRack2)
    else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows)

        # if (WIN32)
            # target_link_libraries(${LIB_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            # target_link_libraries(${LIB_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        # endif()
    endif()

    # if (CMAKE_BUILD_TYPE STREQUAL "Release")

        # --strip-debug : Removes only debugging symbols.
        # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        add_custom_command(
            TARGET ${LIB_NAME}
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${LIB_NAME}>
            COMMENT "Stripping debug symbols"
        )

    # endif()

    set(VCV_NAME "TrowaSoft")
    add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # Create working directory "z_benni/Gemini"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}"
        # Move libGemini.dll to working directory Gemini and rename to plugin.dll
        COMMAND ${CMAKE_COMMAND} -E rename
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lib${LIB_NAME}.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}/plugin.dll"
        # Compress directory Gemini to Gemini.tar.zst
        COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni"
                tar -cf "${VCV_NAME}.tar" "${VCV_NAME}"
        COMMAND zstd -o
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar.zst"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar"
        # Move Gemini.tar.zst to final destination and rename to .vcvplugin
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins"
        COMMAND ${CMAKE_COMMAND} -E rename
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni/${VCV_NAME}.tar.zst"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins/${VCV_NAME}.vcvplugin"
    )

    file(COPY media/${VCV_NAME} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni)
endif()

# add_subdirectory(test)
