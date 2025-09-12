set(LIB_NAME de_iconv)

project(${LIB_NAME} C)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/iconv.h
	)
	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/compat.c		
		${LIB_SOURCE_DIR}/genaliases.c
		${LIB_SOURCE_DIR}/genaliases2.c
		${LIB_SOURCE_DIR}/genflags.c
		${LIB_SOURCE_DIR}/gentranslit.c		
		${LIB_SOURCE_DIR}/iconv.c
		#${LIB_SOURCE_DIR}/iconv_no_i18n.c		
		#${LIB_SOURCE_DIR}/lt-iconv_no_i18n.c	
		${LIB_SOURCE_DIR}/aliases.h
		${LIB_SOURCE_DIR}/aliases2.h
		${LIB_SOURCE_DIR}/aliases_aix.h
		${LIB_SOURCE_DIR}/aliases_aix_sysaix.h
		${LIB_SOURCE_DIR}/aliases_dos.h
		${LIB_SOURCE_DIR}/aliases_extra.h
		${LIB_SOURCE_DIR}/aliases_osf1.h
		${LIB_SOURCE_DIR}/aliases_osf1_sysosf1.h
		${LIB_SOURCE_DIR}/aliases_sysaix.h
		${LIB_SOURCE_DIR}/aliases_syshpux.h
		${LIB_SOURCE_DIR}/aliases_sysosf1.h
		${LIB_SOURCE_DIR}/aliases_syssolaris.h
		${LIB_SOURCE_DIR}/aliases_zos.h
		${LIB_SOURCE_DIR}/armscii_8.h
		${LIB_SOURCE_DIR}/ascii.h
		${LIB_SOURCE_DIR}/atarist.h
		${LIB_SOURCE_DIR}/big5.h
		${LIB_SOURCE_DIR}/big5hkscs1999.h
		${LIB_SOURCE_DIR}/big5hkscs2001.h
		${LIB_SOURCE_DIR}/big5hkscs2004.h
		${LIB_SOURCE_DIR}/big5hkscs2008.h
		${LIB_SOURCE_DIR}/big5_2003.h
		${LIB_SOURCE_DIR}/c99.h
		${LIB_SOURCE_DIR}/canonical.h
		${LIB_SOURCE_DIR}/canonical_aix.h
		${LIB_SOURCE_DIR}/canonical_aix_sysaix.h
		${LIB_SOURCE_DIR}/canonical_dos.h
		${LIB_SOURCE_DIR}/canonical_extra.h
		${LIB_SOURCE_DIR}/canonical_local.h
		${LIB_SOURCE_DIR}/canonical_local_sysaix.h
		${LIB_SOURCE_DIR}/canonical_local_syshpux.h
		${LIB_SOURCE_DIR}/canonical_local_sysosf1.h
		${LIB_SOURCE_DIR}/canonical_local_syssolaris.h
		${LIB_SOURCE_DIR}/canonical_osf1.h
		${LIB_SOURCE_DIR}/canonical_osf1_sysosf1.h
		${LIB_SOURCE_DIR}/canonical_sysaix.h
		${LIB_SOURCE_DIR}/canonical_syshpux.h
		${LIB_SOURCE_DIR}/canonical_sysosf1.h
		${LIB_SOURCE_DIR}/canonical_syssolaris.h
		${LIB_SOURCE_DIR}/canonical_zos.h
		${LIB_SOURCE_DIR}/ces_big5.h
		${LIB_SOURCE_DIR}/ces_gbk.h
		${LIB_SOURCE_DIR}/cjk_variants.h
		${LIB_SOURCE_DIR}/cns11643.h
		${LIB_SOURCE_DIR}/cns11643_1.h
		${LIB_SOURCE_DIR}/cns11643_15.h
		${LIB_SOURCE_DIR}/cns11643_2.h
		${LIB_SOURCE_DIR}/cns11643_3.h
		${LIB_SOURCE_DIR}/cns11643_4.h
		${LIB_SOURCE_DIR}/cns11643_4a.h
		${LIB_SOURCE_DIR}/cns11643_4b.h
		${LIB_SOURCE_DIR}/cns11643_5.h
		${LIB_SOURCE_DIR}/cns11643_6.h
		${LIB_SOURCE_DIR}/cns11643_7.h
		${LIB_SOURCE_DIR}/cns11643_inv.h
		${LIB_SOURCE_DIR}/config.h
		${LIB_SOURCE_DIR}/converters.h
		${LIB_SOURCE_DIR}/cp1046.h
		${LIB_SOURCE_DIR}/cp1124.h
		${LIB_SOURCE_DIR}/cp1125.h
		${LIB_SOURCE_DIR}/cp1129.h
		${LIB_SOURCE_DIR}/cp1131.h
		${LIB_SOURCE_DIR}/cp1133.h
		${LIB_SOURCE_DIR}/cp1161.h
		${LIB_SOURCE_DIR}/cp1162.h
		${LIB_SOURCE_DIR}/cp1163.h
		${LIB_SOURCE_DIR}/cp1250.h
		${LIB_SOURCE_DIR}/cp1251.h
		${LIB_SOURCE_DIR}/cp1252.h
		${LIB_SOURCE_DIR}/cp1253.h
		${LIB_SOURCE_DIR}/cp1254.h
		${LIB_SOURCE_DIR}/cp1255.h
		${LIB_SOURCE_DIR}/cp1256.h
		${LIB_SOURCE_DIR}/cp1257.h
		${LIB_SOURCE_DIR}/cp1258.h
		${LIB_SOURCE_DIR}/cp437.h
		${LIB_SOURCE_DIR}/cp50221_0208_ext.h
		${LIB_SOURCE_DIR}/cp50221_0212_ext.h
		${LIB_SOURCE_DIR}/cp737.h
		${LIB_SOURCE_DIR}/cp775.h
		${LIB_SOURCE_DIR}/cp850.h
		${LIB_SOURCE_DIR}/cp852.h
		${LIB_SOURCE_DIR}/cp853.h
		${LIB_SOURCE_DIR}/cp855.h
		${LIB_SOURCE_DIR}/cp856.h
		${LIB_SOURCE_DIR}/cp857.h
		${LIB_SOURCE_DIR}/cp858.h
		${LIB_SOURCE_DIR}/cp860.h
		${LIB_SOURCE_DIR}/cp861.h
		${LIB_SOURCE_DIR}/cp862.h
		${LIB_SOURCE_DIR}/cp863.h
		${LIB_SOURCE_DIR}/cp864.h
		${LIB_SOURCE_DIR}/cp865.h
		${LIB_SOURCE_DIR}/cp866.h
		${LIB_SOURCE_DIR}/cp869.h
		${LIB_SOURCE_DIR}/cp874.h
		${LIB_SOURCE_DIR}/cp922.h
		${LIB_SOURCE_DIR}/cp932.h
		${LIB_SOURCE_DIR}/cp932ext.h
		${LIB_SOURCE_DIR}/cp936.h
		${LIB_SOURCE_DIR}/cp936ext.h
		${LIB_SOURCE_DIR}/cp943.h
		${LIB_SOURCE_DIR}/cp949.h
		${LIB_SOURCE_DIR}/cp950.h
		${LIB_SOURCE_DIR}/cp950ext.h
		${LIB_SOURCE_DIR}/dec_hanyu.h
		${LIB_SOURCE_DIR}/dec_kanji.h
		${LIB_SOURCE_DIR}/ebcdic037.h
		${LIB_SOURCE_DIR}/ebcdic1025.h
		${LIB_SOURCE_DIR}/ebcdic1026.h
		${LIB_SOURCE_DIR}/ebcdic1047.h
		${LIB_SOURCE_DIR}/ebcdic1097.h
		${LIB_SOURCE_DIR}/ebcdic1112.h
		${LIB_SOURCE_DIR}/ebcdic1122.h
		${LIB_SOURCE_DIR}/ebcdic1123.h
		${LIB_SOURCE_DIR}/ebcdic1130.h
		${LIB_SOURCE_DIR}/ebcdic1132.h
		${LIB_SOURCE_DIR}/ebcdic1137.h
		${LIB_SOURCE_DIR}/ebcdic1140.h
		${LIB_SOURCE_DIR}/ebcdic1141.h
		${LIB_SOURCE_DIR}/ebcdic1142.h
		${LIB_SOURCE_DIR}/ebcdic1143.h
		${LIB_SOURCE_DIR}/ebcdic1144.h
		${LIB_SOURCE_DIR}/ebcdic1145.h
		${LIB_SOURCE_DIR}/ebcdic1146.h
		${LIB_SOURCE_DIR}/ebcdic1147.h
		${LIB_SOURCE_DIR}/ebcdic1148.h
		${LIB_SOURCE_DIR}/ebcdic1149.h
		${LIB_SOURCE_DIR}/ebcdic1153.h
		${LIB_SOURCE_DIR}/ebcdic1154.h
		${LIB_SOURCE_DIR}/ebcdic1155.h
		${LIB_SOURCE_DIR}/ebcdic1156.h
		${LIB_SOURCE_DIR}/ebcdic1157.h
		${LIB_SOURCE_DIR}/ebcdic1158.h
		${LIB_SOURCE_DIR}/ebcdic1160.h
		${LIB_SOURCE_DIR}/ebcdic1164.h
		${LIB_SOURCE_DIR}/ebcdic1165.h
		${LIB_SOURCE_DIR}/ebcdic1166.h
		${LIB_SOURCE_DIR}/ebcdic12712.h
		${LIB_SOURCE_DIR}/ebcdic16804.h
		${LIB_SOURCE_DIR}/ebcdic273.h
		${LIB_SOURCE_DIR}/ebcdic277.h
		${LIB_SOURCE_DIR}/ebcdic278.h
		${LIB_SOURCE_DIR}/ebcdic280.h
		${LIB_SOURCE_DIR}/ebcdic282.h
		${LIB_SOURCE_DIR}/ebcdic284.h
		${LIB_SOURCE_DIR}/ebcdic285.h
		${LIB_SOURCE_DIR}/ebcdic297.h
		${LIB_SOURCE_DIR}/ebcdic423.h
		${LIB_SOURCE_DIR}/ebcdic424.h
		${LIB_SOURCE_DIR}/ebcdic425.h
		${LIB_SOURCE_DIR}/ebcdic4971.h
		${LIB_SOURCE_DIR}/ebcdic500.h
		${LIB_SOURCE_DIR}/ebcdic838.h
		${LIB_SOURCE_DIR}/ebcdic870.h
		${LIB_SOURCE_DIR}/ebcdic871.h
		${LIB_SOURCE_DIR}/ebcdic875.h
		${LIB_SOURCE_DIR}/ebcdic880.h
		${LIB_SOURCE_DIR}/ebcdic905.h
		${LIB_SOURCE_DIR}/ebcdic924.h
		${LIB_SOURCE_DIR}/euc_cn.h
		${LIB_SOURCE_DIR}/euc_jisx0213.h
		${LIB_SOURCE_DIR}/euc_jp.h
		${LIB_SOURCE_DIR}/euc_kr.h
		${LIB_SOURCE_DIR}/euc_tw.h
		${LIB_SOURCE_DIR}/export.h
		${LIB_SOURCE_DIR}/flags.h
		${LIB_SOURCE_DIR}/flushwc.h
		${LIB_SOURCE_DIR}/gb12345.h
		${LIB_SOURCE_DIR}/gb12345ext.h
		${LIB_SOURCE_DIR}/gb18030ext.h
		${LIB_SOURCE_DIR}/gb18030uni.h
		${LIB_SOURCE_DIR}/gb18030_2005.h
		${LIB_SOURCE_DIR}/gb18030_2022.h
		${LIB_SOURCE_DIR}/gb2312.h
		${LIB_SOURCE_DIR}/gbk.h
		${LIB_SOURCE_DIR}/gbkext1.h
		${LIB_SOURCE_DIR}/gbkext2.h
		${LIB_SOURCE_DIR}/gbkext_inv.h
		${LIB_SOURCE_DIR}/georgian_academy.h
		${LIB_SOURCE_DIR}/georgian_ps.h
		${LIB_SOURCE_DIR}/hkscs1999.h
		${LIB_SOURCE_DIR}/hkscs2001.h
		${LIB_SOURCE_DIR}/hkscs2004.h
		${LIB_SOURCE_DIR}/hkscs2008.h
		${LIB_SOURCE_DIR}/hp_roman8.h
		${LIB_SOURCE_DIR}/hz.h
		${LIB_SOURCE_DIR}/iconv_open1.h
		${LIB_SOURCE_DIR}/iconv_open2.h
		${LIB_SOURCE_DIR}/iso2022_cn.h
		${LIB_SOURCE_DIR}/iso2022_cnext.h
		${LIB_SOURCE_DIR}/iso2022_jp.h
		${LIB_SOURCE_DIR}/iso2022_jp1.h
		${LIB_SOURCE_DIR}/iso2022_jp2.h
		${LIB_SOURCE_DIR}/iso2022_jp3.h
		${LIB_SOURCE_DIR}/iso2022_jpms.h
		${LIB_SOURCE_DIR}/iso2022_kr.h
		${LIB_SOURCE_DIR}/iso646_cn.h
		${LIB_SOURCE_DIR}/iso646_jp.h
		${LIB_SOURCE_DIR}/iso8859_1.h
		${LIB_SOURCE_DIR}/iso8859_10.h
		${LIB_SOURCE_DIR}/iso8859_11.h
		${LIB_SOURCE_DIR}/iso8859_13.h
		${LIB_SOURCE_DIR}/iso8859_14.h
		${LIB_SOURCE_DIR}/iso8859_15.h
		${LIB_SOURCE_DIR}/iso8859_16.h
		${LIB_SOURCE_DIR}/iso8859_2.h
		${LIB_SOURCE_DIR}/iso8859_3.h
		${LIB_SOURCE_DIR}/iso8859_4.h
		${LIB_SOURCE_DIR}/iso8859_5.h
		${LIB_SOURCE_DIR}/iso8859_6.h
		${LIB_SOURCE_DIR}/iso8859_7.h
		${LIB_SOURCE_DIR}/iso8859_8.h
		${LIB_SOURCE_DIR}/iso8859_9.h
		${LIB_SOURCE_DIR}/isoir165.h
		${LIB_SOURCE_DIR}/isoir165ext.h
		${LIB_SOURCE_DIR}/java.h
		${LIB_SOURCE_DIR}/jisx0201.h
		${LIB_SOURCE_DIR}/jisx0208.h
		${LIB_SOURCE_DIR}/jisx0212.h
		${LIB_SOURCE_DIR}/jisx0213.h
		${LIB_SOURCE_DIR}/johab.h
		${LIB_SOURCE_DIR}/johab_hangul.h
		${LIB_SOURCE_DIR}/koi8_r.h
		${LIB_SOURCE_DIR}/koi8_ru.h
		${LIB_SOURCE_DIR}/koi8_t.h
		${LIB_SOURCE_DIR}/koi8_u.h
		${LIB_SOURCE_DIR}/ksc5601.h
		${LIB_SOURCE_DIR}/libcharset.h
		${LIB_SOURCE_DIR}/localcharset.h
		${LIB_SOURCE_DIR}/loops.h
		${LIB_SOURCE_DIR}/loop_unicode.h
		${LIB_SOURCE_DIR}/loop_wchar.h
		${LIB_SOURCE_DIR}/mac_arabic.h
		${LIB_SOURCE_DIR}/mac_centraleurope.h
		${LIB_SOURCE_DIR}/mac_croatian.h
		${LIB_SOURCE_DIR}/mac_cyrillic.h
		${LIB_SOURCE_DIR}/mac_greek.h
		${LIB_SOURCE_DIR}/mac_hebrew.h
		${LIB_SOURCE_DIR}/mac_iceland.h
		${LIB_SOURCE_DIR}/mac_roman.h
		${LIB_SOURCE_DIR}/mac_romania.h
		${LIB_SOURCE_DIR}/mac_thai.h
		${LIB_SOURCE_DIR}/mac_turkish.h
		${LIB_SOURCE_DIR}/mac_ukraine.h
		${LIB_SOURCE_DIR}/mulelao.h
		${LIB_SOURCE_DIR}/nextstep.h
		${LIB_SOURCE_DIR}/pt154.h
		${LIB_SOURCE_DIR}/riscos1.h
		${LIB_SOURCE_DIR}/rk1048.h
		${LIB_SOURCE_DIR}/shift_jisx0213.h
		${LIB_SOURCE_DIR}/sjis.h
		${LIB_SOURCE_DIR}/tcvn.h
		${LIB_SOURCE_DIR}/tds565.h
		${LIB_SOURCE_DIR}/tis620.h
		${LIB_SOURCE_DIR}/translit.h
		${LIB_SOURCE_DIR}/ucs2.h
		${LIB_SOURCE_DIR}/ucs2be.h
		${LIB_SOURCE_DIR}/ucs2internal.h
		${LIB_SOURCE_DIR}/ucs2le.h
		${LIB_SOURCE_DIR}/ucs2swapped.h
		${LIB_SOURCE_DIR}/ucs4.h
		${LIB_SOURCE_DIR}/ucs4be.h
		${LIB_SOURCE_DIR}/ucs4internal.h
		${LIB_SOURCE_DIR}/ucs4le.h
		${LIB_SOURCE_DIR}/ucs4swapped.h
		${LIB_SOURCE_DIR}/uhc_1.h
		${LIB_SOURCE_DIR}/uhc_2.h
		${LIB_SOURCE_DIR}/utf16.h
		${LIB_SOURCE_DIR}/utf16be.h
		${LIB_SOURCE_DIR}/utf16le.h
		${LIB_SOURCE_DIR}/utf32.h
		${LIB_SOURCE_DIR}/utf32be.h
		${LIB_SOURCE_DIR}/utf32le.h
		${LIB_SOURCE_DIR}/utf7.h
		${LIB_SOURCE_DIR}/utf8.h
		${LIB_SOURCE_DIR}/vietcomb.h
		${LIB_SOURCE_DIR}/viscii.h
		${LIB_SOURCE_DIR}/zos-tag.h
	)

	# if (WIN32)
	# enable_language(RC)
	# # list(APPEND LIB_SOURCES 
	# # ${LIB_SOURCE_DIR}/common/win32thread.c
	# # ${LIB_SOURCE_DIR}/common/win32thread.h )
	# endif ()
	
	# if (BUILD_SHARED)
	# message("${BoldCyan}SHARED_LIBRARY: ${LIB_NAME}${ColourReset}")
	# add_library(${LIB_NAME} SHARED "")
	# else()
	message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
	add_library(${LIB_NAME} STATIC "")
	# endif()
	
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_ICONV=1)
	target_compile_definitions(${LIB_NAME} PUBLIC BUILDING_LIBICONV=1)
	target_compile_definitions(${LIB_NAME} PRIVATE HAVE_CONFIG_H=1)
	
	# target_compile_options(${LIB_NAME} PRIVATE 
		# -Wno-unused-parameter
		# -Wno-sign-compare
		# -Wno-missing-field-initializers
		# -Wno-implicit-fallthrough
		# -Wno-type-limits
		# -Wno-pointer-to-int-cast
	# )
	
	target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})

	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
	# target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/../..) # config.h

	target_link_libraries(${LIB_NAME} PUBLIC de_iconv_charset)
	target_link_libraries(${LIB_NAME} PRIVATE de_iconv_icrt)
	target_link_libraries(${LIB_NAME} PUBLIC m)
	# on Windows:
	if (WIN32)
		target_link_libraries(${LIB_NAME} PUBLIC comctl32 user32 gdi32 kernel32)
	# on Linux:
	else()
		target_link_libraries(${LIB_NAME} PUBLIC X11 Xxf86vm Xi Xcursor)
	endif()

	# target_link_libraries(${LIB_NAME} PUBLIC stdc++fs)
	# target_link_libraries(${LIB_NAME} PUBLIC pthread)
	# target_link_libraries(${LIB_NAME} PUBLIC m)
   
endif()
