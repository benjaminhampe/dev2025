set(LIB_NAME de_cork_triangle)
project(${LIB_NAME}-dev C)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS ${LIB_HEADER_DIR}/triangle.h)
    set(LIB_SOURCES ${LIB_SOURCE_DIR}/triangle.c) # is app if -DTRILIBRARY=1 is not defined
    # file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.[c,h]pp")
    # file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.[c,h]pp")

    message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    add_library(${LIB_NAME} STATIC "")

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PRIVATE TRILIBRARY=1)
    target_compile_definitions(${LIB_NAME} PRIVATE NO_TIMER=1)
    target_compile_definitions(${LIB_NAME} PRIVATE REDUCED=1)
    target_compile_definitions(${LIB_NAME} PRIVATE ANSI_DECLARATORS=1)
    target_compile_definitions(${LIB_NAME} PRIVATE CDT_ONLY=1)

    if (MSVC)
    else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-int-to-pointer-cast)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-pointer-to-int-cast)
        target_link_libraries(${LIB_NAME} PUBLIC m)
    endif()
endif()

# add_subdirectory(test)
