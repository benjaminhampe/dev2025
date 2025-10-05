set(LIB_NAME plugin_45_LunettaModula)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    
    # file(GLOB_RECURSE LIB_SOURCES_HPP ${LIB_SOURCE_DIR}/*.hpp)
    # file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})    
    
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/DeclareModels.hpp
        ${LIB_SOURCE_DIR}/InitialiseModels.hpp
        ${LIB_SOURCE_DIR}/LunettaModula.cpp
        ${LIB_SOURCE_DIR}/LunettaModula.hpp    
      # ${LIB_SOURCE_DIR}/components
        ${LIB_SOURCE_DIR}/components/LunettaModulaComponents.hpp
        ${LIB_SOURCE_DIR}/components/LunettaModulaLEDDisplay.hpp
        ${LIB_SOURCE_DIR}/components/LunettaModulaPushButtons.hpp
        ${LIB_SOURCE_DIR}/components/StdComponentPositions.cpp
        ${LIB_SOURCE_DIR}/components/StdComponentPositions.hpp
        ${LIB_SOURCE_DIR}/components/stdScrews.hpp        
      # ${LIB_SOURCE_DIR}/inc
        ${LIB_SOURCE_DIR}/inc/CMOSInput.hpp
        ${LIB_SOURCE_DIR}/inc/Utility.hpp
      # ${LIB_SOURCE_DIR}/modes
        ${LIB_SOURCE_DIR}/modes/dataFromJson.hpp
        ${LIB_SOURCE_DIR}/modes/dataToJson.hpp
        ${LIB_SOURCE_DIR}/modes/modeMenuItem.hpp
        ${LIB_SOURCE_DIR}/modes/modeMenus.hpp
        ${LIB_SOURCE_DIR}/modes/modeVariables.hpp
        ${LIB_SOURCE_DIR}/modes/setGateVoltage.hpp
      # ${LIB_SOURCE_DIR}/modules
        ${LIB_SOURCE_DIR}/modules/ADC.cpp
        ${LIB_SOURCE_DIR}/modules/Binary8.cpp
        ${LIB_SOURCE_DIR}/modules/Buttons.cpp
        ${LIB_SOURCE_DIR}/modules/CD4000.cpp
        ${LIB_SOURCE_DIR}/modules/CD4001.cpp
        ${LIB_SOURCE_DIR}/modules/CD4002.cpp
        ${LIB_SOURCE_DIR}/modules/CD4008.cpp
        ${LIB_SOURCE_DIR}/modules/CD4010.cpp
        ${LIB_SOURCE_DIR}/modules/CD40106.cpp
        ${LIB_SOURCE_DIR}/modules/CD4011.cpp
        ${LIB_SOURCE_DIR}/modules/CD4012.cpp
        ${LIB_SOURCE_DIR}/modules/CD4015.cpp
        ${LIB_SOURCE_DIR}/modules/CD4017.cpp
        ${LIB_SOURCE_DIR}/modules/CD4019.cpp
        ${LIB_SOURCE_DIR}/modules/CD4020.cpp
        ${LIB_SOURCE_DIR}/modules/CD4022.cpp
        ${LIB_SOURCE_DIR}/modules/CD4023.cpp
        ${LIB_SOURCE_DIR}/modules/CD4024.cpp
        ${LIB_SOURCE_DIR}/modules/CD4025.cpp
        ${LIB_SOURCE_DIR}/modules/CD4030.cpp
        ${LIB_SOURCE_DIR}/modules/CD4031.cpp
        ${LIB_SOURCE_DIR}/modules/CD4040.cpp
        ${LIB_SOURCE_DIR}/modules/CD4041.cpp
        ${LIB_SOURCE_DIR}/modules/CD4042.cpp
        ${LIB_SOURCE_DIR}/modules/CD4048.cpp
        ${LIB_SOURCE_DIR}/modules/CD4049.cpp
        ${LIB_SOURCE_DIR}/modules/CD4063.cpp
        ${LIB_SOURCE_DIR}/modules/CD4068.cpp
        ${LIB_SOURCE_DIR}/modules/CD4071.cpp
        ${LIB_SOURCE_DIR}/modules/CD4072.cpp
        ${LIB_SOURCE_DIR}/modules/CD4073.cpp
        ${LIB_SOURCE_DIR}/modules/CD4075.cpp
        ${LIB_SOURCE_DIR}/modules/CD4077.cpp
        ${LIB_SOURCE_DIR}/modules/CD4078.cpp
        ${LIB_SOURCE_DIR}/modules/CD4081.cpp
        ${LIB_SOURCE_DIR}/modules/CD4082.cpp
        ${LIB_SOURCE_DIR}/modules/CD4086.cpp
        ${LIB_SOURCE_DIR}/modules/CD4514.cpp
        ${LIB_SOURCE_DIR}/modules/CD4515.cpp
        ${LIB_SOURCE_DIR}/modules/CD4516.cpp
        ${LIB_SOURCE_DIR}/modules/CD4532.cpp
        ${LIB_SOURCE_DIR}/modules/CD4539.cpp
        ${LIB_SOURCE_DIR}/modules/CD4562.cpp
        ${LIB_SOURCE_DIR}/modules/ConstantOnes.cpp
        ${LIB_SOURCE_DIR}/modules/ConstantZeroes.cpp
        ${LIB_SOURCE_DIR}/modules/DAC.cpp
        ${LIB_SOURCE_DIR}/modules/MC14530.cpp
        ${LIB_SOURCE_DIR}/modules/Truth2.cpp
        ${LIB_SOURCE_DIR}/modules/Truth3.cpp
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

    set(VCV_NAME "LunettaModula")
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
