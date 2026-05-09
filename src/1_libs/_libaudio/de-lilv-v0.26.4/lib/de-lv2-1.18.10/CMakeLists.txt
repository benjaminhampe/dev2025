set(LIB_NAME de_lv2)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/lv2/parameters/parameters.h
    )

    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")
    add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS})

    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LV2=1)
    target_include_directories(${LIB_NAME} INTERFACE ${LIB_HEADER_DIR})

    # target_link_libraries(${LIB_NAME} INTERFACE de_clap)
#	file(COPY ${LIB_HEADERS} DESTINATION ${RJSON_INCLUDE_DIR}) # For the SDK
#	file(COPY ${LIB_HEADERS_INTERNAL} DESTINATION ${RJSON_INCLUDE_DIR}/internal) # For the SDK
#	file(COPY ${LIB_HEADERS_ERROR} DESTINATION ${RJSON_INCLUDE_DIR}/error) # For the SDK
	
endif()
