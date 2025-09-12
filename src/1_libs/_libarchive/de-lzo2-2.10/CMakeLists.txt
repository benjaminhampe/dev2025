set(LIB_NAME de_lzo2)

project(${LIB_NAME}-2.10)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/lzo/lzo1.h
        ${LIB_HEADER_DIR}/lzo/lzo1a.h
        ${LIB_HEADER_DIR}/lzo/lzo1b.h
        ${LIB_HEADER_DIR}/lzo/lzo1c.h
        ${LIB_HEADER_DIR}/lzo/lzo1f.h
        ${LIB_HEADER_DIR}/lzo/lzo1x.h
        ${LIB_HEADER_DIR}/lzo/lzo1y.h
        ${LIB_HEADER_DIR}/lzo/lzo1z.h
        ${LIB_HEADER_DIR}/lzo/lzo2a.h
        ${LIB_HEADER_DIR}/lzo/lzoconf.h
        ${LIB_HEADER_DIR}/lzo/lzodefs.h
        ${LIB_HEADER_DIR}/lzo/lzoutil.h
        ${LIB_HEADER_DIR}/lzo/lzo_asm.h
	)
	
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/compr1b.h
        ${LIB_SOURCE_DIR}/compr1c.h
        ${LIB_SOURCE_DIR}/config.h
        ${LIB_SOURCE_DIR}/config1.h
        ${LIB_SOURCE_DIR}/config1a.h
        ${LIB_SOURCE_DIR}/config1b.h
        ${LIB_SOURCE_DIR}/config1c.h
        ${LIB_SOURCE_DIR}/config1f.h
        ${LIB_SOURCE_DIR}/config1x.h
        ${LIB_SOURCE_DIR}/config1y.h
        ${LIB_SOURCE_DIR}/config1z.h
        ${LIB_SOURCE_DIR}/config2a.h
        ${LIB_SOURCE_DIR}/lzo1.c
        ${LIB_SOURCE_DIR}/lzo1a.c
        ${LIB_SOURCE_DIR}/lzo1a_99.c
        ${LIB_SOURCE_DIR}/lzo1a_de.h
        ${LIB_SOURCE_DIR}/lzo1b_1.c
        ${LIB_SOURCE_DIR}/lzo1b_2.c
        ${LIB_SOURCE_DIR}/lzo1b_3.c
        ${LIB_SOURCE_DIR}/lzo1b_4.c
        ${LIB_SOURCE_DIR}/lzo1b_5.c
        ${LIB_SOURCE_DIR}/lzo1b_6.c
        ${LIB_SOURCE_DIR}/lzo1b_7.c
        ${LIB_SOURCE_DIR}/lzo1b_8.c
        ${LIB_SOURCE_DIR}/lzo1b_9.c
        ${LIB_SOURCE_DIR}/lzo1b_99.c
        ${LIB_SOURCE_DIR}/lzo1b_9x.c
        ${LIB_SOURCE_DIR}/lzo1b_cc.c
        ${LIB_SOURCE_DIR}/lzo1b_cc.h
        ${LIB_SOURCE_DIR}/lzo1b_d1.c
        ${LIB_SOURCE_DIR}/lzo1b_d2.c
        ${LIB_SOURCE_DIR}/lzo1b_de.h
        ${LIB_SOURCE_DIR}/lzo1b_rr.c
        ${LIB_SOURCE_DIR}/lzo1b_xx.c
        ${LIB_SOURCE_DIR}/lzo1c_1.c
        ${LIB_SOURCE_DIR}/lzo1c_2.c
        ${LIB_SOURCE_DIR}/lzo1c_3.c
        ${LIB_SOURCE_DIR}/lzo1c_4.c
        ${LIB_SOURCE_DIR}/lzo1c_5.c
        ${LIB_SOURCE_DIR}/lzo1c_6.c
        ${LIB_SOURCE_DIR}/lzo1c_7.c
        ${LIB_SOURCE_DIR}/lzo1c_8.c
        ${LIB_SOURCE_DIR}/lzo1c_9.c
        ${LIB_SOURCE_DIR}/lzo1c_99.c
        ${LIB_SOURCE_DIR}/lzo1c_9x.c
        ${LIB_SOURCE_DIR}/lzo1c_cc.c
        ${LIB_SOURCE_DIR}/lzo1c_cc.h
        ${LIB_SOURCE_DIR}/lzo1c_d1.c
        ${LIB_SOURCE_DIR}/lzo1c_d2.c
        ${LIB_SOURCE_DIR}/lzo1c_rr.c
        ${LIB_SOURCE_DIR}/lzo1c_xx.c
        ${LIB_SOURCE_DIR}/lzo1f_1.c
        ${LIB_SOURCE_DIR}/lzo1f_9x.c
        ${LIB_SOURCE_DIR}/lzo1f_d1.c
        ${LIB_SOURCE_DIR}/lzo1f_d2.c
        ${LIB_SOURCE_DIR}/lzo1x_1.c
        ${LIB_SOURCE_DIR}/lzo1x_1k.c
        ${LIB_SOURCE_DIR}/lzo1x_1l.c
        ${LIB_SOURCE_DIR}/lzo1x_1o.c
        ${LIB_SOURCE_DIR}/lzo1x_9x.c
        ${LIB_SOURCE_DIR}/lzo1x_d1.c
        ${LIB_SOURCE_DIR}/lzo1x_d2.c
        ${LIB_SOURCE_DIR}/lzo1x_d3.c
        ${LIB_SOURCE_DIR}/lzo1x_o.c
        ${LIB_SOURCE_DIR}/lzo1y_1.c
        ${LIB_SOURCE_DIR}/lzo1y_9x.c
        ${LIB_SOURCE_DIR}/lzo1y_d1.c
        ${LIB_SOURCE_DIR}/lzo1y_d2.c
        ${LIB_SOURCE_DIR}/lzo1y_d3.c
        ${LIB_SOURCE_DIR}/lzo1y_o.c
        ${LIB_SOURCE_DIR}/lzo1z_9x.c
        ${LIB_SOURCE_DIR}/lzo1z_d1.c
        ${LIB_SOURCE_DIR}/lzo1z_d2.c
        ${LIB_SOURCE_DIR}/lzo1z_d3.c
        ${LIB_SOURCE_DIR}/lzo1_99.c
        ${LIB_SOURCE_DIR}/lzo2a_9x.c
        ${LIB_SOURCE_DIR}/lzo2a_d1.c
        ${LIB_SOURCE_DIR}/lzo2a_d2.c
        ${LIB_SOURCE_DIR}/lzo_conf.h
        ${LIB_SOURCE_DIR}/lzo_crc.c
        ${LIB_SOURCE_DIR}/lzo_dict.h
        ${LIB_SOURCE_DIR}/lzo_func.h
        ${LIB_SOURCE_DIR}/lzo_init.c
        ${LIB_SOURCE_DIR}/lzo_ptr.c
        ${LIB_SOURCE_DIR}/lzo_ptr.h
        ${LIB_SOURCE_DIR}/lzo_str.c
        ${LIB_SOURCE_DIR}/lzo_supp.h
        ${LIB_SOURCE_DIR}/lzo_util.c
        ${LIB_SOURCE_DIR}/stats1a.h
        ${LIB_SOURCE_DIR}/stats1b.h
        ${LIB_SOURCE_DIR}/stats1c.h
    )
	
    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
		# target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})

	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBLZO2=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LZO=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)

    # target_compile_options(${LIB_NAME} PRIVATE -fopenmp)
		
	# target_link_libraries(${LIB_NAME} PUBLIC de_kissfft)
endif()

# add_subdirectory(test)
