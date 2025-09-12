set(LIB_NAME de_OpenEXR_Imath)
project(${LIB_NAME}-dev CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/Imath)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/ImathConfig.h
		${LIB_HEADER_DIR}/half.h
		${LIB_HEADER_DIR}/halfFunction.h
		${LIB_HEADER_DIR}/halfLimits.h
		${LIB_HEADER_DIR}/ImathBox.h
		${LIB_HEADER_DIR}/ImathBoxAlgo.h
		${LIB_HEADER_DIR}/ImathColor.h
		${LIB_HEADER_DIR}/ImathColorAlgo.h
		${LIB_HEADER_DIR}/ImathEuler.h
		${LIB_HEADER_DIR}/ImathExport.h
		${LIB_HEADER_DIR}/ImathForward.h
		${LIB_HEADER_DIR}/ImathFrame.h
		${LIB_HEADER_DIR}/ImathFrustum.h
		${LIB_HEADER_DIR}/ImathFrustumTest.h
		${LIB_HEADER_DIR}/ImathFun.h
		${LIB_HEADER_DIR}/ImathGL.h
		${LIB_HEADER_DIR}/ImathGLU.h
		${LIB_HEADER_DIR}/ImathInt64.h
		${LIB_HEADER_DIR}/ImathInterval.h
		${LIB_HEADER_DIR}/ImathLine.h
		${LIB_HEADER_DIR}/ImathLineAlgo.h
		${LIB_HEADER_DIR}/ImathMath.h
		${LIB_HEADER_DIR}/ImathMatrix.h
		${LIB_HEADER_DIR}/ImathMatrixAlgo.h
		${LIB_HEADER_DIR}/ImathNamespace.h
		${LIB_HEADER_DIR}/ImathPlane.h
		${LIB_HEADER_DIR}/ImathPlatform.h
		${LIB_HEADER_DIR}/ImathQuat.h
		${LIB_HEADER_DIR}/ImathRandom.h
		${LIB_HEADER_DIR}/ImathRoots.h
		${LIB_HEADER_DIR}/ImathShear.h
		${LIB_HEADER_DIR}/ImathSphere.h
		${LIB_HEADER_DIR}/ImathTypeTraits.h
		${LIB_HEADER_DIR}/ImathVec.h
		${LIB_HEADER_DIR}/ImathVecAlgo.h		
	)

	set(LIB_SOURCES	
		${LIB_SOURCE_DIR}/half.cpp
		${LIB_SOURCE_DIR}/ImathColorAlgo.cpp
		${LIB_SOURCE_DIR}/ImathFun.cpp
		${LIB_SOURCE_DIR}/ImathMatrixAlgo.cpp
		${LIB_SOURCE_DIR}/ImathRandom.cpp
                #${LIB_SOURCE_DIR}/toFloat.cpp
                ${LIB_SOURCE_DIR}/toFloat.h
	)

    if (WIN32)
        enable_language(RC)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    endif()

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

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libimage)
    else()
		if(OPENEXR_ENABLE_THREADING)
		# target_link_libraries(${LIB_NAME} PUBLIC Threads::Threads)
		endif()
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()

	#target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
	#target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)	

    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
