set(LIB_NAME de_mbedtls_static)

project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)    
	set(LIB_HEADERS 
		${LIB_HEADER_DIR}/everest/everest.h
		${LIB_HEADER_DIR}/everest/Hacl_Curve25519.h
		${LIB_HEADER_DIR}/everest/kremlib.h
		${LIB_HEADER_DIR}/everest/kremlib/FStar_UInt128.h
		${LIB_HEADER_DIR}/everest/kremlib/FStar_UInt64_FStar_UInt32_FStar_UInt16_FStar_UInt8.h
		${LIB_HEADER_DIR}/everest/kremlin/c_endianness.h
		${LIB_HEADER_DIR}/everest/kremlin/internal/builtin.h
		${LIB_HEADER_DIR}/everest/kremlin/internal/callconv.h
		${LIB_HEADER_DIR}/everest/kremlin/internal/compat.h
		${LIB_HEADER_DIR}/everest/kremlin/internal/debug.h
		${LIB_HEADER_DIR}/everest/kremlin/internal/target.h
		${LIB_HEADER_DIR}/everest/kremlin/internal/types.h
		${LIB_HEADER_DIR}/everest/kremlin/internal/wasmsupport.h
		${LIB_HEADER_DIR}/everest/vs2013/Hacl_Curve25519.h
		${LIB_HEADER_DIR}/everest/x25519.h
		${LIB_HEADER_DIR}/mbedtls/aes.h
		${LIB_HEADER_DIR}/mbedtls/aria.h
		${LIB_HEADER_DIR}/mbedtls/asn1.h
		${LIB_HEADER_DIR}/mbedtls/asn1write.h
		${LIB_HEADER_DIR}/mbedtls/base64.h
		${LIB_HEADER_DIR}/mbedtls/bignum.h
		${LIB_HEADER_DIR}/mbedtls/block_cipher.h
		${LIB_HEADER_DIR}/mbedtls/build_info.h
		${LIB_HEADER_DIR}/mbedtls/camellia.h
		${LIB_HEADER_DIR}/mbedtls/ccm.h
		${LIB_HEADER_DIR}/mbedtls/chacha20.h
		${LIB_HEADER_DIR}/mbedtls/chachapoly.h
		${LIB_HEADER_DIR}/mbedtls/check_config.h
		${LIB_HEADER_DIR}/mbedtls/cipher.h
		${LIB_HEADER_DIR}/mbedtls/cmac.h
		${LIB_HEADER_DIR}/mbedtls/compat-2.x.h
		${LIB_HEADER_DIR}/mbedtls/config_adjust_legacy_crypto.h
		${LIB_HEADER_DIR}/mbedtls/config_adjust_legacy_from_psa.h
		${LIB_HEADER_DIR}/mbedtls/config_adjust_psa_from_legacy.h
		${LIB_HEADER_DIR}/mbedtls/config_adjust_psa_superset_legacy.h
		${LIB_HEADER_DIR}/mbedtls/config_adjust_ssl.h
		${LIB_HEADER_DIR}/mbedtls/config_adjust_x509.h
		${LIB_HEADER_DIR}/mbedtls/config_psa.h
		${LIB_HEADER_DIR}/mbedtls/constant_time.h
		${LIB_HEADER_DIR}/mbedtls/ctr_drbg.h
		${LIB_HEADER_DIR}/mbedtls/debug.h
		${LIB_HEADER_DIR}/mbedtls/des.h
		${LIB_HEADER_DIR}/mbedtls/dhm.h
		${LIB_HEADER_DIR}/mbedtls/ecdh.h
		${LIB_HEADER_DIR}/mbedtls/ecdsa.h
		${LIB_HEADER_DIR}/mbedtls/ecjpake.h
		${LIB_HEADER_DIR}/mbedtls/ecp.h
		${LIB_HEADER_DIR}/mbedtls/entropy.h
		${LIB_HEADER_DIR}/mbedtls/error.h
		${LIB_HEADER_DIR}/mbedtls/gcm.h
		${LIB_HEADER_DIR}/mbedtls/hkdf.h
		${LIB_HEADER_DIR}/mbedtls/hmac_drbg.h
		${LIB_HEADER_DIR}/mbedtls/lms.h
		${LIB_HEADER_DIR}/mbedtls/mbedtls_config.h
		${LIB_HEADER_DIR}/mbedtls/md.h
		${LIB_HEADER_DIR}/mbedtls/md5.h
		${LIB_HEADER_DIR}/mbedtls/memory_buffer_alloc.h
		${LIB_HEADER_DIR}/mbedtls/net_sockets.h
		${LIB_HEADER_DIR}/mbedtls/nist_kw.h
		${LIB_HEADER_DIR}/mbedtls/oid.h
		${LIB_HEADER_DIR}/mbedtls/pem.h
		${LIB_HEADER_DIR}/mbedtls/pk.h
		${LIB_HEADER_DIR}/mbedtls/pkcs12.h
		${LIB_HEADER_DIR}/mbedtls/pkcs5.h
		${LIB_HEADER_DIR}/mbedtls/pkcs7.h
		${LIB_HEADER_DIR}/mbedtls/platform.h
		${LIB_HEADER_DIR}/mbedtls/platform_time.h
		${LIB_HEADER_DIR}/mbedtls/platform_util.h
		${LIB_HEADER_DIR}/mbedtls/poly1305.h
		${LIB_HEADER_DIR}/mbedtls/private_access.h
		${LIB_HEADER_DIR}/mbedtls/psa_util.h
		${LIB_HEADER_DIR}/mbedtls/ripemd160.h
		${LIB_HEADER_DIR}/mbedtls/rsa.h
		${LIB_HEADER_DIR}/mbedtls/sha1.h
		${LIB_HEADER_DIR}/mbedtls/sha256.h
		${LIB_HEADER_DIR}/mbedtls/sha3.h
		${LIB_HEADER_DIR}/mbedtls/sha512.h
		${LIB_HEADER_DIR}/mbedtls/ssl.h
		${LIB_HEADER_DIR}/mbedtls/ssl_cache.h
		${LIB_HEADER_DIR}/mbedtls/ssl_ciphersuites.h
		${LIB_HEADER_DIR}/mbedtls/ssl_cookie.h
		${LIB_HEADER_DIR}/mbedtls/ssl_ticket.h
		${LIB_HEADER_DIR}/mbedtls/threading.h
		${LIB_HEADER_DIR}/mbedtls/timing.h
		${LIB_HEADER_DIR}/mbedtls/version.h
		${LIB_HEADER_DIR}/mbedtls/x509.h
		${LIB_HEADER_DIR}/mbedtls/x509_crl.h
		${LIB_HEADER_DIR}/mbedtls/x509_crt.h
		${LIB_HEADER_DIR}/mbedtls/x509_csr.h
		${LIB_HEADER_DIR}/psa/build_info.h
		${LIB_HEADER_DIR}/psa/crypto.h
		${LIB_HEADER_DIR}/psa/crypto_adjust_auto_enabled.h
		${LIB_HEADER_DIR}/psa/crypto_adjust_config_dependencies.h
		${LIB_HEADER_DIR}/psa/crypto_adjust_config_key_pair_types.h
		${LIB_HEADER_DIR}/psa/crypto_adjust_config_synonyms.h
		${LIB_HEADER_DIR}/psa/crypto_builtin_composites.h
		${LIB_HEADER_DIR}/psa/crypto_builtin_key_derivation.h
		${LIB_HEADER_DIR}/psa/crypto_builtin_primitives.h
		${LIB_HEADER_DIR}/psa/crypto_compat.h
		${LIB_HEADER_DIR}/psa/crypto_config.h
		${LIB_HEADER_DIR}/psa/crypto_driver_common.h
		${LIB_HEADER_DIR}/psa/crypto_driver_contexts_composites.h
		${LIB_HEADER_DIR}/psa/crypto_driver_contexts_key_derivation.h
		${LIB_HEADER_DIR}/psa/crypto_driver_contexts_primitives.h
		${LIB_HEADER_DIR}/psa/crypto_extra.h
		${LIB_HEADER_DIR}/psa/crypto_legacy.h
		${LIB_HEADER_DIR}/psa/crypto_platform.h
		${LIB_HEADER_DIR}/psa/crypto_se_driver.h
		${LIB_HEADER_DIR}/psa/crypto_sizes.h
		${LIB_HEADER_DIR}/psa/crypto_struct.h
		${LIB_HEADER_DIR}/psa/crypto_types.h
		${LIB_HEADER_DIR}/psa/crypto_values.h
	)
    
	# if (BUILD_SHARED)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED IMPORTED GLOBAL) # GLOBAL
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC IMPORTED GLOBAL) # GLOBAL
    # endif()

	target_sources(${LIB_NAME} INTERFACE "${LIB_HEADERS}")

	target_include_directories(${LIB_NAME} INTERFACE "${LIB_HEADER_DIR}")
	
	set_property(TARGET ${LIB_NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${LIB_HEADER_DIR}")
	
	set(LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)
	set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_IMPLIB "${LIB_DIR}/libeverest.a")
	set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_IMPLIB "${LIB_DIR}/libmbedcrypto.a")
	set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_IMPLIB "${LIB_DIR}/libmbedtls.a")
	set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_IMPLIB "${LIB_DIR}/libmbedx509.a")
	set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_IMPLIB "${LIB_DIR}/libp256m.a")

	# set(BIN_DIR ${CMAKE_CURRENT_LIST_DIR}/bin)
	# set_property(TARGET ${LIB_NAME} PROPERTY IMPORTED_LOCATION "${BIN_DIR}/audioadapter.dll")

	target_link_libraries(${LIB_NAME} INTERFACE 
		"${LIB_DIR}/libeverest.a"
		"${LIB_DIR}/libmbedcrypto.a"
		"${LIB_DIR}/libmbedtls.a"
		"${LIB_DIR}/libmbedx509.a"
		"${LIB_DIR}/libp256m.a"
	)
	
	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBMBEDCRYPTO=1)
    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_MBEDTLS=1)

	# Define to 1 if you have the 'SHA256Init' function.
    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_SHA256INIT=1)
	# Define to 1 if the system has the type 'SHA256_CTX'.
	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_SHA256_CTX=1)
	# Define to 1 if you have the <sha256.h> header file.
    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_SHA256_H=1)
	# Define to 1 if you have the 'SHA256_Init' function.
	target_compile_definitions(${LIB_NAME} INTERFACE HAVE_SHA256_INIT=1)
	# Define to 1 if the system has the type 'SHA2_CTX'.
    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_SHA2_CTX=1)		
	# Define to 1 if you have the <sha2.h> header file.
    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_SHA2_H=1)

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
