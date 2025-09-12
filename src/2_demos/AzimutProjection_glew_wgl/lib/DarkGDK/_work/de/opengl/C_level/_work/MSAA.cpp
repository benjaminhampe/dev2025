#include "MSAA.hpp"
#include <de/GL_Validate.hpp>
#include "VideoDriver.hpp"

namespace de {
namespace gpu {


MyMeshBuffer::MyMeshBuffer( PrimitiveType primType )
   : TMeshBuffer< MSAA_Vertex >( primType )
{}

MyMeshBuffer::~MyMeshBuffer()
{
   destroy();
}

bool
MyMeshBuffer::upload( int mode )
{
   return BufferTools::upload( getVBO(), getIBO(), getVAO(),
      getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
      getIndices(), getIndexCount(), getIndexType() );
}

void
MyMeshBuffer::destroy()
{
   BufferTools::destroy( getVBO(), getIBO(), getVAO() );
}

void
MyMeshBuffer::draw( int mode )
{
   ES10::draw( getVBO(), getIBO(), getVAO(),
               getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
               getIndices(), getIndexCount(), getIndexType() );
/*
   if ( mode == 0 )
   {
      BufferTools::drawImmediate( getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
         getIndices(), getIndexCount(), getIndexType() );
   }
   else
   {
      BufferTools::drawDetached( getVBO(), getIBO(), getVAO(),
                        getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
                        getIndices(), getIndexCount(), getIndexType() );
   }
   */
}

MyMeshBuffer
MSAA_createQuad()
{
   MyMeshBuffer screenQuad( PrimitiveType::Triangles );

   // B---C
   // |  /|
   // | / |
   // |/  |
   // A---D
   float x1 = -1.0f;
   float y1 = -1.0f;
   float x2 =  1.0f;
   float y2 =  1.0f;

//      screenQuad.Vertices.emplace_back( S3DVertex( x1,y1,z, 0,0,-1, 0xFFFFFFFF, 0,0 ) ); // A
//      screenQuad.Vertices.emplace_back( S3DVertex( x1,y2,z, 0,0,-1, 0xFFFFFFFF, 0,1 ) ); // B
//      screenQuad.Vertices.emplace_back( S3DVertex( x2,y2,z, 0,0,-1, 0xFFFFFFFF, 1,1 ) ); // C
//      screenQuad.Vertices.emplace_back( S3DVertex( x2,y1,z, 0,0,-1, 0xFFFFFFFF, 1,0 ) ); // D
   screenQuad.Vertices.emplace_back( MSAA_Vertex( x1,y1,0,0 ) ); // A
   screenQuad.Vertices.emplace_back( MSAA_Vertex( x1,y2,0,1 ) ); // B
   screenQuad.Vertices.emplace_back( MSAA_Vertex( x2,y2,1,1 ) ); // C
   screenQuad.Vertices.emplace_back( MSAA_Vertex( x2,y1,1,0 ) ); // D
   screenQuad.Indices.emplace_back( 0 );
   screenQuad.Indices.emplace_back( 1 );
   screenQuad.Indices.emplace_back( 2 );  // Triangle ABC
   screenQuad.Indices.emplace_back( 0 );
   screenQuad.Indices.emplace_back( 2 );
   screenQuad.Indices.emplace_back( 3 );  // Triangle ACD
   screenQuad.getMaterial().state.depth = Depth::disabled();
   screenQuad.getMaterial().state.culling = Culling::disabled();
   //state.lineWidth.m_Now = getMSAA();
   return screenQuad;
}

Tex2D* MSAA_createTex( int w, int h, TexFormat texFormat, VideoDriver* driver )
{
   if ( !driver )
   {
      DEM_ERROR("No videoDriver")
      return nullptr;
   }

   GLuint texId = 0;
   ::glGenTextures( 1, &texId ); GL_VALIDATE;
   if ( !texId )
   {
      DEM_ERROR("Cant create TexR8G8B8A8")
      return nullptr;
   }

   int stage = driver->bindTextureId( texId );
   if ( stage < 0 )
   {
      DEM_ERROR("Cant bind TexR8G8B8A8")
      return nullptr;
   }

   TexInternalFormat tif;
   if ( texFormat == TexFormat::R8G8B8A8 )
   {
      tif.internal = GL_RGBA;
      tif.format = GL_RGBA;
      tif.type = GL_UNSIGNED_BYTE;
   }
   else if ( texFormat == TexFormat::R8G8B8 )
   {
      tif.internal = GL_RGB;
      tif.format = GL_RGB;
      tif.type = GL_UNSIGNED_BYTE;
   }
   else if ( texFormat == TexFormat::D24S8 )
   {
      tif.internal = GL_DEPTH24_STENCIL8;
      tif.format = GL_DEPTH_STENCIL;
      tif.type = GL_UNSIGNED_INT_24_8;
   }

   ::glTexImage2D( GL_TEXTURE_2D,   // GLenum target
                  0,                // GLint level
                  GLenum( tif.internal ), // GLenum internalFormat
                  GLsizei( w ),  // GLsizei width
                  GLsizei( h ), // GLsizei height
                  0,                // GLint border
                  GLenum( tif.format ), // GLenum format
                  GLenum( tif.type ),   // GLenum type
                  nullptr ); GL_VALIDATE;

   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); GL_VALIDATE
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); GL_VALIDATE
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); GL_VALIDATE
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); GL_VALIDATE
   //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, af ); GL_VALIDATE

   auto tex = new Tex2D( driver->getTexUnitManager() );
   tex->m_Width = w;
   tex->m_Height = h;
   tex->m_SamplerOptions = SO( SO::MagLinear, SO::MinLinear, SO::ClampToEdge, SO::ClampToEdge );
   tex->m_Format = texFormat;
   tex->m_TexUnit = stage;
   tex->m_TextureId = texId;
   tex->m_InternalFormat = tif;
   return tex;
}

bool
MSAA_create( MSAA_intelli_R8G8B8A8_D24S8 & m_MSAA, int w, int h, int msaa, VideoDriver* driver )
{
   // Nothing todo (0)
   if ( !driver ) { DEM_ERROR("No driver") return false; }
   // Nothing todo (1)
   //if ( msaa < 1 ) { DEM_DEBUG("Ok you wanted nothing") return true; }

   if ( m_MSAA.screenQuad.Vertices.empty() )
   {
      m_MSAA.screenQuad = MSAA_createQuad();
   }
   // Determine max. msaa possible, depends on max supported texture size.
   int32_t msaa2 = msaa;
   int32_t maxTexW = driver->getTexture2DMaxSize();
   while ( msaa2 * w > maxTexW )
   {
      msaa2--;
   }

   // If determined msaa differs from user request value, say something...
   if ( msaa2 != msaa )
   {
      DEM_WARN("msaa(",msaa,") not supported, fallback to msaa2(",msaa2,") for resolution(",w,",",h,")")
      msaa = msaa2;
   }

   // Nothing todo (2)
   //if ( msaa < 2 ) { DEM_ERROR("No msaa > 1 available for resolution(",w,",",h,")") return true; }

   // Start giving precious data...
   m_MSAA.msaa = msaa;
   m_MSAA.width = w;
   m_MSAA.height = h;

   if ( m_MSAA.msaa > 0 )
   {
      // Start creating FBO offscreen rt1
      if ( !m_MSAA.texR8G8B8A8 )
      {
         m_MSAA.texR8G8B8A8 = MSAA_createTex( m_MSAA.w(), m_MSAA.h(), TexFormat::R8G8B8A8, driver );
         if ( !m_MSAA.texR8G8B8A8 ) { DEM_ERROR("No texR8G8B8A8") return false; }
      }

      // Start creating FBO offscreen rt2
      if ( !m_MSAA.texD24S8 )
      {
         m_MSAA.texD24S8 = MSAA_createTex( m_MSAA.w(), m_MSAA.h(), TexFormat::D24S8, driver );
         if ( !m_MSAA.texD24S8 ) { DEM_ERROR("No texD24S8") return false; }
      }

      // Start creating FBO
      if ( !m_MSAA.fbo )
      {
         //GLuint old_fbo = glGetIntegerDE( GL_FRAMEBUFFER_BINDING );
         GLuint fbo = 0;
         ::glGenFramebuffers( 1, &fbo ); GL_VALIDATE
         ::glBindFramebuffer( GL_FRAMEBUFFER, fbo ); GL_VALIDATE
         auto t1 = m_MSAA.texR8G8B8A8->getTextureId();
         auto t2 = m_MSAA.texD24S8->getTextureId();
         ::glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t1, 0 ); GL_VALIDATE
         ::glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, t2, 0 ); GL_VALIDATE
         GLenum status = ::glCheckFramebufferStatus( GL_FRAMEBUFFER ); // GL_READ_F, GL_DRAW_F

         //auto nb = size_t(w) * size_t(h) * size_t(4+4) * size_t(msaa) * size_t(msaa);
         //auto ns = dbStrByteCount( nb );
         //DEM_DEBUG("Create MSAA(",msaa,"), w(",w*msaa,"), h(",h*msaa,"), fbo-size(",ns,")")

         bool ok = (status == GL_FRAMEBUFFER_COMPLETE);
         if (!ok)
         {
            std::string msg = "UNKNOWN_ERROR";
            switch ( status )
            {
               case GL_FRAMEBUFFER_UNDEFINED: msg = "GL_FRAMEBUFFER_UNDEFINED"; break;
               case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: msg = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
               case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: msg = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
               case GL_FRAMEBUFFER_UNSUPPORTED: msg = "GL_FRAMEBUFFER_UNSUPPORTED The combination of internal formats"
                     "used by attachments in the framebuffer results in a nonrenderable target."; break;
               case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: msg = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
               default: break;
            }
            DEM_ERROR("Incomplete MSAA ", msg )
         }
         ::glBindFramebuffer( GL_FRAMEBUFFER, fbo ); GL_VALIDATE

         m_MSAA.fbo = fbo;

         if ( !driver->unbindTexture( m_MSAA.texR8G8B8A8 ) )
         {
            DEM_ERROR("Cant unbind texA8R8G8B8")
         }
         if ( !driver->unbindTexture( m_MSAA.texD24S8 ) )
         {
            DEM_ERROR("Cant unbind texD24S8")
         }
      }
   }


   //::glBindFramebuffer( GL_FRAMEBUFFER, 0 ); GL_VALIDATE

   return true;
}


void MSAA_destroy( MSAA_intelli_R8G8B8A8_D24S8 & dat, VideoDriver* driver )
{
   if (!driver)
   {
      DEM_ERROR("No driver")
      return;
   }

   GLint fboi = 0;
   ::glGetIntegerv( GL_FRAMEBUFFER_BINDING, &fboi ); GL_VALIDATE;
   GLuint fbo = fboi;

   if ( fbo != 0 && fbo == dat.fbo ) {
      ::glBindFramebuffer( GL_FRAMEBUFFER, 0 ); GL_VALIDATE
   }

   // Unbind textures.
   driver->unbindTexture( dat.texR8G8B8A8 );
   driver->unbindTexture( dat.texD24S8 );

   // Destroy fbo.
   if ( dat.fbo ) {
      ::glDeleteFramebuffers( 1, &dat.fbo ); GL_VALIDATE
      dat.fbo = 0;
   }

   // Destroy textures.
   if ( dat.texR8G8B8A8 )
   {
      GLuint tex = dat.texR8G8B8A8->getTextureId();
      if ( tex )
      {
         ::glDeleteTextures( 1, &tex );  GL_VALIDATE
      }
      delete dat.texR8G8B8A8;
      dat.texR8G8B8A8 = nullptr;
   }

   if ( dat.texD24S8 )
   {
      GLuint tex = dat.texD24S8->getTextureId();
      if ( tex )
      {
         ::glDeleteTextures( 1, &tex );  GL_VALIDATE
      }
      delete dat.texD24S8;
      dat.texD24S8 = nullptr;
   }

}

void MSAA_resize( MSAA_intelli_R8G8B8A8_D24S8 & dat, int w, int h, int msaa, VideoDriver* driver )
{
   if (!driver)
   {
      DEM_ERROR("No driver")
      return;
   }

   if ( msaa != dat.msaa || w != dat.width || h != dat.height )
   {
      dat.isLocked = true;

      //size_t fboBytes = size_t(w) * size_t(h) * size_t(4+4);
      //DEM_DEBUG("Resize MSAA(",msaa,"), w(",w,"), h(",h, "), fbo-size(", dbStrByteCount( fboBytes ),")" )
      MSAA_destroy( dat, driver );
      MSAA_create( dat, w, h, msaa, driver);

      dat.isLocked = false;

      if ( driver->getCamera() )
      {
         driver->getCamera()->setScreenSize( dat.w(), dat.h() );
      }
   }
}

bool
MSAA_renderPass1( MSAA_intelli_R8G8B8A8_D24S8 & m_MSAA, VideoDriver* driver )
{
   if (!driver)
   {
      DEM_ERROR("No driver")
      return true;
   }

   if (m_MSAA.msaa < 1)
   {
      DEM_ERROR("m_MSAA.msaa < 1")
      return true;
   }

   if (m_MSAA.isLocked)
   {
      DEM_ERROR("Render disabled")
      return true;
   }

   int screenW = driver->getScreenWidth();
   int screenH = driver->getScreenHeight();
   MSAA_resize( m_MSAA, screenW, screenH, m_MSAA.msaa, driver );

   // Bind MSAA [FBO].
   ::glBindFramebuffer( GL_FRAMEBUFFER, m_MSAA.fbo ); GL_VALIDATE
   ::glViewport( 0, 0, m_MSAA.w(), m_MSAA.h() );
   //::glClearColor( 0.f, 0.f, 0.f, 1.f ); GL_VALIDATE
   ::glClearDepthf( 1.f ); GL_VALIDATE
   ::glClearStencil( 0 ); GL_VALIDATE
   ::glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); GL_VALIDATE

   if ( driver->getCamera() )
   {
      driver->getCamera()->setScreenSize( m_MSAA.w(), m_MSAA.h() );
   }
   return true;
}



bool
MSAA_renderPass2( MSAA_intelli_R8G8B8A8_D24S8 & m_MSAA, VideoDriver* driver )
{
   if (!driver)
   {
      DEM_ERROR("No driver")
      return false;
   }

   if (m_MSAA.msaa < 1)
   {
      DEM_ERROR("m_MSAA.msaa < 1")
      return true;
   }

   if (m_MSAA.isLocked)
   {
      DEM_ERROR("Render disabled")
      return true;
   }

   ::glBindFramebuffer( GL_FRAMEBUFFER, 0 ); GL_VALIDATE
   ::glViewport( 0, 0, driver->getScreenWidth(), driver->getScreenHeight() ); GL_VALIDATE
   // [Clear] Color always different to see better fps rate.
   //auto rainbow = de::RainbowColor::computeColor128( dbAbs( sin( 0.13 * dbSeconds() ) ) );
   //::glClearColor( 0.f, 0.f, 0.f, 1.f ); GL_VALIDATE
   //::glClearDepthf( 1.f ); GL_VALIDATE
   //::glClearStencil( 0 ); GL_VALIDATE
   //::glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); GL_VALIDATE

   auto cam = driver->getCamera();
   driver->setCamera( nullptr );

   // [BindShader]
   Shader* shader = driver->getShader( "msaa" );
   if ( !shader )
   {
   //   auto vs = dbLoadText( "../../media/shader/msaa.vs");
   //   auto fs = dbLoadText( "../../media/shader/msaa.fs");

      std::string vs = R"(precision highp float;

      in vec2 a_pos;
      in vec2 a_tex;

   // out gl_Position[0]
      out vec2 v_tex;

      void main()
      {
         gl_Position = vec4( a_pos.x, a_pos.y, 0.0, 1.0 );
         v_tex = a_tex;
      })";

      std::string fs = R"(precision highp float;

      in vec2 v_tex;

      uniform sampler2D u_texColor;
      uniform sampler2D u_texDepth;

      out vec4 color;

      void main()
      {
         vec4 diffuse_color = texture( u_texColor, v_tex );
         vec4 depth_color = texture( u_texDepth, v_tex );
         color = diffuse_color;
      })";

      shader = driver->createShader( "msaa", vs, fs );
   }

   // [BindShader]
   if ( !shader )
   {
      DEM_ERROR("No shader msaa")
      return false;
   }
   if ( !driver->setShader( shader ) )
   {
      DEM_ERROR("No active shader msaa")
      return false;
   }

   // [BindTex] colorRGBA texR8G8B8A8
   if ( !m_MSAA.texR8G8B8A8 )
   {
      DEM_ERROR("No texR8G8B8A8")
      return false;
   }
   auto tuColor = driver->bindTexture( m_MSAA.texR8G8B8A8 );
   if ( tuColor < 0 )
   {
      DEM_ERROR("No texunit texR8G8B8A8")
      return false;
   }
   if ( !shader->setUniformi( "u_texColor", tuColor ) )
   {
      DEM_ERROR("No sampler u_texColor" )
      return false;
   }

   // [BindTex] depth+stencil texD24S8
   if ( !m_MSAA.texD24S8 )
   {
      DEM_ERROR("No texD24S8")
      return false;
   }
   auto tuDepth = driver->bindTexture( m_MSAA.texD24S8 );
   if ( tuDepth < 0 )
   {
      DEM_ERROR("No texunit texD24S8")
      return false;
   }
   if ( !shader->setUniformi( "u_texDepth", tuDepth ) )
   {
      DEM_ERROR("No sampler u_texDepth" )
      return false;
   }

   // [SetState]
   driver->setState( m_MSAA.screenQuad.getMaterial().state );
   driver->resetModelMatrix();

   // [DrawCall]
   driver->drawRaw( m_MSAA.screenQuad );

   // [Restore]
   //::glBindFramebuffer( GL_FRAMEBUFFER, 0 ); GL_VALIDATE
   driver->setCamera( cam );

   return true;
}

} // end namespace gpu.
} // end namespace de.


/*
      std::stringstream vs;
      vs << R"(
         precision highp float;

         in vec2 a_pos;
         in vec2 a_tex;

         // uniform vec2 u_screenSize;

         out vec2 v_tex;

         void main()
         {
            gl_Position = vec4( a_pos.x, a_pos.y, 0.0, 1.0 );
//               vec4 pos = u_mvp * vec4( a_pos, 1.0 );
//               // after mvp ( incl. Projection ) coords are in screen, not clipSpace
//               pos.x += a_normal.x * 0.5;
//               pos.y += a_normal.y * 0.5 * (u_screenSize.x / u_screenSize.y);
//               pos.z += a_normal.z;
//               gl_Position = pos;
//               v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
            v_tex = a_tex;
         }
      )";

      std::stringstream fs;
      fs << R"(
         precision highp float;

         uniform sampler2D u_texColor;
         uniform sampler2D u_texDepth;

         in vec2 v_tex;

         out vec4 color;

         void main()
         {
            vec4 diffuse_color = texture( u_texColor, v_tex );
            vec4 ds_color = texture( u_texDepth, v_tex );

            vec4 fog_color = vec4( 1,1,1,1 );
            float depth = gl_FragCoord.z / gl_FragCoord.w;
            float u_fogNear = 900.0f;
            float u_fogFar = 1000.0f;
            float fogFactor = smoothstep(u_fogNear, u_fogFar, depth);

//            vec2 off = vec2(16.0, 16.0);


            vec4 pixel = (
                  16.0 * diffuse_color
                  + texture(u_texColor, v_tex + vec2(-off.x,-off.y))
                  + texture(u_texColor, v_tex + vec2(-off.x,   0.0))
                  + texture(u_texColor, v_tex + vec2(-off.x, off.y))
                  + texture(u_texColor, v_tex + vec2(0.0,   -off.y))
                  + texture(u_texColor, v_tex + vec2(0.0,    off.y))
                  + texture(u_texColor, v_tex + vec2(off.x, -off.y))
                  + texture(u_texColor, v_tex + vec2(off.x,    0.0))
                  + texture(u_texColor, v_tex + vec2(off.x,  off.y)) ) / 24.0;

//            vec4 pixel = (
//                  16.0 * diffuse_color
//                  + texture(u_texColor, v_tex + vec2(-off.x,-off.y))
//                  + texture(u_texColor, v_tex + vec2(-off.x,   0.0))
//                  + texture(u_texColor, v_tex + vec2(-off.x, off.y))
//                  + texture(u_texColor, v_tex + vec2(0.0,   -off.y))
//                  + texture(u_texColor, v_tex + vec2(0.0,    off.y))
//                  + texture(u_texColor, v_tex + vec2(off.x, -off.y))
//                  + texture(u_texColor, v_tex + vec2(off.x,    0.0))
//                  + texture(u_texColor, v_tex + vec2(off.x,  off.y)) ) / 24.0;

//            vec4 pixel = (
//                  16.0 * texture(u_texColor, v_tex)
//                  + texture(u_texColor, v_tex + vec2(off.x,0.0))
//                  + texture(u_texColor, v_tex + vec2(0.0,off.y))
//                  + texture(u_texColor, v_tex + vec2(off.x,off.y)) ) / 19.0;

//            vec4 pixel = (
//                  2.0 * texture(u_texColor, v_tex) +
//                  1.0 * texture(u_texColor, v_tex + vec2(off.x,0.0)) +
//                  1.0 * texture(u_texColor, v_tex + vec2(0.0,off.y)) ) / 4.0;

//            vec4 pixel = (
//                  4.0 * texture(u_texColor, v_tex) +
//                        texture(u_texColor, v_tex + vec2(-off.x,0.0)) +
//                        texture(u_texColor, v_tex + vec2( off.x,0.0)) +
//                        texture(u_texColor, v_tex + vec2(0.0,-off.y)) +
//                        texture(u_texColor, v_tex + vec2(0.0, off.y)) ) / 7.0;

//            vec4 pixel = (
//                  4.0 * texture(u_texColor, v_tex) +
//                  2.0 * texture(u_texColor, v_tex + vec2(-off.x,0.0)) +
//                  2.0 * texture(u_texColor, v_tex + vec2( off.x,0.0)) +
//                  2.0 * texture(u_texColor, v_tex + vec2(   0.0,-off.y)) +
//                  2.0 * texture(u_texColor, v_tex + vec2(   0.0,off.y)) +
//                        texture(u_texColor, v_tex + vec2(-off.x,-off.y)) +
//                        texture(u_texColor, v_tex + vec2(-off.x, off.y)) +
//                        texture(u_texColor, v_tex + vec2( off.x,-off.y)) +
//                        texture(u_texColor, v_tex + off) ) / 7.0;

            //color = vec4(diffuse_color.rgb * (1.0-depth), diffuse_color.a ); //
            color = vec4(pixel.rgb, pixel.a ); //  * depth
         }
      )";
*/
