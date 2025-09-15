set(LIB_NAME DarkMidi_2504)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/de/midi/DurationComputer.h
        ${LIB_HEADER_DIR}/de/midi/DurationComputerTest.h
        ${LIB_HEADER_DIR}/de/midi/GeneralMidi.h
        ${LIB_HEADER_DIR}/de/midi/IParserListener.h
        ${LIB_HEADER_DIR}/de/midi/MidiFile.h
        ${LIB_HEADER_DIR}/de/midi/MidiFileListener.h
        ${LIB_HEADER_DIR}/de/midi/MidiUtil.h
        ${LIB_HEADER_DIR}/de/midi/Parser.h
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/de/midi/MidiFile.cpp
        ${LIB_SOURCE_DIR}/de/midi/MidiFileListener.cpp
        ${LIB_SOURCE_DIR}/de/midi/MidiUtil.cpp
        ${LIB_SOURCE_DIR}/de/midi/Parser.cpp
    )

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

    target_link_libraries(${LIB_NAME} PUBLIC DarkImage_2504)

endif()

add_subdirectory(test)
