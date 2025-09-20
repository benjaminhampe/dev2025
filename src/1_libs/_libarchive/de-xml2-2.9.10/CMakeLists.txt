set(LIB_NAME de_xml2) # cmake target name
project(${LIB_NAME}-2.9.10)

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/libxml/c14n.h
		${LIB_HEADER_DIR}/libxml/catalog.h
		${LIB_HEADER_DIR}/libxml/chvalid.h
		${LIB_HEADER_DIR}/libxml/config.h
		${LIB_HEADER_DIR}/libxml/debugXML.h
		${LIB_HEADER_DIR}/libxml/dict.h
		${LIB_HEADER_DIR}/libxml/DOCBparser.h
		${LIB_HEADER_DIR}/libxml/encoding.h
		${LIB_HEADER_DIR}/libxml/entities.h
		${LIB_HEADER_DIR}/libxml/globals.h
		${LIB_HEADER_DIR}/libxml/hash.h
		${LIB_HEADER_DIR}/libxml/HTMLparser.h
		${LIB_HEADER_DIR}/libxml/HTMLtree.h
		${LIB_HEADER_DIR}/libxml/list.h
		${LIB_HEADER_DIR}/libxml/nanoftp.h
		${LIB_HEADER_DIR}/libxml/nanohttp.h
		${LIB_HEADER_DIR}/libxml/parser.h
		${LIB_HEADER_DIR}/libxml/parserInternals.h
		${LIB_HEADER_DIR}/libxml/pattern.h
		${LIB_HEADER_DIR}/libxml/relaxng.h
		${LIB_HEADER_DIR}/libxml/SAX.h
		${LIB_HEADER_DIR}/libxml/SAX2.h
		${LIB_HEADER_DIR}/libxml/schemasInternals.h
		${LIB_HEADER_DIR}/libxml/schematron.h
		${LIB_HEADER_DIR}/libxml/threads.h
		${LIB_HEADER_DIR}/libxml/tree.h
		${LIB_HEADER_DIR}/libxml/uri.h
		${LIB_HEADER_DIR}/libxml/valid.h
		${LIB_HEADER_DIR}/libxml/xinclude.h
		${LIB_HEADER_DIR}/libxml/xlink.h
		${LIB_HEADER_DIR}/libxml/xmlautomata.h
		${LIB_HEADER_DIR}/libxml/xmlerror.h
		${LIB_HEADER_DIR}/libxml/xmlexports.h
		${LIB_HEADER_DIR}/libxml/xmlIO.h
		${LIB_HEADER_DIR}/libxml/xmlmemory.h
		${LIB_HEADER_DIR}/libxml/xmlmodule.h
		${LIB_HEADER_DIR}/libxml/xmlreader.h
		${LIB_HEADER_DIR}/libxml/xmlregexp.h
		${LIB_HEADER_DIR}/libxml/xmlsave.h
		${LIB_HEADER_DIR}/libxml/xmlschemas.h
		${LIB_HEADER_DIR}/libxml/xmlschemastypes.h
		${LIB_HEADER_DIR}/libxml/xmlstring.h
		${LIB_HEADER_DIR}/libxml/xmlunicode.h
		${LIB_HEADER_DIR}/libxml/xmlversion.h
		${LIB_HEADER_DIR}/libxml/xmlwriter.h
		${LIB_HEADER_DIR}/libxml/xpath.h
		${LIB_HEADER_DIR}/libxml/xpathInternals.h
		${LIB_HEADER_DIR}/libxml/xpointer.h
		${LIB_HEADER_DIR}/libxml/win32config.h
		${LIB_HEADER_DIR}/libxml/wsockcompat.h
    )
	set(LIB_SOURCES
		${LIB_SOURCE_DIR}/buf.c
		${LIB_SOURCE_DIR}/buf.h
		${LIB_SOURCE_DIR}/c14n.c
		${LIB_SOURCE_DIR}/catalog.c
		${LIB_SOURCE_DIR}/chvalid.c
		${LIB_SOURCE_DIR}/config.h
		${LIB_SOURCE_DIR}/debugXML.c
		${LIB_SOURCE_DIR}/dict.c
		${LIB_SOURCE_DIR}/DOCBparser.c
		${LIB_SOURCE_DIR}/elfgcchack.h
		${LIB_SOURCE_DIR}/enc.h
		${LIB_SOURCE_DIR}/encoding.c
		${LIB_SOURCE_DIR}/entities.c
		${LIB_SOURCE_DIR}/error.c
		${LIB_SOURCE_DIR}/globals.c
		${LIB_SOURCE_DIR}/hash.c
		${LIB_SOURCE_DIR}/HTMLparser.c
		${LIB_SOURCE_DIR}/HTMLtree.c
		${LIB_SOURCE_DIR}/legacy.c
		${LIB_SOURCE_DIR}/libxml.h
		${LIB_SOURCE_DIR}/list.c
		${LIB_SOURCE_DIR}/parser.c
		${LIB_SOURCE_DIR}/parserInternals.c
		${LIB_SOURCE_DIR}/pattern.c
		${LIB_SOURCE_DIR}/relaxng.c
		${LIB_SOURCE_DIR}/save.h
		${LIB_SOURCE_DIR}/SAX.c
		${LIB_SOURCE_DIR}/SAX2.c
		${LIB_SOURCE_DIR}/schematron.c
		${LIB_SOURCE_DIR}/threads.c
		${LIB_SOURCE_DIR}/timsort.h
		${LIB_SOURCE_DIR}/tree.c
		${LIB_SOURCE_DIR}/trio.c
		${LIB_SOURCE_DIR}/trio.h
		${LIB_SOURCE_DIR}/triodef.h
        #${LIB_SOURCE_DIR}/trionan.c
		${LIB_SOURCE_DIR}/trionan.h
		${LIB_SOURCE_DIR}/triop.h
		${LIB_SOURCE_DIR}/triostr.c
		${LIB_SOURCE_DIR}/triostr.h
		${LIB_SOURCE_DIR}/uri.c
		${LIB_SOURCE_DIR}/valid.c
		${LIB_SOURCE_DIR}/xinclude.c
		${LIB_SOURCE_DIR}/xlink.c
        #${LIB_SOURCE_DIR}/xmlcatalog.c
		${LIB_SOURCE_DIR}/xmlIO.c
        # ${LIB_SOURCE_DIR}/xmllint.c
		${LIB_SOURCE_DIR}/xmlmemory.c
		${LIB_SOURCE_DIR}/xmlmodule.c
		${LIB_SOURCE_DIR}/xmlreader.c
		${LIB_SOURCE_DIR}/xmlregexp.c
		${LIB_SOURCE_DIR}/xmlsave.c
		${LIB_SOURCE_DIR}/xmlschemas.c
		${LIB_SOURCE_DIR}/xmlschemastypes.c
		${LIB_SOURCE_DIR}/xmlstring.c
		${LIB_SOURCE_DIR}/xmlunicode.c
		${LIB_SOURCE_DIR}/xmlwriter.c
		${LIB_SOURCE_DIR}/xpath.c
		${LIB_SOURCE_DIR}/xpointer.c
		${LIB_SOURCE_DIR}/xzlib.c
		${LIB_SOURCE_DIR}/xzlib.h

        ${LIB_SOURCE_DIR}/nanoftp.c
        ${LIB_SOURCE_DIR}/nanohttp.c
		# ${LIB_SOURCE_DIR}/runsuite.c
		# ${LIB_SOURCE_DIR}/runtest.c
		# ${LIB_SOURCE_DIR}/runxmlconf.c
		# ${LIB_SOURCE_DIR}/testapi.c
		# ${LIB_SOURCE_DIR}/testAutomata.c
		# ${LIB_SOURCE_DIR}/testC14N.c
		# ${LIB_SOURCE_DIR}/testchar.c
		# ${LIB_SOURCE_DIR}/testdict.c
		# ${LIB_SOURCE_DIR}/testdso.c
		# ${LIB_SOURCE_DIR}/testHTML.c
		# ${LIB_SOURCE_DIR}/testlimits.c
		# ${LIB_SOURCE_DIR}/testModule.c
		# ${LIB_SOURCE_DIR}/testReader.c
		# ${LIB_SOURCE_DIR}/testrecurse.c
		# ${LIB_SOURCE_DIR}/testRegexp.c
		# ${LIB_SOURCE_DIR}/testRelax.c
		# ${LIB_SOURCE_DIR}/testSAX.c
		# ${LIB_SOURCE_DIR}/testSchemas.c
		# ${LIB_SOURCE_DIR}/testThreads.c
		# ${LIB_SOURCE_DIR}/testURI.c
		# ${LIB_SOURCE_DIR}/testXPath.c
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/libxml)

	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBXML2=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBXML_XMLREADER_H=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBXML_XMLVERSION_H=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBXML_XMLWRITER_H=1)

    if (WIN32)
        target_link_libraries(${LIB_NAME} PRIVATE ws2_32) # nanoftp/nanohttp
        target_link_libraries(${LIB_NAME} PRIVATE shlwapi) # for PathRemoveFileSpecW
        target_link_libraries(${LIB_NAME} PRIVATE dbghelp) # for SymInitialize, SymFromAddr
    endif()

    # if (MSVC)
	# else()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-variable)
    # endif()
    
    target_link_libraries(${LIB_NAME} PRIVATE de_zlib)
    target_link_libraries(${LIB_NAME} PRIVATE de_bzip2)
    target_link_libraries(${LIB_NAME} PRIVATE de_xz_static)
    target_link_libraries(${LIB_NAME} PRIVATE de_iconv)

endif()
