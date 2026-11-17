#include <de/gpu/VideoDriver.h>
#include <de/IrrlichtDevice.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

#ifdef _WIN32
    // #ifndef WIN32_LEAN_AND_MEAN
    // #define WIN32_LEAN_AND_MEAN
    // #endif
    // #include <windows.h>
    #define APIENTRY __stdcall
#else
    #define APIENTRY
#endif

#ifndef USE_GL_DEBUG_CALLBACK
#define USE_GL_DEBUG_CALLBACK
#endif

namespace de {
namespace gpu {

VideoDriver* createVideoDriver( int w, int h, uint64_t windowId)
{
    VideoDriver* pDriver = new VideoDriver();
    if (!pDriver->open(w,h))
    {
        DE_ERROR("Driver not opened")
        delete pDriver;
        pDriver = nullptr;
    }
    return pDriver;
}

#ifdef USE_GL_DEBUG_CALLBACK

std::string dbgSourceStr(u32 debugSource)
{
    switch (debugSource)
    {
    case GL_DEBUG_SOURCE_API: return "API"; // Direkt aus den offiziellen OpenGL-Funktionsaufrufen.
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW_SYSTEM"; // Vom Fenstermanager oder dem Betriebssystem (z. B. Probleme mit dem Kontext).
    case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER_COMPILER"; // Vom Shader-Compiler (z. B. Warnungen beim Kompilieren von GLSL).
    case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD_PARTY"; // Von einer externen Applikation oder Middleware (z. B. einer GUI-Bibliothek).
    case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION"; // Von dir selbst (du kannst mit glDebugMessageInsert eigene Nachrichten einspeisen).
    case GL_DEBUG_SOURCE_OTHER: return "OTHER"; // Keine der oben genannten Quellen.
    default: return dbStr("UNKNOWN_SOURCE_",dbHex(debugSource));
    }
}

std::string dbgTypeStr(u32 debugType)
{
    switch (debugType)
    {
    case GL_DEBUG_TYPE_ERROR: return "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
    case GL_DEBUG_TYPE_OTHER: return "OTHER";
    case GL_DEBUG_TYPE_MARKER: return "MARKER";
    case GL_DEBUG_TYPE_PUSH_GROUP: return "PUSH_GROUP";
    case GL_DEBUG_TYPE_POP_GROUP: return "POP_GROUP";
    default: return dbStr("UNKNOWN_TYPE_",dbHex(debugType));
    }
}

std::string dbgSeverStr(u32 debugSeverity)
{
    switch (debugSeverity)
    {
    case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
    case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
    case GL_DEBUG_SEVERITY_LOW: return "LOW";
    case GL_DEBUG_SEVERITY_NOTIFICATION: return "INFO";
    default: return dbStr("UNKNOWN_SEVERITY_",dbHex(debugSeverity));
    }
}

static void APIENTRY VD_DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    DE_ERROR("GL[",dbgTypeStr(type),"] "
        "source(",dbgSourceStr(source),"), "
        "severity(",dbgSeverStr(severity),")")
    DE_ERROR("message(",message,")")
}
#endif

std::string glGetStdString( u32 value )
{
    auto p = (char const*)glGetString( value );
    if ( p )
    {
        return p;
    }
    else
    {
        return "";
    }
}


// ===========================================================================
VideoDriver::VideoDriver()
// ===========================================================================
    : m_screenWidth(800)
    , m_screenHeight(600)
    , m_modelMatrix( 1.0 )
{
    m_camera = &m_camera0;
}

VideoDriver::~VideoDriver()
{
    close();
}

/*
void VideoDriver::setIrrlichtDevice(IrrlichtDevice* device)
{
    m_device = device;
}

IrrlichtDevice* VideoDriver::getIrrlichtDevice()
{
    return m_device;
}
*/

bool VideoDriver::open(int w, int h, int debugLevel)
{
    m_debugLevel = debugLevel;
    if (m_debugLevel > 0)
    {
        DE_OK("w(",w,"), h(",h,")")
    }
    m_screenWidth = w;
    m_screenHeight = h;
    m_shader = nullptr;
    m_rt = nullptr;
    //m_fpsComputer.reset();
    //m_createParams = params;
    m_shaderVersionMajor = 0;
    m_shaderVersionMinor = 0;
    m_useGLES = false;
    m_useCoreProfile = false;

    ensureDesktopOpenGL(); // initGlew()
GL_VALIDATE
    // std::string s_GL_EXTENSIONS;
    // std::string s_GL_VERSION;
    // std::string s_GL_VENDOR;
    // std::string s_GL_RENDERER;
    // std::string s_GL_SHADING_LANGUAGE_VERSION;

    // s_GL_EXTENSIONS = (char const*)glGetString(GL_EXTENSIONS);
    // s_GL_VERSION = (char const*)glGetString(GL_VERSION);
    // s_GL_VENDOR = (char const*)glGetString(GL_VENDOR);
    // s_GL_RENDERER = (char const*)glGetString(GL_RENDERER);
    // s_GL_SHADING_LANGUAGE_VERSION = (char const*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (m_debugLevel > 0)
    {
        DE_OK("GL_VERSION = ", glGetStdString(GL_VERSION))
        DE_OK("GL_VENDOR = ", glGetStdString(GL_VENDOR))
        DE_OK("GL_RENDERER = ", glGetStdString(GL_RENDERER))
        DE_OK("GL_SHADING_LANGUAGE_VERSION = ", glGetStdString(GL_SHADING_LANGUAGE_VERSION))
        auto extensionList = glGetStdString(GL_EXTENSIONS);
        auto extensions = StringUtil::split(extensionList,' ');
        DE_OK("GL_EXTENSIONS = ", extensions.size())
        if (m_debugLevel > 1)
        {
            for (size_t i = 0; i < extensions.size(); ++i)
            {
                DE_TRACE("GL_EXTENSION[",i,"] ", extensions[i])
            }

            //dumpVideoModes();
        }
    }


// Effing spam on JUCE + IntelDriver -> deactivate, i know how to program gfx.
#ifdef USE_GL_DEBUG_CALLBACK
    // DebugOutput
    glEnable(GL_DEBUG_OUTPUT); GL_VALIDATE;
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); GL_VALIDATE;
    glDebugMessageCallback(VD_DebugMessageCallback, 0);
#endif

GL_VALIDATE
    // ShaderManager
    glGetIntegerv( GL_MAJOR_VERSION, &m_shaderVersionMajor );
    glGetIntegerv( GL_MINOR_VERSION, &m_shaderVersionMinor );
GL_VALIDATE
    // TexManager
    m_texMgr.init();

    // createRenderTarget_HDR("hdr",1024,768);

    // RenderStates
    m_state = State::query();
    if (m_debugLevel > 0)
    {
        DE_DEBUG("Initial Viewport: ",m_state.viewport.str())
        DE_DEBUG("Initial Scissor: ",m_state.scissor.str())
        DE_DEBUG("Initial CullMode: ",m_state.culling.str())
        DE_DEBUG("Initial DepthTest: ",m_state.depth.str())
        DE_DEBUG("Initial StencilTest: ",m_state.stencil.str())
        DE_DEBUG("Initial BlendMode: ",m_state.blend.str())
        DE_DEBUG("Initial PointSize: ",m_state.pointSize.str())
        DE_DEBUG("Initial LineWidth: ",m_state.lineWidth.str())
        DE_DEBUG("Initial RasterizerDiscard: ",m_state.rasterizerDiscard.str())
        DE_DEBUG("Initial PolygonOffset: ",m_state.polygonOffset.str())
    }

    State state;
    state.culling = Culling();
    state.depth = Depth();
    state.stencil = Stencil::disabled();
    state.blend = Blend::disabled();
    setState(state);

    if (m_debugLevel > 0)
    {
        DE_DEBUG("Current CullMode: ",m_state.culling.str())
        DE_DEBUG("Current DepthTest: ",m_state.depth.str())
        DE_DEBUG("Current StencilTest: ",m_state.stencil.str())
        DE_DEBUG("Current BlendMode: ",m_state.blend.str())
    }

    // // Enable depth testing
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);

    // // Enable face culling
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CCW);

    // // Enable blending for transparency
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // // Smooth shading (optional for older OpenGL versions)
    //glShadeModel(GL_SMOOTH);

    // WICHTIG: Erzwinge Byte-Alignment für den
    //          CPU-Upload (behebt RGB-Padding Fehler)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    setClearColor( glm::vec4{0.1f,0.1f,0.1f,1.0f} );
GL_VALIDATE
    setClearDepth( 1.0f );
GL_VALIDATE
    setClearStencil( 0 );
GL_VALIDATE
    m_skyboxRenderer.init( this );
GL_VALIDATE
    m_smaterialRenderer.init( this );
GL_VALIDATE
    //m_pmaterialRenderer.init( this );
    m_screenRenderer.init( this );
GL_VALIDATE
    m_fontRenderer5x8.init( this );
GL_VALIDATE
    m_fontRenderer.init( this );
GL_VALIDATE
    m_line3dRenderer.init( this );
GL_VALIDATE
    m_postFxRenderer.init( this );
GL_VALIDATE
    m_screenQuadRenderer.init( this );
GL_VALIDATE
    m_sceneManager.init( this );
GL_VALIDATE
    //m_guienv.init( this );

    // FPSComputer
    m_timeEpoch = dbTimeInSeconds(); // the current epoch ( keeps numeric values low and highp )
    m_timeNow = 0.0;           // relative to epoch
    m_timeLastFPSCompute = 0.0;// relative to epoch

    m_frameCount = 0;
    m_frameCountLast = 0;
    m_fps = 0.0;

    // m_timeFrameStart = 0.0; // relative to epoch
    // m_timeFrameEnd = 0.0;   // relative to epoch

    // m_drawCalls = 0;
    // m_drawCallsLastFrame = 0;
    // m_drawCallsLastSecond = 0;
    // m_drawCallsPerFrame = 0;
    // m_drawCallsPerSecond = 0;
    return true;
}

void VideoDriver::close()
{
    //m_shaderManager.destroy();
    //clearShaders();
    //m_texMgr.deinit();
    clearRenderTargets();
}

void VideoDriver::resize( int w, int h )
{
    if (w < 1 || h < 1)
    {
        // DE_ERROR("")
        return;
    }

    if ((w == m_screenWidth) && (h == m_screenHeight))
    {
        return; // Nothing todo.
    }

    // DE_DEBUG("Screen(",w,",",h,")")

    m_screenWidth = w;
    m_screenHeight = h;
    m_viewport = Recti(0,0,w,h);

    if (getCamera())
    {
        getCamera()->setScreenSize(w,h);
        getCamera()->update();
    }
}

bool VideoDriver::beginRender(const glm::vec4& clearColor)
{
    m_timeNow = dbTimeInSeconds() - m_timeEpoch;
    // DE_BENNI("m_time = ",m_timeNow)

    const int w = getScreenWidth();
    const int h = getScreenHeight();
    if (w < 1 || h < 1) return false;

    if (m_rt)
    {
        DE_WARN("RenderTarget still bound")
        m_rt = nullptr;
    }

    setViewport(Recti(0,0,w,h));
GL_VALIDATE
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
GL_VALIDATE
    glClearDepth(1.0f); // glClearDepthf ist OpenGL ES
GL_VALIDATE
    glClearStencil(0);
GL_VALIDATE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
GL_VALIDATE

    if (getCamera())
    {
        getCamera()->setScreenSize(w,h);
        getCamera()->update();
    }
    return true;
}

void VideoDriver::endRender()
{
    //glFlush();
    //m_window->swapBuffers();
    // Add item to frame history... (not yet)

    m_timeNow = dbTimeInSeconds() - m_timeEpoch;
    //m_timeFrameEnd = m_timeNow;

    m_frameCount++;

    // Compute FPS after one second passed
    double dt = m_timeNow - m_timeLastFPSCompute;
    if ( dt >= 1.0 )
    {
        m_timeLastFPSCompute = m_timeNow;
        m_fps = double( m_frameCount - m_frameCountLast ) / dt; // divide by actual time passed.
        m_frameCountLast = m_frameCount;

        //m_drawCallsPerSecond = double( m_drawCalls - m_drawCallsLastSecond ) / dt;
        //m_drawCallsLastSecond = m_drawCalls;
    }

    //m_drawCallsPerFrame = m_drawCalls - m_drawCallsLastFrame;
    //m_drawCallsLastFrame = m_drawCalls;

    // DE_BENNI("endRender()")
}

void VideoDriver::beginRender( IRenderTarget* rt, const glm::vec4& clearColor )
{
    if (!rt)
    {
        DE_ERROR("No renderTarget")
        return;
    }

    if (m_rt)
    {
        // m_rt->unbind();
        DE_ERROR("RenderTarget still bound")
    }

    m_rt = rt;
    int w = m_rt->w();
    int h = m_rt->h();
    setViewport(Recti(0,0,w,h));

    // auto camera = getCamera();
    // if (camera)
    // {
    //     camera->setScreenSize(w,h);
    //     camera->update();
    // }

    m_rt->bind();
    m_rt->clear(clearColor);
}

void VideoDriver::endRender( IRenderTarget* rt )
{
    if (!rt)
    {
        DE_ERROR("No renderTarget")
        return;
    }

    rt->unbind();

    m_rt = nullptr;
}

int VideoDriver::getScreenWidth() const { return m_screenWidth; }
int VideoDriver::getScreenHeight() const { return m_screenHeight; }
int VideoDriver::getRenderWidth() const { return m_rt ? m_rt->w() : m_screenWidth; }
int VideoDriver::getRenderHeight() const { return m_rt ? m_rt->h() : m_screenHeight; }

uint64_t VideoDriver::getFrameCount() const
{
    return m_frameCount;
}
double VideoDriver::getTime() const
{
    return m_timeNow;
}
double VideoDriver::getFPS() const
{
    return m_fps;
}

Recti
VideoDriver::getViewport() const
{
    return m_viewport.m_rect;
}

int VideoDriver::getViewportX() const { return m_viewport.m_rect.x; }
int VideoDriver::getViewportY() const { return m_viewport.m_rect.y; }
int VideoDriver::getViewportW() const { return m_viewport.m_rect.w; }
int VideoDriver::getViewportH() const { return m_viewport.m_rect.h; }

void
VideoDriver::setViewport(const Recti& rect)
{
    m_viewport.m_rect = rect;
    int x = rect.x;
    int y = rect.y;
    int w = rect.w;
    int h = rect.h;
    if ((w > 0) && (h > 0))
    {
        glViewport(x,y,w,h);
    }
}

Recti
VideoDriver::getScissor() const
{
    return m_scissor.m_rect;
}
void
VideoDriver::setScissor(const Recti& rect)
{
    m_scissor.m_rect = rect;
    int x = rect.x;
    int y = getScreenHeight() - (rect.y + rect.h);
    int w = rect.w;
    int h = rect.h;
    if ((w > 0) && (h > 0))
    {
        ::glScissor( x,y,w,h );
        ::glEnable( GL_SCISSOR_TEST );
    }
    else
    {
        ::glDisable( GL_SCISSOR_TEST );
    }
}

State
VideoDriver::getState() const
{
    return m_state;
}

void
VideoDriver::setState( const State & state )
{
    m_state = State::apply( m_state, state );
}

// ##################
// ### PerfTracer ###
// ##################
void VideoDriver::beginPerf( std::string name, uint32_t color )
{
    m_perfTracer.begin( std::move( name ), color );
}
void VideoDriver::endPerf()
{
    m_perfTracer.end();
}

void VideoDriver::drawPerf( Recti pos )
{
    auto rendS = getScreenRenderer();
    auto rendF = getFontRenderer5x8();
    auto font = Font5x8(2,2,0,0,1,1);

    rendS->draw2DRoundRect( pos, glm::ivec2(8,8) );

    int x = pos.x + 10;
    int y = pos.y + 10;

    for ( const auto& pair : m_perfTracer.curves )
    {
        const auto & data = pair.second.data;
        const auto color = pair.second.color;

        std::string dur = "Empty"; // duration

        if (!data.empty())
        {
            dur = dbStrNanoSeconds( data.back().tEnd - data.back().tStart );
        }
        auto msg = dbStr( pair.first, " = ", dur);
        rendF->draw2DText( x,y,msg,color, Align::Default, font );
        y += font.getTextSize("L").height + 5;
    }
}

void VideoDriver::clearShaders()
{
    m_shader = nullptr;
    for ( auto& pair : m_shaders )
    {
        Shader* shader = pair.second;
        if ( shader)
        {
            if (shader->id)
            {
                glDeleteShader(shader->id); GL_VALIDATE;
                shader->id = 0;
            }
            delete shader;
        }
    }
    m_shaders.clear();
}

Shader* VideoDriver::getShader() const { return m_shader; }
int32_t VideoDriver::getShaderVersionMajor() const { return m_shaderVersionMajor; }
int32_t VideoDriver::getShaderVersionMinor() const { return m_shaderVersionMinor; }

std::string VideoDriver::getShaderVersionHeader() const
{
   std::ostringstream s;
   s << "#version " << m_shaderVersionMajor << m_shaderVersionMinor << "0";
   if ( m_useCoreProfile ) s << " core";
   if ( m_useGLES ) s << " es";
   s << "\n";
   return s.str();
}

int VideoDriver::useTexture( Texture* tex, int stage )
{
    if (!tex)
    {
        DE_ERROR("nullptr")
        return -1;
    }

    if (!tex->id())
    {
        DE_ERROR("No handle ", tex->str())
        return -1;
    }

    if (stage < 0)
    {
        //DE_WARN("Auto")
        stage = 0;
    }

    int unit = stage;
    glBindTextureUnit(unit, tex->id());

    //de::gpu::applySamplerOptions(m_chipTexture->so);
    // Das Sampler-Objekt fest an Unit 0 binden (Modernes OpenGL)
    // GLuint samplerId;
    // glGenSamplers(1, &samplerId);

    // // Konfiguration einmalig setzen (Beispiel für Linear + Clamp)
    // glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glBindSampler(0, m_chipTexture->samplerId);


    return unit;
}

bool VideoDriver::useShader( Shader* shader )
{
    if (!shader)
    {
        DE_ERROR("Got nullptr")
        return false;
    }

    if (!shader->id)
    {
        DE_ERROR("Got id = 0, shaderName(",shader->name,")")
        return false;
    }

    GL_VALIDATE

    glUseProgram(shader->id);

    GL_VALIDATE

    m_shader = shader;
    return true;
}

Shader* VideoDriver::getShader( const std::string& name ) const
{
    const auto found = m_shaders.find(name);
    if (found == m_shaders.end())
    {
        return nullptr;
    }

    return found->second;
}

Shader* VideoDriver::createShader(
      const std::string& name,
      const std::string& vsSourceText,
      const std::string& fsSourceText, bool debug )
{
    if (getShader( name ))
    {
        DE_ERROR( "Shader already exist at index. (",name,")" )
        return nullptr;
    }

    std::ostringstream ovs;
    ovs << getShaderVersionHeader() << "// " << name << "_vs\n" << vsSourceText;

    std::ostringstream ofs;
    ofs << getShaderVersionHeader() << "// " << name << "_fs\n" << fsSourceText;

    std::string const vs = ovs.str();
    std::string const fs = ofs.str();
    const char* vsText = vs.c_str();
    const char* fsText = fs.c_str();

    uint32_t shaderId = GT_createShader(name.c_str(), vsText, fsText, debug);
    if (!shaderId)
    {
        DE_ERROR("Cannot create shader ", name)
        return nullptr;
    }

    auto shader = new Shader();
    shader->id = shaderId;
    shader->name = name;
    shader->uniforms = enumerateUniforms(shader->id);
    m_shaders[ name ] = shader;

    if ( debug )
    {
        DE_DEBUG("Created shader ", name, " with id ", shaderId, ":")
        DE_DEBUG("VS[", name, "]:\n", vs )
        DE_DEBUG("FS[", name, "]:\n", fs )
        dumpUniforms(shader->uniforms);
    }

    useShader( shader );
    return shader;
}

/*
void VideoDriver::setViewport( int x, int y, int w, int h )
{
    m_state.viewport.x = x;
    m_state.viewport.y = y;
    m_state.viewport.w = w;
    m_state.viewport.h = h;
    //glViewport(x, y, w, h);
}

void VideoDriver::setScissor( int x, int y, int w, int h )
{
    if ()
    glScissor(x, y, w, h);
}
*/

void VideoDriver::setClearColor( glm::vec4 const & c ) { glClearColor( c.r, c.g, c.b, c.a ); GL_VALIDATE }
void VideoDriver::setClearDepth( float d ) { glClearDepth( d ); GL_VALIDATE }
void VideoDriver::setClearStencil( uint8_t s ) { glClearStencil( s ); GL_VALIDATE }


// ###########################
// ### RenderTargetManager ###
// ###########################

void VideoDriver::clearRenderTargets()
{
    m_rt = nullptr;

    for (auto & rt : m_rts)
    {
        delete rt;
    }

    m_rts.clear();
}

IRenderTarget* VideoDriver::getRenderTarget()
{
    return m_rt;
}

void
VideoDriver::setRenderTarget(IRenderTarget* rt)
{
    m_rt = rt;
    if (m_rt)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_rt->fbo()); // Bind the framebuffer
        glViewport(0,0,m_rt->w(),m_rt->h());
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind the framebuffer
        glViewport(0,0,m_screenWidth,m_screenHeight);
    }
}

IRenderTarget* VideoDriver::getRenderTarget(const std::string& name)
{
    const auto it = std::find_if(m_rts.begin(),m_rts.end(),
        [&](const IRenderTarget* const cached){ return cached && cached->name() == name; });
    if (it == m_rts.end()) { return nullptr; }
    return *it;
}

IRenderTarget* VideoDriver::getRenderTarget(uint32_t index)
{
    if (index >= m_rts.size()) return nullptr;
    return m_rts[index];
}

uint32_t VideoDriver::getRenderTargetCount() const
{
    return m_rts.size();
}

IRenderTarget*
VideoDriver::createRenderTarget(const std::string& name,
                                int w, int h,
                                PixelFormat color,
                                PixelFormat depthStencil)
{
    auto rt = getRenderTarget(name);
    if (rt)
    {
        DE_ERROR("Exists already, ", name)
        return nullptr;
    }

    rt = new RT_RGB();
    rt->init( this, w, h );
    m_rts.push_back(rt);
    DE_TRACE("Created ", name)
    return rt;
}

IRenderTarget*
VideoDriver::createRenderTarget_HDR(const std::string& name,
                                int w, int h,
                                PixelFormat color,
                                PixelFormat depth,
                                PixelFormat stencil)
{
    auto rt = getRenderTarget(name);
    if (rt)
    {
        DE_ERROR("Exists already, ", name)
        return nullptr;
    }

    rt = new RT_HDR();
    rt->init( this, w, h );
    m_rts.push_back(rt);
    DE_TRACE("Created ", name)
    return rt;
}

// Draw Unicode String: (default for FontTTF unicode freetype2 ttf files)

void VideoDriver::draw2DText( int x, int y, const std::wstring& msg, const uint32_t color, const Align align,
                            const Font5x8 & font, uint32_t bgColor, int padding )
{
    getFontRenderer5x8()->draw2DText( x,y, msg, color, align, font, bgColor, padding ); // Has overload for wstring
}


void VideoDriver::draw2DText( int x, int y, const std::wstring& msg, const uint32_t color, const Align align,
                            const Font & font, uint32_t bgColor, int padding )
{
    getFontRenderer()->draw2DText( x,y, msg, color, align, font, bgColor, padding ); // Has native overload for wstring
}

// Draw Multibyte String: (default for Font5x8, since there are so few glyphs we use 7/8 bit char* text)

void VideoDriver::draw2DText( int x, int y, const std::string& msg, const uint32_t color, const Align align,
                            const Font5x8 & font, uint32_t bgColor, int padding )
{
    getFontRenderer5x8()->draw2DText( x,y, msg, color, align, font, bgColor, padding ); // Has native overload for string
}

void VideoDriver::draw2DText( int x, int y, const std::string& msg, const uint32_t color, const Align align,
                            const Font & font, uint32_t bgColor, int padding )
{
    getFontRenderer()->draw2DText( x,y, msg, color, align, font, bgColor, padding ); // Has overload for string
}

void VideoDriver::draw2DPerfOverlay(int mx, int my)
{
    const int w = getScreenWidth();
    const int h = getScreenHeight();
    const int p = 10;

    uint32_t bgColor = dbRGB(0,0,0,200);
    int x = w - 1 - p;
    int y = p;
    Align align = Align::TopRight;
    Font5x8 font5(6,6,0,0,1,1);
    Font5x8 font4(4,4,0,0,1,1);
    Font5x8 font3(3,3,0,0,1,1);
    //Font font( "garton", 36 );

    int ln5 = font5.getTextSize("W").height + p;
    auto s5 = dbStr("FPS ",int(getFPS()));
    draw2DText( x,y, s5, dbRGB(255,255,100), align, font5, bgColor, 1 ); y += ln5;
    // auto s6 = dbStr("T ",int(getCpuThreadCount()));
    // auto s6 = dbStr("MHz ",int(getMHz()));
    // draw2DText( x,y, s5, dbRGB(255,255,100), align, font5, bgColor, 1 ); y += ln5;

    int ln4 = font4.getTextSize("W").height + p;
    auto s1 = dbStr("Time ",StringUtil::seconds(getTime()),")");
    auto s2 = dbStr("Screen(",w,",",h,")");
    auto s3 = dbStr("Mouse(",mx,",",my,")");
    draw2DText( x,y, s1, dbRGB(255,200,100), align, font4, bgColor, 1 ); y += ln4;
    draw2DText( x,y, s2, dbRGB(255,155,100), align, font4, bgColor, 1 ); y += ln4;
    draw2DText( x,y, s3, dbRGB(255,100,100), align, font4, bgColor, 1 ); y += ln4;

    auto camera = getCamera();
    if (camera)
    {
        auto e = camera->getPos();
        auto t = camera->getTarget();
        auto d = camera->getDir();
        auto a = camera->getAng();
        auto s1 = dbStr("Camera-Pos(",int(e.x),",",int(e.y),",",int(e.z),")");
        auto s2 = dbStr("Camera-Look(",int(t.x),",",int(t.y),",",int(t.z),")");
        // auto s3 = dbStr("Camera-Dir-X(",d.x,")");
        // auto s4 = dbStr("Camera-Dir-Y(",d.y,")");
        // auto s5 = dbStr("Camera-Dir-Z(",d.z,")");
        auto s6 = dbStr("Camera-Angle(",int(a.x),",",int(a.y),",",int(a.z),")");
        auto s7 = dbStr("Camera-Near(",camera->getNearValue(),")");
        auto s8 = dbStr("Camera-Far(",camera->getFarValue(),")");

        int ln3 = font3.getTextSize("W").height + p;
        draw2DText( x,y, s1, dbRGB(  0,200,  0), align, font3, bgColor, 1 ); y += ln3;
        draw2DText( x,y, s2, dbRGB(100,200,100), align, font3, bgColor, 1 ); y += ln3;
        //draw2DText( x,y, s3, dbRGB(125,215,125), align, font3, bgColor, 1 ); y += ln3;
        //draw2DText( x,y, s4, dbRGB(145,225,145), align, font3, bgColor, 1 ); y += ln3;
        //draw2DText( x,y, s5, dbRGB(  0,200,  0), align, font3, bgColor, 1 ); y += ln3;
        draw2DText( x,y, s6, dbRGB(125,215,125), align, font3, bgColor, 1 ); y += ln3;
        draw2DText( x,y, s7, dbRGB(145,225,145), align, font3, bgColor, 1 ); y += ln3;
        draw2DText( x,y, s8, dbRGB(100,200,100), align, font3, bgColor, 1 ); y += ln3;

    }

    //int ln = font4.getTextSize("W").height + 10;
    //draw2DText( x,y, dbStr("State = Idle"), 0xFFFFFFFF, align, font4, bgColor, 1 );
}

} // end namespace gpu.
} // end namespace de.
