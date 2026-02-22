set(LIB_NAME de_jpeg)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/jconfig.h
        ${LIB_HEADER_DIR}/jdct.h
        ${LIB_HEADER_DIR}/jerror.h
        ${LIB_HEADER_DIR}/jinclude.h
        ${LIB_HEADER_DIR}/jmemsys.h
        ${LIB_HEADER_DIR}/jmorecfg.h
        ${LIB_HEADER_DIR}/jpegint.h
        ${LIB_HEADER_DIR}/jpeglib.h
        ${LIB_HEADER_DIR}/jversion.h
    )
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
        ${LIB_SOURCE_DIR}/jmemansi.c
        ${LIB_SOURCE_DIR}/jmemmgr.c
        # ${LIB_SOURCE_DIR}/jmemnobs.c
        ${LIB_SOURCE_DIR}/jquant1.c
        ${LIB_SOURCE_DIR}/jquant2.c
        ${LIB_SOURCE_DIR}/jutils.c
    )

	set_source_files_properties(${LIB_SOURCE_DIR}/jcdctmgr.c PROPERTIES COMPILE_FLAGS "-O1")
	
    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        target_compile_definitions(${LIB_NAME} PUBLIC BENNI_LIBJPEG_DLL=1)
        target_compile_definitions(${LIB_NAME} PRIVATE BENNI_LIBJPEG_INTERNAL=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/libjpeg)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_JPEG=1)
	
	# target_compile_definitions(${LIB_NAME} PUBLIC NEED_SHORT_EXTERNAL_NAMES=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_GMP_MINI=1)

    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
    # target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
    # target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)

    # check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

    # target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)

    # target_link_libraries(${LIB_NAME} PUBLIC de_kissfft)

endif()

# add_subdirectory(test)
