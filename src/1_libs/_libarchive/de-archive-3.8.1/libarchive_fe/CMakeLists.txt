set(LIB_NAME de_archive_fe)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/archive/fe/config.h
        ${LIB_HEADER_DIR}/archive/fe/err.h
        ${LIB_HEADER_DIR}/archive/fe/lafe_platform.h
        ${LIB_HEADER_DIR}/archive/fe/line_reader.h
        ${LIB_HEADER_DIR}/archive/fe/passphrase.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/err.c
        ${LIB_SOURCE_DIR}/line_reader.c
        ${LIB_SOURCE_DIR}/passphrase.c
    )

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/archive/fe)

	# target_compile_definitions(${LIB_NAME} PUBLIC HAVE_BZIP2=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBBZ2=1)
	
    if(MSVC)
    else()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-maybe-uninitialized)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)
    endif()
    
    # file(COPY ${LIB_HEADERS} DESTINATION ${LIB_INCLUDE_DIR}) # For the SDK

endif()

# add_subdirectory(test)
