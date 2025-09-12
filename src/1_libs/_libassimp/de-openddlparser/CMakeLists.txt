set(LIB_NAME de_openddlparser)
project(de-lib-openddlparser CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()	
	set(OPENDDL_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/openddlparser)
	set(OPENDDL_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(OPENDDL_INCLUDE_DIR ${OPENDDL_HEADER_DIR}/..)

	set(OPENDDL_HEADERS 
		${OPENDDL_HEADER_DIR}/OpenDDLParser.h
		${OPENDDL_HEADER_DIR}/OpenDDLParserUtils.h
		${OPENDDL_HEADER_DIR}/OpenDDLCommon.h
		${OPENDDL_HEADER_DIR}/OpenDDLExport.h
		${OPENDDL_HEADER_DIR}/OpenDDLStream.h
		${OPENDDL_HEADER_DIR}/DDLNode.h
		${OPENDDL_HEADER_DIR}/Value.h)
		
	set(OPENDDL_SOURCES
		${OPENDDL_SOURCE_DIR}/OpenDDLParser.cpp
		${OPENDDL_SOURCE_DIR}/DDLNode.cpp
		${OPENDDL_SOURCE_DIR}/OpenDDLCommon.cpp
		${OPENDDL_SOURCE_DIR}/OpenDDLExport.cpp
		${OPENDDL_SOURCE_DIR}/Value.cpp
		${OPENDDL_SOURCE_DIR}/OpenDDLStream.cpp)
		
	# if (BUILD_SHARED)
		# message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
		# add_library(${LIB_NAME} SHARED "")
	# else()
		message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
		add_library(${LIB_NAME} STATIC "")
	# endif()				
	target_sources(${LIB_NAME} PUBLIC ${OPENDDL_HEADERS} PRIVATE ${OPENDDL_SOURCES})
	target_include_directories(${LIB_NAME} PUBLIC ${OPENDDL_INCLUDE_DIR})
	target_include_directories(${LIB_NAME} PUBLIC ${OPENDDL_INCLUDE_DIR}/openddlparser)

	target_compile_definitions(${LIB_NAME} PRIVATE OPENDDL_STATIC_LIBARY)
	target_compile_definitions(${LIB_NAME} PRIVATE OPENDDLPARSER_BUILD)
	target_compile_definitions(${LIB_NAME} PRIVATE _VARIADIC_MAX=10)
	target_compile_definitions(${LIB_NAME} PRIVATE GTEST_HAS_PTHREAD=0)
	target_compile_definitions(${LIB_NAME} PRIVATE DDL_DEBUG_HEADER_NAME)

	# file(COPY ${OPENDDL_HEADERS} DESTINATION ${OPENDDL_INCLUDE_DIR}) # For the SDK	
endif()
