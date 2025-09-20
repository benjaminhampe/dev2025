set(LIB_NAME de_pcre2)

project(${LIB_NAME}-2.10)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/pcre2.h
        ${LIB_HEADER_DIR}/pcre2posix.h
    )
    set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/config.h
        ${LIB_SOURCE_DIR}/pcre2_auto_possess.c
        ${LIB_SOURCE_DIR}/pcre2_chartables.c
        ${LIB_SOURCE_DIR}/pcre2_chartables.c.dist
        ${LIB_SOURCE_DIR}/pcre2_chkdint.c
        ${LIB_SOURCE_DIR}/pcre2_compile.c
        ${LIB_SOURCE_DIR}/pcre2_compile.h
        ${LIB_SOURCE_DIR}/pcre2_compile_class.c
        ${LIB_SOURCE_DIR}/pcre2_config.c
        ${LIB_SOURCE_DIR}/pcre2_context.c
        ${LIB_SOURCE_DIR}/pcre2_convert.c
        ${LIB_SOURCE_DIR}/pcre2_dfa_match.c
        ${LIB_SOURCE_DIR}/pcre2_dftables.c
        ${LIB_SOURCE_DIR}/pcre2_error.c
        ${LIB_SOURCE_DIR}/pcre2_extuni.c
        ${LIB_SOURCE_DIR}/pcre2_find_bracket.c
        # ${LIB_SOURCE_DIR}/pcre2_fuzzsupport.c
        ${LIB_SOURCE_DIR}/pcre2_internal.h
        ${LIB_SOURCE_DIR}/pcre2_intmodedep.h
        ${LIB_SOURCE_DIR}/pcre2_maketables.c
        ${LIB_SOURCE_DIR}/pcre2_match.c
        ${LIB_SOURCE_DIR}/pcre2_match_data.c
        ${LIB_SOURCE_DIR}/pcre2_newline.c
        ${LIB_SOURCE_DIR}/pcre2_ord2utf.c
        ${LIB_SOURCE_DIR}/pcre2_pattern_info.c
        # ${LIB_SOURCE_DIR}/pcre2_printint.c
        ${LIB_SOURCE_DIR}/pcre2_script_run.c
        ${LIB_SOURCE_DIR}/pcre2_serialize.c
        ${LIB_SOURCE_DIR}/pcre2_string_utils.c
        ${LIB_SOURCE_DIR}/pcre2_study.c
        ${LIB_SOURCE_DIR}/pcre2_substitute.c
        ${LIB_SOURCE_DIR}/pcre2_substring.c
        ${LIB_SOURCE_DIR}/pcre2_tables.c
        ${LIB_SOURCE_DIR}/pcre2_ucd.c
        ${LIB_SOURCE_DIR}/pcre2_ucp.h
        ${LIB_SOURCE_DIR}/pcre2_ucptables.c
        ${LIB_SOURCE_DIR}/pcre2_util.h
        ${LIB_SOURCE_DIR}/pcre2_valid_utf.c
        ${LIB_SOURCE_DIR}/pcre2_xclass.c
        ${LIB_SOURCE_DIR}/pcre2posix.c        
        # ${LIB_SOURCE_DIR}/pcre2_jit_char_inc.h
        # ${LIB_SOURCE_DIR}/pcre2_jit_compile.c
        # ${LIB_SOURCE_DIR}/pcre2_jit_match.c
        # ${LIB_SOURCE_DIR}/pcre2_jit_misc.c
        # ${LIB_SOURCE_DIR}/pcre2_jit_neon_inc.h
        # ${LIB_SOURCE_DIR}/pcre2_jit_simd_inc.h
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
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/minilzo)

	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBPCRE2=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)
    target_compile_definitions(${LIB_NAME} PRIVATE PCRE2_CODE_UNIT_WIDTH=8)
    # target_compile_options(${LIB_NAME} PRIVATE -fopenmp)
		
	# target_link_libraries(${LIB_NAME} PUBLIC de_kissfft)

endif()

# add_subdirectory(test)
