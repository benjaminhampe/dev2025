set(LIB_NAME Abenton_SineMachine5_vst2)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE LIB_SOURCES_H ${LIB_SOURCE_DIR}/*.h)
    file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    set(LIB_SOURCES ${LIB_SOURCES_H} ${LIB_SOURCES_CPP})

    if (WIN32)
        enable_language(RC)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
      # list(APPEND APP_SOURCES "${APP_RC_DIR}/resource.qrc")
    endif()

    # if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED ${LIB_SOURCES})

        # target_compile_options(${LIB_NAME} PRIVATE -g)
        # target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        # target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        # message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} STATIC "")
    # endif()

    # target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    # target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    # target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)

    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")

    target_link_libraries(${LIB_NAME} PRIVATE de_vst2sdk)
    target_link_libraries(${LIB_NAME} PRIVATE de_glfw)
    target_link_libraries(${LIB_NAME} PRIVATE de_glew)
    target_link_libraries(${LIB_NAME} PRIVATE de_nanovg_tint)
    # target_link_libraries(${LIB_NAME} PRIVATE DarkImage_2504)
    target_link_libraries(${LIB_NAME} PRIVATE DarkImage_2604)
    target_link_libraries(${LIB_NAME} PRIVATE DarkGPU_2604)
    # target_link_libraries(${LIB_NAME} PRIVATE DarkSIMD)

    # target_link_libraries(${LIB_NAME} PRIVATE de_samplerate)
    # target_link_libraries(${LIB_NAME} PRIVATE de_dr_wav)

    # set_target_properties(plugin PROPERTIES LINK_FLAGS "/DEF:${CMAKE_CURRENT_SOURCE_DIR}/plugin.def")

    # target_compile_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_compile_options(${LIB_NAME} PRIVATE -static-libgcc)
    # target_link_options(${LIB_NAME} PRIVATE -static-libstdc++)
    # target_link_options(${LIB_NAME} PRIVATE -static-libgcc)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _VST2_Plugins)
    else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows)

        # if (WIN32)
            # target_link_libraries(${LIB_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
            # target_link_libraries(${LIB_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
        # endif()
    endif()

    set(APP_NAME ${LIB_NAME})
    include(${CMAKE_SOURCE_DIR}/src/embed_static_runtime.cmake)
    
    set_target_properties(${LIB_NAME} PROPERTIES
        PREFIX ""
        SUFFIX ".dll"
        OUTPUT_NAME "Abenton_SineMachine5_vst2"
        # ARCHIVE_OUTPUT_NAME "Abenton_Spectrum3d_1.0.0_x64_vst3"
    )
    
    # if (CMAKE_BUILD_TYPE STREQUAL "Release")

    #     # --strip-debug : Removes only debugging symbols.
    #     # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
    #     add_custom_command(
    #         TARGET ${LIB_NAME}
    #         POST_BUILD
    #         COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${LIB_NAME}>
    #         COMMENT "Stripping debug symbols"
    #     )

    # endif()

    # file(COPY media/${VCV_NAME} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/z_benni)
endif()

# add_subdirectory(test)
