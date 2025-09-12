set(LIB_NAME de_assimp)
project(${LIB_NAME} CXX) # Depends on: utf8cpp clipper poly2tri open3dgc openddlparser irrXML rapidjson unzip minizip z

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    option(BUILD_SHARED_LIBS "Build package with shared libraries." ON )
    option(BUILD_FRAMEWORK "Build package as Mac OS X Framework bundle." OFF)
    option(ASSIMP_DOUBLE_PRECISION "Set to ON to enable double precision processing" OFF)
    option(ASSIMP_OPT_BUILD_PACKAGES "Set to ON to generate CPack configuration files and packaging targets" OFF)
    option(ASSIMP_ANDROID_JNIIOSYSTEM "Android JNI IOSystem support is active" OFF)
    option(ASSIMP_NO_EXPORT "Disable Assimp's export functionality." OFF)
    option(ASSIMP_BUILD_ZLIB "Build your own zlib" OFF)
    option(ASSIMP_BUILD_ASSIMP_TOOLS "If the supplementary tools for Assimp are built in addition to the library." ON)
    option(ASSIMP_BUILD_SAMPLES "If the official samples are built as well (needs Glut)." OFF)
    option(ASSIMP_BUILD_TESTS "If the test suite for Assimp is built in addition to the library." ON)
    option(ASSIMP_COVERALLS "Enable this to measure test coverage." OFF)
    option(ASSIMP_WERROR "Treat warnings as errors." OFF)
    option(ASSIMP_ASAN "Enable AddressSanitizer." OFF)
    option(ASSIMP_UBSAN "Enable Undefined Behavior sanitizer." OFF)
    option(SYSTEM_IRRXML "Use system installed Irrlicht/IrrXML library." OFF)
    option(BUILD_DOCS "Build documentation using Doxygen." OFF)

    ##### add_definitions(-DASSIMP_DOUBLE_PRECISION)

    # Needed for openddl_parser config, no use of c++11 at this moment
    # add_definitions( -DOPENDDL_NO_USE_CPP11 )
    # set_property( GLOBAL PROPERTY CXX_STANDARD 11 )

    # add_definitions( -DASSIMP_BUILD_DLL_EXPORT )

    # Define here the needed parameters
    set(LINK_SEARCH_START_STATIC TRUE)
    set(ASSIMP_VERSION_MAJOR 4)
    set(ASSIMP_VERSION_MINOR 1)
    set(ASSIMP_VERSION_PATCH 0)
    set(ASSIMP_VERSION ${ASSIMP_VERSION_MAJOR}.${ASSIMP_VERSION_MINOR}.${ASSIMP_VERSION_PATCH})
    set(ASSIMP_SOVERSION 4)
    set(PROJECT_VERSION "${ASSIMP_VERSION}")
    set(ASSIMP_PACKAGE_VERSION "0" CACHE STRING "the package-specific version used for uploading the sources" )

    # LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules" )
    # set(LIBASSIMP_COMPONENT     "libassimp${ASSIMP_VERSION_MAJOR}.${ASSIMP_VERSION_MINOR}.${ASSIMP_VERSION_PATCH}" )
    # set(LIBASSIMP-DEV_COMPONENT "libassimp${ASSIMP_VERSION_MAJOR}.${ASSIMP_VERSION_MINOR}.${ASSIMP_VERSION_PATCH}-dev" )
    # set(CPACK_COMPONENTS_ALL assimp-bin ${LIBASSIMP_COMPONENT} ${LIBASSIMP-DEV_COMPONENT} assimp-dev)
    set(ASSIMP_LIBRARY_SUFFIX "" CACHE STRING "Suffix to append to library names")

    # Get the current working branch
    EXECUTE_PROCESS(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    )

    # Get the latest abbreviated commit hash of the working branch
    EXECUTE_PROCESS(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    )

    IF (NOT GIT_COMMIT_HASH)
    set(GIT_COMMIT_HASH 0)
    ENDIF()

    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include/assimp)
    
    CONFIGURE_FILE(
        ${LIB_HEADER_DIR}/revision.h.in
        ${LIB_HEADER_DIR}/revision.h
    )

    CONFIGURE_FILE(
        ${LIB_HEADER_DIR}/config.h.in
        ${LIB_HEADER_DIR}/config.h
    )


    set(LIB_HEADERS
    ${LIB_HEADER_DIR}/Compiler/pushpack1.h
    ${LIB_HEADER_DIR}/Compiler/poppack1.h
    ${LIB_HEADER_DIR}/Compiler/pstdint.h
    ${LIB_HEADER_DIR}/anim.h
    ${LIB_HEADER_DIR}/ai_assert.h
    ${LIB_HEADER_DIR}/camera.h
    ${LIB_HEADER_DIR}/color4.h
    ${LIB_HEADER_DIR}/color4.inl
    ${LIB_HEADER_DIR}/config.h
    ${LIB_HEADER_DIR}/defs.h
    ${LIB_HEADER_DIR}/Defines.h
    ${LIB_HEADER_DIR}/cfileio.h
    ${LIB_HEADER_DIR}/light.h
    ${LIB_HEADER_DIR}/material.h
    ${LIB_HEADER_DIR}/material.inl
    ${LIB_HEADER_DIR}/matrix3x3.h
    ${LIB_HEADER_DIR}/matrix3x3.inl
    ${LIB_HEADER_DIR}/matrix4x4.h
    ${LIB_HEADER_DIR}/matrix4x4.inl
    ${LIB_HEADER_DIR}/mesh.h
    ${LIB_HEADER_DIR}/postprocess.h
    ${LIB_HEADER_DIR}/quaternion.h
    ${LIB_HEADER_DIR}/quaternion.inl
    ${LIB_HEADER_DIR}/scene.h
    ${LIB_HEADER_DIR}/metadata.h
    ${LIB_HEADER_DIR}/texture.h
    ${LIB_HEADER_DIR}/types.h
    ${LIB_HEADER_DIR}/vector2.h
    ${LIB_HEADER_DIR}/vector2.inl
    ${LIB_HEADER_DIR}/vector3.h
    ${LIB_HEADER_DIR}/vector3.inl
    ${LIB_HEADER_DIR}/version.h
    ${LIB_HEADER_DIR}/cimport.h
    ${LIB_HEADER_DIR}/importerdesc.h
    ${LIB_HEADER_DIR}/Importer.hpp
    ${LIB_HEADER_DIR}/DefaultLogger.hpp
    ${LIB_HEADER_DIR}/ProgressHandler.hpp
    ${LIB_HEADER_DIR}/IOStream.hpp
    ${LIB_HEADER_DIR}/IOSystem.hpp
    ${LIB_HEADER_DIR}/Logger.hpp
    ${LIB_HEADER_DIR}/LogStream.hpp
    ${LIB_HEADER_DIR}/NullLogger.hpp
    ${LIB_HEADER_DIR}/cexport.h
    ${LIB_HEADER_DIR}/Exporter.hpp
    ${LIB_HEADER_DIR}/DefaultIOStream.h
    ${LIB_HEADER_DIR}/DefaultIOSystem.h
    ${LIB_HEADER_DIR}/SceneCombiner.h
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
    # Core_SRCS
    ${LIB_SOURCE_DIR}/Assimp.cpp
    # Logging_SRCS
    ${LIB_HEADER_DIR}/DefaultLogger.hpp
    ${LIB_HEADER_DIR}/LogStream.hpp
    ${LIB_HEADER_DIR}/Logger.hpp
    ${LIB_HEADER_DIR}/NullLogger.hpp
    ${LIB_SOURCE_DIR}/Win32DebugLogStream.h
    ${LIB_SOURCE_DIR}/DefaultLogger.cpp
    ${LIB_SOURCE_DIR}/FileLogStream.h
    ${LIB_SOURCE_DIR}/StdOStreamLogStream.h
    ${LIB_SOURCE_DIR}/irrXMLWrapper.h
    # Common_SRCS
    ${LIB_SOURCE_DIR}/fast_atof.h
    ${LIB_SOURCE_DIR}/qnan.h
    ${LIB_SOURCE_DIR}/BaseImporter.cpp
    ${LIB_SOURCE_DIR}/BaseImporter.h
    ${LIB_SOURCE_DIR}/BaseProcess.cpp
    ${LIB_SOURCE_DIR}/BaseProcess.h
    ${LIB_SOURCE_DIR}/Importer.h
    ${LIB_SOURCE_DIR}/ScenePrivate.h
    ${LIB_SOURCE_DIR}/PostStepRegistry.cpp
    ${LIB_SOURCE_DIR}/ImporterRegistry.cpp
    ${LIB_SOURCE_DIR}/ByteSwapper.h
    ${LIB_SOURCE_DIR}/DefaultProgressHandler.h
    ${LIB_SOURCE_DIR}/DefaultIOStream.cpp
    ${LIB_SOURCE_DIR}/DefaultIOSystem.cpp
    ${LIB_SOURCE_DIR}/CInterfaceIOWrapper.cpp
    ${LIB_SOURCE_DIR}/CInterfaceIOWrapper.h
    ${LIB_SOURCE_DIR}/Hash.h
    ${LIB_SOURCE_DIR}/Importer.cpp
    ${LIB_SOURCE_DIR}/IFF.h
    ${LIB_SOURCE_DIR}/MemoryIOWrapper.h
    ${LIB_SOURCE_DIR}/ParsingUtils.h
    ${LIB_SOURCE_DIR}/StreamReader.h
    ${LIB_SOURCE_DIR}/StreamWriter.h
    ${LIB_SOURCE_DIR}/StringComparison.h
    ${LIB_SOURCE_DIR}/StringUtils.h
    ${LIB_SOURCE_DIR}/SGSpatialSort.cpp
    ${LIB_SOURCE_DIR}/SGSpatialSort.h
    ${LIB_SOURCE_DIR}/VertexTriangleAdjacency.cpp
    ${LIB_SOURCE_DIR}/VertexTriangleAdjacency.h
    ${LIB_SOURCE_DIR}/GenericProperty.h
    ${LIB_SOURCE_DIR}/SpatialSort.cpp
    ${LIB_SOURCE_DIR}/SpatialSort.h
    ${LIB_SOURCE_DIR}/SceneCombiner.cpp
    ${LIB_SOURCE_DIR}/ScenePreprocessor.cpp
    ${LIB_SOURCE_DIR}/ScenePreprocessor.h
    ${LIB_SOURCE_DIR}/SkeletonMeshBuilder.cpp
    ${LIB_SOURCE_DIR}/SkeletonMeshBuilder.h
    ${LIB_SOURCE_DIR}/SplitByBoneCountProcess.cpp
    ${LIB_SOURCE_DIR}/SplitByBoneCountProcess.h
    ${LIB_SOURCE_DIR}/ScaleProcess.cpp
    ${LIB_SOURCE_DIR}/ScaleProcess.h
    ${LIB_SOURCE_DIR}/SmoothingGroups.h
    ${LIB_SOURCE_DIR}/StandardShapes.cpp
    ${LIB_SOURCE_DIR}/StandardShapes.h
    ${LIB_SOURCE_DIR}/TargetAnimation.cpp
    ${LIB_SOURCE_DIR}/TargetAnimation.h
    ${LIB_SOURCE_DIR}/RemoveComments.cpp
    ${LIB_SOURCE_DIR}/RemoveComments.h
    ${LIB_SOURCE_DIR}/Subdivision.cpp
    ${LIB_SOURCE_DIR}/Subdivision.h
    ${LIB_SOURCE_DIR}/scene.cpp
    ${LIB_SOURCE_DIR}/Vertex.h
    ${LIB_SOURCE_DIR}/LineSplitter.h
    ${LIB_SOURCE_DIR}/TinyFormatter.h
    ${LIB_SOURCE_DIR}/Profiler.h
    ${LIB_SOURCE_DIR}/LogAux.h
    ${LIB_SOURCE_DIR}/Bitmap.cpp
    ${LIB_SOURCE_DIR}/Bitmap.h
    ${LIB_SOURCE_DIR}/XMLTools.h
    ${LIB_SOURCE_DIR}/Version.cpp
    ${LIB_SOURCE_DIR}/IOStreamBuffer.h
    ${LIB_SOURCE_DIR}/CreateAnimMesh.h
    ${LIB_SOURCE_DIR}/CreateAnimMesh.cpp

    # # AMF
    # ${LIB_SOURCE_DIR}/AMFImporter.hpp
    # ${LIB_SOURCE_DIR}/AMFImporter_Macro.hpp
    # ${LIB_SOURCE_DIR}/AMFImporter_Node.hpp
    # ${LIB_SOURCE_DIR}/AMFImporter.cpp
    # ${LIB_SOURCE_DIR}/AMFImporter_Geometry.cpp
    # ${LIB_SOURCE_DIR}/AMFImporter_Material.cpp
    # ${LIB_SOURCE_DIR}/AMFImporter_Postprocess.cpp

    # # IFC ( do i need it for STEP? )
    # ${LIB_SOURCE_DIR}/IFCLoader.cpp
    # ${LIB_SOURCE_DIR}/IFCLoader.h
    # ${LIB_SOURCE_DIR}/IFCReaderGen1.cpp
    # ${LIB_SOURCE_DIR}/IFCReaderGen2.cpp
    # ${LIB_SOURCE_DIR}/IFCReaderGen.h
    # ${LIB_SOURCE_DIR}/IFCUtil.h
    # ${LIB_SOURCE_DIR}/IFCUtil.cpp
    # ${LIB_SOURCE_DIR}/IFCGeometry.cpp
    # ${LIB_SOURCE_DIR}/IFCMaterial.cpp
    # ${LIB_SOURCE_DIR}/IFCProfile.cpp
    # ${LIB_SOURCE_DIR}/IFCCurve.cpp
    # ${LIB_SOURCE_DIR}/IFCBoolean.cpp
    # ${LIB_SOURCE_DIR}/IFCOpenings.cpp
    # GLTF
    ${LIB_SOURCE_DIR}/glTFAsset.h
    ${LIB_SOURCE_DIR}/glTFAsset.inl
    ${LIB_SOURCE_DIR}/glTFAssetWriter.h
    ${LIB_SOURCE_DIR}/glTFAssetWriter.inl
    ${LIB_SOURCE_DIR}/glTFImporter.cpp
    ${LIB_SOURCE_DIR}/glTFImporter.h
    ${LIB_SOURCE_DIR}/glTFExporter.h
    ${LIB_SOURCE_DIR}/glTFExporter.cpp
    ${LIB_SOURCE_DIR}/glTF2Asset.h
    ${LIB_SOURCE_DIR}/glTF2Asset.inl
    ${LIB_SOURCE_DIR}/glTF2AssetWriter.h
    ${LIB_SOURCE_DIR}/glTF2AssetWriter.inl
    ${LIB_SOURCE_DIR}/glTF2Importer.cpp
    ${LIB_SOURCE_DIR}/glTF2Importer.h
    ${LIB_SOURCE_DIR}/glTF2Exporter.h
    ${LIB_SOURCE_DIR}/glTF2Exporter.cpp
    # # FBX ?
    # ${LIB_SOURCE_DIR}/FBXImporter.cpp
    # ${LIB_SOURCE_DIR}/FBXCompileConfig.h
    # ${LIB_SOURCE_DIR}/FBXImporter.h
    # ${LIB_SOURCE_DIR}/FBXParser.cpp
    # ${LIB_SOURCE_DIR}/FBXParser.h
    # ${LIB_SOURCE_DIR}/FBXTokenizer.cpp
    # ${LIB_SOURCE_DIR}/FBXTokenizer.h
    # ${LIB_SOURCE_DIR}/FBXImportSettings.h
    # ${LIB_SOURCE_DIR}/FBXConverter.h
    # ${LIB_SOURCE_DIR}/FBXConverter.cpp
    # ${LIB_SOURCE_DIR}/FBXUtil.h
    # ${LIB_SOURCE_DIR}/FBXUtil.cpp
    # ${LIB_SOURCE_DIR}/FBXDocument.h
    # ${LIB_SOURCE_DIR}/FBXDocument.cpp
    # ${LIB_SOURCE_DIR}/FBXProperties.h
    # ${LIB_SOURCE_DIR}/FBXProperties.cpp
    # ${LIB_SOURCE_DIR}/FBXMeshGeometry.h
    # ${LIB_SOURCE_DIR}/FBXMeshGeometry.cpp
    # ${LIB_SOURCE_DIR}/FBXMaterial.cpp
    # ${LIB_SOURCE_DIR}/FBXModel.cpp
    # ${LIB_SOURCE_DIR}/FBXAnimation.cpp
    # ${LIB_SOURCE_DIR}/FBXNodeAttribute.cpp
    # ${LIB_SOURCE_DIR}/FBXDeformer.cpp
    # ${LIB_SOURCE_DIR}/FBXBinaryTokenizer.cpp
    # ${LIB_SOURCE_DIR}/FBXDocumentUtil.cpp

    # Non Free C4D sources
    ${LIB_SOURCE_DIR}/C4DImporter.cpp
    ${LIB_SOURCE_DIR}/C4DImporter.h

    # OFF
    ${LIB_SOURCE_DIR}/OFFLoader.cpp
    ${LIB_SOURCE_DIR}/OFFLoader.h

    # 3DS
    ${LIB_SOURCE_DIR}/3DSConverter.cpp
    ${LIB_SOURCE_DIR}/3DSHelper.h
    ${LIB_SOURCE_DIR}/3DSLoader.cpp
    ${LIB_SOURCE_DIR}/3DSLoader.h
    ${LIB_SOURCE_DIR}/3DSExporter.h
    ${LIB_SOURCE_DIR}/3DSExporter.cpp

    # STEP
    ${LIB_SOURCE_DIR}/STEPFile.h
    ${LIB_SOURCE_DIR}/STEPFileReader.h
    ${LIB_SOURCE_DIR}/STEPFileReader.cpp
    ${LIB_SOURCE_DIR}/STEPFileEncoding.cpp
    ${LIB_SOURCE_DIR}/STEPFileEncoding.h
    # PLY
    ${LIB_SOURCE_DIR}/PlyLoader.cpp
    ${LIB_SOURCE_DIR}/PlyLoader.h
    ${LIB_SOURCE_DIR}/PlyParser.cpp
    ${LIB_SOURCE_DIR}/PlyParser.h
    ${LIB_SOURCE_DIR}/PlyExporter.cpp
    ${LIB_SOURCE_DIR}/PlyExporter.h
    # MS3D
    ${LIB_SOURCE_DIR}/MS3DLoader.cpp
    ${LIB_SOURCE_DIR}/MS3DLoader.h
    # COB
    ${LIB_SOURCE_DIR}/COBLoader.cpp
    ${LIB_SOURCE_DIR}/COBLoader.h
    ${LIB_SOURCE_DIR}/COBScene.h
    # BLEND
    ${LIB_SOURCE_DIR}/BlenderLoader.cpp
    ${LIB_SOURCE_DIR}/BlenderLoader.h
    ${LIB_SOURCE_DIR}/BlenderDNA.cpp
    ${LIB_SOURCE_DIR}/BlenderDNA.h
    ${LIB_SOURCE_DIR}/BlenderDNA.inl
    ${LIB_SOURCE_DIR}/BlenderScene.cpp
    ${LIB_SOURCE_DIR}/BlenderScene.h
    ${LIB_SOURCE_DIR}/BlenderSceneGen.h
    ${LIB_SOURCE_DIR}/BlenderIntermediate.h
    ${LIB_SOURCE_DIR}/BlenderModifier.h
    ${LIB_SOURCE_DIR}/BlenderModifier.cpp
    ${LIB_SOURCE_DIR}/BlenderBMesh.h
    ${LIB_SOURCE_DIR}/BlenderBMesh.cpp
    ${LIB_SOURCE_DIR}/BlenderTessellator.h
    ${LIB_SOURCE_DIR}/BlenderTessellator.cpp

    # B3D
    ${LIB_SOURCE_DIR}/B3DImporter.cpp
    ${LIB_SOURCE_DIR}/B3DImporter.h
    # BVH
    ${LIB_SOURCE_DIR}/BVHLoader.cpp
    ${LIB_SOURCE_DIR}/BVHLoader.h
    # DXF
    ${LIB_SOURCE_DIR}/DXFLoader.cpp
    ${LIB_SOURCE_DIR}/DXFLoader.h
    ${LIB_SOURCE_DIR}/DXFHelper.h
    # COLLADA
    ${LIB_SOURCE_DIR}/ColladaHelper.h
    ${LIB_SOURCE_DIR}/ColladaLoader.cpp
    ${LIB_SOURCE_DIR}/ColladaLoader.h
    ${LIB_SOURCE_DIR}/ColladaParser.cpp
    ${LIB_SOURCE_DIR}/ColladaParser.h
    ${LIB_SOURCE_DIR}/ColladaExporter.h
    ${LIB_SOURCE_DIR}/ColladaExporter.cpp
    # IRRMESH
    ${LIB_SOURCE_DIR}/IRRMeshLoader.cpp
    ${LIB_SOURCE_DIR}/IRRMeshLoader.h
    ${LIB_SOURCE_DIR}/IRRShared.cpp
    ${LIB_SOURCE_DIR}/IRRShared.h
    # IRR
    ${LIB_SOURCE_DIR}/IRRLoader.cpp
    ${LIB_SOURCE_DIR}/IRRLoader.h
    ${LIB_SOURCE_DIR}/IRRShared.cpp
    ${LIB_SOURCE_DIR}/IRRShared.h
    # LWO
    ${LIB_SOURCE_DIR}/LWOAnimation.cpp
    ${LIB_SOURCE_DIR}/LWOAnimation.h
    ${LIB_SOURCE_DIR}/LWOBLoader.cpp
    ${LIB_SOURCE_DIR}/LWOFileData.h
    ${LIB_SOURCE_DIR}/LWOLoader.cpp
    ${LIB_SOURCE_DIR}/LWOLoader.h
    ${LIB_SOURCE_DIR}/LWOMaterial.cpp
    # LWS
    ${LIB_SOURCE_DIR}/LWSLoader.cpp
    ${LIB_SOURCE_DIR}/LWSLoader.h
    # MD2
    ${LIB_SOURCE_DIR}/MD2FileData.h
    ${LIB_SOURCE_DIR}/MD2Loader.cpp
    ${LIB_SOURCE_DIR}/MD2Loader.h
    ${LIB_SOURCE_DIR}/MD2NormalTable.h
    # MD3
    ${LIB_SOURCE_DIR}/MD3FileData.h
    ${LIB_SOURCE_DIR}/MD3Loader.cpp
    ${LIB_SOURCE_DIR}/MD3Loader.h
    # Extra_SRCS
    ${LIB_SOURCE_DIR}/MD4FileData.h
    # MD5
    ${LIB_SOURCE_DIR}/MD5Loader.cpp
    ${LIB_SOURCE_DIR}/MD5Loader.h
    ${LIB_SOURCE_DIR}/MD5Parser.cpp
    ${LIB_SOURCE_DIR}/MD5Parser.h
    # MDC
    ${LIB_SOURCE_DIR}/MDCFileData.h
    ${LIB_SOURCE_DIR}/MDCLoader.cpp
    ${LIB_SOURCE_DIR}/MDCLoader.h
    ${LIB_SOURCE_DIR}/MDCNormalTable.h
    # MDL
    ${LIB_SOURCE_DIR}/MDLDefaultColorMap.h
    ${LIB_SOURCE_DIR}/MDLFileData.h
    ${LIB_SOURCE_DIR}/MDLLoader.cpp
    ${LIB_SOURCE_DIR}/MDLLoader.h
    ${LIB_SOURCE_DIR}/MDLMaterialLoader.cpp
    # CSM
    ${LIB_SOURCE_DIR}/CSMLoader.cpp
    ${LIB_SOURCE_DIR}/CSMLoader.h
    # HMP
    ${LIB_SOURCE_DIR}/HMPFileData.h
    ${LIB_SOURCE_DIR}/HMPLoader.cpp
    ${LIB_SOURCE_DIR}/HMPLoader.h
    ${LIB_SOURCE_DIR}/HalfLifeFileData.h
    # NFF
    ${LIB_SOURCE_DIR}/NFFLoader.cpp
    ${LIB_SOURCE_DIR}/NFFLoader.h
    # NDO
    ${LIB_SOURCE_DIR}/NDOLoader.cpp
    ${LIB_SOURCE_DIR}/NDOLoader.h
    # OBJ
    ${LIB_SOURCE_DIR}/ObjFileData.h
    ${LIB_SOURCE_DIR}/ObjFileImporter.cpp
    ${LIB_SOURCE_DIR}/ObjFileImporter.h
    ${LIB_SOURCE_DIR}/ObjFileMtlImporter.cpp
    ${LIB_SOURCE_DIR}/ObjFileMtlImporter.h
    ${LIB_SOURCE_DIR}/ObjFileParser.cpp
    ${LIB_SOURCE_DIR}/ObjFileParser.h
    ${LIB_SOURCE_DIR}/ObjTools.h
    ${LIB_SOURCE_DIR}/ObjExporter.h
    ${LIB_SOURCE_DIR}/ObjExporter.cpp
    # AC
    ${LIB_SOURCE_DIR}/ACLoader.cpp
    ${LIB_SOURCE_DIR}/ACLoader.h
    # ASE
    ${LIB_SOURCE_DIR}/ASELoader.cpp
    ${LIB_SOURCE_DIR}/ASELoader.h
    ${LIB_SOURCE_DIR}/ASEParser.cpp
    ${LIB_SOURCE_DIR}/ASEParser.h
    # ASSBIN
    ${LIB_SOURCE_DIR}/AssbinExporter.h
    ${LIB_SOURCE_DIR}/AssbinExporter.cpp
    ${LIB_SOURCE_DIR}/AssbinLoader.h
    ${LIB_SOURCE_DIR}/AssbinLoader.cpp
    # ASSXML
    ${LIB_SOURCE_DIR}/AssxmlExporter.h
    ${LIB_SOURCE_DIR}/AssxmlExporter.cpp
    # XGL
    ${LIB_SOURCE_DIR}/XGLLoader.cpp
    ${LIB_SOURCE_DIR}/XGLLoader.h
    # OPENGEX
    ${LIB_SOURCE_DIR}/OpenGEXExporter.cpp
    ${LIB_SOURCE_DIR}/OpenGEXExporter.h
    ${LIB_SOURCE_DIR}/OpenGEXImporter.cpp
    ${LIB_SOURCE_DIR}/OpenGEXImporter.h
    ${LIB_SOURCE_DIR}/OpenGEXStructs.h
    # OGRE
    ${LIB_SOURCE_DIR}/OgreImporter.h
    ${LIB_SOURCE_DIR}/OgreStructs.h
    ${LIB_SOURCE_DIR}/OgreParsingUtils.h
    ${LIB_SOURCE_DIR}/OgreBinarySerializer.h
    ${LIB_SOURCE_DIR}/OgreXmlSerializer.h
    ${LIB_SOURCE_DIR}/OgreImporter.cpp
    ${LIB_SOURCE_DIR}/OgreStructs.cpp
    ${LIB_SOURCE_DIR}/OgreBinarySerializer.cpp
    ${LIB_SOURCE_DIR}/OgreXmlSerializer.cpp
    ${LIB_SOURCE_DIR}/OgreMaterial.cpp
    # Q3D
    ${LIB_SOURCE_DIR}/Q3DLoader.cpp
    ${LIB_SOURCE_DIR}/Q3DLoader.h
    # Q3BSP
    ${LIB_SOURCE_DIR}/Q3BSPFileData.h
    ${LIB_SOURCE_DIR}/Q3BSPFileParser.h
    ${LIB_SOURCE_DIR}/Q3BSPFileParser.cpp
    ${LIB_SOURCE_DIR}/Q3BSPFileImporter.h
    ${LIB_SOURCE_DIR}/Q3BSPFileImporter.cpp
    ${LIB_SOURCE_DIR}/Q3BSPZipArchive.h
    ${LIB_SOURCE_DIR}/Q3BSPZipArchive.cpp
    # RAW
    ${LIB_SOURCE_DIR}/RawLoader.cpp
    ${LIB_SOURCE_DIR}/RawLoader.h
    # SIB
    ${LIB_SOURCE_DIR}/SIBImporter.cpp
    ${LIB_SOURCE_DIR}/SIBImporter.h
    # SMD
    ${LIB_SOURCE_DIR}/SMDLoader.cpp
    ${LIB_SOURCE_DIR}/SMDLoader.h
    # STL
    ${LIB_SOURCE_DIR}/STLLoader.cpp
    ${LIB_SOURCE_DIR}/STLLoader.h
    ${LIB_SOURCE_DIR}/STLExporter.h
    ${LIB_SOURCE_DIR}/STLExporter.cpp
    # TERRAGEN
    ${LIB_SOURCE_DIR}/TerragenLoader.cpp
    ${LIB_SOURCE_DIR}/TerragenLoader.h
    # 3D
    ${LIB_SOURCE_DIR}/UnrealLoader.cpp
    ${LIB_SOURCE_DIR}/UnrealLoader.h
    # X
    ${LIB_SOURCE_DIR}/XFileHelper.h
    ${LIB_SOURCE_DIR}/XFileImporter.cpp
    ${LIB_SOURCE_DIR}/XFileImporter.h
    ${LIB_SOURCE_DIR}/XFileParser.cpp
    ${LIB_SOURCE_DIR}/XFileParser.h
    ${LIB_SOURCE_DIR}/XFileExporter.h
    ${LIB_SOURCE_DIR}/XFileExporter.cpp
    # FI
    ${LIB_SOURCE_DIR}/FIReader.hpp
    ${LIB_SOURCE_DIR}/FIReader.cpp
    # X3D
    ${LIB_SOURCE_DIR}/X3DExporter.cpp
    ${LIB_SOURCE_DIR}/X3DExporter.hpp
    ${LIB_SOURCE_DIR}/X3DImporter.cpp
    ${LIB_SOURCE_DIR}/X3DImporter.hpp
    ${LIB_SOURCE_DIR}/X3DImporter_Geometry2D.cpp
    ${LIB_SOURCE_DIR}/X3DImporter_Geometry3D.cpp
    ${LIB_SOURCE_DIR}/X3DImporter_Group.cpp
    ${LIB_SOURCE_DIR}/X3DImporter_Light.cpp
    ${LIB_SOURCE_DIR}/X3DImporter_Macro.hpp
    ${LIB_SOURCE_DIR}/X3DImporter_Metadata.cpp
    ${LIB_SOURCE_DIR}/X3DImporter_Networking.cpp
    ${LIB_SOURCE_DIR}/X3DImporter_Node.hpp
    ${LIB_SOURCE_DIR}/X3DImporter_Postprocess.cpp
    ${LIB_SOURCE_DIR}/X3DImporter_Rendering.cpp
    ${LIB_SOURCE_DIR}/X3DImporter_Shape.cpp
    ${LIB_SOURCE_DIR}/X3DImporter_Texturing.cpp
    ${LIB_SOURCE_DIR}/X3DVocabulary.cpp
    # 3MF
    ${LIB_SOURCE_DIR}/D3MFImporter.h
    ${LIB_SOURCE_DIR}/D3MFImporter.cpp
    ${LIB_SOURCE_DIR}/D3MFExporter.h
    ${LIB_SOURCE_DIR}/D3MFExporter.cpp
    ${LIB_SOURCE_DIR}/D3MFOpcPackage.h
    ${LIB_SOURCE_DIR}/D3MFOpcPackage.cpp
    ${LIB_SOURCE_DIR}/3MFXmlTags.h
    # MMD
    ${LIB_SOURCE_DIR}/MMDCpp14.h
    ${LIB_SOURCE_DIR}/MMDImporter.cpp
    ${LIB_SOURCE_DIR}/MMDImporter.h
    ${LIB_SOURCE_DIR}/MMDPmdParser.h
    ${LIB_SOURCE_DIR}/MMDPmxParser.h
    ${LIB_SOURCE_DIR}/MMDPmxParser.cpp
    ${LIB_SOURCE_DIR}/MMDVmdParser.h
    # Step_SRCS
    ${LIB_SOURCE_DIR}/StepExporter.h
    ${LIB_SOURCE_DIR}/StepExporter.cpp
    # Exporter_SRCS
    ${LIB_SOURCE_DIR}/Exporter.cpp
    ${LIB_SOURCE_DIR}/AssimpCExport.cpp
    ${LIB_SOURCE_DIR}/BlobIOSystem.h
    # MaterialSystem_SRCS
    ${LIB_SOURCE_DIR}/MaterialSystem.cpp
    ${LIB_SOURCE_DIR}/MaterialSystem.h

    # PostProcessing_SRCS
    ${LIB_SOURCE_DIR}/CalcTangentsProcess.cpp
    ${LIB_SOURCE_DIR}/CalcTangentsProcess.h
    ${LIB_SOURCE_DIR}/ComputeUVMappingProcess.cpp
    ${LIB_SOURCE_DIR}/ComputeUVMappingProcess.h
    ${LIB_SOURCE_DIR}/ConvertToLHProcess.cpp
    ${LIB_SOURCE_DIR}/ConvertToLHProcess.h
    ${LIB_SOURCE_DIR}/FindDegenerates.cpp
    ${LIB_SOURCE_DIR}/FindDegenerates.h
    ${LIB_SOURCE_DIR}/FindInstancesProcess.cpp
    ${LIB_SOURCE_DIR}/FindInstancesProcess.h
    ${LIB_SOURCE_DIR}/FindInvalidDataProcess.cpp
    ${LIB_SOURCE_DIR}/FindInvalidDataProcess.h
    ${LIB_SOURCE_DIR}/FixNormalsStep.cpp
    ${LIB_SOURCE_DIR}/FixNormalsStep.h
    ${LIB_SOURCE_DIR}/GenFaceNormalsProcess.cpp
    ${LIB_SOURCE_DIR}/GenFaceNormalsProcess.h
    ${LIB_SOURCE_DIR}/GenVertexNormalsProcess.cpp
    ${LIB_SOURCE_DIR}/GenVertexNormalsProcess.h
    ${LIB_SOURCE_DIR}/PretransformVertices.cpp
    ${LIB_SOURCE_DIR}/PretransformVertices.h
    ${LIB_SOURCE_DIR}/ImproveCacheLocality.cpp
    ${LIB_SOURCE_DIR}/ImproveCacheLocality.h
    ${LIB_SOURCE_DIR}/JoinVerticesProcess.cpp
    ${LIB_SOURCE_DIR}/JoinVerticesProcess.h
    ${LIB_SOURCE_DIR}/LimitBoneWeightsProcess.cpp
    ${LIB_SOURCE_DIR}/LimitBoneWeightsProcess.h
    ${LIB_SOURCE_DIR}/RemoveRedundantMaterials.cpp
    ${LIB_SOURCE_DIR}/RemoveRedundantMaterials.h
    ${LIB_SOURCE_DIR}/RemoveVCProcess.cpp
    ${LIB_SOURCE_DIR}/RemoveVCProcess.h
    ${LIB_SOURCE_DIR}/SortByPTypeProcess.cpp
    ${LIB_SOURCE_DIR}/SortByPTypeProcess.h
    ${LIB_SOURCE_DIR}/SplitLargeMeshes.cpp
    ${LIB_SOURCE_DIR}/SplitLargeMeshes.h
    ${LIB_SOURCE_DIR}/TextureTransform.cpp
    ${LIB_SOURCE_DIR}/TextureTransform.h
    ${LIB_SOURCE_DIR}/TriangulateProcess.cpp
    ${LIB_SOURCE_DIR}/TriangulateProcess.h
    ${LIB_SOURCE_DIR}/ValidateDataStructure.cpp
    ${LIB_SOURCE_DIR}/ValidateDataStructure.h
    ${LIB_SOURCE_DIR}/OptimizeGraph.cpp
    ${LIB_SOURCE_DIR}/OptimizeGraph.h
    ${LIB_SOURCE_DIR}/OptimizeMeshes.cpp
    ${LIB_SOURCE_DIR}/OptimizeMeshes.h
    ${LIB_SOURCE_DIR}/DeboneProcess.cpp
    ${LIB_SOURCE_DIR}/DeboneProcess.h
    ${LIB_SOURCE_DIR}/ProcessHelper.h
    ${LIB_SOURCE_DIR}/ProcessHelper.cpp
    ${LIB_SOURCE_DIR}/PolyTools.h
    ${LIB_SOURCE_DIR}/MakeVerboseFormat.cpp
    ${LIB_SOURCE_DIR}/MakeVerboseFormat.h
    )

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    # IFC specific flags
    if (MSVC)
	else()
         set_source_files_properties( ${LIB_SOURCE_DIR}/IFCReaderGen1.cpp
                                      ${LIB_SOURCE_DIR}/IFCReaderGen2.cpp
                                      PROPERTIES COMPILE_FLAGS "-Wa,-mbig-obj")
     endif()


    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR}/..)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_HEADER_DIR} ${LIB_SOURCE_DIR})

    # Enables ai_assert()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_DEBUG=1)
    endif()

    target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_OWN_ZLIB )
    target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_C4D_IMPORTER) # currently only msvc?
    target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_IFC_IMPORTER)
    target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_FBX_IMPORTER)
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_GLTF_IMPORTER)
    target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_AMF_IMPORTER)
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_IFC_IMPORTER )
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_XGL_IMPORTER )
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_FBX_IMPORTER )
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_GLTF_IMPORTER )
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_GLTF_EXPORTER )
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_CSM_IMPORTER )
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_3D_IMPORTER )
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_OGRE_IMPORTER )
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_OPENGEX_IMPORTER )
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_X3D_IMPORTER )
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_X3D_EXPORTER )
    #target_compile_definitions(${LIB_NAME} PUBLIC ASSIMP_BUILD_NO_MMD_IMPORTER )


    target_link_libraries(${LIB_NAME} PRIVATE de_rapidjson de_utf8cpp de_clipper de_poly2tri)
    target_link_libraries(${LIB_NAME} PRIVATE de_unzip de_minizip de_zlib)
    target_link_libraries(${LIB_NAME} PRIVATE de_open3dgc de_openddlparser de_irrXML)

    #   file(COPY ${LIB_HEADERS} DESTINATION ${ASSIMP_INCLUDE_DIR}) # For the SDK
    #   file(COPY ${ASSIMP_HEADERS_II} DESTINATION ${ASSIMP_INCLUDE_DIR}/Compiler) # For the SDK

endif()
