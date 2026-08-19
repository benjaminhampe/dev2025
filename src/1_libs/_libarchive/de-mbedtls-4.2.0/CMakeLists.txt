set(LIB_NAME de_mbedtls_420)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
        
    set(LIB_SOURCES_COMMON
        ${LIB_SOURCE_DIR}/common.h
    )
    
    set(LIB_HEADERS_EVEREST
        ${LIB_HEADER_DIR}/everest/everest.h
        ${LIB_HEADER_DIR}/everest/Hacl_Curve25519.h
        ${LIB_HEADER_DIR}/everest/x25519.h
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
    )
    set(LIB_SOURCES_EVEREST
        ${LIB_SOURCE_DIR}/everest/everest.c
        ${LIB_SOURCE_DIR}/everest/x25519.c
        ${LIB_SOURCE_DIR}/everest/Hacl_Curve25519_joined.c
    )
    
    set(LIB_HEADERS_P256M
        ${LIB_HEADER_DIR}/p256-m/p256-m.h
        ${LIB_HEADER_DIR}/p256-m/p256-m_driver_entrypoints.h
    )
    set(LIB_SOURCES_P256M
        ${LIB_SOURCE_DIR}/p256-m/p256-m.c
        ${LIB_SOURCE_DIR}/p256-m/p256-m_driver_entrypoints.c
    )
    
    set(LIB_HEADERS_X509
        ${LIB_HEADER_DIR}/mbedtls/pkcs7.h
        ${LIB_HEADER_DIR}/mbedtls/x509.h
        ${LIB_HEADER_DIR}/mbedtls/x509_crl.h
        ${LIB_HEADER_DIR}/mbedtls/x509_crt.h
        ${LIB_HEADER_DIR}/mbedtls/x509_csr.h
    )
    set(LIB_SOURCES_X509
        ${LIB_SOURCE_DIR}/x509/pkcs7.c
        ${LIB_SOURCE_DIR}/x509/x509_internal.h        
        ${LIB_SOURCE_DIR}/x509/x509.c
        ${LIB_SOURCE_DIR}/x509/x509_create.c
        ${LIB_SOURCE_DIR}/x509/x509_crl.c
        ${LIB_SOURCE_DIR}/x509/x509_crt.c
        ${LIB_SOURCE_DIR}/x509/x509_csr.c
        ${LIB_SOURCE_DIR}/x509/x509write.c
        ${LIB_SOURCE_DIR}/x509/x509write_crt.c
        ${LIB_SOURCE_DIR}/x509/x509write_csr.c
    )
    
    set(LIB_HEADERS_TLS
        ${LIB_HEADER_DIR}/mbedtls/debug.h
        ${LIB_HEADER_DIR}/mbedtls/net_sockets.h
        ${LIB_HEADER_DIR}/mbedtls/ssl.h
        ${LIB_HEADER_DIR}/mbedtls/ssl_cache.h
        ${LIB_HEADER_DIR}/mbedtls/ssl_ciphersuites.h
        ${LIB_HEADER_DIR}/mbedtls/ssl_cookie.h
        ${LIB_HEADER_DIR}/mbedtls/ssl_ticket.h
    )
    
    set(LIB_SOURCES_TLS
        ${LIB_SOURCE_DIR}/tls/debug.c
        ${LIB_SOURCE_DIR}/tls/debug_internal.h
        ${LIB_SOURCE_DIR}/tls/mps_common.h
        ${LIB_SOURCE_DIR}/tls/mps_error.h
        ${LIB_SOURCE_DIR}/tls/mps_reader.c
        ${LIB_SOURCE_DIR}/tls/mps_reader.h
        ${LIB_SOURCE_DIR}/tls/mps_trace.c
        ${LIB_SOURCE_DIR}/tls/mps_trace.h
        ${LIB_SOURCE_DIR}/tls/net_sockets.c
        ${LIB_SOURCE_DIR}/tls/ssl_cache.c
        ${LIB_SOURCE_DIR}/tls/ssl_ciphersuites.c
        ${LIB_SOURCE_DIR}/tls/ssl_ciphersuites_internal.h
        ${LIB_SOURCE_DIR}/tls/ssl_client.c
        ${LIB_SOURCE_DIR}/tls/ssl_client.h
        ${LIB_SOURCE_DIR}/tls/ssl_cookie.c
        ${LIB_SOURCE_DIR}/tls/ssl_debug_helpers.h
        ${LIB_SOURCE_DIR}/tls/ssl_debug_helpers_generated.c
        ${LIB_SOURCE_DIR}/tls/ssl_misc.h
        ${LIB_SOURCE_DIR}/tls/ssl_msg.c
        ${LIB_SOURCE_DIR}/tls/ssl_ticket.c
        ${LIB_SOURCE_DIR}/tls/ssl_tls.c
        ${LIB_SOURCE_DIR}/tls/ssl_tls12_client.c
        ${LIB_SOURCE_DIR}/tls/ssl_tls12_server.c
        ${LIB_SOURCE_DIR}/tls/ssl_tls13_client.c
        ${LIB_SOURCE_DIR}/tls/ssl_tls13_generic.c
        ${LIB_SOURCE_DIR}/tls/ssl_tls13_invasive.h
        ${LIB_SOURCE_DIR}/tls/ssl_tls13_keys.c
        ${LIB_SOURCE_DIR}/tls/ssl_tls13_keys.h
        ${LIB_SOURCE_DIR}/tls/ssl_tls13_server.c        
    )
    
    set(LIB_HEADERS_CRYPTO
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
        ${LIB_HEADER_DIR}/mbedtls/nist_kw.h
        ${LIB_HEADER_DIR}/mbedtls/oid.h
        ${LIB_HEADER_DIR}/mbedtls/pem.h
        ${LIB_HEADER_DIR}/mbedtls/pk.h
        ${LIB_HEADER_DIR}/mbedtls/pkcs12.h
        ${LIB_HEADER_DIR}/mbedtls/pkcs5.h
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
        ${LIB_HEADER_DIR}/mbedtls/threading.h
        ${LIB_HEADER_DIR}/mbedtls/timing.h
        ${LIB_HEADER_DIR}/mbedtls/version.h

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
    set(LIB_SOURCES_CRYPTO
        ${LIB_SOURCE_DIR}/crypto/aes.c
        ${LIB_SOURCE_DIR}/crypto/aesce.c
        ${LIB_SOURCE_DIR}/crypto/aesce.h
        ${LIB_SOURCE_DIR}/crypto/aesni.c
        ${LIB_SOURCE_DIR}/crypto/aesni.h
        ${LIB_SOURCE_DIR}/crypto/alignment.h
        ${LIB_SOURCE_DIR}/crypto/aria.c
        ${LIB_SOURCE_DIR}/crypto/asn1parse.c
        ${LIB_SOURCE_DIR}/crypto/asn1write.c
        ${LIB_SOURCE_DIR}/crypto/base64.c
        ${LIB_SOURCE_DIR}/crypto/base64_internal.h
        ${LIB_SOURCE_DIR}/crypto/bignum.c
        ${LIB_SOURCE_DIR}/crypto/bignum_core.c
        ${LIB_SOURCE_DIR}/crypto/bignum_core.h
        ${LIB_SOURCE_DIR}/crypto/bignum_core_invasive.h
        ${LIB_SOURCE_DIR}/crypto/bignum_internal.h
        ${LIB_SOURCE_DIR}/crypto/bignum_mod.c
        ${LIB_SOURCE_DIR}/crypto/bignum_mod.h
        ${LIB_SOURCE_DIR}/crypto/bignum_mod_raw.c
        ${LIB_SOURCE_DIR}/crypto/bignum_mod_raw.h
        ${LIB_SOURCE_DIR}/crypto/bignum_mod_raw_invasive.h
        ${LIB_SOURCE_DIR}/crypto/block_cipher.c
        ${LIB_SOURCE_DIR}/crypto/block_cipher_internal.h
        ${LIB_SOURCE_DIR}/crypto/bn_mul.h
        ${LIB_SOURCE_DIR}/crypto/camellia.c
        ${LIB_SOURCE_DIR}/crypto/ccm.c
        ${LIB_SOURCE_DIR}/crypto/chacha20.c
        ${LIB_SOURCE_DIR}/crypto/chachapoly.c
        ${LIB_SOURCE_DIR}/crypto/check_crypto_config.h
        ${LIB_SOURCE_DIR}/crypto/cipher.c
        ${LIB_SOURCE_DIR}/crypto/cipher_invasive.h
        ${LIB_SOURCE_DIR}/crypto/cipher_wrap.c
        ${LIB_SOURCE_DIR}/crypto/cipher_wrap.h
        ${LIB_SOURCE_DIR}/crypto/cmac.c
        # ${LIB_SOURCE_DIR}/crypto/common.h
        ${LIB_SOURCE_DIR}/crypto/constant_time.c
        ${LIB_SOURCE_DIR}/crypto/constant_time_impl.h
        ${LIB_SOURCE_DIR}/crypto/constant_time_internal.h
        ${LIB_SOURCE_DIR}/crypto/ctr.h
        ${LIB_SOURCE_DIR}/crypto/ctr_drbg.c
        ${LIB_SOURCE_DIR}/crypto/des.c
        ${LIB_SOURCE_DIR}/crypto/dhm.c
        ${LIB_SOURCE_DIR}/crypto/ecdh.c
        ${LIB_SOURCE_DIR}/crypto/ecdsa.c
        ${LIB_SOURCE_DIR}/crypto/ecjpake.c
        ${LIB_SOURCE_DIR}/crypto/ecp.c
        ${LIB_SOURCE_DIR}/crypto/ecp_curves.c
        ${LIB_SOURCE_DIR}/crypto/ecp_curves_new.c
        ${LIB_SOURCE_DIR}/crypto/ecp_internal_alt.h
        ${LIB_SOURCE_DIR}/crypto/ecp_invasive.h
        ${LIB_SOURCE_DIR}/crypto/entropy.c
        ${LIB_SOURCE_DIR}/crypto/entropy_poll.c
        ${LIB_SOURCE_DIR}/crypto/entropy_poll.h
        ${LIB_SOURCE_DIR}/crypto/error.c
        ${LIB_SOURCE_DIR}/crypto/gcm.c
        ${LIB_SOURCE_DIR}/crypto/hkdf.c
        ${LIB_SOURCE_DIR}/crypto/hmac_drbg.c
        ${LIB_SOURCE_DIR}/crypto/lmots.c
        ${LIB_SOURCE_DIR}/crypto/lmots.h
        ${LIB_SOURCE_DIR}/crypto/lms.c
        ${LIB_SOURCE_DIR}/crypto/md.c
        ${LIB_SOURCE_DIR}/crypto/md5.c
        ${LIB_SOURCE_DIR}/crypto/md_psa.h
        ${LIB_SOURCE_DIR}/crypto/md_wrap.h
        ${LIB_SOURCE_DIR}/crypto/memory_buffer_alloc.c
        ${LIB_SOURCE_DIR}/crypto/nist_kw.c
        ${LIB_SOURCE_DIR}/crypto/oid.c
        ${LIB_SOURCE_DIR}/crypto/padlock.c
        ${LIB_SOURCE_DIR}/crypto/padlock.h
        ${LIB_SOURCE_DIR}/crypto/pem.c
        ${LIB_SOURCE_DIR}/crypto/pk.c
        ${LIB_SOURCE_DIR}/crypto/pkcs12.c
        ${LIB_SOURCE_DIR}/crypto/pkcs5.c
        ${LIB_SOURCE_DIR}/crypto/pkparse.c
        ${LIB_SOURCE_DIR}/crypto/pkwrite.c
        ${LIB_SOURCE_DIR}/crypto/pkwrite.h
        ${LIB_SOURCE_DIR}/crypto/pk_ecc.c
        ${LIB_SOURCE_DIR}/crypto/pk_internal.h
        ${LIB_SOURCE_DIR}/crypto/pk_wrap.c
        ${LIB_SOURCE_DIR}/crypto/pk_wrap.h
        ${LIB_SOURCE_DIR}/crypto/platform.c
        ${LIB_SOURCE_DIR}/crypto/platform_util.c
        ${LIB_SOURCE_DIR}/crypto/poly1305.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_aead.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_aead.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_cipher.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_cipher.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_client.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_core.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_core_common.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_driver_wrappers.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_driver_wrappers_no_static.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_driver_wrappers_no_static.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_ecp.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_ecp.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_ffdh.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_ffdh.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_hash.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_hash.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_invasive.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_its.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_mac.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_mac.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_pake.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_pake.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_random.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_random.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_random_impl.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_rsa.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_rsa.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_se.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_se.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_slot_management.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_slot_management.h
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_storage.c
        ${LIB_SOURCE_DIR}/crypto/psa_crypto_storage.h
        ${LIB_SOURCE_DIR}/crypto/psa_its_file.c
        ${LIB_SOURCE_DIR}/crypto/psa_util.c
        ${LIB_SOURCE_DIR}/crypto/psa_util_internal.h
        ${LIB_SOURCE_DIR}/crypto/ripemd160.c
        ${LIB_SOURCE_DIR}/crypto/rsa.c
        ${LIB_SOURCE_DIR}/crypto/rsa_alt_helpers.c
        ${LIB_SOURCE_DIR}/crypto/rsa_alt_helpers.h
        ${LIB_SOURCE_DIR}/crypto/rsa_internal.h
        ${LIB_SOURCE_DIR}/crypto/sha1.c
        ${LIB_SOURCE_DIR}/crypto/sha256.c
        ${LIB_SOURCE_DIR}/crypto/sha3.c
        ${LIB_SOURCE_DIR}/crypto/sha512.c
        ${LIB_SOURCE_DIR}/crypto/threading.c
        ${LIB_SOURCE_DIR}/crypto/threading_internal.h
        ${LIB_SOURCE_DIR}/crypto/timing.c
        ${LIB_SOURCE_DIR}/crypto/version.c
        ${LIB_SOURCE_DIR}/crypto/version_features.c
    )

    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        #target_compile_definitions(${LIB_NAME} PUBLIC -DZLIB_DLL=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} 
        PUBLIC 
        ${LIB_HEADERS_EVEREST}
        ${LIB_HEADERS_P256M}
        ${LIB_HEADERS_X509}
        ${LIB_HEADERS_TLS}
        ${LIB_HEADERS_CRYPTO}
        PRIVATE 
        ${LIB_SOURCES_COMMON}
        ${LIB_SOURCES_EVEREST}
        ${LIB_SOURCES_P256M}
        ${LIB_SOURCES_X509}
        ${LIB_SOURCES_TLS}
        ${LIB_SOURCES_CRYPTO}
    )

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/everest)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/p256-m)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/mbedtls)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/psa)

    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/everest)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/p256-m)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/crypto)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/x509)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/tls)    
    # target_link_libraries(${LIB_NAME} PUBLIC de_mbedtls_420_everest)
    # target_link_libraries(${LIB_NAME} PUBLIC de_mbedtls_420_p256-m)


    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBMBEDCRYPTO=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_MBEDTLS=1)

    target_compile_definitions(${LIB_NAME} PUBLIC MBEDTLS_THREADING_C=1)
    target_compile_definitions(${LIB_NAME} PUBLIC MBEDTLS_THREADING_PTHREAD=1)

    # Define to 1 if you have the 'SHA256Init' function.
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SHA256INIT=1)
    # Define to 1 if the system has the type 'SHA256_CTX'.
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SHA256_CTX=1)
    # Define to 1 if you have the <sha256.h> header file.
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SHA256_H=1)
    # Define to 1 if you have the 'SHA256_Init' function.
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SHA256_INIT=1)
    # Define to 1 if the system has the type 'SHA2_CTX'.
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SHA2_CTX=1)
    # Define to 1 if you have the <sha2.h> header file.
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SHA2_H=1)

    if (WIN32)
        target_link_libraries(${LIB_NAME} PUBLIC ws2_32 bcrypt)
        target_link_libraries(${LIB_NAME} PUBLIC dnsapi)
        # target_link_libraries(${LIB_NAME} PUBLIC msvcrt) # vsnprintf_s
        # # target_link_libraries(${LIB_NAME} PUBLIC ucrt) # vsnprintf_s        
    endif()

    # install(TARGETS enet
        # RUNTIME DESTINATION bin
        # ARCHIVE DESTINATION lib/static
        # LIBRARY DESTINATION lib)

    # install(DIRECTORY include/
            # DESTINATION include)

endif()
