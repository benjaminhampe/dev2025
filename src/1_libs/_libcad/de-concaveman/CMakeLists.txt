set(LIB_NAME de_concaveman)
project(${LIB_NAME}-dev CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(CONCAVEMAN_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(CONCAVEMAN_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE CONCAVEMAN_HEADERS "${CONCAVEMAN_HEADER_DIR}/*.[c,h]pp")
    # file(GLOB_RECURSE CONCAVEMAN_SOURCES "${CONCAVEMAN_SOURCE_DIR}/*.[c,h]pp")

    set(CONCAVEMAN_HEADERS ${CONCAVEMAN_HEADER_DIR}/concaveman.h)
    set(CONCAVEMAN_SOURCES ${CONCAVEMAN_SOURCE_DIR}/concaveman.cpp)

    if (BUILD_SHARED)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_include_directories(${LIB_NAME} PUBLIC ${CONCAVEMAN_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${CONCAVEMAN_SOURCE_DIR})
    target_sources(${LIB_NAME} PUBLIC ${CONCAVEMAN_HEADERS} PRIVATE ${CONCAVEMAN_SOURCES})

    if (MSVC)
    else()
        target_link_libraries(${LIB_NAME} PUBLIC m)
    endif()
endif()

add_subdirectory(test)
