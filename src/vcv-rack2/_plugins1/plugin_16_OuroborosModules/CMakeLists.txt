set(LIB_NAME plugin_16_OuroborosModules)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    add_subdirectory(lib)

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/AudioFile.h
        ${LIB_SOURCE_DIR}/CommonTypes.cpp
        ${LIB_SOURCE_DIR}/CommonTypes.hpp
        ${LIB_SOURCE_DIR}/Ouroboros_Constants.cpp
        ${LIB_SOURCE_DIR}/Ouroboros_Constants.hpp
        ${LIB_SOURCE_DIR}/JsonUtils.cpp
        ${LIB_SOURCE_DIR}/JsonUtils.hpp
        ${LIB_SOURCE_DIR}/Ouroboros_Logging.cpp
        ${LIB_SOURCE_DIR}/Ouroboros_Logging.hpp
        ${LIB_SOURCE_DIR}/Ouroboros_Math.cpp
        ${LIB_SOURCE_DIR}/Ouroboros_Math.hpp
        ${LIB_SOURCE_DIR}/MetaHandler.cpp
        ${LIB_SOURCE_DIR}/MetaHandler.hpp
        ${LIB_SOURCE_DIR}/ModelDeclarations.cpp
        ${LIB_SOURCE_DIR}/ModelDeclarations.hpp
      # ${LIB_SOURCE_DIR}/ModelDeclarations.x
        ${LIB_SOURCE_DIR}/ModuleBase.cpp
        ${LIB_SOURCE_DIR}/ModuleBase.hpp
        ${LIB_SOURCE_DIR}/PluginDef.cpp
        ${LIB_SOURCE_DIR}/PluginDef.hpp
        ${LIB_SOURCE_DIR}/PluginSettings.cpp
        ${LIB_SOURCE_DIR}/PluginSettings.hpp
        ${LIB_SOURCE_DIR}/SampleChannel.cpp
        ${LIB_SOURCE_DIR}/SampleChannel.hpp
      # ${LIB_SOURCE_DIR}/Settings_Def.x
        ${LIB_SOURCE_DIR}/Settings_Def_Stubs.hpp
      # ${LIB_SOURCE_DIR}/Styles_Def.x
        ${LIB_SOURCE_DIR}/Utils.cpp
        ${LIB_SOURCE_DIR}/Utils.hpp
        ${LIB_SOURCE_DIR}/UISystemUpdater.cpp
        ${LIB_SOURCE_DIR}/UISystemUpdater.hpp
        ${LIB_SOURCE_DIR}/Bernoulli/Bernoulli.cpp
        ${LIB_SOURCE_DIR}/Bernoulli/Bernoulli.hpp
        ${LIB_SOURCE_DIR}/Bernoulli/Widget.cpp
        ${LIB_SOURCE_DIR}/Branch/Branch.cpp
        ${LIB_SOURCE_DIR}/Branch/Branch.hpp
        ${LIB_SOURCE_DIR}/Branch/Widget.cpp
        ${LIB_SOURCE_DIR}/Chroma/CableColorManager.cpp
        ${LIB_SOURCE_DIR}/Chroma/CableColorManager.hpp
        ${LIB_SOURCE_DIR}/Chroma/CCM_Common.cpp
        ${LIB_SOURCE_DIR}/Chroma/CCM_Common.hpp
        ${LIB_SOURCE_DIR}/Chroma/Chroma.cpp
        ${LIB_SOURCE_DIR}/Chroma/Chroma.hpp
        ${LIB_SOURCE_DIR}/Chroma/ColorDisplayWidget.cpp
        ${LIB_SOURCE_DIR}/Chroma/ColorDisplayWidget.hpp
        ${LIB_SOURCE_DIR}/Chroma/HistoryActions.hpp
        ${LIB_SOURCE_DIR}/Chroma/KeyContainer.cpp
        ${LIB_SOURCE_DIR}/Chroma/Widget.cpp
        ${LIB_SOURCE_DIR}/DSP/ClockDivider.hpp
        ${LIB_SOURCE_DIR}/DSP/Filters.hpp
        ${LIB_SOURCE_DIR}/Junction/Junction.cpp
        ${LIB_SOURCE_DIR}/Junction/Junction.hpp
        ${LIB_SOURCE_DIR}/Junction/Widget.cpp
        ${LIB_SOURCE_DIR}/Median/Median.cpp
        ${LIB_SOURCE_DIR}/Median/Median.hpp
        ${LIB_SOURCE_DIR}/Median/Widget.cpp
        ${LIB_SOURCE_DIR}/Meta/Audio.cpp
        ${LIB_SOURCE_DIR}/Meta/Cables.cpp
        ${LIB_SOURCE_DIR}/Meta/Meta.cpp
        ${LIB_SOURCE_DIR}/Meta/Meta.hpp
        ${LIB_SOURCE_DIR}/Meta/MetaSounds.cpp
        ${LIB_SOURCE_DIR}/Meta/SampleSlot.cpp
        ${LIB_SOURCE_DIR}/Meta/Widget.cpp
        ${LIB_SOURCE_DIR}/STVCA/STVCA.cpp
        ${LIB_SOURCE_DIR}/STVCA/STVCA.hpp
        ${LIB_SOURCE_DIR}/STVCA/Widget.cpp
        ${LIB_SOURCE_DIR}/UI/CommonWidgets.cpp
        ${LIB_SOURCE_DIR}/UI/CommonWidgets.hpp
        ${LIB_SOURCE_DIR}/UI/ImageWidget.cpp
        ${LIB_SOURCE_DIR}/UI/ImageWidget.hpp
        ${LIB_SOURCE_DIR}/UI/Input.cpp
        ${LIB_SOURCE_DIR}/UI/Input.hpp
        ${LIB_SOURCE_DIR}/UI/ThemeStyles.hpp
        ${LIB_SOURCE_DIR}/UI/ThemeUtils.cpp
        ${LIB_SOURCE_DIR}/UI/ThemeUtils.hpp
        ${LIB_SOURCE_DIR}/UI/WidgetBase.cpp
        ${LIB_SOURCE_DIR}/UI/WidgetBase.hpp
        ${LIB_SOURCE_DIR}/UI/WidgetUtils.hpp
        ${LIB_SOURCE_DIR}/UI/MenuItems/ColorPicker.hpp
        ${LIB_SOURCE_DIR}/UI/MenuItems/CommonItems.cpp
        ${LIB_SOURCE_DIR}/UI/MenuItems/CommonItems.hpp
        ${LIB_SOURCE_DIR}/UI/MenuItems/TextField.cpp
        ${LIB_SOURCE_DIR}/UI/MenuItems/TextField.hpp
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
    target_link_libraries(${LIB_NAME} PRIVATE de_fmt)
    target_link_libraries(${LIB_NAME} PRIVATE de_vcv_rackthemer)
    target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
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

    set(VCV_NAME "OuroborosModules")
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
