set(LIB_NAME de_lz4)

project(${LIB_NAME}-1.10.0)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
		${LIB_HEADER_DIR}/lz4.h
		${LIB_HEADER_DIR}/lz4hc.h
	)
    set(LIB_SOURCES
        # ${LIB_SOURCE_DIR}/liblz4-dll.rc.in
        # ${LIB_SOURCE_DIR}/liblz4.pc.in
        # ${LIB_SOURCE_DIR}/LICENSE
        ${LIB_SOURCE_DIR}/lz4.c
        ${LIB_SOURCE_DIR}/lz4file.c
        ${LIB_SOURCE_DIR}/lz4file.h
        ${LIB_SOURCE_DIR}/lz4frame.c
        ${LIB_SOURCE_DIR}/lz4frame.h
        ${LIB_SOURCE_DIR}/lz4frame_static.h
        ${LIB_SOURCE_DIR}/lz4hc.c
        # ${LIB_SOURCE_DIR}/Makefile
        # ${LIB_SOURCE_DIR}/README.md
        ${LIB_SOURCE_DIR}/xxhash.c
        ${LIB_SOURCE_DIR}/xxhash.h
    )
	
    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
		target_compile_definitions(${LIB_NAME} PUBLIC LZ4_DLL_EXPORT=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBLZ4=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LZ4=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LZ4_H=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LZ4HC_H=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)
    target_compile_definitions(${LIB_NAME} PRIVATE XXH_NAMESPACE=LZ4_)
	
	# -O3 
# DEBUGFLAGS:= -Wall -Wextra -Wcast-qual -Wcast-align -Wshadow \
             # -Wswitch-enum -Wdeclaration-after-statement -Wstrict-prototypes \
             # -Wundef -Wpointer-arith -Wstrict-aliasing=1
    # target_compile_options(${LIB_NAME} PRIVATE -fopenmp)
		
	# target_link_libraries(${LIB_NAME} PUBLIC de_kissfft)
endif()

# add_subdirectory(test)
