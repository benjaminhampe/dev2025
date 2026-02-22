set(LIB_NAME de_opencv_imgcodecs4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/imgcodecs/imgcodecs.hpp
        ${LIB_HEADER_DIR}/opencv2/imgcodecs/imgcodecs_c.h
        ${LIB_HEADER_DIR}/opencv2/imgcodecs/ios.h
        ${LIB_HEADER_DIR}/opencv2/imgcodecs/legacy
        ${LIB_HEADER_DIR}/opencv2/imgcodecs/legacy/constants_c.h
        ${LIB_HEADER_DIR}/opencv2/imgcodecs/macosx.h
        ${LIB_HEADER_DIR}/opencv2/imgcodecs.hpp
    )
    set(LIB_SOURCES
        # ${LIB_SOURCE_DIR}/apple_conversions.h
        # ${LIB_SOURCE_DIR}/apple_conversions.mm
        ${LIB_SOURCE_DIR}/bitstrm.cpp
        ${LIB_SOURCE_DIR}/bitstrm.hpp
        ${LIB_SOURCE_DIR}/exif.cpp
        ${LIB_SOURCE_DIR}/exif.hpp
        ${LIB_SOURCE_DIR}/grfmts.hpp
        ${LIB_SOURCE_DIR}/grfmt_avif.cpp
        ${LIB_SOURCE_DIR}/grfmt_avif.hpp
        ${LIB_SOURCE_DIR}/grfmt_base.cpp
        ${LIB_SOURCE_DIR}/grfmt_base.hpp
        ${LIB_SOURCE_DIR}/grfmt_bmp.cpp
        ${LIB_SOURCE_DIR}/grfmt_bmp.hpp
        ${LIB_SOURCE_DIR}/grfmt_exr.cpp
        ${LIB_SOURCE_DIR}/grfmt_exr.hpp
        ${LIB_SOURCE_DIR}/grfmt_gdal.cpp
        ${LIB_SOURCE_DIR}/grfmt_gdal.hpp
        ${LIB_SOURCE_DIR}/grfmt_gdcm.cpp
        ${LIB_SOURCE_DIR}/grfmt_gdcm.hpp
        ${LIB_SOURCE_DIR}/grfmt_gif.cpp
        ${LIB_SOURCE_DIR}/grfmt_gif.hpp
        ${LIB_SOURCE_DIR}/grfmt_hdr.cpp
        ${LIB_SOURCE_DIR}/grfmt_hdr.hpp
        ${LIB_SOURCE_DIR}/grfmt_jpeg.cpp
        ${LIB_SOURCE_DIR}/grfmt_jpeg.hpp
        ${LIB_SOURCE_DIR}/grfmt_jpeg2000.cpp
        ${LIB_SOURCE_DIR}/grfmt_jpeg2000.hpp
        ${LIB_SOURCE_DIR}/grfmt_jpeg2000_openjpeg.cpp
        ${LIB_SOURCE_DIR}/grfmt_jpeg2000_openjpeg.hpp
        ${LIB_SOURCE_DIR}/grfmt_jpegxl.cpp
        ${LIB_SOURCE_DIR}/grfmt_jpegxl.hpp
        ${LIB_SOURCE_DIR}/grfmt_pam.cpp
        ${LIB_SOURCE_DIR}/grfmt_pam.hpp
        ${LIB_SOURCE_DIR}/grfmt_pfm.cpp
        ${LIB_SOURCE_DIR}/grfmt_pfm.hpp
        ${LIB_SOURCE_DIR}/grfmt_png.cpp
        ${LIB_SOURCE_DIR}/grfmt_png.hpp
        ${LIB_SOURCE_DIR}/grfmt_pxm.cpp
        ${LIB_SOURCE_DIR}/grfmt_pxm.hpp
        ${LIB_SOURCE_DIR}/grfmt_spng.cpp
        ${LIB_SOURCE_DIR}/grfmt_spng.hpp
        ${LIB_SOURCE_DIR}/grfmt_sunras.cpp
        ${LIB_SOURCE_DIR}/grfmt_sunras.hpp
        ${LIB_SOURCE_DIR}/grfmt_tiff.cpp
        ${LIB_SOURCE_DIR}/grfmt_tiff.hpp
        ${LIB_SOURCE_DIR}/grfmt_webp.cpp
        ${LIB_SOURCE_DIR}/grfmt_webp.hpp
        # ${LIB_SOURCE_DIR}/ios_conversions.mm
        ${LIB_SOURCE_DIR}/loadsave.cpp
        # ${LIB_SOURCE_DIR}/macosx_conversions.mm
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/rgbe.cpp
        ${LIB_SOURCE_DIR}/rgbe.hpp
        ${LIB_SOURCE_DIR}/utils.cpp
        ${LIB_SOURCE_DIR}/utils.hpp
    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        # target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/_benni)

    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_MINIGMP=1)
    target_compile_definitions(${LIB_NAME} PRIVATE __OPENCV_BUILD=1)
    target_compile_definitions(${LIB_NAME} PRIVATE HAVE_OPENJPEG=1)
    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
    # target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
    # target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)

    # check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

    # target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)

    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_core4130)
    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_imgproc4130)
    target_link_libraries(${LIB_NAME} PUBLIC de_jpeg) # 9f
    target_link_libraries(${LIB_NAME} PUBLIC de_png)
    target_link_libraries(${LIB_NAME} PUBLIC de_spng)
    target_link_libraries(${LIB_NAME} PUBLIC de_tiff)
    target_link_libraries(${LIB_NAME} PUBLIC de_openexr)
    target_link_libraries(${LIB_NAME} PUBLIC de_openjpeg)


endif()

# add_subdirectory(test)
