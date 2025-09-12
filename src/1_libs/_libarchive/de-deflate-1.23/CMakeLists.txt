set(LIB_NAME de_deflate)
project(${LIB_NAME}-1.2.3-dev CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/lib)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/libdeflate.h
	)

	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/bt_matchfinder.h
		${LIB_SOURCE_DIR}/cpu_features_common.h
		${LIB_SOURCE_DIR}/crc32_multipliers.h
		${LIB_SOURCE_DIR}/crc32_tables.h
		${LIB_SOURCE_DIR}/decompress_template.h
		${LIB_SOURCE_DIR}/deflate_compress.h
		${LIB_SOURCE_DIR}/deflate_constants.h
		${LIB_SOURCE_DIR}/gzip_constants.h
		${LIB_SOURCE_DIR}/hc_matchfinder.h
		${LIB_SOURCE_DIR}/ht_matchfinder.h
		${LIB_SOURCE_DIR}/lib_common.h
		${LIB_SOURCE_DIR}/matchfinder_common.h
		${LIB_SOURCE_DIR}/zlib_constants.h
		${LIB_SOURCE_DIR}/adler32.c
		${LIB_SOURCE_DIR}/crc32.c
		${LIB_SOURCE_DIR}/deflate_compress.c
		${LIB_SOURCE_DIR}/deflate_decompress.c
		${LIB_SOURCE_DIR}/gzip_compress.c
		${LIB_SOURCE_DIR}/gzip_decompress.c
		${LIB_SOURCE_DIR}/utils.c
		${LIB_SOURCE_DIR}/zlib_compress.c
		${LIB_SOURCE_DIR}/zlib_decompress.c
		#${LIB_SOURCE_DIR}/arm
		#${LIB_SOURCE_DIR}/arm/adler32_impl.h
		#${LIB_SOURCE_DIR}/arm/cpu_features.c
		#${LIB_SOURCE_DIR}/arm/cpu_features.h
		#${LIB_SOURCE_DIR}/arm/crc32_impl.h
		#${LIB_SOURCE_DIR}/arm/crc32_pmull_helpers.h
		#${LIB_SOURCE_DIR}/arm/crc32_pmull_wide.h
		#${LIB_SOURCE_DIR}/arm/matchfinder_impl.h
		#${LIB_SOURCE_DIR}/riscv
		#${LIB_SOURCE_DIR}/riscv/matchfinder_impl.h
		#${LIB_SOURCE_DIR}/x86
		${LIB_SOURCE_DIR}/x86/adler32_impl.h
		${LIB_SOURCE_DIR}/x86/adler32_template.h
		${LIB_SOURCE_DIR}/x86/cpu_features.c
		${LIB_SOURCE_DIR}/x86/cpu_features.h
		${LIB_SOURCE_DIR}/x86/crc32_impl.h
		${LIB_SOURCE_DIR}/x86/crc32_pclmul_template.h
		${LIB_SOURCE_DIR}/x86/decompress_impl.h
		${LIB_SOURCE_DIR}/x86/matchfinder_impl.h
	)

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")	
        target_compile_definitions(${LIB_NAME} PUBLIC LIBDEFLATE_DLL=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libarchive)
    endif()
    
    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
