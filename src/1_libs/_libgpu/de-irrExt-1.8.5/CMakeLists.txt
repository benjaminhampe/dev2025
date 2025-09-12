set(LIB_NAME de_irrExt)
project(${LIB_NAME}-dev CXX)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/irrExt)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/irrExt)

    file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.cpp")

    if (BUILD_SHARED)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})
    #target_compile_definitions(${LIB_NAME} PUBLIC -D_IRR_STATIC_LIB_=1)
    if (MSVC)
    else()
    target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)
    endif()
    target_link_libraries(${LIB_NAME} PUBLIC de_irrlicht)
    target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)
    target_link_libraries(${LIB_NAME} PUBLIC de_tinyxml2)
    target_link_libraries(${LIB_NAME} PUBLIC de_glm)
    target_link_libraries(${LIB_NAME} PUBLIC de_glew)
    #target_link_libraries(${LIB_NAME} PUBLIC de_glad)


endif()
