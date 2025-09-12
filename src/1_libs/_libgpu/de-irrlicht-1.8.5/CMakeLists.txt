set(LIB_NAME de_irrlicht)
project(lib${LIB_NAME}-1.8.5-dev)

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    # add_subdirectory(lib)
   
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/Irrlicht)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
	
    file(GLOB_RECURSE LIB_HEADERS "${LIB_HEADER_DIR}/*.h")
    #file(GLOB_RECURSE LIB_HEADERS_HPP "${LIB_HEADER_DIR}/*.hpp")
    #list(APPEND LIB_HEADERS "${LIB_HEADERS_HPP}")
    #list(APPEND LIB_HEADERS "${LIB_HEADER_DIR}/de_irrlicht.hpp")

    # file(GLOB_RECURSE LIB_SOURCES "${LIB_SOURCE_DIR}/*.cpp")

    set(LIB_SOURCES_EXTERN_ZLIB
      # links with de_zlib, commented out because of multiple definitions.
#      ${LIB_SOURCE_DIR}/zlib/adler32.c
#      ${LIB_SOURCE_DIR}/zlib/compress.c
#      ${LIB_SOURCE_DIR}/zlib/crc32.c
#      ${LIB_SOURCE_DIR}/zlib/deflate.c
#      ${LIB_SOURCE_DIR}/zlib/gzclose.c
#      ${LIB_SOURCE_DIR}/zlib/gzlib.c
#      ${LIB_SOURCE_DIR}/zlib/gzread.c
#      ${LIB_SOURCE_DIR}/zlib/gzwrite.c
#      ${LIB_SOURCE_DIR}/zlib/infback.c
#      ${LIB_SOURCE_DIR}/zlib/inffast.c
#      ${LIB_SOURCE_DIR}/zlib/inflate.c
#      ${LIB_SOURCE_DIR}/zlib/inftrees.c
#      ${LIB_SOURCE_DIR}/zlib/trees.c
#      ${LIB_SOURCE_DIR}/zlib/uncompr.c
#      ${LIB_SOURCE_DIR}/zlib/zutil.c
      # links with de_bzip2, commented out because of multiple definitions.
#      ${LIB_SOURCE_DIR}/bzip2/blocksort.c
#      ${LIB_SOURCE_DIR}/bzip2/bzcompress.c
#      ${LIB_SOURCE_DIR}/bzip2/bzlib.c
#      ${LIB_SOURCE_DIR}/bzip2/crctable.c
#      ${LIB_SOURCE_DIR}/bzip2/decompress.c
#      ${LIB_SOURCE_DIR}/bzip2/huffman.c
#      ${LIB_SOURCE_DIR}/bzip2/randtable.c
      #####
      # ${LIB_SOURCE_DIR}/bzip2/bzip2.c
      # ${LIB_SOURCE_DIR}/bzip2/bzip2recover.c
      # ${LIB_SOURCE_DIR}/bzip2/dlltest.c # main()
      # ${LIB_SOURCE_DIR}/bzip2/mk251.c
      # ${LIB_SOURCE_DIR}/bzip2/spewG.c
      # ${LIB_SOURCE_DIR}/bzip2/unzcrash.c
   )
   # links with de_bzip2, commented out because of multiple definitions.
   set(LIB_SOURCES_EXTERN_BZ2
#      ${LIB_SOURCE_DIR}/bzip2/blocksort.c
#      ${LIB_SOURCE_DIR}/bzip2/bzcompress.c
#      ${LIB_SOURCE_DIR}/bzip2/bzlib.c
#      ${LIB_SOURCE_DIR}/bzip2/crctable.c
#      ${LIB_SOURCE_DIR}/bzip2/decompress.c
#      ${LIB_SOURCE_DIR}/bzip2/huffman.c
#      ${LIB_SOURCE_DIR}/bzip2/randtable.c
      #####
      # ${LIB_SOURCE_DIR}/bzip2/bzip2.c
      # ${LIB_SOURCE_DIR}/bzip2/bzip2recover.c
      # ${LIB_SOURCE_DIR}/bzip2/dlltest.c # main()
      # ${LIB_SOURCE_DIR}/bzip2/mk251.c
      # ${LIB_SOURCE_DIR}/bzip2/spewG.c
      # ${LIB_SOURCE_DIR}/bzip2/unzcrash.c
   )

    set(LIB_SOURCES_EXTERN_JPEG
       ${LIB_SOURCE_DIR}/jpeglib/cdjpeg.c
       # ${LIB_SOURCE_DIR}/jpeglib/cjpeg.c
       # ${LIB_SOURCE_DIR}/jpeglib/ckconfig.c
       # ${LIB_SOURCE_DIR}/jpeglib/djpeg.c
       ${LIB_SOURCE_DIR}/jpeglib/jaricom.c
       ${LIB_SOURCE_DIR}/jpeglib/jcapimin.c
       ${LIB_SOURCE_DIR}/jpeglib/jcapistd.c
       ${LIB_SOURCE_DIR}/jpeglib/jcarith.c
       ${LIB_SOURCE_DIR}/jpeglib/jccoefct.c
       ${LIB_SOURCE_DIR}/jpeglib/jccolor.c
       ${LIB_SOURCE_DIR}/jpeglib/jcdctmgr.c
       ${LIB_SOURCE_DIR}/jpeglib/jchuff.c
       ${LIB_SOURCE_DIR}/jpeglib/jcinit.c
       ${LIB_SOURCE_DIR}/jpeglib/jcmainct.c
       ${LIB_SOURCE_DIR}/jpeglib/jcmarker.c
       ${LIB_SOURCE_DIR}/jpeglib/jcmaster.c
       ${LIB_SOURCE_DIR}/jpeglib/jcomapi.c
       ${LIB_SOURCE_DIR}/jpeglib/jcparam.c
       ${LIB_SOURCE_DIR}/jpeglib/jcprepct.c
       ${LIB_SOURCE_DIR}/jpeglib/jcsample.c
       ${LIB_SOURCE_DIR}/jpeglib/jctrans.c
       ${LIB_SOURCE_DIR}/jpeglib/jdapimin.c
       ${LIB_SOURCE_DIR}/jpeglib/jdapistd.c
       ${LIB_SOURCE_DIR}/jpeglib/jdarith.c
       ${LIB_SOURCE_DIR}/jpeglib/jdatadst.c
       ${LIB_SOURCE_DIR}/jpeglib/jdatasrc.c
       ${LIB_SOURCE_DIR}/jpeglib/jdcoefct.c
       ${LIB_SOURCE_DIR}/jpeglib/jdcolor.c
       ${LIB_SOURCE_DIR}/jpeglib/jddctmgr.c
       ${LIB_SOURCE_DIR}/jpeglib/jdhuff.c
       ${LIB_SOURCE_DIR}/jpeglib/jdinput.c
       ${LIB_SOURCE_DIR}/jpeglib/jdmainct.c
       ${LIB_SOURCE_DIR}/jpeglib/jdmarker.c
       ${LIB_SOURCE_DIR}/jpeglib/jdmaster.c
       ${LIB_SOURCE_DIR}/jpeglib/jdmerge.c
       ${LIB_SOURCE_DIR}/jpeglib/jdpostct.c
       ${LIB_SOURCE_DIR}/jpeglib/jdsample.c
       ${LIB_SOURCE_DIR}/jpeglib/jdtrans.c
       ${LIB_SOURCE_DIR}/jpeglib/jerror.c
       ${LIB_SOURCE_DIR}/jpeglib/jfdctflt.c
       ${LIB_SOURCE_DIR}/jpeglib/jfdctfst.c
       ${LIB_SOURCE_DIR}/jpeglib/jfdctint.c
       ${LIB_SOURCE_DIR}/jpeglib/jidctflt.c
       ${LIB_SOURCE_DIR}/jpeglib/jidctfst.c
       ${LIB_SOURCE_DIR}/jpeglib/jidctint.c
       ${LIB_SOURCE_DIR}/jpeglib/jinclude.h
       ${LIB_SOURCE_DIR}/jpeglib/jmemansi.c
       # ${LIB_SOURCE_DIR}/jpeglib/jmemdos.c
       # ${LIB_SOURCE_DIR}/jpeglib/jmemmac.c
       ${LIB_SOURCE_DIR}/jpeglib/jmemmgr.c
       ${LIB_SOURCE_DIR}/jpeglib/jmemname.c
       # ${LIB_SOURCE_DIR}/jpeglib/jmemnobs.c
       # ${LIB_SOURCE_DIR}/jpeglib/jpegtran.c
       ${LIB_SOURCE_DIR}/jpeglib/jquant1.c
       ${LIB_SOURCE_DIR}/jpeglib/jquant2.c
       ${LIB_SOURCE_DIR}/jpeglib/jutils.c
       ${LIB_SOURCE_DIR}/jpeglib/rdbmp.c
       ${LIB_SOURCE_DIR}/jpeglib/rdcolmap.c
       ${LIB_SOURCE_DIR}/jpeglib/rdgif.c
       # ${LIB_SOURCE_DIR}/jpeglib/rdjpgcom.c
       ${LIB_SOURCE_DIR}/jpeglib/rdppm.c
       ${LIB_SOURCE_DIR}/jpeglib/rdrle.c
       ${LIB_SOURCE_DIR}/jpeglib/rdswitch.c
       ${LIB_SOURCE_DIR}/jpeglib/rdtarga.c
       ${LIB_SOURCE_DIR}/jpeglib/transupp.c
       ${LIB_SOURCE_DIR}/jpeglib/wrbmp.c
       ${LIB_SOURCE_DIR}/jpeglib/wrgif.c
       # ${LIB_SOURCE_DIR}/jpeglib/wrjpgcom.c
       ${LIB_SOURCE_DIR}/jpeglib/wrppm.c
       ${LIB_SOURCE_DIR}/jpeglib/wrrle.c
       ${LIB_SOURCE_DIR}/jpeglib/wrtarga.c
    )

    set(LIB_SOURCES_EXTERN_PNG
       ${LIB_SOURCE_DIR}/libpng/example.c
       ${LIB_SOURCE_DIR}/libpng/png.c
       ${LIB_SOURCE_DIR}/libpng/pngerror.c
       ${LIB_SOURCE_DIR}/libpng/pngget.c
       ${LIB_SOURCE_DIR}/libpng/pngmem.c
       ${LIB_SOURCE_DIR}/libpng/pngpread.c
       ${LIB_SOURCE_DIR}/libpng/pngread.c
       ${LIB_SOURCE_DIR}/libpng/pngrio.c
       ${LIB_SOURCE_DIR}/libpng/pngrtran.c
       ${LIB_SOURCE_DIR}/libpng/pngrutil.c
       ${LIB_SOURCE_DIR}/libpng/pngset.c
       ${LIB_SOURCE_DIR}/libpng/pngtest.c
       ${LIB_SOURCE_DIR}/libpng/pngtrans.c
       ${LIB_SOURCE_DIR}/libpng/pngwio.c
       ${LIB_SOURCE_DIR}/libpng/pngwrite.c
       ${LIB_SOURCE_DIR}/libpng/pngwtran.c
       ${LIB_SOURCE_DIR}/libpng/pngwutil.c
    )

    set(LIB_SOURCES_LIB_LZMA
        ${LIB_SOURCE_DIR}/lzma/LzmaDec.c
    )

    set(LIB_SOURCES_LIB_AES_GLADMAN
        ${LIB_SOURCE_DIR}/aesGladman/pwd2key.cpp
        ${LIB_SOURCE_DIR}/aesGladman/aescrypt.cpp
        ${LIB_SOURCE_DIR}/aesGladman/aeskey.cpp
        ${LIB_SOURCE_DIR}/aesGladman/aestab.cpp
        ${LIB_SOURCE_DIR}/aesGladman/fileenc.cpp
        ${LIB_SOURCE_DIR}/aesGladman/hmac.cpp
        ${LIB_SOURCE_DIR}/aesGladman/prng.cpp
        ${LIB_SOURCE_DIR}/aesGladman/sha1.cpp
        ${LIB_SOURCE_DIR}/aesGladman/sha2.cpp
    )

    set(LIB_SOURCES
        ${LIB_SOURCES_LIB_LZMA}         # LZMA
        ${LIB_SOURCES_LIB_AES_GLADMAN}  # AES-gladman
        ${LIB_SOURCE_DIR}/C3DSMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CAnimatedMeshHalfLife.cpp
        ${LIB_SOURCE_DIR}/CAnimatedMeshMD2.cpp
        ${LIB_SOURCE_DIR}/CAnimatedMeshMD3.cpp
        ${LIB_SOURCE_DIR}/CAnimatedMeshSceneNode.cpp
        ${LIB_SOURCE_DIR}/CAttributes.cpp
        ${LIB_SOURCE_DIR}/CB3DMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CBillboardSceneNode.cpp
        ${LIB_SOURCE_DIR}/CBoneSceneNode.cpp
        ${LIB_SOURCE_DIR}/CBSPMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CBurningShader_Raster_Reference.cpp
        ${LIB_SOURCE_DIR}/CCameraSceneNode.cpp
        ${LIB_SOURCE_DIR}/CCgMaterialRenderer.cpp
        ${LIB_SOURCE_DIR}/CColladaFileLoader.cpp
        ${LIB_SOURCE_DIR}/CColladaMeshWriter.cpp
        ${LIB_SOURCE_DIR}/CColorConverter.cpp
        ${LIB_SOURCE_DIR}/CCSMLoader.cpp
        ${LIB_SOURCE_DIR}/CCubeSceneNode.cpp
        ${LIB_SOURCE_DIR}/CD3D8Driver.cpp
        ${LIB_SOURCE_DIR}/CD3D8NormalMapRenderer.cpp
        ${LIB_SOURCE_DIR}/CD3D8ParallaxMapRenderer.cpp
        ${LIB_SOURCE_DIR}/CD3D8ShaderMaterialRenderer.cpp
        ${LIB_SOURCE_DIR}/CD3D8Texture.cpp
        ${LIB_SOURCE_DIR}/CD3D9CgMaterialRenderer.cpp
        ${LIB_SOURCE_DIR}/CD3D9Driver.cpp
        ${LIB_SOURCE_DIR}/CD3D9HLSLMaterialRenderer.cpp
        ${LIB_SOURCE_DIR}/CD3D9NormalMapRenderer.cpp
        ${LIB_SOURCE_DIR}/CD3D9ParallaxMapRenderer.cpp
        ${LIB_SOURCE_DIR}/CD3D9ShaderMaterialRenderer.cpp
        ${LIB_SOURCE_DIR}/CD3D9Texture.cpp
        ${LIB_SOURCE_DIR}/CDefaultGUIElementFactory.cpp
        ${LIB_SOURCE_DIR}/CDefaultSceneNodeAnimatorFactory.cpp
        ${LIB_SOURCE_DIR}/CDefaultSceneNodeFactory.cpp
        ${LIB_SOURCE_DIR}/CDepthBuffer.cpp
        ${LIB_SOURCE_DIR}/CDMFLoader.cpp
        ${LIB_SOURCE_DIR}/CDummyTransformationSceneNode.cpp
        ${LIB_SOURCE_DIR}/CEmptySceneNode.cpp
        ${LIB_SOURCE_DIR}/CFileList.cpp
        ${LIB_SOURCE_DIR}/CFileSystem.cpp
        ${LIB_SOURCE_DIR}/CFPSCounter.cpp
        ${LIB_SOURCE_DIR}/CGeometryCreator.cpp
        ${LIB_SOURCE_DIR}/CGUIButton.cpp
        ${LIB_SOURCE_DIR}/CGUICheckBox.cpp
        ${LIB_SOURCE_DIR}/CGUIColorSelectDialog.cpp
        ${LIB_SOURCE_DIR}/CGUIComboBox.cpp
        ${LIB_SOURCE_DIR}/CGUIContextMenu.cpp
        ${LIB_SOURCE_DIR}/CGUIEditBox.cpp
        ${LIB_SOURCE_DIR}/CGUIEnvironment.cpp
        ${LIB_SOURCE_DIR}/CGUIFileOpenDialog.cpp
        ${LIB_SOURCE_DIR}/CGUIFont.cpp
        ${LIB_SOURCE_DIR}/CGUIImage.cpp
        ${LIB_SOURCE_DIR}/CGUIImageList.cpp
        ${LIB_SOURCE_DIR}/CGUIInOutFader.cpp
        ${LIB_SOURCE_DIR}/CGUIListBox.cpp
        ${LIB_SOURCE_DIR}/CGUIMenu.cpp
        ${LIB_SOURCE_DIR}/CGUIMeshViewer.cpp
        ${LIB_SOURCE_DIR}/CGUIMessageBox.cpp
        ${LIB_SOURCE_DIR}/CGUIModalScreen.cpp
        ${LIB_SOURCE_DIR}/CGUIScrollBar.cpp
        ${LIB_SOURCE_DIR}/CGUISkin.cpp
        ${LIB_SOURCE_DIR}/CGUISpinBox.cpp
        ${LIB_SOURCE_DIR}/CGUISpriteBank.cpp
        ${LIB_SOURCE_DIR}/CGUIStaticText.cpp
        ${LIB_SOURCE_DIR}/CGUITabControl.cpp
        ${LIB_SOURCE_DIR}/CGUITable.cpp
        ${LIB_SOURCE_DIR}/CGUIToolBar.cpp
        ${LIB_SOURCE_DIR}/CGUITreeView.cpp
        ${LIB_SOURCE_DIR}/CGUIWindow.cpp
        ${LIB_SOURCE_DIR}/CImage.cpp
        ${LIB_SOURCE_DIR}/CImageLoaderBMP.cpp
        ${LIB_SOURCE_DIR}/CImageLoaderDDS.cpp
        ${LIB_SOURCE_DIR}/CImageLoaderJPG.cpp
        ${LIB_SOURCE_DIR}/CImageLoaderPCX.cpp
        ${LIB_SOURCE_DIR}/CImageLoaderPNG.cpp
        ${LIB_SOURCE_DIR}/CImageLoaderPPM.cpp
        ${LIB_SOURCE_DIR}/CImageLoaderPSD.cpp
        ${LIB_SOURCE_DIR}/CImageLoaderRGB.cpp
        ${LIB_SOURCE_DIR}/CImageLoaderTGA.cpp
        ${LIB_SOURCE_DIR}/CImageLoaderWAL.cpp
        ${LIB_SOURCE_DIR}/CImageWriterBMP.cpp
        ${LIB_SOURCE_DIR}/CImageWriterJPG.cpp
        ${LIB_SOURCE_DIR}/CImageWriterPCX.cpp
        ${LIB_SOURCE_DIR}/CImageWriterPNG.cpp
        ${LIB_SOURCE_DIR}/CImageWriterPPM.cpp
        ${LIB_SOURCE_DIR}/CImageWriterPSD.cpp
        ${LIB_SOURCE_DIR}/CImageWriterTGA.cpp
        ${LIB_SOURCE_DIR}/CIrrDeviceConsole.cpp
        ${LIB_SOURCE_DIR}/CIrrDeviceFB.cpp
        ${LIB_SOURCE_DIR}/CIrrDeviceLinux.cpp
        ${LIB_SOURCE_DIR}/CIrrDeviceSDL.cpp
        ${LIB_SOURCE_DIR}/CIrrDeviceStub.cpp
        ${LIB_SOURCE_DIR}/CIrrDeviceWin32.cpp
        ${LIB_SOURCE_DIR}/CIrrDeviceWinCE.cpp
        ${LIB_SOURCE_DIR}/CIrrMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CIrrMeshWriter.cpp
        ${LIB_SOURCE_DIR}/CLightSceneNode.cpp
        ${LIB_SOURCE_DIR}/CLimitReadFile.cpp
        ${LIB_SOURCE_DIR}/CLMTSMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CLogger.cpp
        ${LIB_SOURCE_DIR}/CLWOMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CMD2MeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CMD3MeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CMemoryFile.cpp
        ${LIB_SOURCE_DIR}/CMeshCache.cpp
        ${LIB_SOURCE_DIR}/CMeshManipulator.cpp
        ${LIB_SOURCE_DIR}/CMeshSceneNode.cpp
        ${LIB_SOURCE_DIR}/CMetaTriangleSelector.cpp
        ${LIB_SOURCE_DIR}/CMountPointReader.cpp
        ${LIB_SOURCE_DIR}/CMS3DMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CMY3DMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CNPKReader.cpp
        ${LIB_SOURCE_DIR}/CNullDriver.cpp
        ${LIB_SOURCE_DIR}/COBJMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/COBJMeshWriter.cpp
        ${LIB_SOURCE_DIR}/COCTLoader.cpp
        ${LIB_SOURCE_DIR}/COctreeSceneNode.cpp
        ${LIB_SOURCE_DIR}/COctreeTriangleSelector.cpp
        ${LIB_SOURCE_DIR}/COgreMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/COpenGLCgMaterialRenderer.cpp
        ${LIB_SOURCE_DIR}/COpenGLDriver.cpp
        ${LIB_SOURCE_DIR}/COpenGLExtensionHandler.cpp
        ${LIB_SOURCE_DIR}/COpenGLNormalMapRenderer.cpp
        ${LIB_SOURCE_DIR}/COpenGLParallaxMapRenderer.cpp
        ${LIB_SOURCE_DIR}/COpenGLShaderMaterialRenderer.cpp
        ${LIB_SOURCE_DIR}/COpenGLSLMaterialRenderer.cpp
        ${LIB_SOURCE_DIR}/COpenGLTexture.cpp
        ${LIB_SOURCE_DIR}/COSOperator.cpp
        ${LIB_SOURCE_DIR}/CPakReader.cpp
        ${LIB_SOURCE_DIR}/CParticleAnimatedMeshSceneNodeEmitter.cpp
        ${LIB_SOURCE_DIR}/CParticleAttractionAffector.cpp
        ${LIB_SOURCE_DIR}/CParticleBoxEmitter.cpp
        ${LIB_SOURCE_DIR}/CParticleCylinderEmitter.cpp
        ${LIB_SOURCE_DIR}/CParticleFadeOutAffector.cpp
        ${LIB_SOURCE_DIR}/CParticleGravityAffector.cpp
        ${LIB_SOURCE_DIR}/CParticleMeshEmitter.cpp
        ${LIB_SOURCE_DIR}/CParticlePointEmitter.cpp
        ${LIB_SOURCE_DIR}/CParticleRingEmitter.cpp
        ${LIB_SOURCE_DIR}/CParticleRotationAffector.cpp
        ${LIB_SOURCE_DIR}/CParticleScaleAffector.cpp
        ${LIB_SOURCE_DIR}/CParticleSphereEmitter.cpp
        ${LIB_SOURCE_DIR}/CParticleSystemSceneNode.cpp
        ${LIB_SOURCE_DIR}/CPLYMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CPLYMeshWriter.cpp
        ${LIB_SOURCE_DIR}/CQ3LevelMesh.cpp
        ${LIB_SOURCE_DIR}/CQuake3ShaderSceneNode.cpp
        ${LIB_SOURCE_DIR}/CReadFile.cpp
        ${LIB_SOURCE_DIR}/CSceneCollisionManager.cpp
        ${LIB_SOURCE_DIR}/CSceneLoaderIrr.cpp
        ${LIB_SOURCE_DIR}/CSceneManager.cpp
        ${LIB_SOURCE_DIR}/CSceneNodeAnimatorCameraFPS.cpp
        ${LIB_SOURCE_DIR}/CSceneNodeAnimatorCameraMaya.cpp
        ${LIB_SOURCE_DIR}/CSceneNodeAnimatorCollisionResponse.cpp
        ${LIB_SOURCE_DIR}/CSceneNodeAnimatorDelete.cpp
        ${LIB_SOURCE_DIR}/CSceneNodeAnimatorFlyCircle.cpp
        ${LIB_SOURCE_DIR}/CSceneNodeAnimatorFlyStraight.cpp
        ${LIB_SOURCE_DIR}/CSceneNodeAnimatorFollowSpline.cpp
        ${LIB_SOURCE_DIR}/CSceneNodeAnimatorRotation.cpp
        ${LIB_SOURCE_DIR}/CSceneNodeAnimatorTexture.cpp
        ${LIB_SOURCE_DIR}/CShadowVolumeSceneNode.cpp
        ${LIB_SOURCE_DIR}/CSkinnedMesh.cpp
        ${LIB_SOURCE_DIR}/CSkyBoxSceneNode.cpp
        ${LIB_SOURCE_DIR}/CSkyDomeSceneNode.cpp
        ${LIB_SOURCE_DIR}/CSMFMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CSoftwareDriver.cpp
        ${LIB_SOURCE_DIR}/CSoftwareDriver2.cpp
        ${LIB_SOURCE_DIR}/CSoftwareTexture.cpp
        ${LIB_SOURCE_DIR}/CSoftwareTexture2.cpp
        ${LIB_SOURCE_DIR}/CSphereSceneNode.cpp
        ${LIB_SOURCE_DIR}/CSTLMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CSTLMeshWriter.cpp
        ${LIB_SOURCE_DIR}/CTarReader.cpp
        ${LIB_SOURCE_DIR}/CTerrainSceneNode.cpp
        ${LIB_SOURCE_DIR}/CTerrainTriangleSelector.cpp
        ${LIB_SOURCE_DIR}/CTextSceneNode.cpp
        ${LIB_SOURCE_DIR}/CTRFlat.cpp
        ${LIB_SOURCE_DIR}/CTRFlatWire.cpp
        ${LIB_SOURCE_DIR}/CTRGouraud.cpp
        ${LIB_SOURCE_DIR}/CTRGouraud2.cpp
        ${LIB_SOURCE_DIR}/CTRGouraudAlpha2.cpp
        ${LIB_SOURCE_DIR}/CTRGouraudAlphaNoZ2.cpp
        ${LIB_SOURCE_DIR}/CTRGouraudWire.cpp
        ${LIB_SOURCE_DIR}/CTriangleBBSelector.cpp
        ${LIB_SOURCE_DIR}/CTriangleBBSelector.h
        ${LIB_SOURCE_DIR}/CTriangleSelector.cpp
        ${LIB_SOURCE_DIR}/CTriangleSelector.h
        ${LIB_SOURCE_DIR}/CTRNormalMap.cpp
        ${LIB_SOURCE_DIR}/CTRStencilShadow.cpp
        ${LIB_SOURCE_DIR}/CTRTextureBlend.cpp
        ${LIB_SOURCE_DIR}/CTRTextureDetailMap2.cpp
        ${LIB_SOURCE_DIR}/CTRTextureFlat.cpp
        ${LIB_SOURCE_DIR}/CTRTextureFlatWire.cpp
        ${LIB_SOURCE_DIR}/CTRTextureGouraud.cpp
        ${LIB_SOURCE_DIR}/CTRTextureGouraud.h
        ${LIB_SOURCE_DIR}/CTRTextureGouraud2.cpp
        ${LIB_SOURCE_DIR}/CTRTextureGouraudAdd.cpp
        ${LIB_SOURCE_DIR}/CTRTextureGouraudAdd2.cpp
        ${LIB_SOURCE_DIR}/CTRTextureGouraudAddNoZ2.cpp
        ${LIB_SOURCE_DIR}/CTRTextureGouraudAlpha.cpp
        ${LIB_SOURCE_DIR}/CTRTextureGouraudAlphaNoZ.cpp
        ${LIB_SOURCE_DIR}/CTRTextureGouraudNoZ.cpp
        ${LIB_SOURCE_DIR}/CTRTextureGouraudNoZ2.cpp
        ${LIB_SOURCE_DIR}/CTRTextureGouraudVertexAlpha2.cpp
        ${LIB_SOURCE_DIR}/CTRTextureGouraudWire.cpp
        ${LIB_SOURCE_DIR}/CTRTextureLightMap2_Add.cpp
        ${LIB_SOURCE_DIR}/CTRTextureLightMap2_M1.cpp
        ${LIB_SOURCE_DIR}/CTRTextureLightMap2_M2.cpp
        ${LIB_SOURCE_DIR}/CTRTextureLightMap2_M4.cpp
        ${LIB_SOURCE_DIR}/CTRTextureLightMapGouraud2_M4.cpp
        ${LIB_SOURCE_DIR}/CTRTextureWire2.cpp
        ${LIB_SOURCE_DIR}/CVideoModeList.cpp
        ${LIB_SOURCE_DIR}/CVolumeLightSceneNode.cpp
        ${LIB_SOURCE_DIR}/CWADReader.cpp
        ${LIB_SOURCE_DIR}/CWaterSurfaceSceneNode.cpp
        ${LIB_SOURCE_DIR}/CWriteFile.cpp
        ${LIB_SOURCE_DIR}/CXMeshFileLoader.cpp
        ${LIB_SOURCE_DIR}/CXMLReader.cpp
        ${LIB_SOURCE_DIR}/CXMLWriter.cpp
        ${LIB_SOURCE_DIR}/CZBuffer.cpp
        ${LIB_SOURCE_DIR}/CZipReader.cpp
        ${LIB_SOURCE_DIR}/IBurningShader.cpp
        ${LIB_SOURCE_DIR}/Irrlicht.cpp
        ${LIB_SOURCE_DIR}/irrXML.cpp
        ${LIB_SOURCE_DIR}/os.cpp
    )

    if (BUILD_SHARED)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_IRRLICHT=1)
    target_compile_definitions(${LIB_NAME} PUBLIC _IRR_STATIC_LIB_=1)

    if (MSVC)
	else()
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-but-set-variable)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-unused-function)
        # target_compile_options(${LIB_NAME} PRIVATE -Wno-maybe-uninitialized)
        target_compile_options(${LIB_NAME} PRIVATE -Wno-strict-aliasing)
        if ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.1))
            target_link_libraries(${LIB_NAME} PRIVATE stdc++fs)
        endif()

        target_link_libraries(${LIB_NAME} PUBLIC pthread)
        target_link_libraries(${LIB_NAME} PUBLIC m)
    endif()
    
    if(WIN32)
		# _UNICODE und _MBCS nicht definiert
		target_compile_definitions(${LIB_NAME} PUBLIC _UNICODE=1)
		target_compile_definitions(${LIB_NAME} PUBLIC UNICODE=1)	
        target_link_libraries(${LIB_NAME} PUBLIC glu32 opengl32 uuid ole32 oleaut32 ComCtl32 SetupAPI)
        target_link_libraries(${LIB_NAME} PUBLIC winmm user32 gdi32 kernel32 advapi32 shell32)
    else() # on Linux:
        target_link_libraries(${LIB_NAME} PUBLIC GLU GL)
        target_link_libraries(${LIB_NAME} PRIVATE X11 Xxf86vm Xi Xcursor)
    endif()
	
    target_link_libraries(${LIB_NAME} PUBLIC de_zlib)
    target_link_libraries(${LIB_NAME} PUBLIC de_bzip2)
    target_link_libraries(${LIB_NAME} PUBLIC de_png)
    target_link_libraries(${LIB_NAME} PUBLIC de_jpeg8d)

    # COPY MEDIA FILES to
    #file( COPY        ${CMAKE_CURRENT_LIST_DIR}/res/irrlicht.ico
    #      DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )

endif()

add_subdirectory(test)
