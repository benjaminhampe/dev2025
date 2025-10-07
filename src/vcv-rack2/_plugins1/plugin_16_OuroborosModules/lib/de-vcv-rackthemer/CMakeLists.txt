set(LIB_NAME de_vcv_rackthemer)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/RackThemer/rackthemer_Common.hpp
        ${LIB_HEADER_DIR}/RackThemer/KeyedString.hpp
        ${LIB_HEADER_DIR}/RackThemer/Logging.hpp
        ${LIB_HEADER_DIR}/RackThemer/RackTheme.hpp
        ${LIB_HEADER_DIR}/RackThemer/SvgHelper.hpp
        ${LIB_HEADER_DIR}/RackThemer/ThemeableSvg.hpp
        ${LIB_HEADER_DIR}/RackThemer/ThemedSvg.hpp
        ${LIB_HEADER_DIR}/RackThemer/ThemedWidget.hpp
        ${LIB_HEADER_DIR}/RackThemer/Widgets.hpp
        ${LIB_HEADER_DIR}/rack_themer.hpp
    )
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/KeyedString.cpp
        ${LIB_SOURCE_DIR}/NanoSVG_Impl.cpp
        ${LIB_SOURCE_DIR}/RackTheme.cpp
        ${LIB_SOURCE_DIR}/ThemeableSvg.cpp
        ${LIB_SOURCE_DIR}/ThemeCache.cpp
        ${LIB_SOURCE_DIR}/ThemeCache.hpp
        ${LIB_SOURCE_DIR}/ThemedWidget.cpp
        ${LIB_SOURCE_DIR}/ThemeLoader.cpp
        ${LIB_SOURCE_DIR}/ThemeLoader.hpp
        ${LIB_SOURCE_DIR}/Widgets.cpp
    )

    if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
        # target_compile_options(${LIB_NAME} PRIVATE -fvisibility=hidden)
        # target_link_options(${LIB_NAME} PRIVATE -static-libgcc -static-libstdc++)
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
    # target_compile_definitions(${LIB_NAME} PRIVATE BUILDING_RACK_DLL=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE _RACK_VERSION="2.6.2")
    
    target_link_libraries(${LIB_NAME} PRIVATE de_rack2)
    target_link_libraries(${LIB_NAME} PRIVATE de_fmt)
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

endif()

# add_subdirectory(test)
