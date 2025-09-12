set(LIB_NAME DarkMidi_midi)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)

    file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
    file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
    file(GLOB_RECURSE LIB_HEADERS_C   "${LIB_HEADER_DIR}/*.c")
    file(GLOB_RECURSE LIB_HEADERS_CPP "${LIB_HEADER_DIR}/*.cpp")
    set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP} ${LIB_HEADERS_C} ${LIB_HEADERS_CPP})

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE LIB_SOURCES_H   "${LIB_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE LIB_SOURCES_HPP "${LIB_SOURCE_DIR}/*.hpp")
    file(GLOB_RECURSE LIB_SOURCES_C   "${LIB_SOURCE_DIR}/*.c")
    file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
    set(LIB_SOURCES ${LIB_SOURCES_H} ${LIB_SOURCES_HPP} ${LIB_SOURCES_C} ${LIB_SOURCES_CPP})

    if (WIN32) # Only on windows (yet). D.k., had problems on Linux with windres.exe app icons.
        enable_language(RC) # Can this work on Linux at all? Exe icons are as i recall defined with a special text file and file extension similar .desktop or so to have app icon on Linux desktops.
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

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

    target_link_libraries(${LIB_NAME} PUBLIC DarkCore_midi)

endif()

