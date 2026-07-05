set(LIB_NAME de_poly2tri_2026)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/poly2tri/poly2tri.h
        ${LIB_HEADER_DIR}/poly2tri/common/dll_symbol.h
        ${LIB_HEADER_DIR}/poly2tri/common/shapes.h
        ${LIB_HEADER_DIR}/poly2tri/common/utils.h
        ${LIB_HEADER_DIR}/poly2tri/sweep/advancing_front.h
        ${LIB_HEADER_DIR}/poly2tri/sweep/cdt.h
        ${LIB_HEADER_DIR}/poly2tri/sweep/sweep.h
        ${LIB_HEADER_DIR}/poly2tri/sweep/sweep_context.h
    )

    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/common/shapes.cc
        ${LIB_SOURCE_DIR}/sweep/advancing_front.cc
        ${LIB_SOURCE_DIR}/sweep/cdt.cc
        ${LIB_SOURCE_DIR}/sweep/sweep.cc
        ${LIB_SOURCE_DIR}/sweep/sweep_context.cc
    )

    # if (BUILD_SHARED_LIBS)
        # message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # add_library(${LIB_NAME} SHARED "")
    # else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    # endif()

    target_compile_definitions(${LIB_NAME} PRIVATE P2T_STATIC_EXPORTS=1)

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})

    # file(COPY ${LIB_HEADERS} DESTINATION ${LIB_INCLUDE_DIR}) # For the SDK

endif()
