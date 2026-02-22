#pragma once

#include <de/gpu/VideoDriver.h>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/camera.h>
#include <assimp/color4.h>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/Defines.h>
#include <assimp/light.h>
#include <assimp/IOStream.hpp>
#include <assimp/IOSystem.hpp>
#include <assimp/postprocess.h>

std::ostream &
operator<< ( std::ostream & out, aiColor4D const & color );

std::ostream &
operator<< ( std::ostream & out, aiColor3D const & color );

std::ostream &
operator<< ( std::ostream & out, aiPrimitiveType const & primitiveType );

namespace de {
namespace smesh {

   class AssimpFile
   {
      aiScene const * m_File;
      std::string m_FileName;

   public:
      AssimpFile();
      AssimpFile( std::string uri );
      ~AssimpFile();
      std::string const &   getFileName() const;
      aiScene const *       getScene() const;
      bool                  is_open() const;
      void                  close();
      bool                  open( std::string uri );
   };

   struct AssimpTexture
   {
      std::string m_uri;
      aiTextureType m_texType;      // aiTextureType_DIFFUSE
      aiTextureMapping m_uvMode; //aiTextureMapping_UV
      uint32_t m_uvIndex;
      ai_real m_blend;
      aiTextureOp m_texOp;       // aiTextureOp_Add/Multiply
      aiTextureMapMode m_wrapMode;

      // Specifies texture coordinate offsets/scaling/rotations
      ai_real m_OffsetU;
      ai_real m_OffsetV;
      ai_real m_ScaleU;
      ai_real m_ScaleV;
      ai_real m_Rotation; // counter cw

      AssimpTexture();

      std::string
      toString() const;
   };

   // e.g. face[ 1 ] = x, means there are x points.
   // e.g. face[ 2 ] = 13, means there are 13 lines.
   // e.g. face[ 3 ] = 24, means there are 24 triangles.
   typedef std::map< int, int > FaceMap;

   struct AssimpTools
   {
      /// @brief
      static glm::vec4 convertColor( aiColor3D const & color );

      /// @brief
      static glm::vec4 convertColor( aiColor4D const & color );

      /// @brief
      static Box3f computeBoundingBox( aiMesh const * mesh );

      static uint32_t computeMeshVertexCount( aiMesh const * const mesh );

      static uint32_t computeMeshFaceCount( aiMesh const * const mesh );

      static uint32_t computeMeshIndexCount( aiMesh const * const mesh );

      static FaceMap getFaceMap( aiMesh const * const mesh );

      /*
      /// @brief
      static uint32_t getMeshCount( aiScene const * const assimpScene );
      /// @brief
      static aiMesh const * getMesh( aiScene const * const assimpScene, uint32_t meshIndex );
      /// @brief
      static GLenum convertTextureMode( aiTextureMapMode const & texMode );
      */

      static bool parseMaterial( aiScene const * const assimpScene,
                                uint32_t matIndex,
                                SMaterial & out,
                                gpu::VideoDriver* driver,
                                std::string baseDir );

      static void dumpBones( int k, aiMesh const * const mesh );

      static void dumpNode( aiScene const * const scene, aiNode const * const node, int lvl );

      static void dumpScene( aiScene const * const scene );

      static glm::mat4 convertMatrix( aiMatrix4x4 const & assimpMatrix );

      static S3DVertex parseVertex( aiMesh const * assimpMesh, uint32_t i, glm::vec4 color );
   };

} // end namespace smesh.
} // end namespace de.
