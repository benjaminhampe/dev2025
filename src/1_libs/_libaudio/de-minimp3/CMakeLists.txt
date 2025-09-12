set(LIB_NAME de_minimp3)
project(${LIB_NAME}-1.12.2-dev C)

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")

    set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include") 
    set(LIB_HEADERS 
        "${LIB_INCLUDE_DIR}/minimp3.h"
        "${LIB_INCLUDE_DIR}/minimp3_ex.h"
    )

    add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS})

    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR})
   
    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_MINIMP3=1)
endif()
