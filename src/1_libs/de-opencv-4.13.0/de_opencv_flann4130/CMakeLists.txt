set(LIB_NAME de_opencv_flann4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/flann/allocator.h
        ${LIB_HEADER_DIR}/opencv2/flann/all_indices.h
        ${LIB_HEADER_DIR}/opencv2/flann/any.h
        ${LIB_HEADER_DIR}/opencv2/flann/autotuned_index.h
        ${LIB_HEADER_DIR}/opencv2/flann/composite_index.h
        ${LIB_HEADER_DIR}/opencv2/flann/config.h
        ${LIB_HEADER_DIR}/opencv2/flann/defines.h
        ${LIB_HEADER_DIR}/opencv2/flann/dist.h
        ${LIB_HEADER_DIR}/opencv2/flann/dummy.h
        ${LIB_HEADER_DIR}/opencv2/flann/dynamic_bitset.h
        ${LIB_HEADER_DIR}/opencv2/flann/flann.hpp
        ${LIB_HEADER_DIR}/opencv2/flann/flann_base.hpp
        ${LIB_HEADER_DIR}/opencv2/flann/general.h
        ${LIB_HEADER_DIR}/opencv2/flann/ground_truth.h
        ${LIB_HEADER_DIR}/opencv2/flann/heap.h
        ${LIB_HEADER_DIR}/opencv2/flann/hierarchical_clustering_index.h
        ${LIB_HEADER_DIR}/opencv2/flann/index_testing.h
        ${LIB_HEADER_DIR}/opencv2/flann/kdtree_index.h
        ${LIB_HEADER_DIR}/opencv2/flann/kdtree_single_index.h
        ${LIB_HEADER_DIR}/opencv2/flann/kmeans_index.h
        ${LIB_HEADER_DIR}/opencv2/flann/linear_index.h
        ${LIB_HEADER_DIR}/opencv2/flann/logger.h
        ${LIB_HEADER_DIR}/opencv2/flann/lsh_index.h
        ${LIB_HEADER_DIR}/opencv2/flann/lsh_table.h
        ${LIB_HEADER_DIR}/opencv2/flann/matrix.h
        ${LIB_HEADER_DIR}/opencv2/flann/miniflann.hpp
        ${LIB_HEADER_DIR}/opencv2/flann/nn_index.h
        ${LIB_HEADER_DIR}/opencv2/flann/object_factory.h
        ${LIB_HEADER_DIR}/opencv2/flann/params.h
        ${LIB_HEADER_DIR}/opencv2/flann/random.h
        ${LIB_HEADER_DIR}/opencv2/flann/result_set.h
        ${LIB_HEADER_DIR}/opencv2/flann/sampling.h
        ${LIB_HEADER_DIR}/opencv2/flann/saving.h
        ${LIB_HEADER_DIR}/opencv2/flann/simplex_downhill.h
        ${LIB_HEADER_DIR}/opencv2/flann/timer.h
        ${LIB_HEADER_DIR}/opencv2/flann.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/flann.cpp
        ${LIB_SOURCE_DIR}/miniflann.cpp
        ${LIB_SOURCE_DIR}/precomp.hpp
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

endif()

# add_subdirectory(test)
