#include "SMeshIO_ASSIMP_Tools.hpp"
std::ostream &
operator<< ( std::ostream & out, aiColor4D const & color )
{
   out << int32_t( 255.0f * color.r ) << "," << int32_t( 255.0f * color.g ) << "," << int32_t( 255.0f * color.b ) << "," << int32_t( 255.0f * color.a );
   return out;
}

std::ostream &
operator<< ( std::ostream & out, aiColor3D const & color )
{
   out << int32_t( 255.0f * color.r ) << "," << int32_t( 255.0f * color.g ) << "," << int32_t( 255.0f * color.b );
   return out;
}

std::ostream &
operator<< ( std::ostream & out, aiPrimitiveType const & primitiveType )
{
   int k = 0;
   if ( primitiveType & aiPrimitiveType::aiPrimitiveType_POINT )
   {
      out << "EPT_POINT";
      ++k;
   }
   if ( k > 0 ) out << "|";
   if ( primitiveType & aiPrimitiveType::aiPrimitiveType_LINE )
   {
      out << "EPT_LINE";
      ++k;
   }
   if ( k > 0 ) out << "|";
   if ( primitiveType & aiPrimitiveType::aiPrimitiveType_TRIANGLE )
   {
      out << "EPT_TRIANGLE";
      ++k;
   }
   if ( k > 0 ) out << "|";
   if ( primitiveType & aiPrimitiveType::aiPrimitiveType_POLYGON )
   {
      out << "EPT_POLYGON";
      ++k;
   }
   return out;
}

namespace de {
namespace gpu {

AssimpFile::AssimpFile()
   : m_File( nullptr ), m_FileName( "" ) {}
AssimpFile::AssimpFile( std::string uri )
   : m_File( nullptr ), m_FileName( "" ) { open( uri ); }
AssimpFile::~AssimpFile() { close(); }
std::string const &
AssimpFile::getFileName() const { return m_FileName; }
aiScene const *
AssimpFile::getScene() const { return m_File; }
bool
AssimpFile::is_open() const { return ( m_File != nullptr ); }
void
AssimpFile::close()
{
   if ( m_File )
   {
      aiReleaseImport( m_File );
      m_File = nullptr;
   }
}
bool
AssimpFile::open( std::string uri )
{
   if ( !FileSystem::existFile( uri ) )
   {
      DE_ERROR("Not a file ", uri)
      return false;
   }

   close();
   // Get assimp model from file
   //   aiImportFile(
   //    aiProcess_CalcTangentSpace      |
   //    aiProcess_GenNormals            |
   //    aiProcess_JoinIdenticalVertices |
   //    aiProcess_Triangulate           |
   //    aiProcess_GenUVCoords           |
   //    aiProcess_SortByPType           |
   //    0 )
   //PerformanceTimer timer;
   //timer.start();

   uint32_t flags = 0; // no processing for large models.

   if ( FileSystem::fileSize( uri ) < 64*1024*1024 )
   {
      flags = aiProcess_Triangulate           |
       aiProcess_GenNormals            |
       aiProcess_SortByPType;
         // aiProcess_CalcTangentSpace      |
         // aiProcess_JoinIdenticalVertices |
         // aiProcess_GenUVCoords           |
       //flags = aiProcessPreset_TargetRealtime_MaxQuality;
   }

   m_File = aiImportFile( uri.c_str(), flags );

   //timer.stop();
   if ( m_File )
   {
      m_FileName = uri;
      //DE_DEBUG("Loaded 3d file(",uri,"), timeMs(",timer.toMilliseconds(),")")
      return true;
   }
   else
   {
      DE_ERROR("Cant load 3d file(",uri,"), error(",aiGetErrorString(),")" )
      //DE_ERROR("Cant load 3d file(",uri,"), error(",aiGetErrorString(),"), timeMs(",timer.toMilliseconds(),")" )
      return false;
   }
}

AssimpTexture::AssimpTexture()
   : m_FileName()
   , m_Type( aiTextureType_NONE ) // diffuse tex
   , m_UVMode( aiTextureMapping_UV )
   , m_UVIndex( 0 )
   , m_Blend( 0.0 )
   , m_TexOp( aiTextureOp_Add )
   , m_WrapMode( aiTextureMapMode_Clamp )
{}

std::string
AssimpTexture::toString() const
{
   std::stringstream s;
   s << "uri:" << m_FileName << "|";
   s << "type:" << m_Type << "|";
   s << "wrap-mode:" << m_WrapMode << "|";
//         s << "UVMode:" << m_UVMode << "|";
//         s << "UVIndex:" << m_UVIndex << "|";
//         s << "Blend:" << m_Blend << "|";
//         s << "TexOp:" << m_TexOp << "|";
   return s.str();
}

/*
struct AssimpMaterial
{
DE_CREATE_LOGGER("de.assimp.Material")

int32_t m_Index;
std::string m_Name;
bool m_BackfaceCulling;
bool m_Wireframe;
int32_t m_ShadingModel;
uint32_t m_BlendMode;
uint32_t m_DiffuseColor;
uint32_t m_AmbientColor;
uint32_t m_EmissiveColor;
uint32_t m_SpecularColor;
uint32_t m_TransparentColor;
uint32_t m_ReflectiveColor;
float32_t m_Shininess;
float32_t m_ShininessStrength;
std::vector< AssimpTexture > m_Textures;
AssimpMaterial()
   : m_Index(-1)
   , m_Name()
   , m_BackfaceCulling( false )
   , m_Wireframe( false )
   , m_ShadingModel( -1 )
   , m_BlendMode( 0 )
   , m_DiffuseColor( 0x00000000 )
   , m_AmbientColor( 0x00000000 )
   , m_EmissiveColor( 0x00000000 )
   , m_SpecularColor( 0x00000000 )
   , m_TransparentColor( 0x00000000 )
   , m_ReflectiveColor( 0x00000000 )
   , m_Shininess( 1.0f )
   , m_ShininessStrength( 1.0f )
{}
std::string
toString() const
{
   std::stringstream s;
   s << m_Index << "|";
   s << m_Name << "|";
   s << m_BackfaceCulling << "|";
   if ( m_Textures.size() > 0 )
   {
      s << "tex:" << m_Textures[ 0 ].m_FileName << "|";
   }
   s << "d:" << color2string( m_DiffuseColor ) << "|";
   s << "a:" << color2string( m_AmbientColor ) << "|";
   s << "s:" << color2string( m_SpecularColor ) << "|";
   s << "e:" << m_Shininess << "|";
   s << "t:" << m_ShininessStrength << "|";
   s << "w:" << m_Wireframe << "|";
   s << "l:" << m_ShadingModel << "|";
   s << "b:" << m_BlendMode << "|";
//         s << "c_emissive:" << rgba2string( m_EmissiveColor << "\n";
//         s << "c_transparent:" << rgba2string( m_TransparentColor << "\n";
//         s << "c_reflective:" << rgba2string( m_ReflectiveColor << "\n";
   return s.str();
}
};
*/

//static
glm::vec4
AssimpTools::convertColor( aiColor3D const & color )
{
   return glm::vec4( color.r, color.g, color.b, 1.0f );
}
//static
glm::vec4
AssimpTools::convertColor( aiColor4D const & color )
{
   return glm::vec4( color.r, color.g, color.b, color.a );
//   return RGBA( uint32_t( 255.0f * color.r ),
//                       uint32_t( 255.0f * color.g ),
//                       uint32_t( 255.0f * color.b ),
//                       uint32_t( 255.0f * color.a ) );
}

//static
Box3f
AssimpTools::computeBoundingBox( aiMesh const * mesh )
{
   Box3f bbox;
   if ( !mesh ) return bbox;

   if ( mesh->mNumVertices > 0 )
   {
      aiVector3D pos = mesh->mVertices[ 0 ];
      bbox.reset( pos.x, pos.y, pos.z );
   }

   for ( uint32_t i = 1; i < mesh->mNumVertices; ++i )
   {
      aiVector3D pos = mesh->mVertices[ i ];
      bbox.addInternalPoint( pos.x, pos.y, pos.z );
   }

   return bbox;
}

//static
uint32_t
AssimpTools::computeMeshVertexCount( aiMesh const * const mesh )
{
   return mesh ? mesh->mNumVertices : 0;
}

//static
uint32_t
AssimpTools::computeMeshFaceCount( aiMesh const * const mesh )
{
   return mesh ? mesh->mNumFaces : 0;
}

//static
uint32_t
AssimpTools::computeMeshIndexCount( aiMesh const * const mesh )
{
   if ( !mesh ) return 0;
   uint32_t n = 0;
   for ( uint32_t i = 0; i < mesh->mNumFaces; ++i )
   {
      n += mesh->mFaces[ i ].mNumIndices;
   }
   return n;
}

//static
FaceMap
AssimpTools::getFaceMap( aiMesh const * const mesh )
{
   FaceMap map;

   if ( !mesh ) return map;

   for ( uint32_t i = 0; i < mesh->mNumFaces; ++i )
   {
      int indexCount = int( mesh->mFaces[ i ].mNumIndices );
      map[ indexCount ]++;
   }

   return map;
}

/*
/// @brief
static uint32_t
AssimpTools::getMeshCount( aiScene const * const assimpScene )
{
   if ( !assimpScene ) { return 0; }
   return assimpScene->mNumMeshes;
}
/// @brief
static aiMesh const *
AssimpTools::getMesh( aiScene const * const assimpScene, uint32_t meshIndex )
{
   assert( assimpScene );
   uint32_t const meshCount = assimpScene->mNumMeshes;
   if ( meshIndex >= meshCount )
   {
      DE_ERROR("Invalid meshIndex(",meshIndex,") of meshCount(", meshCount,")")
      return nullptr;
   }
   return assimpScene->mMeshes[ meshIndex ];
}

/// @brief
static GLenum
AssimpTools::convertTextureMode( aiTextureMapMode const & texMode )
{
   GLenum e = GL_CLAMP_TO_EDGE;
   switch( texMode )
   {
      case aiTextureMapMode_Clamp:  e = GL_CLAMP_TO_BORDER; break;
      case aiTextureMapMode_Wrap:   e = GL_REPEAT;          break;
      case aiTextureMapMode_Mirror: e = GL_MIRRORED_REPEAT; break;
      case aiTextureMapMode_Decal:  e = GL_REPEAT;          break;
      default: break;
   }
   return e;
}
*/

//static
bool
AssimpTools::parseMaterial( aiScene const * const assimpScene, uint32_t matIndex, Material & out, VideoDriver* driver, std::string baseDir )
{
   // Validate param 1
   if ( !assimpScene ) { DE_ERROR("No aiScene." ) return false; }
   // Validate param 4
   //if ( !driver ) { DE_ERROR("No driver." ) return false; }

   // Validate param 2
   uint32_t const matCount = assimpScene->mNumMaterials;
   if ( matIndex >= matCount )
   {
      std::cout << "[Error] " << __FUNCTION__ << " :: Invalid index(" << matIndex << ") of materials(" << matCount << ")\n";
      return false;
   }

   // Validate material ptr
   aiMaterial const * const assimpMaterial = assimpScene->mMaterials[ matIndex ];
   if ( !assimpMaterial )
   {
      std::cout << "[Error] " << __FUNCTION__ << " :: Invalid pointer to aiMaterial\n";
      return false;
   }

   // Read all fields ...
   aiString txt;
   aiColor4D color( 0.f, 0.f, 0.f, 0.f );
   float32_t value;
   int32_t ivalue = 0;

   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_NAME, txt ) )
   {
      out.Name = txt.C_Str();
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_COLOR_DIFFUSE, color ) )
   {
      out.Kd = AssimpTools::convertColor( color );
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_COLOR_AMBIENT, color ) )
   {
      out.Ka = AssimpTools::convertColor( color );
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_COLOR_EMISSIVE, color ) )
   {
      out.Ke = AssimpTools::convertColor( color );
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_COLOR_SPECULAR, color ) )
   {
      out.Ks = AssimpTools::convertColor( color );
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_COLOR_REFLECTIVE, color ) )
   {
      out.Kr = AssimpTools::convertColor( color );
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_COLOR_TRANSPARENT, color ) )
   {
      out.Kt = AssimpTools::convertColor( color );
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_SHININESS, value ) )
   {
      out.Shininess = value;
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_SHININESS_STRENGTH, value ) )
   {
      out.Shininess_strength = value;
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_BLEND_FUNC, ivalue ) )
   {
      // parse blend-mode
      //out.blendMode = ivalue;
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_TWOSIDED, ivalue ) )
   {
      if ( ivalue < 1 )
         out.state.culling = Culling::enabled();
      else {
         out.state.culling = Culling::disabled();
      }
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_ENABLE_WIREFRAME, ivalue ) )
   {
      out.Wireframe = ( ivalue > 0 );
   }
   if ( aiReturn_SUCCESS == assimpMaterial->Get( AI_MATKEY_SHADING_MODEL, ivalue ) )
   {
      out.Lighting = ivalue;
   }

   // #define AI_MATKEY_BLEND_FUNC "$mat.blend",0,0
   // #define AI_MATKEY_OPACITY "$mat.opacity",0,0
   // #define AI_MATKEY_BUMPSCALING "$mat.bumpscaling",0,0
   // #define AI_MATKEY_REFLECTIVITY "$mat.reflectivity",0,0
   // #define AI_MATKEY_REFRACTI "$mat.refracti",0,0


   // Read textures

   std::vector< AssimpTexture > textures;

   // Loop libAssimp texture types
   for ( uint32_t i = uint32_t( aiTextureType_NONE );
                  i < uint32_t( aiTextureType_UNKNOWN ); ++i )
   {
      auto group = aiTextureType( i );
      auto typeCount = assimpMaterial->GetTextureCount( group );
      if ( typeCount < 1 ) continue;

      aiString name;
      aiTextureMapping uvMode;
      uint32_t uvIndex = 0;
      ai_real blend = ai_real( 0.0 );
      aiTextureOp op;
      aiTextureMapMode wrapMode;

      for ( uint32_t k = 0; k < typeCount; ++k )
      {
         auto retVal = assimpMaterial->GetTexture( group, k, &name, &uvMode, &uvIndex, &blend, &op, &wrapMode );

         if ( aiReturn_SUCCESS != retVal )
         {
            DE_ERROR("Cannot read texture ", aiGetErrorString() )
            continue;
         }

         std::string uri = name.C_Str();

         dbStrReplace( uri, "\\", "/" );

#ifdef MORE_DEBUG
         if ( dbIsAbsolute( uri ) )
         {
            DE_WARN("Uri is absolute, making relative now")
         }
#endif

         uri = dbGetFileName( uri );
         uri = baseDir + "/" + uri; // Make absolute ( relative to mesh )

         if ( !dbExistFile( uri ) )
         {
            DE_WARN( "Texture file not found, try *.jpg ( uri ) = ", uri )
            uri = dbGetFileDir( uri ) + "/" + dbGetFileBaseName( uri ) + ".jpg";
         }

         if ( !dbExistFile( uri ) )
         {
            DE_WARN( "Texture file not found, try *.png ( uri ) = ", uri )
            uri = dbGetFileDir( uri ) + "/" + dbGetFileBaseName( uri ) + ".png";
         }


         if ( !dbExistFile( uri ) )
         {
            DE_WARN( "Texture file not found, try *.bmp ( uri ) = ", uri )
            uri = dbGetFileDir( uri ) + "/" + dbGetFileBaseName( uri ) + ".bmp";
         }

//               if ( !dbExistFile( uri ) )
//               {
//                  DE_WARN( "Texture file not found, try *.tga ( uri ) = ", uri )
//                  uri = dbGetFileDir( uri ) + "/" + dbGetFileBaseName( uri ) + ".jpg";
//               }

//               if ( !dbExistFile( uri ) )
//               {
//                  DE_WARN( "Texture file not found, try *.dds ( uri ) = ", uri )
//                  uri = dbGetFileDir( uri ) + "/" + dbGetFileBaseName( uri ) + ".jpg";
//               }
         AssimpTexture tex;
         tex.m_FileName = uri;
         tex.m_Type = group;
         tex.m_UVMode = uvMode;
         tex.m_UVIndex = uvIndex;
         tex.m_TexOp = op;
         tex.m_WrapMode = wrapMode;
         // tex.m_Texture = nullptr; // AssimpTools::readMaterialAssimpTexture( driver, texName.C_Str(), mediaDir );
         textures.emplace_back( std::move( tex ) );
      }
   }

   if ( textures.size() > 0 && driver )
   {
      if ( textures.size() > 4 )
      {
         DE_WARN("Mesh has too many textures ", textures.size() )
      }

      for ( size_t k = 0; k < textures.size(); ++k )
      {
         std::string const uri = textures[ k ].m_FileName;
         TexRef const ref = driver->getTexture( uri );
         if ( ref.m_tex )
         {
            out.setTexture( k, ref );
         }
      }
   }

   /*
      if ( assimpMaterial.m_ShadingModel > 0 )
      {
         material.lighting = true;
      }
      if ( assimpMaterial.m_BackfaceCulling )
      {
         material.state.cull = CullTest( false, true, GL_BACK );
      }
      else
      {
         material.state.cull = CullTest( false, true, GL_BACK );
      }
   */


   // DE_DEBUG("ReadMaterial[",matIndex,"] ", out.toString() )
   return true;
}

//static
void
AssimpTools::dumpBones( int k, aiMesh const * const mesh )
{
   if ( !mesh ) { return;      }

}

//static
void
AssimpTools::dumpNode( aiScene const * const scene, aiNode const * const node, int lvl )
{
   if ( !scene ) { return; }
   if ( !node ) { return; }

   std::string name = node->mName.C_Str();
   uint32_t nChildren = node->mNumChildren;
   uint32_t nMeshes = node->mNumMeshes;
   glm::mat4 transform = convertMatrix( node->mTransformation );

   DE_DEBUG("Node[", lvl, "] "
            "name(",name,"), "
            "children(",nChildren,"), "
            "meshes(",nMeshes,"), "
            "transform(",transform,")" )

   // Recursive Print children nodes and their stuff above
   for ( uint32_t c = 0; c < nChildren; ++c )
   {
      dumpNode( scene, node->mChildren[ c ], lvl + 1 );
   }
/*
   // Print Meshes
   //DE_DEBUG( "MeshCount = ",nMeshes )
   for ( uint32_t i = 0; i < nMeshes; ++i )
   {
      aiMesh const * const mesh = scene->mMeshes[ i ];
      if ( !mesh ) continue;

      std::string name = mesh->mName.C_Str();
      uint32_t nAnimes = mesh->mNumAnimMeshes;
      uint32_t nBones = mesh->mNumBones;
      uint32_t iMaterial = mesh->mMaterialIndex;
      uint32_t nVertices = mesh->mNumVertices;
      uint32_t nFaces = AssimpTools::computeMeshFaceCount( mesh );
      uint32_t nIndices = AssimpTools::computeMeshIndexCount( mesh );

      DE_DEBUG("Node[",lvl,"].Mesh[",i,"] "
               "name(",name,"), "
               "iMaterial(",iMaterial,"), "
               "nVertices(",nVertices,"), "
               "nFaces(",nFaces,"), "
               "nIndices(",nIndices,"), "
               "nAnimes(", nAnimes,"), "
               "nBones(",nBones,")" )

      // Print Bones
      for ( uint32_t k = 0; k < nBones; ++k )
      {
         aiBone const * const bone = mesh->mBones[ k ];
         if ( !bone ) continue;
         std::string boneName = bone->mName.C_Str();
         glm::mat4 boneMatrix = convertMatrix( bone->mOffsetMatrix );
         DE_DEBUG("Node[",lvl,"].Mesh[",i,"].Bone[",k,"] name(",boneName,"), matrix(",boneMatrix,")")
      }

      // Print Anims
      for ( uint32_t k = 0; k < nAnimes; ++k )
      {
         aiAnimMesh const * const anime = mesh->mAnimMeshes[ k ];
         if ( !anime ) continue;

         uint32_t nVertices = anime->mNumVertices;
         float fWeight = anime->mWeight;

         DE_DEBUG( "Node[",lvl,"].Mesh[",i,"].Anim[",k,"] = nVertices(", nVertices,"), fWeight(",fWeight,")" )
      }
   }
*/


}

//static
void
AssimpTools::dumpScene( aiScene const * const scene )
{
   if ( !scene )
   {
      return;
   }

   uint32_t nFlags = scene->mFlags;
   uint32_t nAnimations = scene->mNumAnimations;
   uint32_t nCameras = scene->mNumCameras;
   uint32_t nLights = scene->mNumLights;
   uint32_t nMaterials = scene->mNumMaterials;
   uint32_t nMeshes = scene->mNumMeshes;
   uint32_t nTextures = scene->mNumTextures;

   DE_DEBUG("Scene has "
            "nFlags(", nFlags, "), "
            "nAnimations(",nAnimations, "), "
            "nCameras(", nCameras, "), "
            "nLights(", nLights, "), "
            "nMaterials(", nMaterials, "), "
            "nMeshes(", nMeshes, "), "
            "nTextures(", nTextures, ")" )

   // DE_DEBUG("AnimationCount = ",nAnimations)

   for ( uint32_t i = 0; i < nAnimations; ++i )
   {
      aiAnimation const * const anim = scene->mAnimations[ i ];
      if ( !anim ) continue;
      std::string name = anim->mName.C_Str();
      double fDuration = anim->mDuration;
      double fTicksPerSec = anim->mTicksPerSecond;
      uint32_t nChannels = anim->mNumChannels;
      uint32_t nMeshChannels = anim->mNumMeshChannels;
      uint32_t nMorphMeshChannels = anim->mNumMorphMeshChannels;
      //glm::mat4 m = convertMatrix( bone->mOffsetMatrix );
      DE_DEBUG("Animation[",i,"] = ",name, ", "
               "time(", fDuration,"), "
               "freq(",fTicksPerSec,"), "
               "ch(", nChannels,"), "
               "mesh-ch(", nMeshChannels,"), "
               "morph-mesh-ch(", nMorphMeshChannels,")" )
   }

   // Print Nodes
   dumpNode( scene, scene->mRootNode, 0 );
}

//static
glm::mat4
AssimpTools::convertMatrix( aiMatrix4x4 const & assimpMatrix )
{
   glm::mat4 mat;
   float* m = glm::value_ptr( mat );
   m[0]=assimpMatrix.a1;
   m[1]=assimpMatrix.a2;
   m[2]=assimpMatrix.a3;
   m[3]=assimpMatrix.a4;
   m[4]=assimpMatrix.b1;
   m[5]=assimpMatrix.b2;
   m[6]=assimpMatrix.b3;
   m[7]=assimpMatrix.b4;
   m[8]=assimpMatrix.c1;
   m[9]=assimpMatrix.c2;
   m[10]=assimpMatrix.c3;
   m[11]=assimpMatrix.c4;
   m[12]=assimpMatrix.d1;
   m[13]=assimpMatrix.d2;
   m[14]=assimpMatrix.d3;
   m[15]=assimpMatrix.d4;
   return mat;
}

//static
S3DVertex
AssimpTools::parseVertex( aiMesh const * assimpMesh, uint32_t i, glm::vec4 color )
{
   S3DVertex outVertex;
   if ( !assimpMesh ) return outVertex;
   if ( i >= assimpMesh->mNumVertices ) return outVertex;

   aiVector3D pos = assimpMesh->mVertices[ i ];

   // Optional read a vertex normal ( with exactly numVertices )
   aiVector3D nrm = aiVector3D( 0.0f, 0.0f, 1.0f );
   if ( assimpMesh->HasNormals() )
   {
      nrm = assimpMesh->mNormals[ i ];
   }

   // Best Vertex Color ( with exactly numVertices )
   if ( assimpMesh->HasVertexColors( 0 ) )
   {
      color = AssimpTools::convertColor( assimpMesh->mColors[ 0 ][ i ] );
   }
   else if ( assimpMesh->HasVertexColors( 1 ) )
   {
      color = AssimpTools::convertColor( assimpMesh->mColors[ 1 ][ i ] );
   }

   // Best Vertex TexCoord
   aiVector3D texCoord = aiVector3D( 0.0f, 0.0f, 0.0f );
   if ( assimpMesh->HasTextureCoords( 1 ) )
   {
      texCoord = assimpMesh->mTextureCoords[ 1 ][ i ];
   }
   else if ( assimpMesh->HasTextureCoords( 0 ) )
   {
      texCoord = assimpMesh->mTextureCoords[ 0 ][ i ];
   }

   outVertex = S3DVertex( pos.x, pos.y, pos.z,
                          nrm.x, nrm.y, nrm.z,
                          RGBA( color ),
                          texCoord.x, texCoord.y );

   outVertex.flipV(); // For OpenGL ?
   return outVertex;
}
#if 0

auto rootNode = assimpScene->mRootNode;
if ( rootNode )
{
   DE_DEBUG("RootNode(",rootNode->mName,"), "
            "Children(", rootNode->mNumChildren,")"
            "Meshes(",rootNode->mNumMeshes,"), "
            "Matrix(",AssimpTools::convertMatrix( rootNode->mTransformation ),")"
            ":")

   for ( uint32_t i = 0; i < rootNode->mNumChildren; ++i )
   {
      auto child = rootNode->mChildren[ i ];
      if ( child )
      {
         DE_DEBUG("ChildNode(",child->mName,"), "
            "Children(", child->mNumChildren,")"
            "Meshes(",child->mNumMeshes,"), "
            "Matrix(",AssimpTools::convertMatrix( child->mTransformation ),")"
            ":")
      }

      uint32_t meshIndex = node->mMeshes[ i ];
      aiMesh const * const mesh = scene->mMeshes[ meshIndex ];
      if ( !mesh ) { continue; }
      std::string const name = mesh->mName.C_Str();
      uint32_t const mIndex = mesh->mMaterialIndex;
      uint32_t const vCount = mesh->mNumVertices;
      uint32_t const iCount = AssimpTools::getIndexCount( mesh );

      DE_DEBUG("Mesh[",meshIndex,"]: name(",name,"), material(",mIndex,"), vCount(",vCount,"), iCount(",iCount,").")
   }

for ( uint32_t i = 0; i < childCount; ++i )
{
   printNodes( scene, node->mChildren[ i ] );
}



   //DE_DEBUG("RootNode(",name,"), Meshes(",meshCount,"), Children(",childCount,"), Matrix(",m,").")
   //AssimpTools::printNodes( assimpScene, assimpScene->mRootNode );
}

//DE_DEBUG("3D-Model has Meshes(",nMeshBuffer,"), Materials(",nMaterials,"), uri(",uri,")")
//AssimpTools::printNodes( assimpScene, assimpScene->mRootNode );

SMesh* mesh = new SMesh( uri );

for ( uint32_t i = 0; i < nMeshBuffer; ++i )
{
   aiMesh const * const assimpMesh = assimpScene->mMeshes[ i ];
   if ( !assimpMesh ) { continue; }

   AssimpTools::printBones( i, assimpMesh );


   std::string const limbName = assimpMesh->mName.C_Str();
   uint32_t const materialIndex = assimpMesh->mMaterialIndex;
   uint32_t const indexCount = AssimpTools::getIndexCount( assimpMesh );
   uint32_t const vertexCount = assimpMesh->mNumVertices;
   if ( vertexCount < 1 )
   {
      DE_ERROR("Mesh[", i, "] ",limbName," has no vertices")
      continue;
   }
}
#endif


} // end namespace gpu.
} // end namespace de.

