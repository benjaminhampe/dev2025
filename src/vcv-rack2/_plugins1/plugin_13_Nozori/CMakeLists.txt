set(LIB_NAME plugin_13_Nozori)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    
    
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/a_tables.h
        ${LIB_SOURCE_DIR}/a_utils.h
        ${LIB_SOURCE_DIR}/c_fonctions.h
        ${LIB_SOURCE_DIR}/c_macro.h
        ${LIB_SOURCE_DIR}/c_variables.h
        ${LIB_SOURCE_DIR}/m68_ADSR.ino
        ${LIB_SOURCE_DIR}/m68_AR.ino
        ${LIB_SOURCE_DIR}/m68_CRUSH.ino
        ${LIB_SOURCE_DIR}/m68_CV_REC.ino
        ${LIB_SOURCE_DIR}/m68_Delay.ino
        ${LIB_SOURCE_DIR}/m68_EQ.ino
        ${LIB_SOURCE_DIR}/m68_Gran.ino
        ${LIB_SOURCE_DIR}/m68_KS.ino
        ${LIB_SOURCE_DIR}/m68_LFO.ino
        ${LIB_SOURCE_DIR}/m68_LFO_MOD.ino
        ${LIB_SOURCE_DIR}/m68_Noise.ino
        ${LIB_SOURCE_DIR}/m68_PITCH.ino
        ${LIB_SOURCE_DIR}/m68_RND_LOOP.ino
        ${LIB_SOURCE_DIR}/m68_SIN_MOD.ino
        ${LIB_SOURCE_DIR}/m68_VCA_PAN.ino
        ${LIB_SOURCE_DIR}/m68_VCF.ino
        ${LIB_SOURCE_DIR}/m68_VCF_MORPH.ino
        ${LIB_SOURCE_DIR}/m68_VCO-WS.ino
        ${LIB_SOURCE_DIR}/m68_VCO.ino
        ${LIB_SOURCE_DIR}/m68_VCO_LOOP.ino
        ${LIB_SOURCE_DIR}/m68_VCO_MORPH.ino
        ${LIB_SOURCE_DIR}/m68_VCO_Q.ino
        ${LIB_SOURCE_DIR}/m68_WS.ino
        ${LIB_SOURCE_DIR}/m84_8_bit.ino
        ${LIB_SOURCE_DIR}/m84_ADD.ino
        ${LIB_SOURCE_DIR}/m84_ADSR.ino
        ${LIB_SOURCE_DIR}/m84_CLOCK.ino
        ${LIB_SOURCE_DIR}/m84_Doppler.ino
        ${LIB_SOURCE_DIR}/m84_FM.ino
        ${LIB_SOURCE_DIR}/m84_FM_lin.ino
        ${LIB_SOURCE_DIR}/m84_HARMONICS.ino
        ${LIB_SOURCE_DIR}/m84_JONG.ino
        ${LIB_SOURCE_DIR}/m84_LFO.ino
        ${LIB_SOURCE_DIR}/m84_LFO_SEQ.ino
        ${LIB_SOURCE_DIR}/m84_Modulate.ino
        ${LIB_SOURCE_DIR}/m84_PARAMTRIC.ino
        ${LIB_SOURCE_DIR}/m84_SEQ4.ino
        ${LIB_SOURCE_DIR}/m84_SEQ8.ino
        ${LIB_SOURCE_DIR}/m84_Sin_AM.ino
        ${LIB_SOURCE_DIR}/m84_Sin_FM.ino
        ${LIB_SOURCE_DIR}/m84_Sin_PM.ino
        ${LIB_SOURCE_DIR}/m84_Sin_WS.ino
        ${LIB_SOURCE_DIR}/m84_Stereo_WS.ino
        ${LIB_SOURCE_DIR}/m84_Thomas.ino
        ${LIB_SOURCE_DIR}/m84_Tropical.ino
        ${LIB_SOURCE_DIR}/m84_VCF_MORPH.ino
        # ${LIB_SOURCE_DIR}/nozori_68.cpp.txt
        ${LIB_SOURCE_DIR}/Nozori_68_ADSR.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_AR.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_CRUSH.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_CV_REC.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_DELAY.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_EQ.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_GRAN.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_KS.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_LFO.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_LFO_MOD.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_NOISE.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_PITCH.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_RND_LOOP.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_SIN_MOD.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_VCA_PAN.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_VCF_MOD.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_VCF_MORPH.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_VCO.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_VCO_LOOP.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_VCO_MORPH.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_VCO_QTZ.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_VCO_WS.cpp
        ${LIB_SOURCE_DIR}/Nozori_68_WS.cpp
        ${LIB_SOURCE_DIR}/nozori_84.cpp.txt
        ${LIB_SOURCE_DIR}/Nozori_84_8_BITS.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_ADD.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_ADSR.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_CLOCK.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_DOPPLER.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_FM.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_FM_LIN.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_HARMONICS.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_JONG_LFO.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_JONG_VCO.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_LFO.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_LFO_SEQ.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_MODULATE.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_PARAM.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_SEQ_4.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_SEQ_8.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_SIN_AM.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_SIN_FM.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_SIN_PM.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_SIN_WS.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_THOMAS.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_TROPICAL.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_VCF.cpp
        ${LIB_SOURCE_DIR}/Nozori_84_WS.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp
        ${LIB_SOURCE_DIR}/vcv2nozori.hpp
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

    set(VCV_NAME "Nozori")
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
