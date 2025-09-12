set(LIB_NAME de_open3dgc) # We dont like to clash with system libs.
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(OPEN3DGC_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/open3dgc)
	set(OPEN3DGC_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(OPEN3DGC_INCLUDE_DIR ${OPEN3DGC_HEADER_DIR}/..)
	
	set(OPEN3DGC_HEADERS 
		${OPEN3DGC_HEADER_DIR}/o3dgcAdjacencyInfo.h
		${OPEN3DGC_HEADER_DIR}/o3dgcArithmeticCodec.h
		${OPEN3DGC_HEADER_DIR}/o3dgcBinaryStream.h
		${OPEN3DGC_HEADER_DIR}/o3dgcCommon.h
		${OPEN3DGC_HEADER_DIR}/o3dgcDVEncodeParams.h
		${OPEN3DGC_HEADER_DIR}/o3dgcDynamicVectorDecoder.h
		${OPEN3DGC_HEADER_DIR}/o3dgcDynamicVectorEncoder.h
		${OPEN3DGC_HEADER_DIR}/o3dgcDynamicVector.h
		${OPEN3DGC_HEADER_DIR}/o3dgcFIFO.h
		${OPEN3DGC_HEADER_DIR}/o3dgcIndexedFaceSet.h
		${OPEN3DGC_HEADER_DIR}/o3dgcIndexedFaceSet.inl
		${OPEN3DGC_HEADER_DIR}/o3dgcSC3DMCDecoder.h
		${OPEN3DGC_HEADER_DIR}/o3dgcSC3DMCDecoder.inl
		${OPEN3DGC_HEADER_DIR}/o3dgcSC3DMCEncodeParams.h
		${OPEN3DGC_HEADER_DIR}/o3dgcSC3DMCEncoder.h
		${OPEN3DGC_HEADER_DIR}/o3dgcSC3DMCEncoder.inl
		${OPEN3DGC_HEADER_DIR}/o3dgcTimer.h
		${OPEN3DGC_HEADER_DIR}/o3dgcTriangleFans.h
		${OPEN3DGC_HEADER_DIR}/o3dgcTriangleListDecoder.h
		${OPEN3DGC_HEADER_DIR}/o3dgcTriangleListDecoder.inl
		${OPEN3DGC_HEADER_DIR}/o3dgcTriangleListEncoder.h
		${OPEN3DGC_HEADER_DIR}/o3dgcTriangleListEncoder.inl
		${OPEN3DGC_HEADER_DIR}/o3dgcVector.h
		${OPEN3DGC_HEADER_DIR}/o3dgcVector.inl)
		
	set(OPEN3DGC_SOURCES 
		${OPEN3DGC_SOURCE_DIR}/o3dgcArithmeticCodec.cpp
		${OPEN3DGC_SOURCE_DIR}/o3dgcDynamicVectorDecoder.cpp
		${OPEN3DGC_SOURCE_DIR}/o3dgcDynamicVectorEncoder.cpp
		${OPEN3DGC_SOURCE_DIR}/o3dgcTools.cpp
		${OPEN3DGC_SOURCE_DIR}/o3dgcTriangleFans.cpp)

	# if (BUILD_SHARED)
		# message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
		# add_library(${LIB_NAME} SHARED "")
	# else()
		message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
		add_library(${LIB_NAME} STATIC "")
	# endif()
	target_sources(${LIB_NAME} PUBLIC ${OPEN3DGC_HEADERS} PRIVATE ${OPEN3DGC_SOURCES})
	target_include_directories(${LIB_NAME} PUBLIC ${OPEN3DGC_INCLUDE_DIR}) # For the SDK
	target_include_directories(${LIB_NAME} PRIVATE ${OPEN3DGC_HEADER_DIR}) # For the SDK

	#	file(COPY ${OPEN3DGC_HEADERS} DESTINATION ${OPEN3DGC_INCLUDE_DIR}) # For the SDK
endif()
