#include "H3_Window_qt6.h"

#ifdef HAVE_QT6

#if QT_VERSION_MAJOR == 5
    // empty
#elif QT_VERSION_MAJOR == 6
    #include <QOpenGLVersionFunctionsFactory>
#else
    // empty
#endif

// ===========================================================================
GLWidget::GLWidget(H3_Game* game, QOpenGLContext *sharedContext, QWidget *parent)
// ===========================================================================
    : QOpenGLWidget(parent)
    , m_sharedContext(sharedContext)
    , m_game( game )
{
    setContentsMargins(0,0,0,0);
    //setFocusPolicy(Qt::StrongFocus);    // Process keypress-events
    setMouseTracking(true);             // Process mousemove-events
}

GLWidget::~GLWidget()
{
    close();
    stopFpsTimer();
    //m_renderer.deinitializeGL();
}

void GLWidget::startFpsTimer()
{
    m_fpsTimerId = startTimer(1000 / m_fpsTarget);
}
void GLWidget::stopFpsTimer()
{
    killTimer(m_fpsTimerId);
}

void GLWidget::timerEvent(QTimerEvent* event)
{
    if ( event->timerId() == m_fpsTimerId )
    {
        update();
    }
}

void GLWidget::initializeGL()
{
    context()->setShareContext(m_sharedContext);
    initializeOpenGLFunctions();

    m_game->init( width(), height(), H3_Cfg::createStandard() );

    startFpsTimer();
}

void GLWidget::deinitializeGL()
{
    m_game->deinit();
}

void GLWidget::resizeGL(int w, int h)
{
    //m_screenWidth = w;
    //m_screenHeight = h;
}

void GLWidget::paintGL()
{
    //int w = m_screenWidth;
    //int h = m_screenHeight;
    m_game->render( width(), height() );
}

void GLWidget::mousePressEvent( QMouseEvent* event )
{
    de::MouseButtonEvent e;
    e.x = event->position().x();
    e.y = event->position().y();
    e.flags = de::MouseFlag::Pressed;
    if ( event->buttons() & Qt::LeftButton ) { e.buttons |= de::MouseButton::Left; }
    if ( event->buttons() & Qt::RightButton ) { e.buttons |= de::MouseButton::Right; }
    if ( event->buttons() & Qt::MiddleButton ) { e.buttons |= de::MouseButton::Middle; }

    H3_Game_mousePressEvent( *m_game, e );
}

void GLWidget::mouseReleaseEvent( QMouseEvent* event )
{
    de::MouseButtonEvent e;
    e.x = event->position().x();
    e.y = event->position().y();
    e.flags = de::MouseFlag::Released;
    if ( event->buttons() & Qt::LeftButton ) { e.buttons |= de::MouseButton::Left; }
    if ( event->buttons() & Qt::RightButton ) { e.buttons |= de::MouseButton::Right; }
    if ( event->buttons() & Qt::MiddleButton ) { e.buttons |= de::MouseButton::Middle; }

    H3_Game_mouseReleaseEvent( *m_game, e );
}


void GLWidget::mouseMoveEvent( QMouseEvent* event )
{
    de::MouseMoveEvent e;
    e.x = event->pos().x();
    e.y = event->pos().y();
    //e.flags = de::MouseFlag::Moved;

    H3_Game_mouseMoveEvent( *m_game, e );
}

void GLWidget::wheelEvent( QWheelEvent* event )
{
    de::MouseWheelEvent e;
    e.x = event->angleDelta().x();
    e.y = event->angleDelta().y();

    H3_Game_mouseWheelEvent( *m_game, e );
}


Window::Window(QWidget* parent) : QWidget(parent)
{
    setContentsMargins(0,0,0,0);

    // Create a custom OpenGL context
    QSurfaceFormat format;
    format.setVersion(4, 5);
    //format.setProfile(QSurfaceFormat::CoreProfile);
    //format.setVersion(3, 3);
    //format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setOptions(QSurfaceFormat::DebugContext); // QSurfaceFormat::DeprecatedFunctions |
    //format.setOption(QSurfaceFormat::DebugContext);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(0);
    format.setAlphaBufferSize(0);
    //format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    //format.setColorSpace(QSurfaceFormat::sRGBColorSpace);
    format.setStereo(false);
    format.setSwapInterval(1);
    QSurfaceFormat::setDefaultFormat(format);

    m_customContext = new QOpenGLContext;
    m_customContext->setFormat(format);
    if (!m_customContext->create())
    {
        qDebug() << "Failed to create custom OpenGL context";
        //return -1;
    }

    // Create a temporary offscreen surface to make the custom context current
    m_offscreenSurface = new QOffscreenSurface;
    m_offscreenSurface->setFormat(m_customContext->format());
    m_offscreenSurface->create();

    m_customContext->makeCurrent(m_offscreenSurface);

    m_GLWidget = new GLWidget(&m_game, m_customContext);

    //m_GLWidget->setSampleSource( &m_audioEngine.m_dspSampleCollector );

    m_helpWidget = new QTextBrowser(this);
    QString htmlContent = R"(
    <h1>Welcome!</h1>
    <p>This is a <b>simple</b> HTML text displayed in Qt.</p>
    )";
    m_helpWidget->setHtml(htmlContent);

    m_splitter = new QSplitter(this);
    m_splitter->addWidget(m_GLWidget);
    m_splitter->addWidget(m_helpWidget);

    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(m_splitter);

    setLayout(mainLayout);

    m_helpWidget->setVisible(false);

    H3_Game_setWindow( m_game, this );

    H3_Game_setHelpWindow( m_game, m_helpWidget );

    //m_audioEngine.start();
}

Window::~Window()
{
    //m_audioEngine.stop();
    delete m_customContext;
    delete m_offscreenSurface;
}


void Window::keyPressEvent( QKeyEvent* event )
{
    de::KeyEvent e;
    e.key = translateQKey(event->key());
    e.modifiers = de::KeyModifier::Pressed;

    H3_Game_keyPressEvent( m_game, e );

    if (event->key() == Qt::Key_F1)
    {
        H3_Game_toggleHelpWindow( m_game );
    }
}

void Window::keyReleaseEvent( QKeyEvent* event )
{
    de::KeyEvent e;
    e.key = translateQKey(event->key());
    e.modifiers = de::KeyModifier::Released;

    H3_Game_keyReleaseEvent( m_game, e );
}

// [Keyboard] Qt::Key enum -> Benni's de::EKEY enum
u32 translateQKey( int qtkey )
{
    switch ( qtkey )
    {
    case Qt::Key_0: return ::de::KEY_0; // No VK enums for these? Are they all language dependent aswell?
    case Qt::Key_1: return ::de::KEY_1;
    case Qt::Key_2: return ::de::KEY_2;
    case Qt::Key_3: return ::de::KEY_3;
    case Qt::Key_4: return ::de::KEY_4;
    case Qt::Key_5: return ::de::KEY_5;
    case Qt::Key_6: return ::de::KEY_6;
    case Qt::Key_7: return ::de::KEY_7;
    case Qt::Key_8: return ::de::KEY_8;
    case Qt::Key_9: return ::de::KEY_9;

    case Qt::Key_A: return ::de::KEY_A; // No VK enums for these? Are they all language dependent aswell?
    case Qt::Key_B: return ::de::KEY_B;
    case Qt::Key_C: return ::de::KEY_C;
    case Qt::Key_D: return ::de::KEY_D;
    case Qt::Key_E: return ::de::KEY_E;
    case Qt::Key_F: return ::de::KEY_F;
    case Qt::Key_G: return ::de::KEY_G;
    case Qt::Key_H: return ::de::KEY_H;
    case Qt::Key_I: return ::de::KEY_I;
    case Qt::Key_J: return ::de::KEY_J;
    case Qt::Key_K: return ::de::KEY_K;
    case Qt::Key_L: return ::de::KEY_L;
    case Qt::Key_M: return ::de::KEY_M;
    case Qt::Key_N: return ::de::KEY_N;
    case Qt::Key_O: return ::de::KEY_O;
    case Qt::Key_P: return ::de::KEY_P;
    case Qt::Key_Q: return ::de::KEY_Q;
    case Qt::Key_R: return ::de::KEY_R;
    case Qt::Key_S: return ::de::KEY_S;
    case Qt::Key_T: return ::de::KEY_T;
    case Qt::Key_U: return ::de::KEY_U;
    case Qt::Key_V: return ::de::KEY_V;
    case Qt::Key_W: return ::de::KEY_W;
    case Qt::Key_X: return ::de::KEY_X;
    case Qt::Key_Y: return ::de::KEY_Y;
    case Qt::Key_Z: return ::de::KEY_Z;

    case Qt::Key_F1: return ::de::KEY_F1;
    case Qt::Key_F2: return ::de::KEY_F2;
    case Qt::Key_F3: return ::de::KEY_F3;
    case Qt::Key_F4: return ::de::KEY_F4;
    case Qt::Key_F5: return ::de::KEY_F5;
    case Qt::Key_F6: return ::de::KEY_F6;
    case Qt::Key_F7: return ::de::KEY_F7;
    case Qt::Key_F8: return ::de::KEY_F8;
    case Qt::Key_F9: return ::de::KEY_F9;
    case Qt::Key_F10: return ::de::KEY_F10; // F10 not working on Logitech K280e. d.k. the fuck why.
    case Qt::Key_F11: return ::de::KEY_F11;
    case Qt::Key_F12: return ::de::KEY_F12;
    case Qt::Key_F13: return ::de::KEY_F13;
    case Qt::Key_F14: return ::de::KEY_F14;
    case Qt::Key_F15: return ::de::KEY_F15;
    case Qt::Key_F16: return ::de::KEY_F16;
    case Qt::Key_F17: return ::de::KEY_F17;
    case Qt::Key_F18: return ::de::KEY_F18;
    case Qt::Key_F19: return ::de::KEY_F19;
    case Qt::Key_F20: return ::de::KEY_F20;
    case Qt::Key_F21: return ::de::KEY_F21;
    case Qt::Key_F22: return ::de::KEY_F22;
    case Qt::Key_F23: return ::de::KEY_F23;
    case Qt::Key_F24: return ::de::KEY_F24;

    case Qt::Key_Escape: return ::de::KEY_ESCAPE;
    case Qt::Key_Return: return ::de::KEY_ENTER;
    case Qt::Key_Backspace: return ::de::KEY_BACKSPACE;
    case Qt::Key_Space: return ::de::KEY_SPACE;
    case Qt::Key_Tab: return ::de::KEY_TAB;
    case Qt::Key_Shift: return ::de::KEY_LEFT_SHIFT;
    //case Qt::Key_SHIFT: return ::de::KEY_RIGHT_SHIFT;

    //case Qt::Key_MENU: return ::de::KEY_LEFT_SUPER;
    //case Qt::Key_LMENU: return ::de::KEY_LEFT_SUPER;
    //case Qt::Key_RMENU: return ::de::KEY_RIGHT_SUPER;

    case Qt::Key_Super_L: return ::de::KEY_LEFT_SUPER;
    case Qt::Key_Super_R: return ::de::KEY_RIGHT_SUPER;
    case Qt::Key_Control: return ::de::KEY_LEFT_CTRL;
    //case Qt::Key_RCONTROL: return ::de::KEY_RIGHT_CTRL;

    //      else if (key == GLFW_KEY_LEFT_ALT ) { key_key = KEY_LEFT_ALT; }
    //      else if (key == GLFW_KEY_RIGHT_ALT ) { key_key = KEY_RIGHT_ALT; }
    //      else if (key == GLFW_KEY_LEFT_CONTROL ) { key_key = KEY_LEFT_CTRL; }
    //      else if (key == GLFW_KEY_RIGHT_CONTROL ) { key_key = KEY_RIGHT_CTRL; }
    //      else if (key == GLFW_KEY_LEFT_SUPER ) { key_key = KEY_LEFT_SUPER; }
    //      else if (key == GLFW_KEY_RIGHT_SUPER ) { key_key = KEY_RIGHT_SUPER; }
    //      else if (key == GLFW_KEY_CAPS_LOCK ) { key_key = KEY_CAPS_LOCK; }

    // OEM special keys: All German Umlaute so far and special characters
    // I dont own US or Japanese keyboards. Please add your special chars if needed.
//    case Qt::Key_OEM_1: return ::de::KEY_OEM_1;            // Ü, Qt::Key_OEM_1 = 186 = 0xBA
    case Qt::Key_Plus: return ::de::KEY_OEM_PLUS;      // Qt::Key_OEM_PLUS = 187 = 0xBB
    case Qt::Key_Comma: return ::de::KEY_OEM_COMMA;    // Qt::Key_OEM_COMMA = 188 = 0xBC
    case Qt::Key_Minus: return ::de::KEY_OEM_MINUS;    // Qt::Key_OEM_MINUS = 189 = 0xBD
    case Qt::Key_Period: return ::de::KEY_OEM_PERIOD;  // Qt::Key_OEM_PERIOD = 190 = 0xBE
    // case Qt::Key_OEM_2: return ::de::KEY_OEM_2;            // Qt::Key_OEM_2 = 191 = 0xBF
    // case Qt::Key_OEM_3: return ::de::KEY_OEM_3;            // Ö, Qt::Key_OEM_3 = 192 = 0xC0
    // case Qt::Key_OEM_4: return ::de::KEY_OEM_4;            // ß, Qt::Key_OEM_4 = 219 = 0xDB
    // case Qt::Key_OEM_5: return ::de::KEY_OEM_5;            // Qt::Key_OEM_5 = 220 = 0xDC
    // case Qt::Key_OEM_6: return ::de::KEY_OEM_6;            // Qt::Key_OEM_6 = 221 = 0xDD
    // case Qt::Key_OEM_7: return ::de::KEY_OEM_7;            // Ä, Qt::Key_OEM_7 = 222 = 0xDE
    // case Qt::Key_OEM_102: return ::de::KEY_OEM_102;        // <|>, VK_OEM_8 = 226 = 0xDE

        // 4x Arrow keys:
    case Qt::Key_Up: return ::de::KEY_UP;
    case Qt::Key_Left: return ::de::KEY_LEFT;
    case Qt::Key_Down: return ::de::KEY_DOWN;
    case Qt::Key_Right: return ::de::KEY_RIGHT;

        // 3x control buttons:
    case Qt::Key_Print: return ::de::KEY_SNAPSHOT; // PRINT SCREEN, not the PRINT (only) key
    case Qt::Key_ScrollLock: return ::de::KEY_SCROLL_LOCK;
    case Qt::Key_Pause: return ::de::KEY_PAUSE;

        // 6x control buttons:
    case Qt::Key_Insert: return ::de::KEY_INSERT;
    case Qt::Key_Home: return ::de::KEY_HOME;
    case Qt::Key_End: return ::de::KEY_END;
    case Qt::Key_Delete: return ::de::KEY_DELETE;
    case Qt::Key_PageUp: return ::de::KEY_PAGE_UP;
    case Qt::Key_PageDown: return ::de::KEY_PAGE_DOWN;

        // Numpad:
    case Qt::Key_NumLock: return ::de::KEY_NUM_LOCK;
    // case Qt::Key_NUMPAD0: return ::de::KEY_KP_0;
    // case Qt::Key_NUMPAD1: return ::de::KEY_KP_1;
    // case Qt::Key_NUMPAD2: return ::de::KEY_KP_2;
    // case Qt::Key_NUMPAD3: return ::de::KEY_KP_3;
    // case Qt::Key_NUMPAD4: return ::de::KEY_KP_4;
    // case Qt::Key_NUMPAD5: return ::de::KEY_KP_5;
    // case Qt::Key_NUMPAD6: return ::de::KEY_KP_6;
    // case Qt::Key_NUMPAD7: return ::de::KEY_KP_7;
    // case Qt::Key_NUMPAD8: return ::de::KEY_KP_8;
    // case Qt::Key_NUMPAD9: return ::de::KEY_KP_9;
    case Qt::Key_multiply: return ::de::KEY_KP_MULTIPLY; // Is that correct mapping?
    //case Qt::Key_ADD: return ::de::KEY_KP_ADD; // Is that correct mapping?
    //case Qt::Key_SEPARATOR: return ::de::KEY_KP_SEPARATOR; // Is that correct mapping?
    //case Qt::Key_SUBTRACT: return ::de::KEY_KP_SUBTRACT; // Is that correct mapping?
    //case Qt::Key_DECIMAL: return ::de::KEY_KP_DECIMAL; // Is that correct mapping?
    //case Qt::Key_DIVIDE: return ::de::KEY_KP_DIVIDE; // Is that correct mapping?

    default:
    {
        DE_WARN("Cant translate Qt::Key(", int(qtkey), ")")
        return de::KEY_UNKNOWN;
    }
    }
}

#endif
