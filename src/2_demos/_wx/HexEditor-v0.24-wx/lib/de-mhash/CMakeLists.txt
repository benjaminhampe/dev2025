set(LIB_NAME de_mhash)
project(lib-${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)
    
    set(MHASH_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(MHASH_HEADERS 
        ${MHASH_HEADER_DIR}/mhash.h
        ${MHASH_HEADER_DIR}/mutils/mhash_config.h
        ${MHASH_HEADER_DIR}/mutils/mincludes.h
        ${MHASH_HEADER_DIR}/mutils/mglobal.h
        ${MHASH_HEADER_DIR}/mutils/mtypes.h
        ${MHASH_HEADER_DIR}/mutils/mutils.h
        ${MHASH_HEADER_DIR}/mutils/mhash.h
    )

    # if (WIN32)
    # list(APPEND MHASH_HEADERS "${MHASH_HEADERS_HPP}")      
    # endif()

    # file(GLOB_RECURSE MHASH_HEADERS "${MHASH_HEADER_DIR}/*.h")   
    # list(APPEND MHASH_HEADERS "${MHASH_HEADERS_HPP}")   

    set(MHASH_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(MHASH_SOURCES 
        ${MHASH_SOURCE_DIR}/adler32.c
        ${MHASH_SOURCE_DIR}/crc32.c
        ${MHASH_SOURCE_DIR}/gosthash.c
        ${MHASH_SOURCE_DIR}/haval.c
        ${MHASH_SOURCE_DIR}/keygen.c
        ${MHASH_SOURCE_DIR}/keygen.h
        ${MHASH_SOURCE_DIR}/keygen_asis.c
        ${MHASH_SOURCE_DIR}/keygen_hex.c
        ${MHASH_SOURCE_DIR}/keygen_mcrypt.c
        ${MHASH_SOURCE_DIR}/keygen_s2k.c
        ${MHASH_SOURCE_DIR}/libdefs.h
        ${MHASH_SOURCE_DIR}/md2.c
        ${MHASH_SOURCE_DIR}/md4.c
        ${MHASH_SOURCE_DIR}/md5.c
        ${MHASH_SOURCE_DIR}/mhash.c
        ${MHASH_SOURCE_DIR}/mhash_adler32.h
        ${MHASH_SOURCE_DIR}/mhash_crc32.h
        ${MHASH_SOURCE_DIR}/mhash_gost.h
        ${MHASH_SOURCE_DIR}/mhash_haval.h
        ${MHASH_SOURCE_DIR}/mhash_int.h
        ${MHASH_SOURCE_DIR}/mhash_md2.h
        ${MHASH_SOURCE_DIR}/mhash_md4.h
        ${MHASH_SOURCE_DIR}/mhash_md5.h
        ${MHASH_SOURCE_DIR}/mhash_ripemd.h
        ${MHASH_SOURCE_DIR}/mhash_sha1.h
        ${MHASH_SOURCE_DIR}/mhash_sha256.h
        ${MHASH_SOURCE_DIR}/mhash_sha256_sha224.h
        ${MHASH_SOURCE_DIR}/mhash_sha512_sha384.h
        ${MHASH_SOURCE_DIR}/mhash_snefru.h
        ${MHASH_SOURCE_DIR}/mhash_tiger.h
        ${MHASH_SOURCE_DIR}/mhash_whirlpool.h
        ${MHASH_SOURCE_DIR}/ripemd.c
        ${MHASH_SOURCE_DIR}/sha1.c
        ${MHASH_SOURCE_DIR}/sha256.c
        ${MHASH_SOURCE_DIR}/sha256_sha224.c
        ${MHASH_SOURCE_DIR}/sha512_sha384.c
        ${MHASH_SOURCE_DIR}/snefru.c
        ${MHASH_SOURCE_DIR}/stdfns.c
        ${MHASH_SOURCE_DIR}/tiger.c
        ${MHASH_SOURCE_DIR}/tiger_sboxes.c
        ${MHASH_SOURCE_DIR}/whirlpool.c   
    )
    # file(GLOB_RECURSE MHASH_HEADERS_HPP "${MHASH_HEADER_DIR}/*.hpp")

    # file(GLOB_RECURSE MHASH_SOURCES "${MHASH_SOURCE_DIR}/*.cpp")
    # set(MHASH_SOURCES 
    #    ${MHASH_SOURCE_DIR}/lzma/LzmaDec.c )

    # add_library(${LIB_NAME} IMPORT)
    # message("${BoldBlue}[benni] SHARED_LIB: ${LIB_NAME}${ColourReset}")   

    # if (BUILD_SHARED)
    # add_library(${LIB_NAME} SHARED "")
    # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    # else()
    add_library(${LIB_NAME} STATIC "")
    message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    # endif()

    # target_compile_definitions(${LIB_NAME} PUBLIC -DUSE_IRRLICHT=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -D_MHASH_STATIC_LIB_=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -D_UNICODE=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC -DUNICODE=1)
    # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)
    # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-function)
    # target_compile_options(${LIB_NAME} PRIVATE -Wno-maybe-uninitialized)
    # target_compile_options(${LIB_NAME} PRIVATE -Wno-strict-aliasing)
    target_include_directories(${LIB_NAME} PUBLIC ${MHASH_HEADER_DIR})
    # target_include_directories(${LIB_NAME} PUBLIC ${MHASH_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${MHASH_SOURCE_DIR})
    target_sources(${LIB_NAME} PUBLIC ${MHASH_HEADERS} PRIVATE ${MHASH_SOURCES})

    # target_link_libraries(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib/libmhash.a)

    if (MSVC)
    else()
        # target_link_libraries(${LIB_NAME} PUBLIC stdc++fs)
        # target_link_libraries(${LIB_NAME} PUBLIC pthread)
        target_link_libraries(${LIB_NAME} PUBLIC m)

    endif()

   # if(WIN32)
      # target_link_libraries(${LIB_NAME} PUBLIC glu32 opengl32
               # uuid ole32 oleaut32 ComCtl32
               # SetupAPI winmm user32 gdi32 kernel32 advapi32 shell32 )
   # else() # on Linux:
      # target_link_libraries(${LIB_NAME} PUBLIC GLU GL)
      # target_link_libraries(${LIB_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
   # endif()

endif()

# add_subdirectory(test)
