set(LIB_NAME de_rapidjson)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/rapidjson)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/allocators.h
        ${LIB_HEADER_DIR}/document.h
        ${LIB_HEADER_DIR}/encodedstream.h
        ${LIB_HEADER_DIR}/encodings.h
        ${LIB_HEADER_DIR}/filereadstream.h
        ${LIB_HEADER_DIR}/filewritestream.h
        ${LIB_HEADER_DIR}/fwd.h
        ${LIB_HEADER_DIR}/istreamwrapper.h
        ${LIB_HEADER_DIR}/memorybuffer.h
        ${LIB_HEADER_DIR}/memorystream.h
        ${LIB_HEADER_DIR}/ostreamwrapper.h
        ${LIB_HEADER_DIR}/pointer.h
        ${LIB_HEADER_DIR}/prettywriter.h
        ${LIB_HEADER_DIR}/rapidjson.h
        ${LIB_HEADER_DIR}/reader.h
        ${LIB_HEADER_DIR}/schema.h
        ${LIB_HEADER_DIR}/stream.h
        ${LIB_HEADER_DIR}/stringbuffer.h
        ${LIB_HEADER_DIR}/writer.h
        ${LIB_HEADER_DIR}/internal/biginteger.h
        ${LIB_HEADER_DIR}/internal/diyfp.h
        ${LIB_HEADER_DIR}/internal/dtoa.h
        ${LIB_HEADER_DIR}/internal/ieee754.h
        ${LIB_HEADER_DIR}/internal/itoa.h
        ${LIB_HEADER_DIR}/internal/meta.h
        ${LIB_HEADER_DIR}/internal/pow10.h
        ${LIB_HEADER_DIR}/internal/regex.h
        ${LIB_HEADER_DIR}/internal/stack.h
        ${LIB_HEADER_DIR}/internal/strfunc.h
        ${LIB_HEADER_DIR}/internal/strtod.h
        ${LIB_HEADER_DIR}/internal/swap.h
        ${LIB_HEADER_DIR}/error/en.h
        ${LIB_HEADER_DIR}/error/error.h
    )

    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
    add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS})

    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_RAPIDJSON=1)
    target_include_directories(${LIB_NAME} INTERFACE ${LIB_HEADER_DIR}/..)

#	file(COPY ${LIB_HEADERS} DESTINATION ${RJSON_INCLUDE_DIR}) # For the SDK
#	file(COPY ${LIB_HEADERS_INTERNAL} DESTINATION ${RJSON_INCLUDE_DIR}/internal) # For the SDK
#	file(COPY ${LIB_HEADERS_ERROR} DESTINATION ${RJSON_INCLUDE_DIR}/error) # For the SDK
	
endif()
