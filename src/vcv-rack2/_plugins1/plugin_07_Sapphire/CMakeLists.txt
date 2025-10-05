set(LIB_NAME plugin_07_Sapphire)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    
    
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/chaops_vcv.cpp
        ${LIB_SOURCE_DIR}/chaos.hpp
        ${LIB_SOURCE_DIR}/echo_vcv.cpp
        ${LIB_SOURCE_DIR}/elastika_engine.hpp
        ${LIB_SOURCE_DIR}/elastika_mesh.cpp
        ${LIB_SOURCE_DIR}/elastika_mesh.hpp
        ${LIB_SOURCE_DIR}/elastika_vcv.cpp
        ${LIB_SOURCE_DIR}/env_pitch_detect.hpp
        ${LIB_SOURCE_DIR}/env_vcv.cpp
        ${LIB_SOURCE_DIR}/frolic_vcv.cpp
        ${LIB_SOURCE_DIR}/galaxy_engine.hpp
        ${LIB_SOURCE_DIR}/galaxy_vcv.cpp
        ${LIB_SOURCE_DIR}/glee_vcv.cpp
        ${LIB_SOURCE_DIR}/gravy_engine.hpp
        ${LIB_SOURCE_DIR}/gravy_vcv.cpp
        ${LIB_SOURCE_DIR}/hiss_vcv.cpp
        ${LIB_SOURCE_DIR}/lark_vcv.cpp
        ${LIB_SOURCE_DIR}/mesh_physics.cpp
        ${LIB_SOURCE_DIR}/mesh_physics.hpp
        ${LIB_SOURCE_DIR}/moots_vcv.cpp
        ${LIB_SOURCE_DIR}/nucleus_engine.hpp
        ${LIB_SOURCE_DIR}/nucleus_init.hpp
        ${LIB_SOURCE_DIR}/nucleus_panel.hpp
        ${LIB_SOURCE_DIR}/nucleus_reset.hpp
        ${LIB_SOURCE_DIR}/nucleus_shared.hpp
        ${LIB_SOURCE_DIR}/nucleus_vcv.cpp
        ${LIB_SOURCE_DIR}/pivot_vcv.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp
        ${LIB_SOURCE_DIR}/polynucleus_panel.hpp
        ${LIB_SOURCE_DIR}/polynucleus_vcv.cpp
        ${LIB_SOURCE_DIR}/pop_engine.hpp
        ${LIB_SOURCE_DIR}/pop_vcv.cpp
        ${LIB_SOURCE_DIR}/rotini_vcv.cpp
        ${LIB_SOURCE_DIR}/sam_vcv.cpp
        ${LIB_SOURCE_DIR}/sapphire_calcparser.cpp
        ${LIB_SOURCE_DIR}/sapphire_calcparser.hpp
        ${LIB_SOURCE_DIR}/sapphire_calculator.hpp
        ${LIB_SOURCE_DIR}/sapphire_chaos_module.hpp
        ${LIB_SOURCE_DIR}/sapphire_crossfader.hpp
        ${LIB_SOURCE_DIR}/sapphire_engine.hpp
        ${LIB_SOURCE_DIR}/sapphire_envelope_follower.hpp
        ${LIB_SOURCE_DIR}/sapphire_globals.cpp
        ${LIB_SOURCE_DIR}/sapphire_multitap.hpp
        ${LIB_SOURCE_DIR}/sapphire_panel.cpp
        ${LIB_SOURCE_DIR}/sapphire_panel.hpp
        ${LIB_SOURCE_DIR}/sapphire_prog_chaos.cpp
        ${LIB_SOURCE_DIR}/sapphire_prog_chaos.hpp
        ${LIB_SOURCE_DIR}/sapphire_random.hpp
        ${LIB_SOURCE_DIR}/sapphire_simd.hpp
        ${LIB_SOURCE_DIR}/sapphire_smoother.hpp
        ${LIB_SOURCE_DIR}/sapphire_tapeloop.hpp
        ${LIB_SOURCE_DIR}/sapphire_vcvrack.hpp
        ${LIB_SOURCE_DIR}/sapphire_vcv_resampler.hpp
        ${LIB_SOURCE_DIR}/sapphire_widget.cpp
        ${LIB_SOURCE_DIR}/sapphire_widget.hpp
        ${LIB_SOURCE_DIR}/sauce_engine.hpp
        ${LIB_SOURCE_DIR}/sauce_vcv.cpp
        ${LIB_SOURCE_DIR}/tin_vcv.cpp
        ${LIB_SOURCE_DIR}/tout_vcv.cpp
        ${LIB_SOURCE_DIR}/tricorder_vcv.cpp
        ${LIB_SOURCE_DIR}/tubeunit_engine.hpp
        ${LIB_SOURCE_DIR}/tubeunit_vcv.cpp
        ${LIB_SOURCE_DIR}/zoo_vcv.cpp
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
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
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

    set(VCV_NAME "Sapphire")
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
