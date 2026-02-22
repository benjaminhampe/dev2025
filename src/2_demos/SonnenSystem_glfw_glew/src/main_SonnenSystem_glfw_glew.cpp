#include "SonSys_Common.h"
#include "Body.h"
#include "BodyRenderer.h"

GLFWwindow* m_window = nullptr;
int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;
uint64_t totalFrames = 0;

std::string m_mediaDir;
de::gl::Bridge* m_driver = nullptr;
BodyDatabase m_bodyDatabase;
BodyRenderer m_bodyRenderer;
// float ASPECT = 16.0/9.0;
// float fov = 90;
// de::gpu::Camera m_camera;
/*
struct CameraKeybinds
{
    int FORWARD     = GLFW_KEY_W;
    int BACKWARD    = GLFW_KEY_S;
    int LEFT        = GLFW_KEY_A;
    int RIGHT       = GLFW_KEY_D;
    int UP          = GLFW_KEY_SPACE;
    int DOWN        = GLFW_KEY_LEFT_SHIFT;
    int ROLL_LEFT   = GLFW_KEY_Q;
    int ROLL_RIGHT  = GLFW_KEY_E;
    //bool camera_keybind_window_active = false;
    //int* keybind_to_be_changed;
};
CameraKeybinds m_cameraKeybinds;
*/

std::array<bool, 1024> m_keyStates;

int m_mouseX = 0;
int m_mouseY = 0;
int m_mouseMoveX = 0;
int m_mouseMoveY = 0;
int m_cameraMouseLeftX = 0;
int m_cameraMouseLeftY = 0;
int m_cameraFreeMoveEnabled = true;
bool m_firstMouse = true;
bool m_isMouseLeftPressed = false;
bool m_isMouseRightPressed = false;
bool m_isMouseMiddlePressed = false;

// de::gpu::TexManager m_texManager;

// de::gpu::Lights m_lights;

// H3_ObjRenderer m_objRenderer;
// H3_ObjMeshBuffer m_objMeshBuffer0; // Sphere
// de::gpu::Texture* m_objTexture;


void createScene()
{
    // ===================
    // === init camera ===
    // ===================
    auto camera = m_driver->getCamera();
    camera->setScreenSize( SCREEN_WIDTH, SCREEN_HEIGHT );
    camera->setFOV( 90.0f );
    camera->setNearValue( 1.0f );
    camera->setFarValue( 38000.0f );
    camera->lookAt( glm::vec3(10,100,-100), glm::vec3(0,0,0) );
    camera->setMoveSpeed(1.0);
    camera->setStrafeSpeed(1.0);
    camera->setUpSpeed(1.0);

    // ===================
    // === init lights ===
    // ===================
    auto lights = m_driver->getLights();
    de::gpu::Light light0; // (sun)
    light0.pos = glm::vec3(0,300,0);
    light0.color = glm::vec3(1,1,0.9);
    lights->push_back( light0 );

    m_mediaDir = "../../media/SonnenSystem/";
    m_mediaDir = "C:/Users/firestarter/Downloads/_asteroids/_0solarsystem/_best/";

    m_bodyDatabase.init( m_driver );
    m_bodyRenderer.init( m_driver );

/*
    de::gpu::SMeshBuffer sphere;
    de::gpu::SMeshSphere::add(sphere, glm::vec3(100,100,100), 0xFFFFFFFF, 32, 32);
    // de::gpu::SMeshBufferTool::flipNormals( sphere );
    m_objMeshBuffer0 = H3_ObjMeshBuffer::toTMesh( sphere );
    m_objMeshBuffer0.upload();
    m_objMeshBuffer0.m_instanceMat.emplace_back( glm::mat4(1.0f) );
    m_objMeshBuffer0.m_instanceColor.emplace_back( 0xFFFFFFFF );
    m_objMeshBuffer0.uploadInstances();

    de::Image img;
    dbLoadImage(img, "../../media/SonnenSystem/z_starmap_2020_8k_gal.webp");
    m_objTexture = m_texManager.createTexture2D("background", img);

    initTessSphereShader(glm::vec3(-250,0,0), glm::vec3(100,100,100) );

    de::gpu::SO soMars(
        1.0f,
        de::gpu::SO::Minify::Linear,
        de::gpu::SO::Magnify::Linear,
        de::gpu::SO::Wrap::Repeat,
        de::gpu::SO::Wrap::Repeat,
        de::gpu::SO::Wrap::Repeat);

    de::Image img_mars_Kd;
    dbLoadImage(img_mars_Kd, "../../media/SonnenSystem/4_0_mars_8k.webp");
    m_marsDiffuseMap = m_texManager.createTexture2D("mars_Kd", img_mars_Kd, soMars);

    de::Image img_mars_bump;
    dbLoadImage(img_mars_bump, "../../media/SonnenSystem/4_0_mars_8k_bump.webp");
    m_marsBumpMap = m_texManager.createTexture2D("mars_bump", img_mars_bump, soMars);

    initTessSphereShader2(
        glm::vec3(250,0,0),
        glm::vec3(100,100,100),
        m_marsDiffuseMap,
        m_marsBumpMap,
        3.0f );
*/

}

void drawScene()
{
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClearDepthf(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto it = m_bodyDatabase.m_data.begin();
              it != m_bodyDatabase.m_data.end(); ++it)
    {
        Body & body = it->second;
        m_bodyRenderer.draw( body );
    }
}

void resizeEvent(GLFWwindow* window, int width, int height)
{
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    //glViewport(0, 0, width, height);
    // Optional: trigger a redraw here

}

void paintEvent(GLFWwindow* window)
{
    drawScene();
    glfwSwapBuffers(m_window);
    totalFrames++;
}
// glfwGetWindowUserPointer(window)
// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
// camera.freeze = true;
// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

void mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    const int mx = xpos;
    const int my = ypos;

    if ( m_cameraFreeMoveEnabled )
    {
        if (m_firstMouse)
        {
            m_firstMouse = false;
        }
        else
        {
            m_mouseMoveX = mx - m_mouseX;
            m_mouseMoveY = my - m_mouseY;

            auto camera = m_driver->getCamera();
            camera->yaw( 0.003f * m_mouseMoveX );
            camera->pitch( 0.003f * m_mouseMoveY );
        }
    }

    m_mouseMoveX = 0; // Reset
    m_mouseMoveY = 0; // Reset
    m_mouseX = mx; // Store current value
    m_mouseY = my; // Store current value

    // pick();
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        m_keyStates[ key ] = true;
    }
    if (action == GLFW_RELEASE)
    {
        m_keyStates[ key ] = false;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        //     const auto key = event.keyPressEvent.key;
        //     // Toggle visibility of MainMenu:
        //     if (key == de::KEY_ESCAPE)
        //     {
        //         // if (m_uiMainmenu)
        //         // {
        //         //     bool bVisible = m_uiMainmenu->isVisible();
        //         //     m_uiMainmenu->setVisible( !bVisible );
        //         // }
        //     }

        //     // F11 - Toggle window resizable
        //     if (key == de::KEY_F11)
        //     {
        //         auto window = m_device->getWindow();
        //         if (window)
        //         {
        //             window->setResizable( !window->isResizable() );
        //         }
        //     }
        //     // F12|F - Toggle window fullscreen
        //     if (key == de::KEY_F12 || key == de::KEY_F )
        //     {
        //         auto window = m_device->getWindow();
        //         if (window)
        //         {
        //             window->setFullScreen( !window->isFullScreen() );
        //         }
        //     }


        //     // SPACE - Toggle help overlay
        //     if (key == de::KEY_SPACE) // SPACE - Toggle overlay
        //     {
        //         m_isCameraMouseInputEnabled = !m_isCameraMouseInputEnabled;
        //         if (m_isCameraMouseInputEnabled)
        //         {
        //             m_firstMouse = true;
        //         }
        //         //camera->setInputEnabled( !camera->isInputEnabled() );
        //         // m_showHelpOverlay = !m_showHelpOverlay;
        //     }
        //     // PAGE_UP - Increase font size
        //     if (key == de::KEY_PAGE_UP)
        //     {
        //         // setScalePc( std::min( 400, getScalePc() + 10 ) );
        //         // UI_resizeLayouts();
        //     }
        //     // PAGE_DOWN - decrease font size
        //     if (key == de::KEY_PAGE_DOWN)
        //     {
        //         // setScalePc( std::max( 50, getScalePc() - 10 ) );
        //         // UI_resizeLayouts();
        //     }
        //     // ARROW_UP - Increase frame wait time in ms - lower FPS
        //     if (key == de::KEY_UP)
        //     {
        //     }

        //     // ARROW_DOWN - Decrease frame wait time in ms - increase FPS
        //     if (key == de::KEY_DOWN)
        //     {
        //     }

        //     // // Move camera
        //     // auto camera = getCamera();
        //     // if (camera)
        //     // {
        //     //     if (key == de::KEY_UP)   { camera->move( 2.0f ); }
        //     //     if (key == de::KEY_DOWN) { camera->move( -1.0f ); }
        //     //     if (key == de::KEY_LEFT) { camera->strafe( -1.0f ); }
        //     //     if (key == de::KEY_RIGHT) { camera->strafe( 1.0f ); }
        //     //     if (key == de::KEY_W) { camera->move( 2.0f ); }
        //     //     if (key == de::KEY_S) { camera->move( -1.0f ); }
        //     //     if (key == de::KEY_A) { camera->strafe( -1.0f ); }
        //     //     if (key == de::KEY_D) { camera->strafe( 1.0f ); }
        //     // }
    }

}


int main()
{


    if (!glfwInit())
    {
		glfwTerminate();
        DE_ERROR("No glfwInit()")
        return 1;
    }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
    m_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, " ", NULL, NULL);
    if (!m_window)
    {
		glfwTerminate();
        DE_ERROR("No window")
        return 1;
    }

    glfwMakeContextCurrent(m_window);

    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK)
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
			glfwTerminate();
            DE_ERROR("No glew")
            return 1;
        }
    }

    m_driver = new de::gl::Bridge();
    m_driver->init();

    createScene();

    //glfwSetWindowUserPointer(m_window, &cameraHandler);
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetCursorPosCallback(m_window, mousePositionCallback);
    glfwSetFramebufferSizeCallback(m_window, resizeEvent);
    glfwSetWindowRefreshCallback(m_window, paintEvent);

    // MainLoop:
    double FRAME_DURATION = 1.0 / 60.0; // tRenderWait
    double m_timeStart = dbTimeInSeconds();
    double m_timeNow = 0.0;
    double m_timeLastMainLoop = 0.0;
    double m_timeLastCameraUpdate = 0.0;
    double m_timeLastRenderUpdate = 0.0;
    double m_timeLastWindowTitleUpdate = 0.0;
    // fpsComputer.reset();

    //glfwPollEvents();

    while (!glfwWindowShouldClose(m_window))
    {
        m_timeNow = dbTimeInSeconds() - m_timeStart;
        double dtMainLoop = m_timeNow - m_timeLastMainLoop;
        m_timeLastMainLoop = m_timeNow;

        // camera
        // -----
        double fMoveSpeed = 75.5;
        double dtCameraUpdate = m_timeNow - m_timeLastCameraUpdate;
        if ( dtCameraUpdate >= (1.0 / 200.0) )
        {
            m_timeLastCameraUpdate = m_timeNow;

            auto camera = m_driver->getCamera();
            if (camera)
            {
                if (m_keyStates[GLFW_KEY_UP] || m_keyStates[GLFW_KEY_W])
                {
                    camera->move(fMoveSpeed*dtCameraUpdate);
                }
                if (m_keyStates[GLFW_KEY_DOWN] || m_keyStates[GLFW_KEY_S])
                {
                    camera->move(-fMoveSpeed*dtCameraUpdate);
                }
                if (m_keyStates[GLFW_KEY_LEFT] || m_keyStates[GLFW_KEY_A])
                {
                    camera->strafe(-fMoveSpeed*dtCameraUpdate);
                }
                if (m_keyStates[GLFW_KEY_RIGHT] || m_keyStates[GLFW_KEY_D])
                {
                    camera->strafe(fMoveSpeed*dtCameraUpdate);
                }
            }
        }

        // // render
        // // -----
        // double tRenderWait = 1.0 / 60.0;
        // double dtRenderUpdate = m_timeNow - m_timeLastRenderUpdate;
        // if ( dtCameraUpdate >= tRenderWait )
        // {
        //     m_timeLastRenderUpdate = m_timeNow;

        // }

        paintEvent(m_window);
    //     drawAll()

    //     // Swap buffers and poll events
    //     glfwSwapBuffers(m_window);
    //     totalFrames++;
        glfwPollEvents();

        // --- Frame limiting ---
        double sleepTime = FRAME_DURATION - dtMainLoop;
        if (sleepTime > 0.0)
        {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
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

    glfwDestroyWindow(m_window);
	glfwTerminate();
	return 0;
}
