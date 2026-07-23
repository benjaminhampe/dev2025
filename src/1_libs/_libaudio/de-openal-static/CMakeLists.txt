set(LIB_NAME de_openal)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)
    set(LIB_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/bin)

    set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/AL/al.h
        ${LIB_HEADER_DIR}/AL/alc.h
        ${LIB_HEADER_DIR}/AL/alext.h
        ${LIB_HEADER_DIR}/AL/efx.h
        ${LIB_HEADER_DIR}/AL/efx-creative.h
        ${LIB_HEADER_DIR}/AL/efx-presets.h
    )
    #set(LIB_SOURCES ${LIB_SOURCE_DIR}/de_sfml.cpp)

    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")

    add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS})

    target_link_directories(${LIB_NAME} INTERFACE ${LIB_LIBRARY_DIR})

    target_link_libraries(${LIB_NAME} INTERFACE ${LIB_LIBRARY_DIR}/libopenal32.a)

    target_include_directories(${LIB_NAME} INTERFACE ${LIB_HEADER_DIR})

    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_OPENAL=1)
 
    file(COPY ${LIB_BINARY_DIR}/openal32.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

endif()
