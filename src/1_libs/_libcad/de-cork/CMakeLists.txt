set(LIB_NAME de_cork)
project(${LIB_NAME}-dev CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
    else()
    add_subdirectory(lib)

    set(CORK_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/cork)
    set(CORK_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    # file(GLOB_RECURSE CORK_HEADERS "${CORK_HEADER_DIR}/*.[c,h]pp")
    # file(GLOB_RECURSE CORK_SOURCES "${CORK_SOURCE_DIR}/*.[c,h]pp")

    set(CORK_HEADERS 
        ${CORK_HEADER_DIR}/cork.h
        ${CORK_HEADER_DIR}/vec.h
        ${CORK_HEADER_DIR}/prelude.h
        ${CORK_HEADER_DIR}/rawMesh.h
        ${CORK_HEADER_DIR}/rawMesh.tpp
        ${CORK_HEADER_DIR}/files.h)
    set(CORK_SOURCES 
        ${CORK_SOURCE_DIR}/aabvh.h
        ${CORK_SOURCE_DIR}/absext4.h
        ${CORK_SOURCE_DIR}/bbox.h
        ${CORK_SOURCE_DIR}/cork.cpp
        ${CORK_SOURCE_DIR}/empty3d.cpp
        ${CORK_SOURCE_DIR}/empty3d.h
        ${CORK_SOURCE_DIR}/ext4.h
        ${CORK_SOURCE_DIR}/files.cpp
        ${CORK_SOURCE_DIR}/fixext4.h
        ${CORK_SOURCE_DIR}/fixint.h
        ${CORK_SOURCE_DIR}/gmpext4.h
        ${CORK_SOURCE_DIR}/ifs.cpp
        ${CORK_SOURCE_DIR}/iterPool.h
        ${CORK_SOURCE_DIR}/log.cpp
        ${CORK_SOURCE_DIR}/memPool.h
        ${CORK_SOURCE_DIR}/mesh.bool.tpp
        ${CORK_SOURCE_DIR}/mesh.decl.h
        ${CORK_SOURCE_DIR}/mesh.h
        ${CORK_SOURCE_DIR}/mesh.isct.tpp
        ${CORK_SOURCE_DIR}/mesh.remesh.tpp
        ${CORK_SOURCE_DIR}/mesh.topoCache.tpp
        ${CORK_SOURCE_DIR}/mesh.tpp
        ${CORK_SOURCE_DIR}/off.cpp
        # ${CORK_SOURCE_DIR}/off2obj.cpp # app
        ${CORK_SOURCE_DIR}/quantization.cpp
        ${CORK_SOURCE_DIR}/quantization.h
        ${CORK_SOURCE_DIR}/ray.h
        ${CORK_SOURCE_DIR}/shortVec.h
        ${CORK_SOURCE_DIR}/timer.cpp
        #${CORK_SOURCE_DIR}/triangle.c # is app if -DTRILIBRARY=1 is not defined
        #${CORK_SOURCE_DIR}/triangle.h # app-interface
        ${CORK_SOURCE_DIR}/unionFind.h
        ${CORK_SOURCE_DIR}/unsafeRayTriIsct.h
    )

    if (BUILD_SHARED)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_include_directories(${LIB_NAME} PUBLIC ${CORK_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${CORK_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${CORK_SOURCE_DIR})
    target_sources(${LIB_NAME} PUBLIC ${CORK_HEADERS} PRIVATE ${CORK_SOURCES})

    target_compile_definitions(${LIB_NAME} PRIVATE TRILIBRARY=1)
    
    if (MSVC)
    else()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-pointer-to-int-cast)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-int-to-pointer-cast)
        target_link_libraries(${LIB_NAME} PUBLIC m)
    endif()
    target_link_libraries(${LIB_NAME} PUBLIC de_cork_triangle)
    target_link_libraries(${LIB_NAME} PUBLIC de_gmp_static_612)
endif()

add_subdirectory(test)
