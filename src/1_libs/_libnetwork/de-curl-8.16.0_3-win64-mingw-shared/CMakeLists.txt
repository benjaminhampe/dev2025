set(LIB_NAME de_curl_shared)

project(${LIB_NAME}-8.16.0_3)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    
    set(LIB_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    
    message("${BoldCyan}[benni] DLL: ${LIB_NAME}${ColourReset}")
    add_library(${LIB_NAME} SHARED IMPORTED GLOBAL)

    # target_sources(${LIB_NAME} INTERFACE "${LIB_HEADERS}")

    # target_include_directories(${LIB_NAME} INTERFACE "${LIB_HEADER_DIR}")

    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBCURL=1)

    set_target_properties(${LIB_NAME} PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/bin/libcurl-x64.dll"   # Runtime DLL
        IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/libcurl.dll.a"       # Import library
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include"   # Optional headers
        # IMPORTED_CONFIGURATIONS "Release"
    )

    # set(BIN_DIR ${CMAKE_CURRENT_LIST_DIR}/bin)
    # set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_LOCATION "${BIN_DIR}/audioadapter.dll")
    # set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_LOCATION "${BIN_DIR}/jack_dummy.dll")

    # set_property(TARGET ${LIB_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${LIB_HEADER_DIR}")

    # set(LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib64)
    # set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_IMPLIB "${LIB_DIR}/libcrypto.a")
    # set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_IMPLIB "${LIB_DIR}/libssl.a")

    # target_link_libraries(${LIB_NAME} INTERFACE
	# "${LIB_DIR}/libcrypto.a"
        # "${LIB_DIR}/libssl.a"
    # )

    # if (WIN32)
        # target_link_libraries(${LIB_NAME} INTERFACE ws2_32.a crypt32.a)
    # endif()

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libnetwork)
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
	# file(COPY "${LIB_ROOT_DIR}/bin/jack_dummy.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    
	file(COPY "${CMAKE_CURRENT_LIST_DIR}/bin/libcurl-x64.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    # file(COPY "${CMAKE_CURRENT_LIST_DIR}/bin/libcurl-x64.dll" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()
