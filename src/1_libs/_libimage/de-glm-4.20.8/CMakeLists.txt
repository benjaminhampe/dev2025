# Benni Khronos glm library:
set(LIB_NAME de_glm)
project(${LIB_NAME}-dev)

if (TARGET ${LIB_NAME})
    message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    message("${BoldYellow}[benni] IFL: ${LIB_NAME} - Khronos OpenGL Math, header only.${ColourReset}")

    set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include") 
    file(GLOB_RECURSE GLM_HEADERS "${GLM_INCLUDE_DIR}/*.hpp")

    add_library(${LIB_NAME} INTERFACE IMPORTED GLOBAL ${GLM_HEADERS})

    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_GLM=1)     # Used as #define in .cpp source code.
    target_include_directories(${LIB_NAME} INTERFACE ${GLM_INCLUDE_DIR})
   
endif()
