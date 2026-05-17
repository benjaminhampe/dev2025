set(LIB_NAME DarkMP4)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    file(GLOB_RECURSE LIB_HEADERS ${LIB_HEADER_DIR}/*.h)
    # set(LIB_HEADERS
    #     "${LIB_HEADER_DIR}/de/file/mp4/IParserListener.h"
    #     "${LIB_HEADER_DIR}/de/file/mp4/Parser.h"
    # )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE LIB_SOURCES ${LIB_SOURCE_DIR}/*.cpp)
    # set(LIB_SOURCES
    #     "${LIB_SOURCE_DIR}/de/file/mp4/Parser.cpp"
    # )

    # if (WIN32) # Only on windows (yet). D.k., had problems on Linux with windres.exe app icons.
        # enable_language(RC) # Can this work on Linux at all? Exe icons are as i recall defined with a special text file and file extension similar .desktop or so to have app icon on Linux desktops.
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    
    target_link_libraries(${LIB_NAME} PUBLIC DarkImage_2604)

endif()

add_subdirectory(test)
