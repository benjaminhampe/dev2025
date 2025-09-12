#include <de_gpu/de_GL_VideoDriver.h>

namespace de {

IVideoDriver* createVideoDriverOpenGL( CreateParams const & params )
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
