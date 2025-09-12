set(LIB_NAME de_openssl_static)

project(${LIB_NAME}-3.5.2)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    
    set(LIB_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    # set(LIB_HEADERS "${LIB_HEADER_DIR}/jack/control.h")
    
    message("${BoldCyan}[benni] DLL: ${LIB_NAME}${ColourReset}")
    add_library(${LIB_NAME} SHARED IMPORTED GLOBAL) # GLOBAL

    target_sources(${LIB_NAME} INTERFACE "${LIB_HEADERS}")

    target_include_directories(${LIB_NAME} INTERFACE "${LIB_HEADER_DIR}")

    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_OPENSSL=1)

    # set(BIN_DIR ${CMAKE_CURRENT_LIST_DIR}/bin)
    # set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_LOCATION "${BIN_DIR}/audioadapter.dll")
    # set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_LOCATION "${BIN_DIR}/jack_dummy.dll")

    set_property(TARGET ${LIB_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${LIB_HEADER_DIR}")

    set(LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib64)
    set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_IMPLIB "${LIB_DIR}/libcrypto.a")
    set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_IMPLIB "${LIB_DIR}/libssl.a")

    target_link_libraries(${LIB_NAME} INTERFACE
	"${LIB_DIR}/libcrypto.a"
        "${LIB_DIR}/libssl.a"
    )

    if (WIN32)
        target_link_libraries(${LIB_NAME} INTERFACE ws2_32.a crypt32.a)
    endif()

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
    
	# file(COPY media/H3 DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/media)
endif()
