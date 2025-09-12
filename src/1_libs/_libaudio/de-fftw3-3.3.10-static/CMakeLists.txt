set(LIB_NAME de_fftw3)

project(${LIB_NAME}-3.3.10-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS ${LIB_HEADER_DIR}/fftw3.h
                    ${LIB_HEADER_DIR}/de_fftw3.h)
    set(LIB_SOURCES ${LIB_SOURCE_DIR}/de_fftw3.cpp)
    set(LIB_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)
    set(LIB_BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/bin)

    # if (BUILD_SHARED)
    #     message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    #     # add_library(${LIB_NAME} SHARED "")
    #     add_library(${LIB_NAME} INTERFACE)
    #     target_compile_definitions(${LIB_NAME} INTERFACE FFTW_DLL=1)
    #     target_link_directories(${LIB_NAME} INTERFACE ${LIB_LIBRARY_DIR})
    #     target_link_libraries(${LIB_NAME} INTERFACE ${LIB_LIBRARY_DIR}/libfftw3.dll.a)
    #     file(COPY ${LIB_BINARY_DIR}/libfftw3-3.dll DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    # else()
    #     message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    #     # add_library(${LIB_NAME} STATIC "")
    #     add_library(${LIB_NAME} INTERFACE)
    #     #target_compile_definitions(${LIB_NAME} PUBLIC -DFLAC__NO_DLL=1)
    #     target_compile_definitions(${LIB_NAME} INTERFACE FFTW_DLL=1)
    #     target_link_directories(${LIB_NAME} INTERFACE ${LIB_LIBRARY_DIR})
    #     target_link_libraries(${LIB_NAME} INTERFACE ${LIB_LIBRARY_DIR}/libfftw3.a)
    # endif()

    message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    add_library(${LIB_NAME} STATIC "")
    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_link_libraries(${LIB_NAME} PUBLIC ${LIB_LIBRARY_DIR}/libfftw3.a)
    target_link_libraries(${LIB_NAME} PUBLIC ${LIB_LIBRARY_DIR}/libfftw3_omp.a)
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    #   endif()

    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_FFTWF=1)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER karate/lib/audio)
    endif()

    # target_sources(${LIB_NAME} INTERFACE ${LIB_HEADERS})

    # target_include_directories(${LIB_NAME} INTERFACE ${LIB_HEADER_DIR})

    # target_compile_definitions(${LIB_NAME} INTERFACE HAVE_FFTW=1)

    # if (MSVC)
    #     set_target_properties(${LIB_NAME} PROPERTIES FOLDER lib/audio)
    # endif()

endif()
