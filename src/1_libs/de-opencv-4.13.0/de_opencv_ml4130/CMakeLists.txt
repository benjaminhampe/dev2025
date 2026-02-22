set(LIB_NAME de_opencv_ml4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/ml/ml.hpp
        ${LIB_HEADER_DIR}/opencv2/ml/ml.inl.hpp
        ${LIB_HEADER_DIR}/opencv2/ml.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/ann_mlp.cpp
        ${LIB_SOURCE_DIR}/boost.cpp
        ${LIB_SOURCE_DIR}/data.cpp
        ${LIB_SOURCE_DIR}/em.cpp
        ${LIB_SOURCE_DIR}/gbt.cpp
        ${LIB_SOURCE_DIR}/inner_functions.cpp
        ${LIB_SOURCE_DIR}/kdtree.cpp
        ${LIB_SOURCE_DIR}/kdtree.hpp
        ${LIB_SOURCE_DIR}/knearest.cpp
        ${LIB_SOURCE_DIR}/lr.cpp
        ${LIB_SOURCE_DIR}/nbayes.cpp
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/rtrees.cpp
        ${LIB_SOURCE_DIR}/svm.cpp
        ${LIB_SOURCE_DIR}/svmsgd.cpp
        ${LIB_SOURCE_DIR}/testset.cpp
        ${LIB_SOURCE_DIR}/tree.cpp
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
