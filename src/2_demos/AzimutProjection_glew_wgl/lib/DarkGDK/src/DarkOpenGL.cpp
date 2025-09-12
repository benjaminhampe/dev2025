#include "DarkOpenGL_details.h"
#include "DarkWindow_details.h"
#include "DarkWindow_WGL.h"
#include <de_opengl.h>
#include "globstruct.h"

namespace DarkGDK
{

void dbBeginRender()
{
    //fpsComputer.beginFrame();
    glClearColor(0.1f,0.1f,0.3f,1.0f);
    glClearDepth( 1.0 );
    glClearStencil( 0 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //w = dbClientWidth();
    //h = dbClientHeight();
}

void dbEndRender()
{
    //fpsComputer.beginFrame();
    // glClearColor(0.1f,0.1f,0.3f,1.0f);
    // glClearDepth( 1.0 );
    // glClearStencil( 0 );
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // w = dbClientWidth();
    // h = dbClientHeight();

    if (m_window)
    {
        m_window->swapBuffers();
        //m_window->yield(); // Save power
    }

    //fpsComputer.endFrame();
}

f64 dbFPS()
{
    return 0.0;
}
   
} // end namespace DarkGDK.
