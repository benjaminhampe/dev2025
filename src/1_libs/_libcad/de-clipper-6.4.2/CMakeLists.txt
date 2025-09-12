set(LIB_NAME de_clipper_642) # We dont like to clash with system libs.
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS ${LIB_HEADER_DIR}/ClipperLib/clipper.hpp)
    set(LIB_SOURCES ${LIB_SOURCE_DIR}/clipper.cpp)

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})

    # file(COPY ${LIB_HEADERS} DESTINATION ${LIB_INCLUDE_DIR}) # For the SDK
endif()
