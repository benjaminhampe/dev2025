set(LIB_NAME de_samplerate)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)
	
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    file(GLOB_RECURSE LIB_HEADERS ${LIB_HEADER_DIR}/*.h)
    file(GLOB_RECURSE LIB_SOURCES_H   ${LIB_SOURCE_DIR}/*.h)
    file(GLOB_RECURSE LIB_SOURCES_CPP ${LIB_SOURCE_DIR}/*.cpp)
    set(LIB_HEADERS
		${LIB_HEADER_DIR}/samplerate.h
	)
	set(LIB_SOURCES 
		# ${LIB_SOURCE_DIR}/check_asm.sh
		# ${LIB_SOURCE_DIR}/CMakeLists.txt
		${LIB_SOURCE_DIR}/common.h
		${LIB_SOURCE_DIR}/config.h
		${LIB_SOURCE_DIR}/fastest_coeffs.h
		${LIB_SOURCE_DIR}/high_qual_coeffs.h
		${LIB_SOURCE_DIR}/mid_qual_coeffs.h
		${LIB_SOURCE_DIR}/samplerate.c
		${LIB_SOURCE_DIR}/src_linear.c
		${LIB_SOURCE_DIR}/src_sinc.c
		${LIB_SOURCE_DIR}/src_zoh.c
		# ${LIB_SOURCE_DIR}/Version_script.in
	)

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
        # # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lame.rc ${LIB_SOURCE_DIR}/logoe.ico )
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}) # for user.
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SAMPLERATE=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)
	
    target_link_libraries(${LIB_NAME} PUBLIC de_sndfile)
    target_link_libraries(${LIB_NAME} PUBLIC de_fftw3)

    if (MSVC)
    else()
        target_link_libraries(${LIB_NAME} PRIVATE m)
	endif()

    # file(COPY media/H3 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
	
endif()

# add_subdirectory(test)
