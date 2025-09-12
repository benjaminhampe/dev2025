set(LIB_NAME de_xml2) # cmake target name
project(${LIB_NAME}-2.9.10)

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(XML2_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include") 
	# file(GLOB_RECURSE XML2_HEADERS "${XML2_INCLUDE_DIR}/libxml/*.h")
	# file(GLOB_RECURSE XML2_SOURCES "${XML2_INCLUDE_DIR}/libxml/*.cpp")

	message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
   
	add_library(${LIB_NAME} INTERFACE)

	target_include_directories(${LIB_NAME} INTERFACE ${XML2_INCLUDE_DIR})

	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBXML2=1)
	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBXML_XMLREADER_H=1)
	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBXML_XMLVERSION_H=1)
	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBXML_XMLWRITER_H=1)

   # target_compile_definitions(${LIB_NAME} INTERFACE -DBENNI_WITH_GLM=1)  # Used as #define in .cpp source code.
   # target_compile_definitions(${LIB_NAME} INTERFACE -DDE_WITH_GLM=1)     # Used as #define in .cpp source code.
   # target_compile_definitions(${LIB_NAME} INTERFACE -DUSE_LIB_GLM=1)     # Used as #define in .cpp source code.

  
endif()
