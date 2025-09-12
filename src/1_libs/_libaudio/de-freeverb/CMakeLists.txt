set(LIB_NAME de_freeverb)

project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/freeverb/allpass.hpp
        ${LIB_HEADER_DIR}/freeverb/comb.hpp
        ${LIB_HEADER_DIR}/freeverb/denormals.h
        ${LIB_HEADER_DIR}/freeverb/revmodel.hpp
        ${LIB_HEADER_DIR}/freeverb/tuning.h
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/allpass.cpp
        ${LIB_SOURCE_DIR}/comb.cpp
        ${LIB_SOURCE_DIR}/revmodel.cpp
    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/freeverb)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_FREEVERB=1)

    # target_link_libraries(${LIB_NAME} PUBLIC de_kissfft)

    if (MSVC)
	else()
        target_link_libraries(${LIB_NAME} PUBLIC m)
        # target_link_libraries(${LIB_NAME} PUBLIC pthread)
    endif()
endif()

# add_subdirectory(test)
