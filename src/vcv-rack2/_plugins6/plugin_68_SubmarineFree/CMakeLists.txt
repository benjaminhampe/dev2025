set(LIB_NAME plugin_68_SubmarineFree)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    

	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/SubmarineFree.cpp
        ${LIB_SOURCE_DIR}/SubmarineFree.hpp    
        ${LIB_SOURCE_DIR}/AG1.cpp
        ${LIB_SOURCE_DIR}/AG2.cpp
        ${LIB_SOURCE_DIR}/AO1.cpp
        ${LIB_SOURCE_DIR}/BB1.cpp
        ${LIB_SOURCE_DIR}/BP1.cpp
        ${LIB_SOURCE_DIR}/DN1.cpp
        ${LIB_SOURCE_DIR}/DO1.cpp
        ${LIB_SOURCE_DIR}/EN1.cpp
        ${LIB_SOURCE_DIR}/EO1.cpp
        ${LIB_SOURCE_DIR}/FF1.cpp
        ${LIB_SOURCE_DIR}/FF2.cpp
        ${LIB_SOURCE_DIR}/HS1.cpp
        ${LIB_SOURCE_DIR}/LA1.cpp
        ${LIB_SOURCE_DIR}/LA2.cpp
        ${LIB_SOURCE_DIR}/LD1.cpp
        ${LIB_SOURCE_DIR}/LT1.cpp
        ${LIB_SOURCE_DIR}/MZ9.cpp
        ${LIB_SOURCE_DIR}/NG1.cpp
        ${LIB_SOURCE_DIR}/NG2.cpp
        ${LIB_SOURCE_DIR}/OA1.cpp
        ${LIB_SOURCE_DIR}/OG1.cpp
        ${LIB_SOURCE_DIR}/OG2.cpp
        ${LIB_SOURCE_DIR}/PG1.cpp
        ${LIB_SOURCE_DIR}/PO1.cpp
        ${LIB_SOURCE_DIR}/PO2.cpp
        ${LIB_SOURCE_DIR}/SN1.cpp
        ${LIB_SOURCE_DIR}/SS1.cpp
        ${LIB_SOURCE_DIR}/TD1.cpp
        ${LIB_SOURCE_DIR}/TD2.cpp
        ${LIB_SOURCE_DIR}/TD3.cpp
        ${LIB_SOURCE_DIR}/TD4.cpp
        ${LIB_SOURCE_DIR}/TD5.cpp
        ${LIB_SOURCE_DIR}/TF1.cpp
        ${LIB_SOURCE_DIR}/VMX.cpp
        ${LIB_SOURCE_DIR}/WKX.cpp
        ${LIB_SOURCE_DIR}/WM1.cpp
        ${LIB_SOURCE_DIR}/XFX.cpp
        ${LIB_SOURCE_DIR}/XG1.cpp
        ${LIB_SOURCE_DIR}/XG2.cpp
        ${LIB_SOURCE_DIR}/XXX.cpp
        # ${LIB_SOURCE_DIR}/gates
        # ${LIB_SOURCE_DIR}/gates/2-AND
        # ${LIB_SOURCE_DIR}/gates/2-NAND
        # ${LIB_SOURCE_DIR}/gates/3-AND
        # ${LIB_SOURCE_DIR}/gates/3-NAND
        # ${LIB_SOURCE_DIR}/gates/4-AND
        # ${LIB_SOURCE_DIR}/gates/4-NAND
        # ${LIB_SOURCE_DIR}/gates/CLK-DELAY
        # ${LIB_SOURCE_DIR}/gates/D-TYPE-FLIPFLOP
        # ${LIB_SOURCE_DIR}/gates/D-TYPE-LATCH
        # ${LIB_SOURCE_DIR}/gates/DELAY
        # ${LIB_SOURCE_DIR}/gates/LATCH
        # ${LIB_SOURCE_DIR}/gates/NC
        # ${LIB_SOURCE_DIR}/gates/NOR
        # ${LIB_SOURCE_DIR}/gates/NOT
        # ${LIB_SOURCE_DIR}/gates/OR
        # ${LIB_SOURCE_DIR}/gates/SR-FLIPFLOP
        # ${LIB_SOURCE_DIR}/gates/SR-LATCH
        # ${LIB_SOURCE_DIR}/gates/XNOR
        # ${LIB_SOURCE_DIR}/gates/XOR        
        ${LIB_SOURCE_DIR}/shared/components.hpp
        ${LIB_SOURCE_DIR}/shared/DS.cpp
        ${LIB_SOURCE_DIR}/shared/DS.hpp
        ${LIB_SOURCE_DIR}/shared/EventWidgets.cpp
        ${LIB_SOURCE_DIR}/shared/ExtensionLight.cpp
        ${LIB_SOURCE_DIR}/shared/LightButton.cpp
        ${LIB_SOURCE_DIR}/shared/LightKnob.cpp
        ${LIB_SOURCE_DIR}/shared/MouseTransformWidget.cpp
        ${LIB_SOURCE_DIR}/shared/Port.cpp
        ${LIB_SOURCE_DIR}/shared/ResizeHandle.cpp
        ${LIB_SOURCE_DIR}/shared/Scheme.cpp
        ${LIB_SOURCE_DIR}/shared/SizeableModuleWidget.cpp
        ${LIB_SOURCE_DIR}/shared/SubLightSlider.cpp
        ${LIB_SOURCE_DIR}/shared/SubSwitch.cpp
        ${LIB_SOURCE_DIR}/shared/SubTooltip.cpp
        ${LIB_SOURCE_DIR}/shared/TextDisplay.cpp        
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
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
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

    set(VCV_NAME "SubmarineFree")
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
