set(LIB_NAME plugin_35_MindMeld)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/MindMeldModular.cpp
        ${LIB_SOURCE_DIR}/MindMeldModular.hpp
        ${LIB_SOURCE_DIR}/MixerMessageBus.hpp    
      # ${LIB_SOURCE_DIR}/comp
        ${LIB_SOURCE_DIR}/comp/GenericComponents.cpp
        ${LIB_SOURCE_DIR}/comp/GenericComponents.hpp
        ${LIB_SOURCE_DIR}/comp/VuMeters.cpp
        ${LIB_SOURCE_DIR}/comp/VuMeters.hpp
      # ${LIB_SOURCE_DIR}/dsp
        ${LIB_SOURCE_DIR}/dsp/ButterworthFilters.hpp
        ${LIB_SOURCE_DIR}/dsp/FirstOrderFilter.hpp
        ${LIB_SOURCE_DIR}/dsp/LinkwitzRileyCrossover.hpp
        ${LIB_SOURCE_DIR}/dsp/QuattroBiQuad.hpp
      # ${LIB_SOURCE_DIR}/EqMaster
        ${LIB_SOURCE_DIR}/EqMaster/EqExpander.cpp
        ${LIB_SOURCE_DIR}/EqMaster/EqMaster.cpp
        ${LIB_SOURCE_DIR}/EqMaster/EqMasterCommon.hpp
        ${LIB_SOURCE_DIR}/EqMaster/EqMenus.hpp
        ${LIB_SOURCE_DIR}/EqMaster/EqWidgets.hpp
      # ${LIB_SOURCE_DIR}/MixMaster
        ${LIB_SOURCE_DIR}/MixMaster/AuxExpander.cpp
        ${LIB_SOURCE_DIR}/MixMaster/AuxExpander.hpp
        ${LIB_SOURCE_DIR}/MixMaster/AuxExpanderWidget.hpp
        ${LIB_SOURCE_DIR}/MixMaster/MixerCommon.cpp
        ${LIB_SOURCE_DIR}/MixMaster/MixerCommon.hpp
        ${LIB_SOURCE_DIR}/MixMaster/MixerMenus.hpp
        ${LIB_SOURCE_DIR}/MixMaster/MixerWidgets.hpp
        ${LIB_SOURCE_DIR}/MixMaster/MixMaster.cpp
        ${LIB_SOURCE_DIR}/MixMaster/MixMaster.hpp
        ${LIB_SOURCE_DIR}/MixMaster/MixMasterWidget.hpp
      # ${LIB_SOURCE_DIR}/PatchSet
        ${LIB_SOURCE_DIR}/PatchSet/MasterChannel.cpp
        ${LIB_SOURCE_DIR}/PatchSet/PatchMaster.cpp
        ${LIB_SOURCE_DIR}/PatchSet/PatchMasterBlank.cpp
        ${LIB_SOURCE_DIR}/PatchSet/PatchMasterUtil.hpp
        ${LIB_SOURCE_DIR}/PatchSet/PatchSetWidgets.hpp
        ${LIB_SOURCE_DIR}/PatchSet/RouteMaster.cpp
      # ${LIB_SOURCE_DIR}/ShapeMaster
        ${LIB_SOURCE_DIR}/ShapeMaster/Bjorklund.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Bjorklund.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Channel.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Channel.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/ClockDetector.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Display.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Display.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/DisplayLight.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/DisplayUtil.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/DisplayUtil.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/History.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/History.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Menus.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Menus.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/PlayHead.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/PlayHead.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/PresetAndShapeManager.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/PresetAndShapeManager.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/RandomSettings.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Shape.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Shape.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/ShapeMaster.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/ShapeMaster.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/ShapeMasterComponents.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Util.cpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Util.hpp
        ${LIB_SOURCE_DIR}/ShapeMaster/Widgets.hpp
      # ${LIB_SOURCE_DIR}/Utilities
        ${LIB_SOURCE_DIR}/Utilities/BassMaster.cpp
        ${LIB_SOURCE_DIR}/Utilities/Meld.cpp
        ${LIB_SOURCE_DIR}/Utilities/MSMelder.cpp
        ${LIB_SOURCE_DIR}/Utilities/Unmeld.cpp
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
    # target_link_libraries(${LIB_NAME} PRIVATE de_freeverb)
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

    set(VCV_NAME "MindMeld")
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
