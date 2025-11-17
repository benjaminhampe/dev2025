set(LIB_NAME plugin_04_StarlingCode)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/src/atsr.cpp
        ${LIB_SOURCE_DIR}/src/gateseq.cpp
        ${LIB_SOURCE_DIR}/src/meta.cpp
        ${LIB_SOURCE_DIR}/src/osc3.cpp
        ${LIB_SOURCE_DIR}/src/scanner.cpp
        ${LIB_SOURCE_DIR}/src/starling.cpp
        ${LIB_SOURCE_DIR}/src/starling.hpp
        ${LIB_SOURCE_DIR}/src/sync.cpp
        ${LIB_SOURCE_DIR}/src/sync3.cpp
        ${LIB_SOURCE_DIR}/src/sync3xl.cpp
        ${LIB_SOURCE_DIR}/src/sync3xlexpand.hpp
        ${LIB_SOURCE_DIR}/src/sync3xllevels.cpp
        ${LIB_SOURCE_DIR}/src/via-module.hpp
        ${LIB_SOURCE_DIR}/src/via-params.hpp

        ${LIB_SOURCE_DIR}/src/dep/starling-dsp/oversampling.hpp
        ${LIB_SOURCE_DIR}/src/dep/starling-dsp/polyblamp.hpp
        ${LIB_SOURCE_DIR}/src/dep/starling-dsp/processors.hpp
        ${LIB_SOURCE_DIR}/src/dep/starling-dsp/starling-dsp.hpp
        
        ${LIB_SOURCE_DIR}/src/dep/starling-rack-ui/starling-rack-ui.hpp

        ${LIB_SOURCE_DIR}/Via/io/inc/via-f373-module.hpp
        ${LIB_SOURCE_DIR}/Via/io/inc/via-f373-system.hpp
        ${LIB_SOURCE_DIR}/Via/io/inc/via-global-signals.hpp
        ${LIB_SOURCE_DIR}/Via/io/inc/via-module-template.hpp
        ${LIB_SOURCE_DIR}/Via/io/inc/via-platform-binding.hpp
        ${LIB_SOURCE_DIR}/Via/io/inc/via-virtual-module.hpp
        ${LIB_SOURCE_DIR}/Via/io/inc/via-virtual-system.hpp
        ${LIB_SOURCE_DIR}/Via/io/src/via-global-signals.cpp

        ${LIB_SOURCE_DIR}/Via/modules/atsr/atsr-helpers.cpp
        ${LIB_SOURCE_DIR}/Via/modules/atsr/atsr-modes.cpp
        ${LIB_SOURCE_DIR}/Via/modules/atsr/atsr-ui-implementation.cpp

        ${LIB_SOURCE_DIR}/Via/modules/calib/calib-ui-implementation.cpp
        ${LIB_SOURCE_DIR}/Via/modules/calib/calibration-helpers.cpp

        ${LIB_SOURCE_DIR}/Via/modules/emptyvia/emptyvia-modes.cpp
        ${LIB_SOURCE_DIR}/Via/modules/emptyvia/emptyvia-ui-implementation.cpp

        ${LIB_SOURCE_DIR}/Via/modules/gateseq/dual-euclidean.cpp
        ${LIB_SOURCE_DIR}/Via/modules/gateseq/gateseq-init.cpp
        ${LIB_SOURCE_DIR}/Via/modules/gateseq/gateseq-interrupt-handlers.cpp
        ${LIB_SOURCE_DIR}/Via/modules/gateseq/gateseq-modes.cpp
        ${LIB_SOURCE_DIR}/Via/modules/gateseq/gateseq-ui-implementation.cpp
        ${LIB_SOURCE_DIR}/Via/modules/gateseq/gateseq-user-seq.cpp
        ${LIB_SOURCE_DIR}/Via/modules/gateseq/sequencer-resources.cpp

        ${LIB_SOURCE_DIR}/Via/modules/inc/atsr.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/calib.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/dsp.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/emptyvia.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/gateseq.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/meta-tables.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/meta.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/osc3.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/scales.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/scanner-tables.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/scanner.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/sync-scale-defs.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/sync-tables.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/sync.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/sync3.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/sync3xl.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/tables.hpp
        ${LIB_SOURCE_DIR}/Via/modules/inc/touch.hpp

        ${LIB_SOURCE_DIR}/Via/modules/meta/meta-aux-outputs.cpp
        ${LIB_SOURCE_DIR}/Via/modules/meta/meta-controller.cpp
        ${LIB_SOURCE_DIR}/Via/modules/meta/meta-init.cpp
        ${LIB_SOURCE_DIR}/Via/modules/meta/meta-interrupt-handlers.cpp
        ${LIB_SOURCE_DIR}/Via/modules/meta/meta-modes.cpp
        ${LIB_SOURCE_DIR}/Via/modules/meta/meta-table-init.cpp
        ${LIB_SOURCE_DIR}/Via/modules/meta/meta-ui-implementation.cpp
        ${LIB_SOURCE_DIR}/Via/modules/meta/meta-wavetable.cpp
        ${LIB_SOURCE_DIR}/Via/modules/meta/simple-envelope.cpp

        ${LIB_SOURCE_DIR}/Via/modules/osc3/osc3-helpers.cpp
        ${LIB_SOURCE_DIR}/Via/modules/osc3/osc3-modes.cpp
        ${LIB_SOURCE_DIR}/Via/modules/osc3/osc3-ui-implementation.cpp

        ${LIB_SOURCE_DIR}/Via/modules/scanner/scanner-init.cpp
        ${LIB_SOURCE_DIR}/Via/modules/scanner/scanner-interrupt-handlers.cpp
        ${LIB_SOURCE_DIR}/Via/modules/scanner/scanner-modes.cpp
        ${LIB_SOURCE_DIR}/Via/modules/scanner/scanner-table-init.cpp
        ${LIB_SOURCE_DIR}/Via/modules/scanner/scanner-ui-implementation.cpp
        ${LIB_SOURCE_DIR}/Via/modules/scanner/three-axis-scanner.cpp

        ${LIB_SOURCE_DIR}/Via/modules/sync/pll-controller.cpp
        ${LIB_SOURCE_DIR}/Via/modules/sync/sync-aux-outputs.cpp
        ${LIB_SOURCE_DIR}/Via/modules/sync/sync-init.cpp
        ${LIB_SOURCE_DIR}/Via/modules/sync/sync-interrupt-handlers.cpp
        ${LIB_SOURCE_DIR}/Via/modules/sync/sync-modes.cpp
        ${LIB_SOURCE_DIR}/Via/modules/sync/sync-scales.cpp
        ${LIB_SOURCE_DIR}/Via/modules/sync/sync-table-init.cpp
        ${LIB_SOURCE_DIR}/Via/modules/sync/sync-ui-implementation.cpp
        ${LIB_SOURCE_DIR}/Via/modules/sync/sync-wavetable.cpp

        ${LIB_SOURCE_DIR}/Via/modules/sync3/sync3-helpers.cpp
        ${LIB_SOURCE_DIR}/Via/modules/sync3/sync3-modes.cpp
        ${LIB_SOURCE_DIR}/Via/modules/sync3/sync3-ui-implementation.cpp

        ${LIB_SOURCE_DIR}/Via/modules/touch/touch-ui-implementation.cpp

        ${LIB_SOURCE_DIR}/Via/ui/inc/touch-lib-link.hpp
        ${LIB_SOURCE_DIR}/Via/ui/inc/user-interface.hpp

        ${LIB_SOURCE_DIR}/Via/ui/src/ui-helpers.cpp
        ${LIB_SOURCE_DIR}/Via/ui/src/ui-main.cpp
        ${LIB_SOURCE_DIR}/Via/ui/src/ui-menus.cpp
        ${LIB_SOURCE_DIR}/Via/ui/src/ui-presets.cpp
        ${LIB_SOURCE_DIR}/Via/ui/src/ui-save-recall.cpp        
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/src)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/src/dep/starling-dsp)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/src/dep/starling-rack-ui)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/io/inc)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/io/src)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/modules/atsr)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/modules/calib)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/modules/emptyvia)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/modules/gateseq)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/modules/inc)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/modules/meta)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/modules/osc3)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/modules/scanner)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/modules/sync)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/modules/sync3)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/modules/touch)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/ui/inc)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Via/ui/src)

    target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
    target_compile_definitions(${LIB_NAME} PRIVATE BUILD_VIRTUAL=1)
    
    # target_compile_definitions(${LIB_NAME} PRIVATE HAVE_LIBRACK2=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
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

    set(VCV_NAME "StarlingCode")
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
