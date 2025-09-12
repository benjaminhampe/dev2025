set(LIB_NAME DarkGDK_2024)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    file(GLOB_RECURSE LIB_HEADERS_H   "${LIB_HEADER_DIR}/*.h")
    file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
    set(LIB_HEADERS ${LIB_HEADERS_H} ${LIB_HEADERS_HPP})

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE LIB_SOURCES_H   "${LIB_SOURCE_DIR}/*.h")
    file(GLOB_RECURSE LIB_SOURCES_HPP "${LIB_SOURCE_DIR}/*.hpp")
    file(GLOB_RECURSE LIB_SOURCES_CPP "${LIB_SOURCE_DIR}/*.cpp")
    set(LIB_SOURCES ${LIB_SOURCES_H} ${LIB_SOURCES_HPP} ${LIB_SOURCES_CPP})

    if (WIN32)
        enable_language(RC)
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

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})

    #target_compile_definitions(${LIB_NAME} PRIVATE UNICODE=1)    # Windows says "Cant create Window" if defined and A versions is used?
    #target_compile_definitions(${LIB_NAME} PRIVATE _UNICODE=1)   # Windows says "Cant create Window" if defined and A versions is used?
    target_compile_definitions(${LIB_NAME} PRIVATE OEMRESOURCE=1) # needed by winuser.h ODC
    target_compile_definitions(${LIB_NAME} PRIVATE GL_EXT_PROTOTYPES=1)
    target_compile_definitions(${LIB_NAME} PRIVATE GL_GLEXT_PROTOTYPES=1)
    target_compile_definitions(${LIB_NAME} PRIVATE WGL_WGLEXT_PROTOTYPES=1)

    if (MSVC)
    else()
        # target_link_options(${LIB_NAME} PRIVATE -mwindows)
        
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-value)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-old-style-cast)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-deprecated-declarations)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-variable)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-function)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)
        
        # target_link_libraries(${LIB_NAME} PRIVATE stdc++fs)
        target_link_libraries(${LIB_NAME} PRIVATE pthread)
        target_link_libraries(${LIB_NAME} PRIVATE m)

    endif()
    
    if (WIN32)
        target_link_libraries(${LIB_NAME} PUBLIC glmf32)
        target_link_libraries(${LIB_NAME} PUBLIC opengl32)
        target_link_libraries(${LIB_NAME} PUBLIC dwmapi)
        target_link_libraries(${LIB_NAME} PUBLIC shell32)
        target_link_libraries(${LIB_NAME} PUBLIC user32)
        target_link_libraries(${LIB_NAME} PUBLIC gdi32)
        target_link_libraries(${LIB_NAME} PUBLIC kernel32)
        
        # target_link_libraries(${LIB_NAME} PUBLIC glu32)
        # target_link_libraries(${LIB_NAME} PUBLIC SetupAPI)
        # target_link_libraries(${LIB_NAME} PUBLIC wmcodecdspuuid)
        # target_link_libraries(${LIB_NAME} PUBLIC mfuuid)
        # target_link_libraries(${LIB_NAME} PUBLIC mf)
        # target_link_libraries(${LIB_NAME} PUBLIC mfplat)
        # target_link_libraries(${LIB_NAME} PUBLIC mf3216 mapi32 mapistub mdminst imm32)
        # target_link_libraries(${LIB_NAME} PUBLIC uuid)
        # target_link_libraries(${LIB_NAME} PUBLIC ks)
        # target_link_libraries(${LIB_NAME} PUBLIC ksuser)
        # target_link_libraries(${LIB_NAME} PUBLIC ksguid)
        # target_link_libraries(${LIB_NAME} PUBLIC winmm)
        # target_link_libraries(${LIB_NAME} PUBLIC dsound)
        # target_link_libraries(${LIB_NAME} PUBLIC comctl32)
        # target_link_libraries(${LIB_NAME} PUBLIC oleaut32)
        # target_link_libraries(${LIB_NAME} PUBLIC ole32)
        # target_link_libraries(${LIB_NAME} PUBLIC advapi32)        
    else()
        target_link_libraries(${LIB_NAME} PUBLIC GL GLU X11 Xxf86vm Xi Xcursor)
    endif()

    target_link_libraries(${LIB_NAME} PRIVATE de_glm)
    target_link_libraries(${LIB_NAME} PRIVATE de_glew)
    target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2)

    target_link_libraries(${LIB_NAME} PRIVATE de_zlib)
    target_link_libraries(${LIB_NAME} PRIVATE de_bzip2)
    target_link_libraries(${LIB_NAME} PRIVATE de_minizip)
    target_link_libraries(${LIB_NAME} PRIVATE de_unzip)
    # target_link_libraries(${LIB_NAME} PRIVATE de_lzma)

    target_link_libraries(${LIB_NAME} PRIVATE de_png)
    target_link_libraries(${LIB_NAME} PRIVATE de_jpeg8d)
    # target_link_libraries(${LIB_NAME} PRIVATE de_tiff4)
    # target_link_libraries(${LIB_NAME} PRIVATE de_gif5)

endif()

# add_subdirectory(test)
