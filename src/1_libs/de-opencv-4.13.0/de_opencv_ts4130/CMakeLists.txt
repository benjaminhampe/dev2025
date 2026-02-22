set(LIB_NAME de_opencv_ts4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/ts/cuda_perf.hpp
        ${LIB_HEADER_DIR}/opencv2/ts/cuda_test.hpp
        ${LIB_HEADER_DIR}/opencv2/ts/ocl_perf.hpp
        ${LIB_HEADER_DIR}/opencv2/ts/ocl_test.hpp
        ${LIB_HEADER_DIR}/opencv2/ts/ts_ext.hpp
        ${LIB_HEADER_DIR}/opencv2/ts/ts_gtest.h
        ${LIB_HEADER_DIR}/opencv2/ts/ts_perf.hpp
        ${LIB_HEADER_DIR}/opencv2/ts.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/cuda_perf.cpp
        ${LIB_SOURCE_DIR}/cuda_test.cpp
        ${LIB_SOURCE_DIR}/ocl_perf.cpp
        ${LIB_SOURCE_DIR}/ocl_test.cpp
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/ts.cpp
        ${LIB_SOURCE_DIR}/ts_arrtest.cpp
        ${LIB_SOURCE_DIR}/ts_func.cpp
        ${LIB_SOURCE_DIR}/ts_gtest.cpp
        ${LIB_SOURCE_DIR}/ts_perf.cpp
        ${LIB_SOURCE_DIR}/ts_tags.cpp
        ${LIB_SOURCE_DIR}/ts_tags.hpp
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/_benni)

    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_MINIGMP=1)
    target_compile_definitions(${LIB_NAME} PRIVATE __OPENCV_BUILD=1)

    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
    # target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
    # target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)

    # check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

    # target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)

	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_core4130)
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_imgproc4130)
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_imgcodecs4130)
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_videoio4130)
	target_link_libraries(${LIB_NAME} PUBLIC de_opencv_highgui4130)

endif()

# add_subdirectory(test)
