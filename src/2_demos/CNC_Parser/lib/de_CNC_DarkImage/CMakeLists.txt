set(LIB_NAME de_CNC_DarkImage)
project(${LIB_NAME}-1.2-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.[c,h]pp")
    file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.[c,h]pp")

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()
    
    # if (BUILD_SHARED)
        # add_library(${LIB_NAME} SHARED "")
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    # else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    # endif()
    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    
    if (WIN32)
        target_link_libraries(${LIB_NAME} PUBLIC comctl32 user32 gdi32 kernel32)
    else()
        target_link_libraries(${LIB_NAME} PUBLIC X11 Xxf86vm Xi Xcursor)
    endif()

    if (MSVC)
    else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-deprecated-declarations)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-strict-aliasing)
        target_compile_options(${LIB_NAME} PUBLIC -Wno-unused-variable)
        target_compile_options(${LIB_NAME} PUBLIC -Wno-unused-but-set-variable)

        # if (BUILD_STATIC)
        # target_compile_options(${LIB_NAME} PRIVATE -static)
        # target_compile_options(${LIB_NAME} PRIVATE -static-libstdc++)
        # target_compile_options(${LIB_NAME} PRIVATE -static-libgcc)
        # target_link_options(${LIB_NAME} PRIVATE -static)
        # target_link_options(${LIB_NAME} PRIVATE -static-libstdc++)
        # target_link_options(${LIB_NAME} PRIVATE -static-libgcc)
        # endif()
        if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.1))
            target_link_libraries(${LIB_NAME} PRIVATE stdc++fs)
        endif()
        target_link_libraries(${LIB_NAME} PRIVATE pthread)
        target_link_libraries(${LIB_NAME} PRIVATE m)
    endif()

    target_link_libraries(${LIB_NAME} PRIVATE de_minizip)
    target_link_libraries(${LIB_NAME} PRIVATE de_unzip)
    target_link_libraries(${LIB_NAME} PRIVATE de_bzip2)
    target_link_libraries(${LIB_NAME} PRIVATE de_zlib)
    target_link_libraries(${LIB_NAME} PRIVATE de_lzma)
    target_link_libraries(${LIB_NAME} PRIVATE de_png)
    target_link_libraries(${LIB_NAME} PRIVATE de_jpeg8d)
    target_link_libraries(${LIB_NAME} PRIVATE de_tiff4)
    target_link_libraries(${LIB_NAME} PRIVATE de_gif5)

    target_link_libraries(${LIB_NAME} PUBLIC de_glm)      # Math
    target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2) # XML

    # target_link_libraries(${LIB_NAME} PUBLIC de_fontawesome)
    # target_link_libraries(${LIB_NAME} PRIVATE de_freetype2)

    # target_link_libraries(${APP_NAME} PUBLIC de_boost)
    # target_link_libraries(${APP_NAME} PUBLIC de_dvdcss)
    # target_link_libraries(${APP_NAME} PUBLIC de_dvdnav)
    # target_link_libraries(${APP_NAME} PUBLIC de_dvdread)

endif()

#add_subdirectory(test)

