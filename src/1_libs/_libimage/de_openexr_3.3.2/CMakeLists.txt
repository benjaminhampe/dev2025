set(LIB_NAME de_openexr)
project(${LIB_NAME} CXX)

if (TARGET ${LIB_NAME})
	message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
	set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
	set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	set(LIB_HEADERS
		${LIB_HEADER_DIR}/OpenEXRConfig.h
		${LIB_HEADER_DIR}/OpenEXRConfigInternal.h

		${LIB_HEADER_DIR}/OpenEXR/ImfAcesFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfArray.h
		${LIB_HEADER_DIR}/OpenEXR/ImfAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfAutoArray.h
		${LIB_HEADER_DIR}/OpenEXR/ImfB44Compressor.h
		${LIB_HEADER_DIR}/OpenEXR/ImfBoxAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfChannelList.h
		${LIB_HEADER_DIR}/OpenEXR/ImfChannelListAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfCheckedArithmetic.h
		${LIB_HEADER_DIR}/OpenEXR/ImfChromaticities.h
		${LIB_HEADER_DIR}/OpenEXR/ImfChromaticitiesAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfCompositeDeepScanLine.h
		${LIB_HEADER_DIR}/OpenEXR/ImfCompression.h
		${LIB_HEADER_DIR}/OpenEXR/ImfCompressionAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfCompressor.h
		${LIB_HEADER_DIR}/OpenEXR/ImfContext.h
		${LIB_HEADER_DIR}/OpenEXR/ImfContextInit.h
		${LIB_HEADER_DIR}/OpenEXR/ImfConvert.h
		${LIB_HEADER_DIR}/OpenEXR/ImfCRgbaFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepCompositing.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepFrameBuffer.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepImageState.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepImageStateAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepScanLineInputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepScanLineInputPart.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepScanLineOutputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepScanLineOutputPart.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepTiledInputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepTiledInputPart.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepTiledOutputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepTiledOutputPart.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDoubleAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDwaCompressor.h
		${LIB_HEADER_DIR}/OpenEXR/ImfEnvmap.h
		${LIB_HEADER_DIR}/OpenEXR/ImfEnvmapAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfExport.h
		${LIB_HEADER_DIR}/OpenEXR/ImfFastHuf.h
		${LIB_HEADER_DIR}/OpenEXR/ImfFloatAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfFloatVectorAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfForward.h
		${LIB_HEADER_DIR}/OpenEXR/ImfFrameBuffer.h
		${LIB_HEADER_DIR}/OpenEXR/ImfFramesPerSecond.h
		${LIB_HEADER_DIR}/OpenEXR/ImfGenericInputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfGenericOutputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfHeader.h
		${LIB_HEADER_DIR}/OpenEXR/ImfHuf.h
		${LIB_HEADER_DIR}/OpenEXR/ImfIDManifest.h
		${LIB_HEADER_DIR}/OpenEXR/ImfIDManifestAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfInputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfInputPart.h
		${LIB_HEADER_DIR}/OpenEXR/ImfInputPartData.h
		${LIB_HEADER_DIR}/OpenEXR/ImfInputStreamMutex.h
		${LIB_HEADER_DIR}/OpenEXR/ImfInt64.h
		${LIB_HEADER_DIR}/OpenEXR/ImfIntAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfIO.h
		${LIB_HEADER_DIR}/OpenEXR/ImfKeyCode.h
		${LIB_HEADER_DIR}/OpenEXR/ImfKeyCodeAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfLineOrder.h
		${LIB_HEADER_DIR}/OpenEXR/ImfLineOrderAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfLut.h
		${LIB_HEADER_DIR}/OpenEXR/ImfMatrixAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfMisc.h
		${LIB_HEADER_DIR}/OpenEXR/ImfMultiPartInputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfMultiPartOutputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfMultiView.h
		${LIB_HEADER_DIR}/OpenEXR/ImfName.h
		${LIB_HEADER_DIR}/OpenEXR/ImfNamespace.h
		${LIB_HEADER_DIR}/OpenEXR/ImfOpaqueAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfOptimizedPixelReading.h
		${LIB_HEADER_DIR}/OpenEXR/ImfOutputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfOutputPart.h
		${LIB_HEADER_DIR}/OpenEXR/ImfOutputPartData.h
		${LIB_HEADER_DIR}/OpenEXR/ImfOutputStreamMutex.h
		${LIB_HEADER_DIR}/OpenEXR/ImfPartHelper.h
		${LIB_HEADER_DIR}/OpenEXR/ImfPartType.h
		${LIB_HEADER_DIR}/OpenEXR/ImfPixelType.h
		${LIB_HEADER_DIR}/OpenEXR/ImfPizCompressor.h
		${LIB_HEADER_DIR}/OpenEXR/ImfPreviewImage.h
		${LIB_HEADER_DIR}/OpenEXR/ImfPreviewImageAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfPxr24Compressor.h
		${LIB_HEADER_DIR}/OpenEXR/ImfRational.h
		${LIB_HEADER_DIR}/OpenEXR/ImfRationalAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfRgba.h
		${LIB_HEADER_DIR}/OpenEXR/ImfRgbaFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfRgbaYca.h
		${LIB_HEADER_DIR}/OpenEXR/ImfRle.h
		${LIB_HEADER_DIR}/OpenEXR/ImfRleCompressor.h
		${LIB_HEADER_DIR}/OpenEXR/ImfScanLineInputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfSimd.h
		${LIB_HEADER_DIR}/OpenEXR/ImfStandardAttributes.h
		${LIB_HEADER_DIR}/OpenEXR/ImfStdIO.h
		${LIB_HEADER_DIR}/OpenEXR/ImfStringAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfStringVectorAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfSystemSpecific.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTestFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfThreading.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTileDescription.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTileDescriptionAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTiledInputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTiledInputPart.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTiledMisc.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTiledOutputFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTiledOutputPart.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTiledRgbaFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTileOffsets.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTimeCode.h
		${LIB_HEADER_DIR}/OpenEXR/ImfTimeCodeAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfVecAttribute.h
		${LIB_HEADER_DIR}/OpenEXR/ImfVersion.h
		${LIB_HEADER_DIR}/OpenEXR/ImfWav.h
		${LIB_HEADER_DIR}/OpenEXR/ImfXdr.h
		${LIB_HEADER_DIR}/OpenEXR/ImfZip.h
		${LIB_HEADER_DIR}/OpenEXR/ImfZipCompressor.h
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

    set(LIB_HEADERS_IEX
		${LIB_HEADER_DIR}/Iex.h
		${LIB_HEADER_DIR}/IexBaseExc.h
		${LIB_HEADER_DIR}/IexErrnoExc.h
		${LIB_HEADER_DIR}/IexExport.h
		${LIB_HEADER_DIR}/IexForward.h
		${LIB_HEADER_DIR}/IexMacros.h
		${LIB_HEADER_DIR}/IexMathExc.h
		${LIB_HEADER_DIR}/IexMathFloatExc.h
		${LIB_HEADER_DIR}/IexMathFpu.h
		${LIB_HEADER_DIR}/IexMathIeeeExc.h
		${LIB_HEADER_DIR}/IexNamespace.h
		${LIB_HEADER_DIR}/IexThrowErrnoExc.h
	)

    set(LIB_SOURCES_IEX
		${LIB_SOURCE_DIR}/Iex/IexBaseExc.cpp
		${LIB_SOURCE_DIR}/Iex/IexMathFloatExc.cpp
		${LIB_SOURCE_DIR}/Iex/IexMathFpu.cpp
		${LIB_SOURCE_DIR}/Iex/IexThrowErrnoExc.cpp
	)

    list(APPEND LIB_HEADERS ${LIB_HEADERS_IEX})
	list(APPEND LIB_SOURCES ${LIB_SOURCES_IEX})

	set(LIB_HEADERS_IMATH
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

    set(LIB_SOURCES_IMATH
		${LIB_SOURCE_DIR}/Imath/half.cpp
		${LIB_SOURCE_DIR}/Imath/ImathColorAlgo.cpp
		${LIB_SOURCE_DIR}/Imath/ImathFun.cpp
		${LIB_SOURCE_DIR}/Imath/ImathMatrixAlgo.cpp
		${LIB_SOURCE_DIR}/Imath/ImathRandom.cpp
		#${LIB_SOURCE_DIR}/Imath/toFloat.cpp
		${LIB_SOURCE_DIR}/Imath/toFloat.h
	)

    list(APPEND LIB_HEADERS ${LIB_HEADERS_IMATH})
	list(APPEND LIB_SOURCES ${LIB_SOURCES_IMATH})

	set(LIB_HEADERS_ILMTHREAD
		${LIB_HEADER_DIR}/IlmThread.h
		${LIB_HEADER_DIR}/IlmThreadExport.h
		${LIB_HEADER_DIR}/IlmThreadForward.h
		${LIB_HEADER_DIR}/IlmThreadMutex.h
		${LIB_HEADER_DIR}/IlmThreadNamespace.h
		${LIB_HEADER_DIR}/IlmThreadPool.h
		${LIB_HEADER_DIR}/IlmThreadProcessGroup.h
		${LIB_HEADER_DIR}/IlmThreadSemaphore.h
	)
    set(LIB_SOURCES_ILMTHREAD
		${LIB_SOURCE_DIR}/IlmThread/IlmThread.cpp
		${LIB_SOURCE_DIR}/IlmThread/IlmThreadPool.cpp
		${LIB_SOURCE_DIR}/IlmThread/IlmThreadSemaphore.cpp
		${LIB_SOURCE_DIR}/IlmThread/IlmThreadSemaphoreOSX.cpp
		${LIB_SOURCE_DIR}/IlmThread/IlmThreadSemaphorePosix.cpp
		${LIB_SOURCE_DIR}/IlmThread/IlmThreadSemaphorePosixCompat.cpp
		${LIB_SOURCE_DIR}/IlmThread/IlmThreadSemaphoreWin32.cpp
	)

    list(APPEND LIB_HEADERS ${LIB_HEADERS_ILMTHREAD})
	list(APPEND LIB_SOURCES ${LIB_SOURCES_ILMTHREAD})

	set(LIB_HEADERS_CORE
		${LIB_HEADER_DIR}/openexr.h
		${LIB_HEADER_DIR}/openexr_attr.h
		${LIB_HEADER_DIR}/openexr_base.h
		${LIB_HEADER_DIR}/openexr_chunkio.h
		${LIB_HEADER_DIR}/openexr_coding.h
		${LIB_HEADER_DIR}/openexr_compression.h
		${LIB_HEADER_DIR}/openexr_config.h
		${LIB_HEADER_DIR}/openexr_context.h
		${LIB_HEADER_DIR}/openexr_debug.h
		${LIB_HEADER_DIR}/openexr_decode.h
		${LIB_HEADER_DIR}/openexr_encode.h
		${LIB_HEADER_DIR}/openexr_errors.h
		${LIB_HEADER_DIR}/openexr_part.h
		${LIB_HEADER_DIR}/openexr_std_attr.h
		${LIB_HEADER_DIR}/openexr_version.h
	)

    set(LIB_SOURCES_CORE
		${LIB_SOURCE_DIR}/Core/backward_compatibility.h
		${LIB_SOURCE_DIR}/Core/dwaLookups.h
		${LIB_SOURCE_DIR}/Core/internal_attr.h
		${LIB_SOURCE_DIR}/Core/internal_channel_list.h
		${LIB_SOURCE_DIR}/Core/internal_coding.h
		${LIB_SOURCE_DIR}/Core/internal_compress.h
		${LIB_SOURCE_DIR}/Core/internal_constants.h
		${LIB_SOURCE_DIR}/Core/internal_cpuid.h
		${LIB_SOURCE_DIR}/Core/internal_decompress.h
		${LIB_SOURCE_DIR}/Core/internal_dwa_channeldata.h
		${LIB_SOURCE_DIR}/Core/internal_dwa_classifier.h
		${LIB_SOURCE_DIR}/Core/internal_dwa_compressor.h
		${LIB_SOURCE_DIR}/Core/internal_dwa_decoder.h
		${LIB_SOURCE_DIR}/Core/internal_dwa_encoder.h
		${LIB_SOURCE_DIR}/Core/internal_dwa_helpers.h
		${LIB_SOURCE_DIR}/Core/internal_dwa_simd.h
		${LIB_SOURCE_DIR}/Core/internal_file.h
		${LIB_SOURCE_DIR}/Core/internal_float_vector.h
		${LIB_SOURCE_DIR}/Core/internal_huf.h
		${LIB_SOURCE_DIR}/Core/internal_memory.h
		${LIB_SOURCE_DIR}/Core/internal_opaque.h
		${LIB_SOURCE_DIR}/Core/internal_posix_file_impl.h
		${LIB_SOURCE_DIR}/Core/internal_preview.h
		${LIB_SOURCE_DIR}/Core/internal_string.h
		${LIB_SOURCE_DIR}/Core/internal_string_vector.h
		${LIB_SOURCE_DIR}/Core/internal_structs.h
		${LIB_SOURCE_DIR}/Core/internal_util.h
		${LIB_SOURCE_DIR}/Core/internal_win32_file_impl.h
		${LIB_SOURCE_DIR}/Core/internal_xdr.h

		${LIB_SOURCE_DIR}/Core/attributes.c
		${LIB_SOURCE_DIR}/Core/base.c
		${LIB_SOURCE_DIR}/Core/channel_list.c
		${LIB_SOURCE_DIR}/Core/chunk.c
		${LIB_SOURCE_DIR}/Core/coding.c
		${LIB_SOURCE_DIR}/Core/compression.c
		${LIB_SOURCE_DIR}/Core/context.c
		${LIB_SOURCE_DIR}/Core/debug.c
		${LIB_SOURCE_DIR}/Core/decoding.c
		${LIB_SOURCE_DIR}/Core/encoding.c
		${LIB_SOURCE_DIR}/Core/float_vector.c
		${LIB_SOURCE_DIR}/Core/internal_b44.c
		${LIB_SOURCE_DIR}/Core/internal_b44_table.c
		${LIB_SOURCE_DIR}/Core/internal_dwa.c
		${LIB_SOURCE_DIR}/Core/internal_huf.c
		${LIB_SOURCE_DIR}/Core/internal_piz.c
		${LIB_SOURCE_DIR}/Core/internal_pxr24.c
		${LIB_SOURCE_DIR}/Core/internal_rle.c
		${LIB_SOURCE_DIR}/Core/internal_structs.c
		${LIB_SOURCE_DIR}/Core/internal_zip.c
		${LIB_SOURCE_DIR}/Core/memory.c
		${LIB_SOURCE_DIR}/Core/opaque.c
		${LIB_SOURCE_DIR}/Core/pack.c
		${LIB_SOURCE_DIR}/Core/parse_header.c
		${LIB_SOURCE_DIR}/Core/part.c
		${LIB_SOURCE_DIR}/Core/part_attr.c
		${LIB_SOURCE_DIR}/Core/preview.c
		${LIB_SOURCE_DIR}/Core/std_attr.c
		${LIB_SOURCE_DIR}/Core/string.c
		${LIB_SOURCE_DIR}/Core/string_vector.c
		${LIB_SOURCE_DIR}/Core/unpack.c
		${LIB_SOURCE_DIR}/Core/validation.c
		${LIB_SOURCE_DIR}/Core/write_header.c
	)

    list(APPEND LIB_HEADERS ${LIB_HEADERS_CORE})
	list(APPEND LIB_SOURCES ${LIB_SOURCES_CORE})

	set(LIB_HEADERS_UTIL
		${LIB_HEADER_DIR}/OpenEXR/ImfCheckFile.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepImage.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepImageChannel.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepImageIO.h
		${LIB_HEADER_DIR}/OpenEXR/ImfDeepImageLevel.h
		${LIB_HEADER_DIR}/OpenEXR/ImfFlatImage.h
		${LIB_HEADER_DIR}/OpenEXR/ImfFlatImageChannel.h
		${LIB_HEADER_DIR}/OpenEXR/ImfFlatImageIO.h
		${LIB_HEADER_DIR}/OpenEXR/ImfFlatImageLevel.h
		${LIB_HEADER_DIR}/OpenEXR/ImfImage.h
		${LIB_HEADER_DIR}/OpenEXR/ImfImageChannel.h
		${LIB_HEADER_DIR}/OpenEXR/ImfImageChannelRenaming.h
		${LIB_HEADER_DIR}/OpenEXR/ImfImageDataWindow.h
		${LIB_HEADER_DIR}/OpenEXR/ImfImageIO.h
		${LIB_HEADER_DIR}/OpenEXR/ImfImageLevel.h
		${LIB_HEADER_DIR}/OpenEXR/ImfSampleCountChannel.h
		${LIB_HEADER_DIR}/OpenEXR/ImfUtilExport.h
	)

    set(LIB_SOURCES_UTIL
		${LIB_SOURCE_DIR}/Util/ImfCheckFile.cpp
		${LIB_SOURCE_DIR}/Util/ImfDeepImage.cpp
		${LIB_SOURCE_DIR}/Util/ImfDeepImageChannel.cpp
		${LIB_SOURCE_DIR}/Util/ImfDeepImageIO.cpp
		${LIB_SOURCE_DIR}/Util/ImfDeepImageLevel.cpp
		${LIB_SOURCE_DIR}/Util/ImfFlatImage.cpp
		${LIB_SOURCE_DIR}/Util/ImfFlatImageChannel.cpp
		${LIB_SOURCE_DIR}/Util/ImfFlatImageIO.cpp
		${LIB_SOURCE_DIR}/Util/ImfFlatImageLevel.cpp
		${LIB_SOURCE_DIR}/Util/ImfImage.cpp
		${LIB_SOURCE_DIR}/Util/ImfImageChannel.cpp
		${LIB_SOURCE_DIR}/Util/ImfImageDataWindow.cpp
		${LIB_SOURCE_DIR}/Util/ImfImageIO.cpp
		${LIB_SOURCE_DIR}/Util/ImfImageLevel.cpp
		${LIB_SOURCE_DIR}/Util/ImfSampleCountChannel.cpp
	)

    list(APPEND LIB_HEADERS ${LIB_HEADERS_UTIL})
	list(APPEND LIB_SOURCES ${LIB_SOURCES_UTIL})

	# if (WIN32)
	    # enable_language(RC)
		# list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
		# list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
	# endif()

	if (BUILD_SHARED_LIBS)
		message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
		add_library(${LIB_NAME} SHARED "")
	else()
		message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
		add_library(${LIB_NAME} STATIC "")
	endif()

	target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

	target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR}/OpenEXR)
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Iex)
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Imath)
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/IlmThread)
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Core)
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/Util)
	target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/OpenEXR)

	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_LIBOPENEXR=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_OPENEXR=1)

	target_compile_definitions(${LIB_NAME} PUBLIC OPENEXR_VERSION=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_PTHREAD=1)
	# target_compile_definitions(${LIB_NAME} PUBLIC HAVE_ZLIB=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_PIZ=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_PXR24=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_B44=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_B44A=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_DWAA=1)
	target_compile_definitions(${LIB_NAME} PUBLIC HAVE_DWAB)

	target_compile_definitions(${LIB_NAME} PUBLIC OPENEXR_BUILD_LIBS=1)

	# if (MSVC)
	    # set_target_properties(${LIB_NAME} PROPERTIES FOLDER _libimage)
		# else()
	    # # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
		# # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
		# # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
		# # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c

		# # target_compile_definitions(${LIB_NAME} PUBLIC OPENEXR_DLL=1)

		# # target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
		# # target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-fallthrough)
		# # target_compile_options(${LIB_NAME} PRIVATE -Wno-shift-negative-value)
		# # target_compile_options(${LIB_NAME} PRIVATE -Wno-main) # jdmainct.c

		# target_compile_options(${LIB_NAME} PRIVATE -Wno-sign-conversion)
		# target_compile_options(${LIB_NAME} PRIVATE -Wno-implicit-int-conversion)
		# target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-parameter)
		# target_compile_options(${LIB_NAME} PRIVATE -Wno-used-but-marked-unused)
		# target_compile_options(${LIB_NAME} PRIVATE -Wno-shorten-64-to-32)
		# target_compile_options(${LIB_NAME} PRIVATE -Wno-sign-compare)
		# target_compile_options(${LIB_NAME} PRIVATE -Wno-padded)
		# target_compile_options(${LIB_NAME} PRIVATE -Wno-cast-qual)
		# target_compile_options(${LIB_NAME} PRIVATE -Wno-disabled-macro-expansion)
		# endif()

	target_link_libraries(${LIB_NAME} PUBLIC de_zlib)
	target_link_libraries(${LIB_NAME} PUBLIC de_deflate)

	# target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Imath)
	# target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Iex)
	# target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_IlmThread)
	# target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Core)
	# target_link_libraries(${LIB_NAME} PUBLIC de_OpenEXR_Util)
	#	file(COPY ${LIB_HEADERS} DESTINATION ${JPEG_INCLUDE_DIR}) # For the SDK
endif()

# add_subdirectory(test)
