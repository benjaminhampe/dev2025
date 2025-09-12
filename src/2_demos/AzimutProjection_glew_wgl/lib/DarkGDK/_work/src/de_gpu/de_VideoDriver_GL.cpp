#include <de_gpu/de_VideoDriver_GL.h>

namespace de {

VideoDriver* createVideoDriverOpenGL( CreateParams const & params )
{
   auto driver = new GL_VideoDriver();
   if ( driver->open( params ) )
   {
      return driver;
   }
   else
   {
      driver->close();
      delete driver;
      return nullptr;
   }
}

} // end namespace GL.
