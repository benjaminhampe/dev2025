set(LIB_NAME de_xz_static)

project(${LIB_NAME}-5.8.1)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)    
	set(LIB_HEADERS 
		${LIB_HEADER_DIR}/lzma.h
		${LIB_HEADER_DIR}/lzma/base.h
		${LIB_HEADER_DIR}/lzma/bcj.h
		${LIB_HEADER_DIR}/lzma/block.h
		${LIB_HEADER_DIR}/lzma/check.h
		${LIB_HEADER_DIR}/lzma/container.h
		${LIB_HEADER_DIR}/lzma/delta.h
		${LIB_HEADER_DIR}/lzma/filter.h
		${LIB_HEADER_DIR}/lzma/hardware.h
		${LIB_HEADER_DIR}/lzma/index.h
		${LIB_HEADER_DIR}/lzma/index_hash.h
		${LIB_HEADER_DIR}/lzma/lzma12.h
		${LIB_HEADER_DIR}/lzma/stream_flags.h
		${LIB_HEADER_DIR}/lzma/version.h
		${LIB_HEADER_DIR}/lzma/vli.h
	)
    
	# if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED IMPORTED GLOBAL) # GLOBAL
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC IMPORTED GLOBAL ${LIB_HEADERS}) # GLOBAL
    # endif()

    set_target_properties(${LIB_NAME} PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/liblzma.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"
    )
	# target_sources(${LIB_NAME} INTERFACE "")

	# target_include_directories(${LIB_NAME} INTERFACE "${LIB_HEADER_DIR}")
    # set_property(TARGET ${LIB_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORY ${LIB_HEADER_DIR})
    
	# target_link_libraries(${LIB_NAME} INTERFACE "${CMAKE_CURRENT_LIST_DIR}/lib/liblzma.a")
    # set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/liblzma.a")
	
	# set(BIN_DIR ${CMAKE_CURRENT_LIST_DIR}/bin)
	# set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_LOCATION "${BIN_DIR}/audioadapter.dll")

	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBLZMA=1)
	# target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBLZMA=1)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libarchive)
    endif()

    # if (CMAKE_BUILD_TYPE STREQUAL "Release")

        # message(STATUS "Building in Release mode, strip symbols")

        # # --strip-debug : Removes only debugging symbols.
        # # --strip-unneeded : Removes debug symbols + other unused symbols (may affect debugging).
        # add_custom_command(
            # TARGET ${APP_NAME}
            # POST_BUILD
            # COMMAND ${CMAKE_OBJCOPY} --strip-unneeded $<TARGET_FILE:${APP_NAME}>
            # COMMENT "Stripping debug symbols"
        # )

    # endif()

	# file(GLOB JACK2_DLLS "${JACK2_ROOT}/bin/*.dll")
	# file(COPY ${JACK2_DLLS} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
	# file(COPY "${LIB_ROOT_DIR}/bin/audioadapter.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
	# file(COPY media/H3 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
