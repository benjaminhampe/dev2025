#include <de/gpu/RenderTarget.hpp>
#include <de/GL_Validate.hpp>

namespace de {
namespace gpu {

// ===========================================================================
bool RenderTargetFactory::create( RenderTargetData & rt, int w, int h, VideoDriver* driver )
// ===========================================================================
{
   if ( !driver )
   {
      DE_ERROR("No driver, cant bind or unbind textures")
      return false;
   }

   rt.w = w;
   rt.h = h;
   rt.fbo = 0;
   rt.colorTex = 0;
   rt.depthTex = 0;

   float af = 4.0f; // Anisotropic filtering

   // Set RestorePoint
   GLint old_fbo = 0;
   ::glGetIntegerv( GL_FRAMEBUFFER_BINDING, &old_fbo ); GL_VALIDATE;

   // Create textures
   GLuint colorTex = 0;
   ::glGenTextures( 1, &colorTex ); GL_VALIDATE;

   if ( driver->bindTextureId( colorTex ) < 0 ) // returns occupied tex unit index
   {
      DE_ERROR("Cant bind colorTex")
   }

   rt.colorTex = colorTex;
   rt.colorFormat.internal = GL_RGBA;
   rt.colorFormat.format = GL_RGBA;
   rt.colorFormat.type = GL_UNSIGNED_BYTE;

   ::glTexImage2D(
         GL_TEXTURE_2D,
         0, // GLint level
         GLenum( rt.colorFormat.internal ),// GLenum internalFormat
         GLsizei( w ),
         GLsizei( h ),
         0, // GLint border
         GLenum( rt.colorFormat.format ),// GLenum format
         GLenum( rt.colorFormat.type ),// GLenum type
         nullptr ); GL_VALIDATE;

   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); GL_VALIDATE
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); GL_VALIDATE
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); GL_VALIDATE
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); GL_VALIDATE
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, af ); GL_VALIDATE

   // Combined DepthStencil Texture
   GLuint depthStencilTex = 0;
   ::glGenTextures( 1, &depthStencilTex ); GL_VALIDATE;

   if ( driver->bindTextureId( depthStencilTex ) < 0 ) // returns occupied tex unit index
   {
      DE_ERROR("Cant bind depthStencilTex")
   }

   rt.depthTex = depthStencilTex;
   rt.depthFormat.internal = GL_DEPTH24_STENCIL8;
   rt.depthFormat.format = GL_DEPTH_STENCIL;
   rt.depthFormat.type = GL_UNSIGNED_INT_24_8;

   ::glTexImage2D(
         GL_TEXTURE_2D, // GLenum target
         0,             // GLint level
         GLenum( rt.depthFormat.internal ), // GLenum internalFormat
         GLsizei( w ),  // GLsizei width
         GLsizei( h ),  // GLsizei height
         0,             // GLint border
         GLenum( rt.depthFormat.format ), // GLenum format
         GLenum( rt.depthFormat.type ), // GLenum type
         nullptr ); GL_VALIDATE;

   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ); GL_VALIDATE
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ); GL_VALIDATE
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); GL_VALIDATE
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); GL_VALIDATE
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, af ); GL_VALIDATE

   // FBO:
   GLuint fbo = 0;
   ::glGenFramebuffers( 1, &fbo ); GL_VALIDATE
   ::glBindFramebuffer( GL_FRAMEBUFFER, fbo ); GL_VALIDATE
   ::glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0 ); GL_VALIDATE
   ::glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthStencilTex, 0 ); GL_VALIDATE

   GLenum status = ::glCheckFramebufferStatus( GL_FRAMEBUFFER ); // GL_READ_F, GL_DRAW_F
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
      DE_ERROR("Incomplete FBO ", msg )
   }
   else
   {
      DE_DEBUG("Created RenderTargetData FBO w:",w,", h:",h)
   }

   // Restore old FBO
   ::glBindFramebuffer( GL_FRAMEBUFFER, old_fbo ); GL_VALIDATE

   if ( !driver->unbindTextureId( colorTex ) )
   {
      DE_ERROR("Cant unbind colorTex")
   }
   if ( !driver->unbindTextureId( depthStencilTex ) )
   {
      DE_ERROR("Cant unbind depthStencilTex")
   }

   return true;
}

// ===========================================================================
void RenderTargetFactory::destroy( RenderTargetData & rt, VideoDriver* driver )
// ===========================================================================
{
   GLint fbo = 0;
   ::glGetIntegerv( GL_FRAMEBUFFER_BINDING, &fbo ); GL_VALIDATE;
   if ( fbo != 0 && uint32_t( fbo ) == rt.fbo )
   {
      ::glBindFramebuffer( GL_FRAMEBUFFER, 0 ); GL_VALIDATE
   }

   // Unbind textures.
   if ( driver )
   {
      driver->unbindTextureId( rt.colorTex );
      driver->unbindTextureId( rt.depthTex );
   }
   // Destroy fbo.
   if ( rt.fbo )
   {
      ::glDeleteFramebuffers( 1, &rt.fbo ); GL_VALIDATE
      rt.fbo = 0;
   }
   // Destroy textures.
   if ( rt.colorTex )
   {
      ::glDeleteTextures( 1, &rt.colorTex );  GL_VALIDATE
      rt.colorTex = 0;
   }
   if ( rt.depthTex )
   {
      ::glDeleteTextures( 1, &rt.depthTex );  GL_VALIDATE
      rt.depthTex = 0;
   }
}


} // end namespace gpu.
} // end namespace de.
