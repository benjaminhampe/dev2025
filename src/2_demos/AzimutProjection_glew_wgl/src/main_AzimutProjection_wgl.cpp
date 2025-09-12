#include "EventReceiver.h"

int main(int argc, char** argv)
{
    dbInitGDK(argc,argv);
    EventReceiver eventReceiver;
    dbSetEventReceiver( &eventReceiver );
    dbSetResizable(true);
    dbSetWindowIcon( aaaa );

   int w = dbClientWidth();
   int h = dbClientHeight();

   //shaderManager.init();

   //screenRenderer.init( w,h, &shaderManager );
   //font58Renderer.init( w,h, &shaderManager );

   // MainLoop
/*
   std::string s_GL_EXTENSIONS;
   std::string s_GL_VERSION;
   std::string s_GL_VENDOR;
   std::string s_GL_RENDERER;
   std::string s_GL_SHADING_LANGUAGE_VERSION;

   s_GL_EXTENSIONS = (char const*)glGetString(GL_EXTENSIONS);
   s_GL_VERSION = (char const*)glGetString(GL_VERSION);
   s_GL_VENDOR = (char const*)glGetString(GL_VENDOR);
   s_GL_RENDERER = (char const*)glGetString(GL_RENDERER);
   s_GL_SHADING_LANGUAGE_VERSION = (char const*)glGetString(GL_SHADING_LANGUAGE_VERSION);

   printVideoModes();

   shaderArt_000.init( &shaderManager );
   shaderArt_001.init( &shaderManager );
   shaderArt_002.init( &shaderManager );
   shaderArt_003.init( &shaderManager );
   shaderArt_004.init( &shaderManager );
   shaderArt_005.init( &shaderManager );
   shaderArt_006.init( &shaderManager );
   shaderArt_007.init( &shaderManager );
   shaderArt_008.init( &shaderManager );
   shaderArt_009.init( &shaderManager );
   shaderArt_F01.init( &shaderManager );
   shaderArt_F02.init( &shaderManager );
   shaderArt_F03.init( &shaderManager );
   shaderArt_F04.init( &shaderManager );
   shaderArt_F05.init( &shaderManager );
   shaderArt_F06.init( &shaderManager );
   shaderArt_F07.init( &shaderManager );
   shaderArt_F08.init( &shaderManager );
   shaderArt_F09.init( &shaderManager );
*/
    double m_timeNow = dbTimeInSeconds();
    double m_timeLastCameraUpdate = m_timeNow;
    double m_timeLastRenderUpdate = m_timeNow;
    double m_timeLastWindowTitleUpdate = m_timeNow;

    // fpsComputer.reset();

    while (dbLoopGDK())
    {
        // fpsComputer.tick();
        m_timeNow = dbTimeInSeconds();

        // render
        // -----
        double dtRenderUpdate = m_timeNow - m_timeLastRenderUpdate;
        if ( dtRenderUpdate >= (1.0 / double( m_controlFps )) ) // 1.0 / 61.0
        {
            m_timeLastRenderUpdate = m_timeNow;

            // BEGIN frame:
            dbBeginRender();
            w = dbClientWidth();
            h = dbClientHeight();

            /*
            auto u_time = f32( dbTimeInSeconds() );
            switch ( m_currentShader )
            {
            case 1: shaderArt_001.render( w, h, u_time ); break;
            case 2: shaderArt_002.render( w, h, u_time ); break;
            case 3: shaderArt_003.render( w, h, u_time ); break;
            case 4: shaderArt_004.render( w, h, u_time ); break;
            case 5: shaderArt_005.render( w, h, u_time ); break;
            case 6: shaderArt_006.render( w, h, u_time ); break;
            case 7: shaderArt_007.render( w, h, u_time ); break;
            case 8: shaderArt_008.render( w, h, u_time ); break;
            case 9: shaderArt_009.render( w, h, u_time ); break;
            case 11: shaderArt_F01.render( w, h, u_time ); break;
            case 12: shaderArt_F02.render( w, h, u_time ); break;
            case 13: shaderArt_F03.render( w, h, u_time ); break;
            case 14: shaderArt_F04.render( w, h, u_time ); break;
            case 15: shaderArt_F05.render( w, h, u_time ); break;
            case 16: shaderArt_F06.render( w, h, u_time ); break;
            case 17: shaderArt_F07.render( w, h, u_time ); break;
            case 18: shaderArt_F08.render( w, h, u_time ); break;
            case 19: shaderArt_F09.render( w, h, u_time ); break;
            default:shaderArt_000.render( w, h, u_time ); break;
            }
            */

            dbEndRender();
        }

        // update window title 2-3x per second
        double dtWindowTitleUpdate = m_timeNow - m_timeLastWindowTitleUpdate;
        if ( dtWindowTitleUpdate >= 0.25 )
        {
            m_timeLastWindowTitleUpdate = m_timeNow;
            updateWindowTitle();
        }
    }

    dbFreeGDK();
    return 0;
}





#if 0

    if ( eventReceiver.m_showHelpOverlay )
    {
    font58Renderer.setScreenSize(w,h);

    de::Font5x8 font( m_fontSize, m_fontSize, 0,0,
          m_fontSize / 2 + 1, m_fontSize / 2 + 1 );

    int x = 20;
    int y = 20;
    int l = font.getTextSize("W").height + 5;
    de::Align align = de::Align::TopLeft;
    std::ostringstream o;

    o.str(""); o << "FPS = " << fpsComputer.getFPS();
    font58Renderer.draw2DText( x,y, o.str(), 0xFFFF9030, align, font ); y += l;

    o.str(""); o << "Control FPS = " << m_controlFps;
    font58Renderer.draw2DText( x,y, o.str(), 0xFFDF9030, align, font ); y += l;

    o.str(""); o << "CurrentShader = " << m_currentShader;
    font58Renderer.draw2DText( x,y, o.str(), 0xFF90DD30, align, font ); y += l;

    o.str(""); o << "FrameCounter = " << fpsComputer.getFrameCount();
    font58Renderer.draw2DText( x,y, o.str(), 0xFF00DDFF, align, font ); y += l;

    o.str(""); o << "FrameTime = " << fpsComputer.getTimeInSeconds();
    font58Renderer.draw2DText( x,y, o.str(), 0xFF00CCFF, align, font ); y += l;

    o.str(""); o << "GL_VERSION = " << s_GL_VERSION;
    font58Renderer.draw2DText( x,y, o.str(), 0xFF4040DF, align, font ); y += l;

    o.str(""); o << "GL_VENDOR = " << s_GL_VENDOR;
    font58Renderer.draw2DText( x,y, o.str(), 0xFF5050FF, align, font ); y += l;

    o.str(""); o << "GL_RENDERER = " << s_GL_RENDERER;
    font58Renderer.draw2DText( x,y, o.str(), 0xFF7070FF, align, font ); y += l;

    o.str(""); o << "GL_SHADING_LANGUAGE_VERSION = " << s_GL_SHADING_LANGUAGE_VERSION;
    font58Renderer.draw2DText( x,y, o.str(), 0xFF9090FF, align, font ); y += l;

    int desktopW = m_window.getDesktopWidth();
    int desktopH = m_window.getDesktopHeight();

    int winW = m_window.getWindowWidth();
    int winH = m_window.getWindowHeight();

    int winX = m_window.getWindowPosX();
    int winY = m_window.getWindowPosY();

    int clientW = m_window.getClientWidth();
    int clientH = m_window.getClientHeight();

    o.str(""); o << "DesktopSize(" << desktopW << "," << desktopH << ")";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFFF7030, align, font ); y += l;

    o.str(""); o << "WindowSize(" << winW << "," << winH << ")";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFFF9030, align, font ); y += l;

    o.str(""); o << "WindowPos(" << winX << "," << winY << ")";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFFFB030, align, font ); y += l;

    o.str(""); o << "FrameBufferSize(" << clientW << "," << clientH << ")";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFFFD030, align, font ); y += l;

    x = w / 2;
    y = h - 2*l;
    align = de::Align::TopCenter;

    o.str(""); o << "Press SPACE to toggle text overlay.";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

    x = w - 20;
    y = 20;
    align = de::Align::TopRight;

    o.str(""); o << "Press 'ESC' or 'Q' to exit program.";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

    o.str(""); o << "Press '0'...'9' to show ShaderArt 0...9.";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

    o.str(""); o << "Press 'F1'...'F9' to show ShaderArt 11...19.";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

    o.str(""); o << "Press 'F11' to toggle window resizable.";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

    o.str(""); o << "Press 'F12' or 'F' to toggle fullscreen.";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

    o.str(""); o << "Press 'PageUp' to increase font size.";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

    o.str(""); o << "Press 'PageDown' to decrease font size.";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFD0FF30, align, font ); y += l;

    o.str(""); o << "Press 'Up' to increase FPS.";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFFFD030, align, font ); y += l;

    o.str(""); o << "Press 'Down' to decrease FPS.";
    font58Renderer.draw2DText( x,y, o.str(), 0xFFEFA030, align, font ); y += l;

#endif
