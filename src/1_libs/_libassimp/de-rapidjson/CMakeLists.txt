set(LIB_NAME de_rapidjson)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(RJSON_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/rapidjson)
	set(RJSON_HEADERS 
		${RJSON_HEADER_DIR}/allocators.h
		${RJSON_HEADER_DIR}/document.h
		${RJSON_HEADER_DIR}/encodedstream.h
		${RJSON_HEADER_DIR}/encodings.h
		${RJSON_HEADER_DIR}/filereadstream.h
		${RJSON_HEADER_DIR}/filewritestream.h
		${RJSON_HEADER_DIR}/fwd.h
		${RJSON_HEADER_DIR}/istreamwrapper.h
		${RJSON_HEADER_DIR}/memorybuffer.h
		${RJSON_HEADER_DIR}/memorystream.h
		${RJSON_HEADER_DIR}/ostreamwrapper.h
		${RJSON_HEADER_DIR}/pointer.h
		${RJSON_HEADER_DIR}/prettywriter.h
		${RJSON_HEADER_DIR}/rapidjson.h
		${RJSON_HEADER_DIR}/reader.h
		${RJSON_HEADER_DIR}/schema.h
		${RJSON_HEADER_DIR}/stream.h
		${RJSON_HEADER_DIR}/stringbuffer.h
		${RJSON_HEADER_DIR}/writer.h)
	set(RJSON_HEADERS_INTERNAL 		
		${RJSON_HEADER_DIR}/internal/biginteger.h
		${RJSON_HEADER_DIR}/internal/diyfp.h
		${RJSON_HEADER_DIR}/internal/dtoa.h
		${RJSON_HEADER_DIR}/internal/ieee754.h
		${RJSON_HEADER_DIR}/internal/itoa.h
		${RJSON_HEADER_DIR}/internal/meta.h
		${RJSON_HEADER_DIR}/internal/pow10.h
		${RJSON_HEADER_DIR}/internal/regex.h
		${RJSON_HEADER_DIR}/internal/stack.h
		${RJSON_HEADER_DIR}/internal/strfunc.h
		${RJSON_HEADER_DIR}/internal/strtod.h
		${RJSON_HEADER_DIR}/internal/swap.h)
	set(RJSON_HEADERS_ERROR
		${RJSON_HEADER_DIR}/error/en.h
		${RJSON_HEADER_DIR}/error/error.h
	)
	
	message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
	add_library(${LIB_NAME} INTERFACE)
	
	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_RAPIDJSON=1)
 	target_include_directories(${LIB_NAME} INTERFACE ${RJSON_HEADER_DIR}/..)

#	file(COPY ${RJSON_HEADERS} DESTINATION ${RJSON_INCLUDE_DIR}) # For the SDK
#	file(COPY ${RJSON_HEADERS_INTERNAL} DESTINATION ${RJSON_INCLUDE_DIR}/internal) # For the SDK
#	file(COPY ${RJSON_HEADERS_ERROR} DESTINATION ${RJSON_INCLUDE_DIR}/error) # For the SDK
	
endif()
