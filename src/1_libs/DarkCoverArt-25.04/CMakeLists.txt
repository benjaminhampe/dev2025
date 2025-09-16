set(LIB_NAME DarkCoverArt_2504)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    #add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/de/audio/EGenre.h
        ${LIB_HEADER_DIR}/de/audio/ETagType.h
        ${LIB_HEADER_DIR}/de/audio/GetCoverArt.h
    )
    
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/de/audio/de_taglib.cpp
        ${LIB_SOURCE_DIR}/de/audio/de_taglib.h
        ${LIB_SOURCE_DIR}/de/audio/GetCoverArt.cpp
        ${LIB_SOURCE_DIR}/de/audio/ID3v1.h
        ${LIB_SOURCE_DIR}/de/audio/ID3v2.h        
    )
        
    # ./res
    # ./res/7z.ico
    # ./res/app.manifest
    # ./res/resource.h
    # ./res/resource.rc

    # enable_language(RC)
    # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
    # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)

    if (BUILD_SHARED)
        add_library(${LIB_NAME} SHARED "")
        message("${Blue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${Blue}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})
	
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
    endif()

    target_link_libraries(${LIB_NAME} PUBLIC DarkImage_2504)    
    target_link_libraries(${LIB_NAME} PRIVATE de_taglib)
    
endif()

# add_subdirectory(test)

