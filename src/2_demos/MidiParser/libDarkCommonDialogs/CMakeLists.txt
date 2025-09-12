# Tries to make it easier to work with windows API.
# Just add de_win32api to your target_link_libraries()
# and you can start develop raw/native win32 api controls.
# --> target_link_libraries(${YOUR_APP} PRIVATE de_win32api)
# With this one command you get header de_win32api.hpp avail.
# just #include <de_win32api.hpp> and start developing with full windows multimedia API.

set(LIB_NAME DarkCommonDialogs_midi)
project(${LIB_NAME}-dev-23.04)

    if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
    else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    # ###############
    # Headers
    # ###############
    # file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.[c,h]pp")
    file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
    file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
    set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

    # ###############
    # Sources
    # ###############
    # file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.[c,h]pp")
    file(GLOB_RECURSE LIB_SOURCES_H   "${LIB_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE LIB_SOURCES_HPP "${LIB_SOURCE_DIR}/*.hpp")
    file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
    file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
    set(LIB_SOURCES ${LIB_SOURCES_C} ${LIB_SOURCES_CPP} ${LIB_SOURCES_H} ${LIB_SOURCES_HPP})

    if (WIN32) # Only on windows (yet). D.k., had problems on Linux with windres.exe app icons.
    enable_language(RC) # Can this work on Linux at all? Exe icons are as i recall defined with a special text file and file extension similar .desktop or so to have app icon on Linux desktops.
    list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
    list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    # ###################
    # Add Library
    # ###################
    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})

    if (MSVC)
    else()
        target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
        target_compile_options(${LIB_NAME} PUBLIC -Wno-unused-variable)
        target_compile_options(${LIB_NAME} PUBLIC -Wno-unused-function)
        target_compile_options(${LIB_NAME} PUBLIC -Wno-unused-but-set-variable)
        # ######################
        # Link Libraries
        # ######################
        if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.1))
            target_link_libraries(${LIB_NAME} PRIVATE stdc++fs)
        endif()
        target_link_libraries(${LIB_NAME} PRIVATE pthread)
        target_link_libraries(${LIB_NAME} PRIVATE m)
    endif()
    
    if (WIN32)
        target_compile_definitions(${LIB_NAME} PRIVATE UNICODE=1)
        target_compile_definitions(${LIB_NAME} PRIVATE _UNICODE=1)

        target_link_libraries(${LIB_NAME} PUBLIC wmcodecdspuuid)
        target_link_libraries(${LIB_NAME} PUBLIC mfuuid)
        target_link_libraries(${LIB_NAME} PUBLIC mf)
        target_link_libraries(${LIB_NAME} PUBLIC mfplat)
        #target_link_libraries(${LIB_NAME} PUBLIC mf3216 mapi32 mapistub mdminst imm32)
        target_link_libraries(${LIB_NAME} PUBLIC uuid)
        target_link_libraries(${LIB_NAME} PUBLIC ks)
        target_link_libraries(${LIB_NAME} PUBLIC ksuser)
        target_link_libraries(${LIB_NAME} PUBLIC ksguid)
        target_link_libraries(${LIB_NAME} PUBLIC winmm)
        target_link_libraries(${LIB_NAME} PUBLIC dsound)
        target_link_libraries(${LIB_NAME} PUBLIC comctl32)
        target_link_libraries(${LIB_NAME} PUBLIC oleaut32)
        target_link_libraries(${LIB_NAME} PUBLIC ole32)
        target_link_libraries(${LIB_NAME} PUBLIC advapi32)
        target_link_libraries(${LIB_NAME} PUBLIC user32)
        target_link_libraries(${LIB_NAME} PUBLIC gdi32)
        target_link_libraries(${LIB_NAME} PUBLIC kernel32)
    else()
        target_link_libraries(${LIB_NAME} PUBLIC X11 Xxf86vm Xi Xcursor)
    endif()

endif()

