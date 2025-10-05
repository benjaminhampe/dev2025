set(LIB_NAME plugin_53_rcm)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    

	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/BaseWidget.cpp
        ${LIB_SOURCE_DIR}/BaseWidget.hpp
        ${LIB_SOURCE_DIR}/ButtonTest.cpp
        ${LIB_SOURCE_DIR}/Consts.hpp
        ${LIB_SOURCE_DIR}/CV0to10.cpp
        ${LIB_SOURCE_DIR}/CV5to5.cpp
        ${LIB_SOURCE_DIR}/CVMmt.cpp
        ${LIB_SOURCE_DIR}/CVS0to10.cpp
        ${LIB_SOURCE_DIR}/CVTgl.cpp
        ${LIB_SOURCE_DIR}/DuckModule.cpp
        ${LIB_SOURCE_DIR}/gverb.c
        ${LIB_SOURCE_DIR}/gverb.h
        ${LIB_SOURCE_DIR}/gverbdsp.c
        ${LIB_SOURCE_DIR}/gverbdsp.h
        ${LIB_SOURCE_DIR}/GVerbModule.cpp
        ${LIB_SOURCE_DIR}/ladspa-util.h
        ${LIB_SOURCE_DIR}/ModuleDragType.cpp
        ${LIB_SOURCE_DIR}/ModuleDragType.hpp
        ${LIB_SOURCE_DIR}/ModuleTextWidget.cpp
        ${LIB_SOURCE_DIR}/ModuleTextWidget.hpp
        ${LIB_SOURCE_DIR}/PianoRollModule.cpp
        ${LIB_SOURCE_DIR}/rcm_plugin.cpp
        ${LIB_SOURCE_DIR}/rcm_plugin.hpp
        ${LIB_SOURCE_DIR}/PolyNos.cpp
        ${LIB_SOURCE_DIR}/SEQAdapter.cpp        
        ${LIB_SOURCE_DIR}/SongRollModule.cpp
        ${LIB_SOURCE_DIR}/Sync.cpp
        ${LIB_SOURCE_DIR}/ValueChangeTrigger.hpp        
      # ${LIB_SOURCE_DIR}/PianoRoll
        ${LIB_SOURCE_DIR}/PianoRoll/Auditioner.cpp
        ${LIB_SOURCE_DIR}/PianoRoll/Auditioner.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/DragModes.cpp
        ${LIB_SOURCE_DIR}/PianoRoll/DragModes.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/PatternData.cpp
        ${LIB_SOURCE_DIR}/PianoRoll/PatternData.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/PatternWidget.cpp
        ${LIB_SOURCE_DIR}/PianoRoll/PatternWidget.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/PianoRollModule.cpp
        ${LIB_SOURCE_DIR}/PianoRoll/PianoRollModule.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/PianoRollWidget.cpp
        ${LIB_SOURCE_DIR}/PianoRoll/PianoRollWidget.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/RollAreaWidget.cpp
        ${LIB_SOURCE_DIR}/PianoRoll/RollAreaWidget.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/Transport.cpp
        ${LIB_SOURCE_DIR}/PianoRoll/Transport.hpp        
      # ${LIB_SOURCE_DIR}/PianoRoll/MenuItems
        ${LIB_SOURCE_DIR}/PianoRoll/MenuItems/CancelPasteItem.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/MenuItems/ClearNotesItem.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/MenuItems/ClockBufferItem.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/MenuItems/CopyMeasureItem.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/MenuItems/CopyPatternItem.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/MenuItems/NotesToShowItem.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/MenuItems/PasteMeasureItem.hpp
        ${LIB_SOURCE_DIR}/PianoRoll/MenuItems/PastePatternItem.hpp
      # ${LIB_SOURCE_DIR}/SongRoll
        ${LIB_SOURCE_DIR}/SongRoll/ClockDivControlWidget.cpp
        ${LIB_SOURCE_DIR}/SongRoll/ClockDivControlWidget.hpp
        ${LIB_SOURCE_DIR}/SongRoll/DragModes.hpp
        ${LIB_SOURCE_DIR}/SongRoll/PatternControllerSlice.cpp
        ${LIB_SOURCE_DIR}/SongRoll/PatternControllerSlice.hpp
        ${LIB_SOURCE_DIR}/SongRoll/PatternControlWidget.cpp
        ${LIB_SOURCE_DIR}/SongRoll/PatternControlWidget.hpp
        ${LIB_SOURCE_DIR}/SongRoll/PatternHeaderWidget.cpp
        ${LIB_SOURCE_DIR}/SongRoll/PatternHeaderWidget.hpp
        ${LIB_SOURCE_DIR}/SongRoll/RepeatControlWidget.cpp
        ${LIB_SOURCE_DIR}/SongRoll/RepeatControlWidget.hpp
        ${LIB_SOURCE_DIR}/SongRoll/RollArea.cpp
        ${LIB_SOURCE_DIR}/SongRoll/RollArea.hpp
        ${LIB_SOURCE_DIR}/SongRoll/SongRollData.cpp
        ${LIB_SOURCE_DIR}/SongRoll/SongRollData.hpp
        ${LIB_SOURCE_DIR}/SongRoll/SongRollModule.cpp
        ${LIB_SOURCE_DIR}/SongRoll/SongRollModule.hpp
        ${LIB_SOURCE_DIR}/SongRoll/SongRollWidget.cpp
        ${LIB_SOURCE_DIR}/SongRoll/SongRollWidget.hpp
        ${LIB_SOURCE_DIR}/SongRoll/Transport.cpp
        ${LIB_SOURCE_DIR}/SongRoll/Transport.hpp
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

    set(VCV_NAME "rcm")
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
