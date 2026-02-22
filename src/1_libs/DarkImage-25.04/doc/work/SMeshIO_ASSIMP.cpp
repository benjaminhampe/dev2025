#include "SMeshIO_ASSIMP.hpp"
#include "SMeshIO_ASSIMP_Tools.hpp"
namespace de {
namespace gpu {

#ifndef DE_ERROR_RETURN
#define DE_ERROR_RETURN()
#endif

   //   std::cout << "[Info] " << __FUNCTION__ << " :: uri(" << uri << ")\n";
   //   std::string mediaDir = getAbsoluteFilePath( uri );
   //   std::cout << "[Info] " << __FUNCTION__ << " :: Assimp mediaDir(" << mediaDir << ")\n";
   //   std::cout << "[Info] " << __FUNCTION__ << " :: Assimp open file(" << uri << ")\n";
   //   std::cout << "[Info] " << __FUNCTION__ << " :: Assimp materials(" << nMaterials << ")\n";
   //   std::cout << "[Info] " << __FUNCTION__ << " :: Assimp meshes(" << nMeshes << ")\n";

bool
SMeshLoadASSIMP::load( SMesh & out, std::string const & uri, VideoDriver* driver, SMeshOptions const & options )
{
   AssimpFile file( uri );
   if ( !file.is_open() )
   {
      DE_ERROR("File not open. (",uri,")")
      return false;
   }

   aiScene const * const assimpScene = file.getScene();
   if ( !assimpScene )
   {
      DE_ERROR("Cant read file with library. (",uri,")")
      return false;
   }

   if ( options.debug )
   {
      AssimpTools::dumpScene( assimpScene );
   }

   //uint32_t nMaterials = assimpScene->mNumMaterials;

   uint32_t nMeshBuffer = assimpScene->mNumMeshes;
   if ( nMeshBuffer < 1 )
   {
      DE_ERROR("File has no meshes. (",uri,")")
      return false;
   }

   std::string meshDir = dbGetFileDir( uri );

   // SMesh* mesh = new SMesh( uri );
   SMesh* mesh = &out;
   mesh->setName( uri );

   for ( uint32_t i = 0; i < nMeshBuffer; ++i )
   {
      aiMesh const * const assimpMesh = assimpScene->mMeshes[ i ];
      if ( !assimpMesh ) { continue; }

      std::string const limbName = assimpMesh->mName.C_Str();
      if ( options.debug )
      {
         DE_DEBUG("Mesh[",i,"] has name (",limbName,")")
         // AssimpTools::printBones( i, assimpMesh );
      }

      //bool hasNormals = assimpMesh->HasNormals();

      //auto primTypes = assimpMesh->mPrimitiveTypes;
      //if ( primTypes & aiPrimitiveType_POINT ) DE_DEBUG("Mesh[",i,"] has POINTS")
      //if ( primTypes & aiPrimitiveType_LINE ) DE_DEBUG("Mesh[",i,"] has LINES")
      //if ( primTypes & aiPrimitiveType_TRIANGLE ) DE_DEBUG("Mesh[",i,"] has TRIANGLES")
      //if ( primTypes & aiPrimitiveType_POLYGON ) DE_DEBUG("Mesh[",i,"] has POLYGONS")

      uint32_t const materialIndex = assimpMesh->mMaterialIndex;
      //uint32_t const indexCount = AssimpTools::computeMeshIndexCount( assimpMesh );
      uint32_t const vertexCount = assimpMesh->mNumVertices;
      if ( vertexCount < 1 )
      {
         DE_ERROR("Mesh[", i, "] ",limbName," has no vertices")
         continue;
      }

      // BoundingBox3D-Parser:
#if 0
      Box3f & bbox = limb.getBoundingBox();

      aiVector3D const & pos = assimpMesh->mVertices[ 0 ];
      bbox.reset( glm::vec3( pos.x, pos.y, pos.z ) );

      for ( uint32_t k = 1; k < vertexCount; ++k )
      {
         aiVector3D const & pos = assimpMesh->mVertices[ k ];
         bbox.addInternalPoint( glm::vec3( pos.x, pos.y, pos.z ) );
      }

      DE_DEBUG("MeshBuffer[", i, "].BoundingBox = (",bbox.getSize(),"), Min(",bbox.getMin(),"), Max(",bbox.getMax(),")")
#endif
      // ============================

      // Material-Parser:
      Material material;
      if ( !AssimpTools::parseMaterial( assimpScene, materialIndex, material, driver, meshDir ) )
      {
         DE_ERROR("MeshBuffer[", i, "] ",limbName," Got read error for material ",materialIndex )
      }

      // ============================
      SMeshBuffer limb( PrimitiveType::Triangles );
      limb.setName( limbName );

      //material.state.culling = Culling::disabled();
      limb.setMaterial( material );
      limb.getMaterial().setLighting( 1 );
      limb.getMaterial().setFog( false );
      limb.moreVertices( vertexCount );

      uint32_t const faceCount = assimpMesh->mNumFaces;
      for ( uint32_t faceIndex = 0; faceIndex < faceCount; ++faceIndex )
      {
         aiFace const & face = assimpMesh->mFaces[ faceIndex ];
         if ( face.mNumIndices == 1 )
         {
            limb.setPrimitiveType( PrimitiveType::Points );
            limb.getMaterial().setLighting( 0 );
            uint32_t indexA = face.mIndices[ 0 ];
            S3DVertex A = AssimpTools::parseVertex( assimpMesh, indexA, material.Kd );
            auto v = limb.getVertexCount();
            limb.addVertex( A );
            limb.addIndex( v );
         }
         else if ( face.mNumIndices == 2 )
         {
            limb.setPrimitiveType( PrimitiveType::Lines );
            limb.getMaterial().setLighting( 0 );
            uint32_t indexA = face.mIndices[ 0 ];
            uint32_t indexB = face.mIndices[ 1 ];
            S3DVertex A = AssimpTools::parseVertex( assimpMesh, indexA, material.Kd );
            S3DVertex B = AssimpTools::parseVertex( assimpMesh, indexB, material.Kd );
            auto v = limb.getVertexCount();
            limb.addVertex( A );
            limb.addVertex( B );
            limb.addIndex( v );
            limb.addIndex( v+1 );
         }
         else if ( face.mNumIndices == 3 )
         {
            limb.setPrimitiveType( PrimitiveType::Triangles );
            uint32_t indexA = face.mIndices[ 0 ];
            uint32_t indexB = face.mIndices[ 1 ];
            uint32_t indexC = face.mIndices[ 2 ];
            S3DVertex A = AssimpTools::parseVertex( assimpMesh, indexA, material.Kd );
            S3DVertex B = AssimpTools::parseVertex( assimpMesh, indexB, material.Kd );
            S3DVertex C = AssimpTools::parseVertex( assimpMesh, indexC, material.Kd );
            //auto v = limb.getVertexCount();
            limb.addVertex( A );
            limb.addVertex( B );
            limb.addVertex( C );
            limb.addIndexedTriangle();
         }
//         else if ( face.mNumIndices == 4 )
//         {
//            limb.setPrimitiveType( PrimitiveType::TriangleStrip );

//            for ( size_t i = 0; i < face.mNumIndices; ++i )
//            {
//               uint32_t index = face.mIndices[ i ];
//               S3DVertex A = AssimpTools::parseVertex( assimpMesh, index, material.Kd );
//               limb.addVertex( A );
//               limb.addIndex( index );
//            }

//         }
         else // if ( face.mNumIndices == 4 )
         {
            DE_DEBUG("[Warn] Face(",faceIndex,") has index-count(",face.mNumIndices,"), try triangle strip")
            limb.setPrimitiveType( PrimitiveType::TriangleStrip );

            for ( size_t i = 0; i < face.mNumIndices; ++i )
            {
               uint32_t index = face.mIndices[ i ];
               S3DVertex A = AssimpTools::parseVertex( assimpMesh, index, material.Kd );
               limb.addVertex( A );
               limb.addIndex( index );
            }

         }

      }


#if BENNI_ENABLED_INDEX_BUFFER_RENDERING
      for ( uint32_t i = 0; i < vertexCount; ++i )
      {
         aiVector3D pos = assimpMesh->mVertices[ i ];
         aiVector3D normal = aiVector3D( 0.0f, 0.0f, 1.0f );
         aiVector3D texCoord = aiVector3D( 0.0f, 0.0f, 0.0f );
         uint32_t color = assimpMaterial.m_DiffuseColor;

         // Optional read a vertex normal ( with exactly numVertices )
         if ( assimpMesh->HasNormals() )
         {
            normal = assimpMesh->mNormals[ i ];
         }

         // Best Vertex Color ( with exactly numVertices )
         if ( assimpMesh->HasVertexColors( 1 ) )
         {
            color = AssimpTools::convertColor( assimpMesh->mColors[ 1 ][ i ] );
         }
         else if ( assimpMesh->HasVertexColors( 0 ) )
         {
            color = AssimpTools::convertColor( assimpMesh->mColors[ 0 ][ i ] );
         }

         // Best Vertex TexCoord
         if ( assimpMesh->HasTextureCoords( 1 ) )
         {
            texCoord = assimpMesh->mTextureCoords[ 1 ][ i ];
         }
         else if ( assimpMesh->HasTextureCoords( 0 ) )
         {
            texCoord = assimpMesh->mTextureCoords[ 0 ][ i ];
         }

         // Write vertex
         limb.addVertex( S3DVertex( pos.x, pos.y, pos.z,
                                           normal.x, normal.y, normal.z,
                                           color, texCoord.x, texCoord.y ) );
      }

      // ============================
      // Parse faces:
      // ============================
      limb.moreIndices( indexCount );

      uint32_t const faceCount = assimpMesh->mNumFaces;
      for ( uint32_t i = 0; i < faceCount; ++i )
      {
         aiFace const & face = assimpMesh->mFaces[ i ];

         if ( face.mNumIndices == 3 )
         {
            uint32_t indexA = face.mIndices[ 0 ];
            uint32_t indexB = face.mIndices[ 1 ];
            uint32_t indexC = face.mIndices[ 2 ];
            limb.addIndex( indexA );
            limb.addIndex( indexB );
            limb.addIndex( indexC );
         }
//            else if ( face.mNumIndices == 4 )
//            {
//               uint32_t indexA = face.mIndices[ 0 ];
//               uint32_t indexB = face.mIndices[ 1 ];
//               uint32_t indexC = face.mIndices[ 2 ];
//               uint32_t indexD = face.mIndices[ 3 ];
//               limb.addIndex( indexA );
//               limb.addIndex( indexC );
//               limb.addIndex( indexB );
//               limb.addIndex( indexA );
//               limb.addIndex( indexD );
//               limb.addIndex( indexC );
//            }
         else // if ( face.mNumIndices != 3 && face.mNumIndices != 4 )
         {
            //DE_DEBUG("[Warn] Face(",i,") has unsupported index-count(",face.mNumIndices,")")
            continue;
         }

      }

      if ( indexCount != uint32_t( limb.m_Indices.size() ) )
      {
         DE_ERROR("[Warn] Expected index count ",indexCount," differs from ",limb.m_Indices.size() )
      }

      DE_DEBUG( "Mesh[",i,"] ",limbName," "
                "v(",vertexCount,"), i(",indexCount,"), "
                "bbox(",limb.getBoundingBox().getSize(),") }, "
                "Material{ ", assimpMaterial.toString(), " }" )

#endif

      // myMaterial.setWireframe( material.m_Wireframe > 0 );
      // myMaterial.setId( material.m_Index > 0 );
      //limb.recalculateBoundingBox();

      limb.recalculateBoundingBox();
      mesh->addMeshBuffer( limb ); // mesh increases ref count

   } // NEXT aiMesh

   return true;
}

} // end namespace gpu.
} // end namespace de.



















#if 0

SMesh*
SMeshReaderASSIMP::load( std::string const & uri, VideoDriver* driver )
{
   //DE_DEBUG("Load 3D Model File ",uri )

   using namespace assimp;

   PerformanceTimer timer;
   timer.start();

   AssimpFile file( uri );
   if ( !file.is_open() )
   {
      DE_ERROR("File not open. (",uri,")")
      return nullptr;
   }

   aiScene const * const assimpScene = file.getScene();
   if ( !assimpScene )
   {
      DE_ERROR("Cant read file with library. (",uri,")")
      return nullptr;
   }

   AssimpTools::dumpScene( assimpScene );

   //uint32_t nMaterials = assimpScene->mNumMaterials;

   uint32_t nMeshBuffer = assimpScene->mNumMeshes;
   if ( nMeshBuffer < 1 )
   {
      DE_ERROR("File has no meshes. (",uri,")")
      return nullptr;
   }

   auto rootNode = assimpScene->mRootNode;
   if ( rootNode )
   {
      DE_DEBUG("RootNode.Name(",rootNode->mName.C_Str(),"), "
         "Children(", rootNode->mNumChildren,"), "
         "Meshes(",rootNode->mNumMeshes,"), "
         "Matrix(",AssimpTools::convertMatrix( rootNode->mTransformation ),")"
         ":")

      for ( uint32_t m = 0; m < rootNode->mNumMeshes; ++m )
      {
         auto mesh = assimpScene->mMeshes[ rootNode->mMeshes[ m ] ];
         if ( mesh )
         {
            uint32_t mIndex = mesh->mMaterialIndex;
            uint32_t vCount = AssimpTools::computeVertexCount( mesh );
            uint32_t iCount = AssimpTools::computeIndexCount( mesh );
            uint32_t fCount = AssimpTools::computeFaceCount( mesh );
            glm::vec3 bbox = AssimpTools::computeBoundingBox( mesh ).getSize();
            DE_DEBUG("RootNode.Mesh[",m,"] Name(",mesh->mName.C_Str(),"), "
               "BBox(",bbox,"), V(", vCount,"), I(",iCount,"), Faces(",fCount,"), Material(",mIndex,")" )
         }
      }

      for ( uint32_t i = 0; i < rootNode->mNumChildren; ++i )
      {
         auto child = rootNode->mChildren[ i ];
         if ( child )
         {
            DE_DEBUG("Child[", i, "] Name(",child->mName.C_Str(),"), "
               "Children(", child->mNumChildren,")"
               "Meshes(",child->mNumMeshes,"), "
               "Matrix(", AssimpTools::convertMatrix( child->mTransformation ),")"
               ":")

            for ( uint32_t m = 0; m < child->mNumMeshes; ++m )
            {
               auto mesh = assimpScene->mMeshes[ child->mMeshes[ m ] ];
               if ( mesh )
               {
                  uint32_t mIndex = mesh->mMaterialIndex;
                  uint32_t vCount = AssimpTools::computeVertexCount( mesh );
                  uint32_t iCount = AssimpTools::computeIndexCount( mesh );
                  uint32_t fCount = AssimpTools::computeFaceCount( mesh );
                  glm::vec3 bbox = AssimpTools::computeBoundingBox( mesh ).getSize();
                  DE_DEBUG("Child[", i, "].Mesh[",m,"] Name(",mesh->mName.C_Str(),"), "
                     "BBox(",bbox,"), V(", vCount,"), I(",iCount,"), Faces(",fCount,"), Material(",mIndex,")" )
               }
            }

         }
      }
   }

   //DE_DEBUG("3D-Model has Meshes(",nMeshBuffer,"), Materials(",nMaterials,"), uri(",uri,")")
   //AssimpTools::printNodes( assimpScene, assimpScene->mRootNode );

   std::string meshDir = dbGetFileDir( uri );

   SMesh* mesh = new SMesh( uri );

   for ( uint32_t i = 0; i < nMeshBuffer; ++i )
   {
      aiMesh const * const assimpMesh = assimpScene->mMeshes[ i ];
      if ( !assimpMesh ) { continue; }

      // Optional read a vertex normal ( with exactly numVertices )
      bool hasNormals = assimpMesh->HasNormals();

      // AssimpTools::printBones( i, assimpMesh );

      std::string const limbName = assimpMesh->mName.C_Str();
      uint32_t const materialIndex = assimpMesh->mMaterialIndex;
      uint32_t const indexCount = AssimpTools::computeMeshIndexCount( assimpMesh );
      uint32_t const vertexCount = assimpMesh->mNumVertices;
      if ( vertexCount < 1 )
      {
         DE_ERROR("Mesh[", i, "] ",limbName," has no vertices")
         continue;
      }

      // BoundingBox3D-Parser:
      Box3f & bbox = limb.getBoundingBox();

      aiVector3D const & pos = assimpMesh->mVertices[ 0 ];
      bbox.reset( glm::vec3( pos.x, pos.y, pos.z ) );

      for ( uint32_t k = 1; k < vertexCount; ++k )
      {
         aiVector3D const & pos = assimpMesh->mVertices[ k ];
         bbox.addInternalPoint( glm::vec3( pos.x, pos.y, pos.z ) );
      }

      DE_DEBUG("MeshBuffer[", i, "].BoundingBox = (",bbox.getSize(),"), Min(",bbox.getMin(),
               "),          Max(",bbox.getMax(),")")

      // ============================

      // Material-Parser:
      Material material;
      if ( !AssimpTools::parseMaterial( assimpScene, materialIndex, material, driver, meshDir ) )
      {
         DE_ERROR("MeshBuffer[", i, "] ",limbName," Got read error for material ",materialIndex )
      }

      // ============================
      SMeshBuffer* limb = new SMeshBuffer( PrimitiveType::Triangles );
      limb.setName( limbName );
      limb.setMaterial( material );
      limb.moreVertices( vertexCount );

      uint32_t const faceCount = assimpMesh->mNumFaces;
      for ( uint32_t faceIndex = 0; faceIndex < faceCount; ++faceIndex )
      {
         aiFace const & face = assimpMesh->mFaces[ faceIndex ];
         if ( face.mNumIndices == 1 )
         {
            limb.getMaterial().setLighting( false );
            limb.getMaterial().setFog( false );
            limb.setPrimitiveType( PrimitiveType::Points );
            uint32_t indexA = face.mIndices[ 0 ];
            S3DVertex A = AssimpTools::parseVertex( assimpMesh, indexA, material.diffuse_color );
            limb.addVertex( A );
         }
         else if ( face.mNumIndices == 2 )
         {
            limb.getMaterial().setLighting( false );
            limb.getMaterial().setFog( false );
            limb.setPrimitiveType( PrimitiveType::Lines );
            uint32_t indexA = face.mIndices[ 0 ];
            uint32_t indexB = face.mIndices[ 1 ];
            S3DVertex A = AssimpTools::parseVertex( assimpMesh, indexA, material.diffuse_color );
            S3DVertex B = AssimpTools::parseVertex( assimpMesh, indexB, material.diffuse_color );
            limb.addVertex( A );
            limb.addVertex( B );
         }
         else if ( face.mNumIndices == 3 )
         {
            limb.setPrimitiveType( PrimitiveType::Triangles );
            uint32_t indexA = face.mIndices[ 0 ];
            uint32_t indexB = face.mIndices[ 1 ];
            uint32_t indexC = face.mIndices[ 2 ];
            S3DVertex A = AssimpTools::parseVertex( assimpMesh, indexA, material.diffuse_color );
            S3DVertex B = AssimpTools::parseVertex( assimpMesh, indexB, material.diffuse_color );
            S3DVertex C = AssimpTools::parseVertex( assimpMesh, indexC, material.diffuse_color );
            limb.addVertex( A );
            limb.addVertex( B );
            limb.addVertex( C );
         }
         else if ( face.mNumIndices == 4 )
         {
            limb.setPrimitiveType( PrimitiveType::Triangles );
            uint32_t indexA = face.mIndices[ 0 ];
            uint32_t indexB = face.mIndices[ 1 ];
            uint32_t indexC = face.mIndices[ 2 ];
            uint32_t indexD = face.mIndices[ 3 ];
            S3DVertex A = AssimpTools::parseVertex( assimpMesh, indexA, material.diffuse_color );
            S3DVertex B = AssimpTools::parseVertex( assimpMesh, indexB, material.diffuse_color );
            S3DVertex C = AssimpTools::parseVertex( assimpMesh, indexC, material.diffuse_color );
            S3DVertex D = AssimpTools::parseVertex( assimpMesh, indexD, material.diffuse_color );
            limb.addVertex( A );
            limb.addVertex( B );
            limb.addVertex( C );
            limb.addVertex( A );
            limb.addVertex( C );
            limb.addVertex( D );
         }
         else if ( face.mNumIndices == 5 )
         {
            limb.setPrimitiveType( PrimitiveType::Lines );
            uint32_t indexA = face.mIndices[ 0 ];
            uint32_t indexB = face.mIndices[ 1 ];
            uint32_t indexC = face.mIndices[ 2 ];
            uint32_t indexD = face.mIndices[ 3 ];
            uint32_t indexE = face.mIndices[ 4 ];
            S3DVertex A = AssimpTools::parseVertex( assimpMesh, indexA, material.diffuse_color );
            S3DVertex B = AssimpTools::parseVertex( assimpMesh, indexB, material.diffuse_color );
            S3DVertex C = AssimpTools::parseVertex( assimpMesh, indexC, material.diffuse_color );
            S3DVertex D = AssimpTools::parseVertex( assimpMesh, indexD, material.diffuse_color );
            S3DVertex E = AssimpTools::parseVertex( assimpMesh, indexE, material.diffuse_color );
            limb.addVertex( A ); limb.addVertex( B );
            limb.addVertex( B ); limb.addVertex( C );
            limb.addVertex( C ); limb.addVertex( D );
            limb.addVertex( D ); limb.addVertex( E );
            limb.addVertex( E ); limb.addVertex( A );
         }
         else // if ( face.mNumIndices != 3 && face.mNumIndices != 4 )
         {
            DE_DEBUG("[Warn] Face(",i,") has unsupported index-count(",face.mNumIndices,")")
            continue;
         }
      }

      // -2-
      for ( uint32_t i = 0; i < vertexCount; ++i )
      {
         aiVector3D pos = assimpMesh->mVertices[ i ];
         aiVector3D normal = aiVector3D( 0.0f, 0.0f, 1.0f );
         aiVector3D texCoord = aiVector3D( 0.0f, 0.0f, 0.0f );
         uint32_t color = assimpMaterial.m_DiffuseColor;

         // Optional read a vertex normal ( with exactly numVertices )
         if ( assimpMesh->HasNormals() )
         {
            normal = assimpMesh->mNormals[ i ];
         }

         // Best Vertex Color ( with exactly numVertices )
         if ( assimpMesh->HasVertexColors( 1 ) )
         {
            color = AssimpTools::convertColor( assimpMesh->mColors[ 1 ][ i ] );
         }
         else if ( assimpMesh->HasVertexColors( 0 ) )
         {
            color = AssimpTools::convertColor( assimpMesh->mColors[ 0 ][ i ] );
         }

         // Best Vertex TexCoord
         if ( assimpMesh->HasTextureCoords( 1 ) )
         {
            texCoord = assimpMesh->mTextureCoords[ 1 ][ i ];
         }
         else if ( assimpMesh->HasTextureCoords( 0 ) )
         {
            texCoord = assimpMesh->mTextureCoords[ 0 ][ i ];
         }

         // Write vertex
         limb.addVertex( S3DVertex( pos.x, pos.y, pos.z,
                                           normal.x, normal.y, normal.z,
                                           color, texCoord.x, texCoord.y ) );
      }

      // ============================
      // Parse faces:
      // ============================
      limb.moreIndices( indexCount );

      uint32_t const faceCount = assimpMesh->mNumFaces;
      for ( uint32_t i = 0; i < faceCount; ++i )
      {
         aiFace const & face = assimpMesh->mFaces[ i ];

         if ( face.mNumIndices == 3 )
         {
            uint32_t indexA = face.mIndices[ 0 ];
            uint32_t indexB = face.mIndices[ 1 ];
            uint32_t indexC = face.mIndices[ 2 ];
            limb.addIndex( indexA );
            limb.addIndex( indexB );
            limb.addIndex( indexC );
         }
//            else if ( face.mNumIndices == 4 )
//            {
//               uint32_t indexA = face.mIndices[ 0 ];
//               uint32_t indexB = face.mIndices[ 1 ];
//               uint32_t indexC = face.mIndices[ 2 ];
//               uint32_t indexD = face.mIndices[ 3 ];
//               limb.addIndex( indexA );
//               limb.addIndex( indexC );
//               limb.addIndex( indexB );
//               limb.addIndex( indexA );
//               limb.addIndex( indexD );
//               limb.addIndex( indexC );
//            }
         else // if ( face.mNumIndices != 3 && face.mNumIndices != 4 )
         {
            //DE_DEBUG("[Warn] Face(",i,") has unsupported index-count(",face.mNumIndices,")")
            continue;
         }

      }

      if ( indexCount != uint32_t( limb.m_Indices.size() ) )
      {
         DE_ERROR("[Warn] Expected index count ",indexCount," differs from ",limb.m_Indices.size() )
      }

      DE_DEBUG( "Mesh[",i,"] ",limbName," "
                "v(",vertexCount,"), i(",indexCount,"), "
                "bbox(",limb.getBoundingBox().getSize(),") }, "
                "Material{ ", assimpMaterial.toString(), " }" )


      // myMaterial.setWireframe( material.m_Wireframe > 0 );
      // myMaterial.setId( material.m_Index > 0 );
      //limb.recalculateBoundingBox();

//         if ( !hasNormals )
//         {
         PerformanceTimer perf_normal; perf_normal.start();
         ::de::gpu::mesh::Tool::computeNormals( *limb );
         perf_normal.stop();
         DE_DEBUG("Computed normals in ms(",perf_normal.ms(),")" )
//         }

      mesh->addMeshBuffer( limb ); // mesh increases ref count

   } // NEXT aiMesh

   //mesh->recalculateBoundingBox();
   //mesh->centerVertices();
   timer.stop();

   DE_DEBUG("Needed ms(",timer.ms(),"), ", mesh->toString() )
   return mesh;
}


#endif











/*
struct SMeshBufferLoader
{
   DE_CREATE_LOGGER("de.gpu.SMeshBufferLoader")

   static bool
   load( SMeshBuffer & out, std::string const & uri, VideoDriver* driver )
   {
      out.clear();

      //DE_DEBUG("Load 3D Model File ",meshUri )
      //using namespace assimp;

      PerformanceTimer timer;
      timer.start();

      AssimpFile file( meshUri );
      if ( !file.is_open() )
      {
         DE_ERROR("File not open. (",meshUri,")")
         return false;
      }

      aiScene const * const assimpScene = file.getScene();
      if ( !assimpScene )
      {
         DE_ERROR("Cant read file with library. (",meshUri,")")
         return false;
      }

      uint32_t nMeshBuffer = assimpScene->mNumMeshes;
      if ( nMeshBuffer < 1 )
      {
         DE_ERROR("File has no meshes. (",meshUri,")")
         return false;
      }

      std::string meshDir = dbGetFileDir( meshUri );

      SMeshBufferLoaderTools::readMeshBuffer( out, 0, assimpScene, driver, meshDir );

      //mesh->recalculateBoundingBox();
      //mesh->centerVertices();
      timer.stop();

      DE_DEBUG("Needed ms(",timer.ms(),"), ", out.toString() )
      return true;
   }

};


inline de::gpu::SMeshSceneNode*
dbLoadObject3D( de::gpu::VideoDriver* driver,
                std::string const & uri,
                de::gpu::ISceneNode* parent = nullptr,
                de::gpu::MeshLoadOptions const & loadOptions = de::gpu::MeshLoadOptions() )
{
   if ( !driver ) return nullptr;
   de::gpu::SceneManager* smgr = driver->getSceneManager();
   if ( !smgr ) return nullptr;

   de::gpu::SMesh* mesh = de::gpu::SMeshReaderASSIMP::load( uri, driver );
   if ( !mesh ) return nullptr;

   de::gpu::SMeshSceneNode* node = smgr->addMeshSceneNode( mesh, parent, -1 );
   if ( !node ) return nullptr;

   //node->setPosition( 0,0,0 );

   if ( loadOptions.flipYZ )
   {
      mesh->flipVertexPosYZ();
   }
   if ( loadOptions.centerVertices )
   {
      mesh->centerVertices();
   }

   if ( loadOptions.scaleToMaxSize )
   {
      auto mesh_size = mesh->getBoundingBox().getSize();
      double mesh_size_max = dbMax( mesh_size.x, mesh_size.y, mesh_size.z );
      double node_scale = double( loadOptions.maxSize ) / mesh_size_max;
      node->setScale( node_scale, node_scale, node_scale );
   }

//   de::gpu::VisualDebugData debugData = node->getDebugData();
//   debugData.flags = de::gpu::VisualDebugData::NORMALS | VisualDebugData::BBOXES | VisualDebugData::BBOX;
//   debugData.normalLength = 0.1f;

   return node;
}



struct SMeshBufferLoaderTools
{
   DE_CREATE_LOGGER("de.gpu.SMeshBufferLoaderTools")

   static int32_t
   findIndexOfBiggestMesh( aiScene const * const assimpScene )
   {
      uint32_t nMeshBuffer = assimpScene->mNumMeshes;
      if ( nMeshBuffer < 1 )
      {
         return -1;
      }
      else if ( nMeshBuffer == 1 )
      {
         return 0;
      }
      else // if ( nMeshBuffer > 1 )
      {
         return 0;
      }

   }

   static bool
   parseFace( SMeshBuffer & out, int32_t faceIndex, aiMesh const * const assimpMesh, uint32_t diffuse_color )
   {
#if 1
      int32_t const faceCount = int32_t( assimpMesh->mNumFaces );
      if ( faceIndex < 0 ) return false;
      if ( faceIndex >= faceCount ) return false;
#endif

      aiFace const & face = assimpMesh->mFaces[ faceIndex ];

      for ( uint32_t i = 0; i < face.mNumIndices; ++i )
      {
         uint32_t index = face.mIndices[ i ];
         S3DVertex vertex = AssimpTools::parseVertex( assimpMesh, index, diffuse_color );
         out.addVertex( vertex );
      }

      if ( face.mNumIndices == 1 )
      {
         out.Material.setLighting( false );
         out.Material.setFog( false );
         out.PrimType = PrimitiveType::Points;
         return true;
      }
      else if ( face.mNumIndices == 2 )
      {
         out.Material.setLighting( false );
         out.Material.setFog( false );
         out.PrimType = PrimitiveType::Lines;
         return true;
      }
      else if ( face.mNumIndices == 3 )
      {
         out.PrimType = PrimitiveType::Triangles;
         return true;
      }
//      else if ( face.mNumIndices == 4 )
//      {
//         out.PrimType = PrimitiveType::Triangles;
//      }
//      else if ( face.mNumIndices == 5 )
//      {
//         out.PrimType = PrimitiveType::Lines;
//         uint32_t indexA = face.mIndices[ 0 ];
//         uint32_t indexB = face.mIndices[ 1 ];
//         uint32_t indexC = face.mIndices[ 2 ];
//         uint32_t indexD = face.mIndices[ 3 ];
//         uint32_t indexE = face.mIndices[ 4 ];
//         S3DVertex A = AssimpTools::parseVertex( assimpMesh, indexA, diffuse_color );
//         S3DVertex B = AssimpTools::parseVertex( assimpMesh, indexB, diffuse_color );
//         S3DVertex C = AssimpTools::parseVertex( assimpMesh, indexC, diffuse_color );
//         S3DVertex D = AssimpTools::parseVertex( assimpMesh, indexD, diffuse_color );
//         S3DVertex E = AssimpTools::parseVertex( assimpMesh, indexE, diffuse_color );
//         out.addVertex( A ); out.addVertex( B );
//         out.addVertex( B ); out.addVertex( C );
//         out.addVertex( C ); out.addVertex( D );
//         out.addVertex( D ); out.addVertex( E );
//         out.addVertex( E ); out.addVertex( A );
//         return true;
//      }
      else // if ( face.mNumIndices != 3 && face.mNumIndices != 4 )
      {
         return false;
      }
   }

   static bool
   readMeshBuffer( SMeshBuffer & out, int32_t meshIndex, aiScene const * const assimpScene, VideoDriver* driver, std::string const & meshDir )
   {
      if ( meshIndex < 0 ) { return false; }
      if ( !assimpScene )  { return false; }

      //uint32_t nMaterials = assimpScene->mNumMaterials;
      int32_t meshCount = int32_t( assimpScene->mNumMeshes );
      if ( meshCount < 1 ) { return false; }
      if ( meshIndex >= meshCount ) { return false; }

      out.clear();
      out.PrimType = PrimitiveType::Points;

      aiMesh const * const assimpMesh = assimpScene->mMeshes[ meshIndex ];
      if ( !assimpMesh ) { return false; }

      out.setName( assimpMesh->mName.C_Str() );

      uint32_t const vertexCount = assimpMesh->mNumVertices;
      if ( vertexCount < 1 ) { return true; }
      out.moreVertices( vertexCount );

      uint32_t const materialIndex = assimpMesh->mMaterialIndex;
      if ( !AssimpTools::parseMaterial( assimpScene, materialIndex, out.Material, driver, meshDir ) )
      {

      }

// WARN

      FaceMap faceMap = AssimpTools::getFaceMap( assimpMesh );
      if ( faceMap.size() > 1 )
      {
         // DE_WARN("SMeshBuffer cant store only one primitive type")
      }

      size_t itemCount = 0;
      DE_DEBUG( "FaceMap.Count = ", faceMap.size() )
      for ( auto const & item : faceMap )
      {
         DE_DEBUG( "[", itemCount, "] FaceMap[", item.first, "] = ", item.second )
      }

// END-WARN

      uint32_t const indexCount = AssimpTools::computeMeshIndexCount( assimpMesh );
      out.moreIndices( indexCount );

      int32_t const faceCount = int32_t( assimpMesh->mNumFaces );
      for ( int32_t faceIndex = 0; faceIndex < faceCount; ++faceIndex )
      {
         parseFace( out, faceIndex, assimpMesh, out.Material.diffuse_color );
      }

      bool hasNormals = assimpMesh->HasNormals();
      if ( !hasNormals )
      {
         SMeshBufferTool::computeNormals( out );
      }

      out.recalculateBoundingBox();
      return true;
   }


};

inline de::gpu::SMeshSceneNode*
dbLoadObject3D( de::gpu::VideoDriver* driver,
                std::string const & uri,
                de::gpu::ISceneNode* parent = nullptr,
                de::gpu::MeshLoadOptions const & loadOptions = de::gpu::MeshLoadOptions() )
{
   if ( !driver ) return nullptr;
   de::gpu::SceneManager* smgr = driver->getSceneManager();
   if ( !smgr ) return nullptr;

   de::gpu::SMesh* mesh = de::gpu::SMeshReaderASSIMP::load( uri, driver );
   if ( !mesh ) return nullptr;

   de::gpu::SMeshSceneNode* node = smgr->addMeshSceneNode( mesh, parent, -1 );
   if ( !node ) return nullptr;

   //node->setPosition( 0,0,0 );

   if ( loadOptions.flipYZ )
   {
      mesh->flipVertexPosYZ();
   }
   if ( loadOptions.centerVertices )
   {
      mesh->centerVertices();
   }

   if ( loadOptions.scaleToMaxSize )
   {
      auto mesh_size = mesh->getBoundingBox().getSize();
      double mesh_size_max = dbMax( mesh_size.x, mesh_size.y, mesh_size.z );
      double node_scale = double( loadOptions.maxSize ) / mesh_size_max;
      node->setScale( node_scale, node_scale, node_scale );
   }

//   de::gpu::VisualDebugData debugData = node->getDebugData();
//   debugData.flags = de::gpu::VisualDebugData::NORMALS | VisualDebugData::BBOXES | VisualDebugData::BBOX;
//   debugData.normalLength = 0.1f;

   return node;
}
*/
