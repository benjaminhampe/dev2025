set(LIB_NAME de_r8brain)
project(${LIB_NAME}-7.1)

if (TARGET ${LIB_NAME})
	message("${BoldRed}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    message("${BoldYellow}[benni] IFL: ${LIB_NAME}${ColourReset}")

    set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include") 
    set(LIB_HEADERS         
        ${LIB_INCLUDE_DIR}/r8b_convenience.h
        ${LIB_INCLUDE_DIR}/CDSPBlockConvolver.h
        ${LIB_INCLUDE_DIR}/CDSPFIRFilter.h
        ${LIB_INCLUDE_DIR}/CDSPFracInterpolator.h
        ${LIB_INCLUDE_DIR}/CDSPHBDownsampler.h
        ${LIB_INCLUDE_DIR}/CDSPHBDownsampler.inc
        ${LIB_INCLUDE_DIR}/CDSPHBUpsampler.h
        ${LIB_INCLUDE_DIR}/CDSPHBUpsampler.inc
        ${LIB_INCLUDE_DIR}/CDSPProcessor.h
        ${LIB_INCLUDE_DIR}/CDSPRealFFT.h
        ${LIB_INCLUDE_DIR}/CDSPResampler.h
        ${LIB_INCLUDE_DIR}/CDSPSincFilterGen.h

        ${LIB_INCLUDE_DIR}/r8bbase.h
        ${LIB_INCLUDE_DIR}/r8bconf.h
        ${LIB_INCLUDE_DIR}/r8butil.h
        
        ${LIB_INCLUDE_DIR}/fft/fft4g.h        
    )

    add_library(${LIB_NAME} INTERFACE ${LIB_HEADERS})

    target_include_directories(${LIB_NAME} INTERFACE ${LIB_INCLUDE_DIR})
   
    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_R8BRAIN=1)
    target_compile_definitions(${LIB_NAME} INTERFACE HAVE_LIBR8BRAIN=1)
    
    target_link_libraries(${LIB_NAME} INTERFACE de_pffftd) # double
endif()

add_subdirectory(test)