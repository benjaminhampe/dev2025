set(LIB_NAME Fundamental)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/8vert.cpp
        ${LIB_SOURCE_DIR}/ADSR.cpp
        ${LIB_SOURCE_DIR}/Compare.cpp
        ${LIB_SOURCE_DIR}/CVMix.cpp
        ${LIB_SOURCE_DIR}/Delay.cpp
        ${LIB_SOURCE_DIR}/Fade.cpp
        ${LIB_SOURCE_DIR}/Gates.cpp
        ${LIB_SOURCE_DIR}/LFO.cpp
        ${LIB_SOURCE_DIR}/Logic.cpp
        ${LIB_SOURCE_DIR}/Merge.cpp
        ${LIB_SOURCE_DIR}/MidSide.cpp
        ${LIB_SOURCE_DIR}/Mixer.cpp
        ${LIB_SOURCE_DIR}/Mult.cpp
        ${LIB_SOURCE_DIR}/Mutes.cpp
        ${LIB_SOURCE_DIR}/Noise.cpp
        ${LIB_SOURCE_DIR}/Octave.cpp
        ${LIB_SOURCE_DIR}/plugin.cpp
        ${LIB_SOURCE_DIR}/plugin.hpp
        ${LIB_SOURCE_DIR}/Process.cpp
        ${LIB_SOURCE_DIR}/Pulses.cpp
        ${LIB_SOURCE_DIR}/Push.cpp
        ${LIB_SOURCE_DIR}/Quantizer.cpp
        ${LIB_SOURCE_DIR}/Random.cpp
        ${LIB_SOURCE_DIR}/RandomValues.cpp
        ${LIB_SOURCE_DIR}/Rescale.cpp
        ${LIB_SOURCE_DIR}/Scope.cpp
        ${LIB_SOURCE_DIR}/SEQ3.cpp
        ${LIB_SOURCE_DIR}/SequentialSwitch.cpp
        ${LIB_SOURCE_DIR}/SHASR.cpp
        ${LIB_SOURCE_DIR}/Split.cpp
        ${LIB_SOURCE_DIR}/Sum.cpp
        ${LIB_SOURCE_DIR}/Unity.cpp
        ${LIB_SOURCE_DIR}/VCA-1.cpp
        ${LIB_SOURCE_DIR}/VCA.cpp
        ${LIB_SOURCE_DIR}/VCF.cpp
        ${LIB_SOURCE_DIR}/VCMixer.cpp
        ${LIB_SOURCE_DIR}/VCO.cpp
        ${LIB_SOURCE_DIR}/Viz.cpp
        ${LIB_SOURCE_DIR}/Wavetable.hpp
        ${LIB_SOURCE_DIR}/WTLFO.cpp
        ${LIB_SOURCE_DIR}/WTVCO.cpp
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
    # else()
        # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} STATIC "")
    # endif()

    # target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    # target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
	# target_compile_definitions(${LIB_NAME} PRIVATE HAVE_LIBRACK2=1)
	
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")
    
    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
    target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)
    
    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libRack2)
    else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows)

        # if (WIN32)
            # target_link_libraries(${LIB_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            # target_link_libraries(${LIB_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        # endif()
    endif()
    
    set_target_properties(${LIB_NAME} PROPERTIES
        OUTPUT_NAME "Fundamental-2.6.2-llvm-win-x64"
        SUFFIX ".vcvplugin"
    )

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
    

    file(COPY media/Rack2 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()

# add_subdirectory(test)
