#include <H3/H3_Game.h>



int main(int argc, char** argv)
{
    //auto name = dbPromtStrA("Schreibe deinen Namen...", "User-input:");
#ifdef HAVE_DE_AUDIO
    de::audio::AudioEngine m_audioEngine;
    m_audioEngine.play();
    Sleep(20000);
    m_audioEngine.stop();
#endif

#if 1
    auto device = new de::IrrlichtDevice();
    if ( !device->open( 1024, 768 ) )
    {
        DE_ERROR("Cant create window, abort main().")
        return 0;
    }

    H3_Game m_game;    
    m_game.init(device);

    // MainLoop:
    double m_timeStart = dbTimeInSeconds();
    double m_timeNow = 0.0;
    double m_timeLastCameraUpdate = 0.0;
    double m_timeLastRenderUpdate = 0.0;
    double m_timeLastWindowTitleUpdate = 0.0;
    // fpsComputer.reset();

    while (m_game.run())
    {
        // fpsComputer.tick();
        m_timeNow = dbTimeInSeconds() - m_timeStart;

        // camera
        // -----
        double fMoveSpeed = 75.5;
        double dtCameraUpdate = m_timeNow - m_timeLastCameraUpdate;
        if ( dtCameraUpdate >= (1.0 / 200.0) )
        {
            m_timeLastCameraUpdate = m_timeNow;

            auto window = m_game.getWindow();
            auto driver = m_game.getDriver();
            if (driver)
            {                
                // Move camera
                auto camera = driver->getCamera();
                if (camera)
                {
                    if (window->getKeyState(de::KEY_UP) || window->getKeyState(de::KEY_W))
                    {
                        camera->move(fMoveSpeed*dtCameraUpdate);
                    }
                    if (window->getKeyState(de::KEY_DOWN) || window->getKeyState(de::KEY_S))
                    {
                        camera->move(-fMoveSpeed*dtCameraUpdate);
                    }
                    if (window->getKeyState(de::KEY_LEFT) || window->getKeyState(de::KEY_A))
                    {
                        camera->strafe(-fMoveSpeed*dtCameraUpdate);
                    }
                    if (window->getKeyState(de::KEY_RIGHT) || window->getKeyState(de::KEY_D))
                    {
                        camera->strafe(fMoveSpeed*dtCameraUpdate);
                    }
                }
                else
                {
                    DE_ERROR("No camera")
                }
            }
            else
            {
                DE_ERROR("No driver")
            }
        }


        // if ( dtWindowTitleUpdate >= 0.25 )
        // {
        //     m_timeLastWindowTitleUpdate = m_timeNow;

        //     m_game.updateWindowTitle();
        // }

        // // update window title 2-3x per second
        // double dtWindowTitleUpdate = m_timeNow - m_timeLastWindowTitleUpdate;
        // if ( dtWindowTitleUpdate >= 0.25 )
        // {
        //     m_timeLastWindowTitleUpdate = m_timeNow;

        //     m_game.updateWindowTitle();
        // }
    }

    delete device;
    DE_INFO("Destroyed device.")

#endif
    return 0;
}

/*
        // camera
        // -----
        double fMoveSpeed = 3.5;
        double dtCameraUpdate = m_timeNow - m_timeLastCameraUpdate;
        if ( dtCameraUpdate >= (1.0 / 200.0) )
        {
            m_timeLastCameraUpdate = m_timeNow;

            if (g_driver)
            {
                Camera* camera = g_driver->getCamera();
                if (camera)
                {
                    if (m_window.getKeyState(de::KEY_UP) || m_window.getKeyState(de::KEY_W))
                    {
                        //DE_DEBUG("UP")
                        camera->move(fMoveSpeed*dtCameraUpdate);
                    }
                    if (m_window.getKeyState(de::KEY_DOWN) || m_window.getKeyState(de::KEY_S))
                    {
                        //DE_DEBUG("DOWN")
                        camera->move(-fMoveSpeed*dtCameraUpdate);
                    }
                    if (m_window.getKeyState(de::KEY_LEFT) || m_window.getKeyState(de::KEY_A))
                    {
                        //DE_DEBUG("LEFT")
                        camera->strafe(-fMoveSpeed*dtCameraUpdate);
                    }
                    if (m_window.getKeyState(de::KEY_RIGHT) || m_window.getKeyState(de::KEY_D))
                    {
                        //DE_DEBUG("RIGHT")
                        camera->strafe(fMoveSpeed*dtCameraUpdate);
                    }
                }
                else
                {
                    DE_ERROR("No camera")
                }
            }
            else
            {
                DE_ERROR("No driver")
            }
        }

        // render
        // -----
        double dtRenderUpdate = m_timeNow - m_timeLastRenderUpdate;
        if ( dtRenderUpdate >= (1.0 / double( m_window.m_controlFps )) ) // 1.0 / 61.0
        {
            m_timeLastRenderUpdate = m_timeNow;

            m_window.update();

            // BEGIN frame:
            dbBeginRender();
            w = dbClientWidth();
            h = dbClientHeight();

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

            dbEndRender();


        }
        */
