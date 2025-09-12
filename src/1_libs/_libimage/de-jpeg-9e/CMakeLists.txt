return()

set(LIB_NAME de_jpeg9e)
project(${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/jpeg9e)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS 
		${LIB_HEADER_DIR}/jconfig.h
		${LIB_HEADER_DIR}/cderror.h
		# ${LIB_HEADER_DIR}/jchuff.h
		${LIB_HEADER_DIR}/jdct.h
		# ${LIB_HEADER_DIR}/jdhuff.h
		${LIB_HEADER_DIR}/jerror.h
		${LIB_HEADER_DIR}/jinclude.h
		${LIB_HEADER_DIR}/jmemsys.h
		${LIB_HEADER_DIR}/jmorecfg.h
		${LIB_HEADER_DIR}/jpegint.h
		${LIB_HEADER_DIR}/jpeglib.h
		${LIB_HEADER_DIR}/jversion.h)
	
	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/jaricom.c
		${LIB_SOURCE_DIR}/jcapimin.c
		${LIB_SOURCE_DIR}/jcapistd.c
		${LIB_SOURCE_DIR}/jcarith.c
		${LIB_SOURCE_DIR}/jccoefct.c
		${LIB_SOURCE_DIR}/jccolor.c
		${LIB_SOURCE_DIR}/jcdctmgr.c
		${LIB_SOURCE_DIR}/jchuff.c
		${LIB_SOURCE_DIR}/jcinit.c
		${LIB_SOURCE_DIR}/jcmainct.c
		${LIB_SOURCE_DIR}/jcmarker.c
		${LIB_SOURCE_DIR}/jcmaster.c
		${LIB_SOURCE_DIR}/jcomapi.c
		${LIB_SOURCE_DIR}/jcparam.c
		${LIB_SOURCE_DIR}/jcprepct.c
		${LIB_SOURCE_DIR}/jcsample.c
		${LIB_SOURCE_DIR}/jctrans.c
		${LIB_SOURCE_DIR}/jdapimin.c
		${LIB_SOURCE_DIR}/jdapistd.c
		${LIB_SOURCE_DIR}/jdarith.c
		${LIB_SOURCE_DIR}/jdatadst.c
		${LIB_SOURCE_DIR}/jdatasrc.c
		${LIB_SOURCE_DIR}/jdcoefct.c
		${LIB_SOURCE_DIR}/jdcolor.c
		${LIB_SOURCE_DIR}/jddctmgr.c
		${LIB_SOURCE_DIR}/jdhuff.c
		${LIB_SOURCE_DIR}/jdinput.c
		${LIB_SOURCE_DIR}/jdmainct.c
		${LIB_SOURCE_DIR}/jdmarker.c
		${LIB_SOURCE_DIR}/jdmaster.c
		${LIB_SOURCE_DIR}/jdmerge.c
		${LIB_SOURCE_DIR}/jdpostct.c
		${LIB_SOURCE_DIR}/jdsample.c
		${LIB_SOURCE_DIR}/jdtrans.c
		${LIB_SOURCE_DIR}/jerror.c
		${LIB_SOURCE_DIR}/jfdctflt.c
		${LIB_SOURCE_DIR}/jfdctfst.c
		${LIB_SOURCE_DIR}/jfdctint.c
		${LIB_SOURCE_DIR}/jidctflt.c
		${LIB_SOURCE_DIR}/jidctfst.c
		${LIB_SOURCE_DIR}/jidctint.c
		${LIB_SOURCE_DIR}/jmemmgr.c
		${LIB_SOURCE_DIR}/jmemnobs.c
		${LIB_SOURCE_DIR}/jquant1.c
		${LIB_SOURCE_DIR}/jquant2.c
		${LIB_SOURCE_DIR}/jutils.c
    )

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

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..) # user
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}) # sources I
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}) # sources II

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _benni)
    else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-main)
    endif()

    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
