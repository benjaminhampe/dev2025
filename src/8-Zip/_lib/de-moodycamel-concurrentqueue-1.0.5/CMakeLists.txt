set(LIB_NAME de_moodycamel_concurrentqueue_105)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")

    set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
    file(GLOB_RECURSE LIB_HEADERS "${LIB_INCLUDE_DIR}/*.h")

    add_library(${LIB_NAME} INTERFACE IMPORTED GLOBAL ${LIB_HEADERS})

    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_MOODYCAMEL_CONCURRENTQUEUE=1)
    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBMOODYCAMEL_CONCURRENTQUEUE=1)
    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR})

    target_link_libraries(${LIB_NAME} INTERFACE pthread)

    # target_compile_definitions(${LIB_NAME} INTERFACE MOODYCAMEL_QUEUE_INTERNAL_DEBUG=1)
    # target_compile_definitions(${LIB_NAME} INTERFACE USE_WINTHREAD=1)
    # target_compile_definitions(${LIB_NAME} INTERFACE MOODYCAMEL_STATIC=1)
    
endif()
