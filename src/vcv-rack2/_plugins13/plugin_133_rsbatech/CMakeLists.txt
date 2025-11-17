set(LIB_NAME plugin_133_rsbatech)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/components.hpp    
        ${LIB_SOURCE_DIR}/helpers.hpp
        ${LIB_SOURCE_DIR}/MapModuleBase.hpp
        ${LIB_SOURCE_DIR}/OrestesOne.cpp
        ${LIB_SOURCE_DIR}/OrestesOne.hpp
        ${LIB_SOURCE_DIR}/OrestesOneWidget.cpp
        ${LIB_SOURCE_DIR}/OrestesOneWidget.hpp
        ${LIB_SOURCE_DIR}/OscController.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp
        ${LIB_SOURCE_DIR}/pluginsettings.cpp
        ${LIB_SOURCE_DIR}/pluginsettings.hpp
        ${LIB_SOURCE_DIR}/Pylades.cpp
        ${LIB_SOURCE_DIR}/Pylades.hpp
        ${LIB_SOURCE_DIR}/PyladesWidget.cpp
        ${LIB_SOURCE_DIR}/PyladesWidget.hpp
        ${LIB_SOURCE_DIR}/RSBATechModules.hpp
        ${LIB_SOURCE_DIR}/RSBATechModulesWidget.hpp
        
        ${LIB_SOURCE_DIR}/components/LedDisplayCenterChoiceEx.hpp
        ${LIB_SOURCE_DIR}/components/LedTextDisplay.hpp
        ${LIB_SOURCE_DIR}/components/LedTextField.hpp
        ${LIB_SOURCE_DIR}/components/MenuLabelEx.hpp
        ${LIB_SOURCE_DIR}/components/MidiWidget.hpp
        ${LIB_SOURCE_DIR}/components/ParamHandleIndicator.hpp
        ${LIB_SOURCE_DIR}/components/SubMenuSlider.hpp

        ${LIB_SOURCE_DIR}/digital/ScaledMapParam.hpp

        ${LIB_SOURCE_DIR}/osc/OscArgs.hpp
        ${LIB_SOURCE_DIR}/osc/OscBundle.hpp
        ${LIB_SOURCE_DIR}/osc/OscController.hpp
        ${LIB_SOURCE_DIR}/osc/OscMessage.hpp
        ${LIB_SOURCE_DIR}/osc/OscReceiver.hpp
        ${LIB_SOURCE_DIR}/osc/OscSender.hpp

        ${LIB_SOURCE_DIR}/ui/OverlayMessageWidget.hpp
        ${LIB_SOURCE_DIR}/ui/ParamWidgetContextExtender.hpp
        ${LIB_SOURCE_DIR}/ui/ThemedModuleWidget.hpp
        ${LIB_SOURCE_DIR}/ui/ViewportHelper.hpp

        # ${LIB_SOURCE_DIR}/osc/oscpack/ip/IpEndpointName.cpp
        # ${LIB_SOURCE_DIR}/osc/oscpack/ip/IpEndpointName.h
        # ${LIB_SOURCE_DIR}/osc/oscpack/ip/NetworkingUtils.h
        # ${LIB_SOURCE_DIR}/osc/oscpack/ip/PacketListener.h

        # ${LIB_SOURCE_DIR}/osc/oscpack/ip/posix/NetworkingUtils.cpp
        # ${LIB_SOURCE_DIR}/osc/oscpack/ip/posix/UdpSocket.cpp
        # ${LIB_SOURCE_DIR}/osc/oscpack/ip/TimerListener.h
        # ${LIB_SOURCE_DIR}/osc/oscpack/ip/UdpSocket.h

        # ${LIB_SOURCE_DIR}/osc/oscpack/ip/win32/NetworkingUtils.cpp
        # ${LIB_SOURCE_DIR}/osc/oscpack/ip/win32/UdpSocket.cpp

        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/MessageMappingOscPacketListener.h
        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/OscException.h
        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/OscHostEndianness.h
        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/OscOutboundPacketStream.cpp
        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/OscOutboundPacketStream.h
        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/OscPacketListener.h
        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/OscPrintReceivedElements.cpp
        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/OscPrintReceivedElements.h
        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/OscReceivedElements.cpp
        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/OscReceivedElements.h
        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/OscTypes.cpp
        # ${LIB_SOURCE_DIR}/osc/oscpack/osc/OscTypes.h
        # ${LIB_SOURCE_DIR}/osc/oscpack/README.md
        
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

    # target_compile_definitions(${LIB_NAME} PRIVATE HAVE_LIBRACK2=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    target_link_libraries(${LIB_NAME} PRIVATE de_oscpack)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

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

    # set_target_properties(${LIB_NAME} PROPERTIES
    #     OUTPUT_NAME "Fundamental-2.6.2-llvm-win-x64"
    #     SUFFIX ".dll"
    # )

    set(VCV_NAME "rsbatech")
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

    # add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E copy
            # "$<TARGET_FILE:${LIB_NAME}>"
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media/Rack2/plugins/myplugin.vcvplugin"
    # )

    # add_custom_command(TARGET ${LIB_NAME} POST_BUILD
        # COMMAND ${CMAKE_COMMAND} -E copy
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/myplugin.dll"
            # "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/vcvplugins/myplugin.vcvplugin"
    # )

    # target_link_libraries(${LIB_NAME} PUBLIC de_rack2_nanovg)

endif()

# add_subdirectory(test)
