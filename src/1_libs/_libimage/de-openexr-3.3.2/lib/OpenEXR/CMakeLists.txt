set(LIB_NAME de_OpenEXR)
project(${LIB_NAME}-dev CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/OpenEXR)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/ImfAcesFile.h
		${LIB_HEADER_DIR}/ImfArray.h
		${LIB_HEADER_DIR}/ImfAttribute.h
		${LIB_HEADER_DIR}/ImfAutoArray.h
		${LIB_HEADER_DIR}/ImfB44Compressor.h
		${LIB_HEADER_DIR}/ImfBoxAttribute.h
		${LIB_HEADER_DIR}/ImfChannelList.h
		${LIB_HEADER_DIR}/ImfChannelListAttribute.h
		${LIB_HEADER_DIR}/ImfCheckedArithmetic.h
		${LIB_HEADER_DIR}/ImfChromaticities.h
		${LIB_HEADER_DIR}/ImfChromaticitiesAttribute.h
		${LIB_HEADER_DIR}/ImfCompositeDeepScanLine.h
		${LIB_HEADER_DIR}/ImfCompression.h
		${LIB_HEADER_DIR}/ImfCompressionAttribute.h
		${LIB_HEADER_DIR}/ImfCompressor.h
		${LIB_HEADER_DIR}/ImfContext.h
		${LIB_HEADER_DIR}/ImfContextInit.h
		${LIB_HEADER_DIR}/ImfConvert.h
		${LIB_HEADER_DIR}/ImfCRgbaFile.h
		${LIB_HEADER_DIR}/ImfDeepCompositing.h
		${LIB_HEADER_DIR}/ImfDeepFrameBuffer.h
		${LIB_HEADER_DIR}/ImfDeepImageState.h
		${LIB_HEADER_DIR}/ImfDeepImageStateAttribute.h
		${LIB_HEADER_DIR}/ImfDeepScanLineInputFile.h
		${LIB_HEADER_DIR}/ImfDeepScanLineInputPart.h
		${LIB_HEADER_DIR}/ImfDeepScanLineOutputFile.h
		${LIB_HEADER_DIR}/ImfDeepScanLineOutputPart.h
		${LIB_HEADER_DIR}/ImfDeepTiledInputFile.h
		${LIB_HEADER_DIR}/ImfDeepTiledInputPart.h
		${LIB_HEADER_DIR}/ImfDeepTiledOutputFile.h
		${LIB_HEADER_DIR}/ImfDeepTiledOutputPart.h
		${LIB_HEADER_DIR}/ImfDoubleAttribute.h
		${LIB_HEADER_DIR}/ImfDwaCompressor.h
		${LIB_HEADER_DIR}/ImfEnvmap.h
		${LIB_HEADER_DIR}/ImfEnvmapAttribute.h
		${LIB_HEADER_DIR}/ImfExport.h
		${LIB_HEADER_DIR}/ImfFastHuf.h
		${LIB_HEADER_DIR}/ImfFloatAttribute.h
		${LIB_HEADER_DIR}/ImfFloatVectorAttribute.h
		${LIB_HEADER_DIR}/ImfForward.h
		${LIB_HEADER_DIR}/ImfFrameBuffer.h
		${LIB_HEADER_DIR}/ImfFramesPerSecond.h
		${LIB_HEADER_DIR}/ImfGenericInputFile.h
		${LIB_HEADER_DIR}/ImfGenericOutputFile.h
		${LIB_HEADER_DIR}/ImfHeader.h
		${LIB_HEADER_DIR}/ImfHuf.h
		${LIB_HEADER_DIR}/ImfIDManifest.h
		${LIB_HEADER_DIR}/ImfIDManifestAttribute.h
		${LIB_HEADER_DIR}/ImfInputFile.h
		${LIB_HEADER_DIR}/ImfInputPart.h
		${LIB_HEADER_DIR}/ImfInputPartData.h
		${LIB_HEADER_DIR}/ImfInputStreamMutex.h
		${LIB_HEADER_DIR}/ImfInt64.h
		${LIB_HEADER_DIR}/ImfIntAttribute.h
		${LIB_HEADER_DIR}/ImfIO.h
		${LIB_HEADER_DIR}/ImfKeyCode.h
		${LIB_HEADER_DIR}/ImfKeyCodeAttribute.h
		${LIB_HEADER_DIR}/ImfLineOrder.h
		${LIB_HEADER_DIR}/ImfLineOrderAttribute.h
		${LIB_HEADER_DIR}/ImfLut.h
		${LIB_HEADER_DIR}/ImfMatrixAttribute.h
		${LIB_HEADER_DIR}/ImfMisc.h
		${LIB_HEADER_DIR}/ImfMultiPartInputFile.h
		${LIB_HEADER_DIR}/ImfMultiPartOutputFile.h
		${LIB_HEADER_DIR}/ImfMultiView.h
		${LIB_HEADER_DIR}/ImfName.h
		${LIB_HEADER_DIR}/ImfNamespace.h
		${LIB_HEADER_DIR}/ImfOpaqueAttribute.h
		${LIB_HEADER_DIR}/ImfOptimizedPixelReading.h
		${LIB_HEADER_DIR}/ImfOutputFile.h
		${LIB_HEADER_DIR}/ImfOutputPart.h
		${LIB_HEADER_DIR}/ImfOutputPartData.h
		${LIB_HEADER_DIR}/ImfOutputStreamMutex.h
		${LIB_HEADER_DIR}/ImfPartHelper.h
		${LIB_HEADER_DIR}/ImfPartType.h
		${LIB_HEADER_DIR}/ImfPixelType.h
		${LIB_HEADER_DIR}/ImfPizCompressor.h
		${LIB_HEADER_DIR}/ImfPreviewImage.h
		${LIB_HEADER_DIR}/ImfPreviewImageAttribute.h
		${LIB_HEADER_DIR}/ImfPxr24Compressor.h
		${LIB_HEADER_DIR}/ImfRational.h
		${LIB_HEADER_DIR}/ImfRationalAttribute.h
		${LIB_HEADER_DIR}/ImfRgba.h
		${LIB_HEADER_DIR}/ImfRgbaFile.h
		${LIB_HEADER_DIR}/ImfRgbaYca.h
		${LIB_HEADER_DIR}/ImfRle.h
		${LIB_HEADER_DIR}/ImfRleCompressor.h
		${LIB_HEADER_DIR}/ImfScanLineInputFile.h
		${LIB_HEADER_DIR}/ImfSimd.h
		${LIB_HEADER_DIR}/ImfStandardAttributes.h
		${LIB_HEADER_DIR}/ImfStdIO.h
		${LIB_HEADER_DIR}/ImfStringAttribute.h
		${LIB_HEADER_DIR}/ImfStringVectorAttribute.h
		${LIB_HEADER_DIR}/ImfSystemSpecific.h
		${LIB_HEADER_DIR}/ImfTestFile.h
		${LIB_HEADER_DIR}/ImfThreading.h
		${LIB_HEADER_DIR}/ImfTileDescription.h
		${LIB_HEADER_DIR}/ImfTileDescriptionAttribute.h
		${LIB_HEADER_DIR}/ImfTiledInputFile.h
		${LIB_HEADER_DIR}/ImfTiledInputPart.h
		${LIB_HEADER_DIR}/ImfTiledMisc.h
		${LIB_HEADER_DIR}/ImfTiledOutputFile.h
		${LIB_HEADER_DIR}/ImfTiledOutputPart.h
		${LIB_HEADER_DIR}/ImfTiledRgbaFile.h
		${LIB_HEADER_DIR}/ImfTileOffsets.h
		${LIB_HEADER_DIR}/ImfTimeCode.h
		${LIB_HEADER_DIR}/ImfTimeCodeAttribute.h
		${LIB_HEADER_DIR}/ImfVecAttribute.h
		${LIB_HEADER_DIR}/ImfVersion.h
		${LIB_HEADER_DIR}/ImfWav.h
		${LIB_HEADER_DIR}/ImfXdr.h
		${LIB_HEADER_DIR}/ImfZip.h
		${LIB_HEADER_DIR}/ImfZipCompressor.h		
	)
	
	set(LIB_SOURCES
                # ${LIB_SOURCE_DIR}/b44ExpLogTable.cpp
                # ${LIB_SOURCE_DIR}/dwaLookups.cpp
		${LIB_SOURCE_DIR}/ImfAcesFile.cpp
		${LIB_SOURCE_DIR}/ImfAttribute.cpp
		${LIB_SOURCE_DIR}/ImfB44Compressor.cpp
		${LIB_SOURCE_DIR}/ImfBoxAttribute.cpp
		${LIB_SOURCE_DIR}/ImfChannelList.cpp
		${LIB_SOURCE_DIR}/ImfChannelListAttribute.cpp
		${LIB_SOURCE_DIR}/ImfChromaticities.cpp
		${LIB_SOURCE_DIR}/ImfChromaticitiesAttribute.cpp
		${LIB_SOURCE_DIR}/ImfCompositeDeepScanLine.cpp
		${LIB_SOURCE_DIR}/ImfCompression.cpp
		${LIB_SOURCE_DIR}/ImfCompressionAttribute.cpp
		${LIB_SOURCE_DIR}/ImfCompressor.cpp
		${LIB_SOURCE_DIR}/ImfContext.cpp
		${LIB_SOURCE_DIR}/ImfContextInit.cpp
		${LIB_SOURCE_DIR}/ImfConvert.cpp
		${LIB_SOURCE_DIR}/ImfCRgbaFile.cpp
		${LIB_SOURCE_DIR}/ImfDeepCompositing.cpp
		${LIB_SOURCE_DIR}/ImfDeepFrameBuffer.cpp
		${LIB_SOURCE_DIR}/ImfDeepImageStateAttribute.cpp
		${LIB_SOURCE_DIR}/ImfDeepScanLineInputFile.cpp
		${LIB_SOURCE_DIR}/ImfDeepScanLineInputPart.cpp
		${LIB_SOURCE_DIR}/ImfDeepScanLineOutputFile.cpp
		${LIB_SOURCE_DIR}/ImfDeepScanLineOutputPart.cpp
		${LIB_SOURCE_DIR}/ImfDeepTiledInputFile.cpp
		${LIB_SOURCE_DIR}/ImfDeepTiledInputPart.cpp
		${LIB_SOURCE_DIR}/ImfDeepTiledOutputFile.cpp
		${LIB_SOURCE_DIR}/ImfDeepTiledOutputPart.cpp
		${LIB_SOURCE_DIR}/ImfDoubleAttribute.cpp
		${LIB_SOURCE_DIR}/ImfDwaCompressor.cpp
		${LIB_SOURCE_DIR}/ImfEnvmap.cpp
		${LIB_SOURCE_DIR}/ImfEnvmapAttribute.cpp
		${LIB_SOURCE_DIR}/ImfFastHuf.cpp
		${LIB_SOURCE_DIR}/ImfFloatAttribute.cpp
		${LIB_SOURCE_DIR}/ImfFloatVectorAttribute.cpp
		${LIB_SOURCE_DIR}/ImfFrameBuffer.cpp
		${LIB_SOURCE_DIR}/ImfFramesPerSecond.cpp
		${LIB_SOURCE_DIR}/ImfGenericInputFile.cpp
		${LIB_SOURCE_DIR}/ImfGenericOutputFile.cpp
		${LIB_SOURCE_DIR}/ImfHeader.cpp
		${LIB_SOURCE_DIR}/ImfHuf.cpp
		${LIB_SOURCE_DIR}/ImfIDManifest.cpp
		${LIB_SOURCE_DIR}/ImfIDManifestAttribute.cpp
		${LIB_SOURCE_DIR}/ImfInputFile.cpp
		${LIB_SOURCE_DIR}/ImfInputPart.cpp
		${LIB_SOURCE_DIR}/ImfInputPartData.cpp
		${LIB_SOURCE_DIR}/ImfIntAttribute.cpp
		${LIB_SOURCE_DIR}/ImfIO.cpp
		${LIB_SOURCE_DIR}/ImfKeyCode.cpp
		${LIB_SOURCE_DIR}/ImfKeyCodeAttribute.cpp
		${LIB_SOURCE_DIR}/ImfLineOrderAttribute.cpp
		${LIB_SOURCE_DIR}/ImfLut.cpp
		${LIB_SOURCE_DIR}/ImfMatrixAttribute.cpp
		${LIB_SOURCE_DIR}/ImfMisc.cpp
		${LIB_SOURCE_DIR}/ImfMultiPartInputFile.cpp
		${LIB_SOURCE_DIR}/ImfMultiPartOutputFile.cpp
		${LIB_SOURCE_DIR}/ImfMultiView.cpp
		${LIB_SOURCE_DIR}/ImfOpaqueAttribute.cpp
		${LIB_SOURCE_DIR}/ImfOutputFile.cpp
		${LIB_SOURCE_DIR}/ImfOutputPart.cpp
		${LIB_SOURCE_DIR}/ImfOutputPartData.cpp
		${LIB_SOURCE_DIR}/ImfPartType.cpp
		${LIB_SOURCE_DIR}/ImfPizCompressor.cpp
		${LIB_SOURCE_DIR}/ImfPreviewImage.cpp
		${LIB_SOURCE_DIR}/ImfPreviewImageAttribute.cpp
		${LIB_SOURCE_DIR}/ImfPxr24Compressor.cpp
		${LIB_SOURCE_DIR}/ImfRational.cpp
		${LIB_SOURCE_DIR}/ImfRationalAttribute.cpp
		${LIB_SOURCE_DIR}/ImfRgbaFile.cpp
		${LIB_SOURCE_DIR}/ImfRgbaYca.cpp
		${LIB_SOURCE_DIR}/ImfRle.cpp
		${LIB_SOURCE_DIR}/ImfRleCompressor.cpp
		${LIB_SOURCE_DIR}/ImfScanLineInputFile.cpp
		${LIB_SOURCE_DIR}/ImfStandardAttributes.cpp
		${LIB_SOURCE_DIR}/ImfStdIO.cpp
		${LIB_SOURCE_DIR}/ImfStringAttribute.cpp
		${LIB_SOURCE_DIR}/ImfStringVectorAttribute.cpp
		${LIB_SOURCE_DIR}/ImfSystemSpecific.cpp
		${LIB_SOURCE_DIR}/ImfTestFile.cpp
		${LIB_SOURCE_DIR}/ImfThreading.cpp
		${LIB_SOURCE_DIR}/ImfTileDescriptionAttribute.cpp
		${LIB_SOURCE_DIR}/ImfTiledInputFile.cpp
		${LIB_SOURCE_DIR}/ImfTiledInputPart.cpp
		${LIB_SOURCE_DIR}/ImfTiledMisc.cpp
		${LIB_SOURCE_DIR}/ImfTiledOutputFile.cpp
		${LIB_SOURCE_DIR}/ImfTiledOutputPart.cpp
		${LIB_SOURCE_DIR}/ImfTiledRgbaFile.cpp
		${LIB_SOURCE_DIR}/ImfTileOffsets.cpp
		${LIB_SOURCE_DIR}/ImfTimeCode.cpp
		${LIB_SOURCE_DIR}/ImfTimeCodeAttribute.cpp
		${LIB_SOURCE_DIR}/ImfVecAttribute.cpp
		${LIB_SOURCE_DIR}/ImfVersion.cpp
		${LIB_SOURCE_DIR}/ImfWav.cpp
		${LIB_SOURCE_DIR}/ImfZip.cpp
		${LIB_SOURCE_DIR}/ImfZipCompressor.cpp			
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

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBOPENEXR=1)

    target_compile_definitions(${LIB_NAME} PUBLIC OPENEXR_VERSION=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_PTHREAD=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_ZLIB=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_PIZ=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_PXR24=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_B44=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_B44A=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_DWAA=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_DWAB)

    if (MSVC)
        set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libimage)
    else()
        # target_compile_definitions(${LIB_NAME} PUBLIC OPENEXR_DLL=1)

        # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c
    endif()

	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Imath)
	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)
	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_IlmThread)
	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Config)
	target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Core)
	target_link_libraries(${LIB_NAME} PUBLIC de_zlib)    
    #	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
