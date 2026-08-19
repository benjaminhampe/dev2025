set(LIB_NAME de_sfml_extlibs)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")

    set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
    file(GLOB_RECURSE LIB_HEADERS "${LIB_INCLUDE_DIR}/*.hpp")

    add_library(${LIB_NAME} INTERFACE IMPORTED GLOBAL ${LIB_HEADERS})

    # target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBGHCFILESYSTEM=1)
    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR})
    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR}/cpp-unicodelib)
    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR}/glad-4.6/include)
    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR}/vulkan)
    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR}/qoi)
    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR}/stb_image)
    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR}/wepoll)

endif()
