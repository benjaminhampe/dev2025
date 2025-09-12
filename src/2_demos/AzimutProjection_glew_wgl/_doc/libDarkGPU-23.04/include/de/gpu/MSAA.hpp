/// ==========================================================================
/// @file MSAA.hpp
/// @author 2018 Benjamin Hampe <benjaminhampe@gmx.de>
/// @copyright Free open source software
/// ==========================================================================
#pragma once
#include "Material.hpp"
#include "VAO.hpp"

namespace de {
namespace gpu {

struct VideoDriver;

// ===========================================================================
#pragma pack( push )
#pragma pack( 1 )

struct MSAA_Vertex
{
   glm::vec2 pos;
   glm::vec2 tex;    // 12 + 12 + 4 + 8 = 36 Bytes if packed

   MSAA_Vertex()
      : pos(), tex()
   {}

   MSAA_Vertex( float x, float y, float u, float v )
      : pos( x,y ), tex( u,v )
   {}

   MSAA_Vertex( glm::vec2 const & p, glm::vec2 const & t )
      : pos( p ), tex( t )
   {}

   static FVF const &
   getFVF()
   {
      static FVF const fvf( sizeof( MSAA_Vertex ),
         {  VertexAttrib( VertexAttribType::F32, 2, VertexAttribSemantic::Pos ),
            VertexAttrib( VertexAttribType::F32, 2, VertexAttribSemantic::Tex )
         }
      );
      return fvf;
   }
};

#pragma pack( pop )

struct MyMeshBuffer : public TMeshBuffer< MSAA_Vertex >
{
   /// @brief Constructor
   MyMeshBuffer( PrimitiveType primType );
   ~MyMeshBuffer();
   bool upload( int mode );
   void destroy();
   void draw( int mode );
};

MyMeshBuffer MSAA_createQuad();

// ===========================================================================
struct MSAA_intelli_R8G8B8A8_D24S8
// ===========================================================================
{
   MSAA_intelli_R8G8B8A8_D24S8()
      : width(0), height(0), msaa(0), fbo(0), texR8G8B8A8(nullptr), texD24S8(nullptr)
      , isLocked(false), screenQuad( PrimitiveType::Triangles )
   {}

   int w() const { return width * msaa; }
   int h() const { return height * msaa; }

   DE_CREATE_LOGGER("de.gpu.MSAA_intelli_R8G8B8A8_D24S8")
   int width;  // screen width
   int height; // screen height
   int msaa;   // msaa ( factored with screen-size )
   GLuint fbo; // GL id
   Tex2D* texR8G8B8A8;  // GL texture id ( RGB color with A transparency, TODO: some HDR and 10bit )
   Tex2D* texD24S8;     // GL texture id ( not a renderbuffer, its a real tex, ergo downloadable/saveable )

   bool isLocked;

   MyMeshBuffer screenQuad;
};



Tex2D* MSAA_createTex( int w, int h, TexFormat texFormat, VideoDriver* driver );

void MSAA_resize( MSAA_intelli_R8G8B8A8_D24S8 & dat, int w, int h, int msaa, VideoDriver* driver );
bool MSAA_create( MSAA_intelli_R8G8B8A8_D24S8 & dat, int w, int h, int msaa, VideoDriver* driver );
void MSAA_destroy( MSAA_intelli_R8G8B8A8_D24S8 & dat, VideoDriver* driver );
bool MSAA_renderPass1( MSAA_intelli_R8G8B8A8_D24S8 & m_MSAA, VideoDriver* driver );
bool MSAA_renderPass2( MSAA_intelli_R8G8B8A8_D24S8 & m_MSAA, VideoDriver* driver );


} // end namespace gpu.
} // end namespace de.

