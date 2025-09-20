set(LIB_NAME de_rack2_filesystem)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")

    set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include") 
    file(GLOB_RECURSE LIB_HEADERS "${LIB_INCLUDE_DIR}/*.hpp")

    add_library(${LIB_NAME} INTERFACE IMPORTED GLOBAL ${LIB_HEADERS})

    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBFILESYSTEM=1)
    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR})
   
endif()
