#include <de/gpu/VideoDriver.h>
#include <de/IrrlichtDevice.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>
#include <de/gpu/GPU.h>

#ifdef _WIN32
    // #ifndef WIN32_LEAN_AND_MEAN
    // #define WIN32_LEAN_AND_MEAN
    // #endif
    // #include <windows.h>
    #define APIENTRY __stdcall
#else
    #define APIENTRY
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
static void APIENTRY VD_DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::ostringstream o;
    o << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
      << " type = 0x" << std::hex << type <<
        ", severity = 0x" << severity <<
        ", message = " << message << std::endl;
    DE_ERROR(o.str())
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

/*
// ===========================================================================
Shader::Shader()
// ===========================================================================
   : id(0)
{
}

Shader::~Shader()
{
   if ( id != 0 )
   {
      printf("You forgot to destroy shader %d\n", id);
   }
}

void
Shader::destroy()
{
   if ( !id ) { return; }
   ::glDeleteProgram( id );
   id = 0;
}
*/

// ------------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) const
{
    setInt(name, (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(id, name.c_str()), value); GL_VALIDATE
}
void Shader::setUInt(const std::string& name, uint32_t value) const
{
    glUniform1ui(glGetUniformLocation(id, name.c_str()), value); GL_VALIDATE
}
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value); GL_VALIDATE
}
void Shader::setVec1f(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(id, name.c_str()), value); GL_VALIDATE
}
void Shader::setVec2f(const std::string& name, glm::vec2 const &value) const
{
    glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr( value )); GL_VALIDATE
}
void Shader::setVec3f(const std::string& name, glm::vec3 const &value) const
{
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr( value )); GL_VALIDATE
}
void Shader::setVec4f(const std::string& name, glm::vec4 const &value) const
{
    glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr( value )); GL_VALIDATE
}
// ------------------------------------------------------------------------
void Shader::setMat2f(const std::string& name, glm::mat2 const &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat )); GL_VALIDATE
}
void Shader::setMat3f(const std::string& name, glm::mat3 const &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat )); GL_VALIDATE
}
void Shader::setMat4f(const std::string& name, glm::mat4 const &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr( mat )); GL_VALIDATE
}


// ===========================================================================
// ===========================================================================
// ===========================================================================

RT_RGB::RT_RGB()
    : m_driver(nullptr), m_fbo(0)
{}
RT_RGB::~RT_RGB()
{}
void
RT_RGB::init( VideoDriver* driver, int w, int h )
{
    m_driver = driver;

    // GLuint fbo = 0;
    // glGenFramebuffers(1, &fbo);
    // glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    if (!m_fbo)
    {
        glGenFramebuffers(1, &m_fbo);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); GL_VALIDATE

    SamplerOptions so(1,
                      SamplerOptions::Minify::Linear,
                      SamplerOptions::Magnify::Linear,
                      SamplerOptions::Wrap::ClampToEdge,
                      SamplerOptions::Wrap::ClampToEdge,
                      SamplerOptions::Wrap::ClampToEdge);

    // ------------------------------------------------------------
    // Color attachment: R8G8B8A8 (GL_RGBA8)
    // ------------------------------------------------------------
    // GLuint texColor = 0;
    // glGenTextures(1, &texColor);
    // glBindTexture(GL_TEXTURE_2D, texColor);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
    //              GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    //                        GL_TEXTURE_2D, texColor, 0);

    auto tC = m_driver->createTexture2D("rt_r8g8b8a8", w, h, nullptr, PixelFormat::R8G8B8A8, so );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tC->target(), tC->id(), 0);
    GL_VALIDATE
    // ------------------------------------------------------------
    // Depth-stencil attachment: D24S8
    // ------------------------------------------------------------
    // GLuint texDepthStencil = 0;
    // glGenTextures(1, &texDepthStencil);
    // glBindTexture(GL_TEXTURE_2D, texDepthStencil);

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0,
    //              GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
    //                        GL_TEXTURE_2D, texDepthStencil, 0);

    auto tDS = m_driver->createTexture2D("rt_d24s8", w, h, nullptr, PixelFormat::D24S8, so );

    glGenTextures(1, &m_depthView);
    glTextureView(m_depthView, GL_TEXTURE_2D, tDS->id(),
                    GL_DEPTH_COMPONENT24, 0, 1, 0, 1);

    glGenTextures(1, &m_stencilView);
    glTextureView(m_stencilView, GL_TEXTURE_2D, tDS->id(),
                GL_STENCIL_INDEX8, 0, 1, 0, 1);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                GL_TEXTURE_2D, m_depthView, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                GL_TEXTURE_2D, m_stencilView, 0);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, tDS->target(), tDS->id(), 0);
    // GL_VALIDATE
    // ------------------------------------------------------------
    // Validate FBO
    // ------------------------------------------------------------
    // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    //     std::cerr << "FBO incomplete\n";
    // }

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Check if the framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        DE_ERROR("Error: Framebuffer is not complete!")
    }
    else
    {
        DE_OK("Framebuffer complete. fbo = ",m_fbo)
    }

    GL_VALIDATE
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer
    GL_VALIDATE

    m_color.view = 0;
    m_color.tex = tC;
    m_color.fmt = PixelFormat::R8G8B8A8;
    m_color.attach = GL_COLOR_ATTACHMENT0;

    m_depth.view = m_depthView;
    m_depth.tex = tDS;
    m_depth.fmt = PixelFormat::D24S8;
    m_depth.attach = GL_DEPTH_ATTACHMENT;

    m_stencil.view = m_stencilView;
    m_stencil.tex = tDS;
    m_stencil.fmt = PixelFormat::D24S8;
    m_stencil.attach = GL_STENCIL_ATTACHMENT;
}


void RT_RGB::bind()
{
    if (!m_fbo)
    {
        DE_ERROR("No FBO")
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);  GL_VALIDATE
}
void RT_RGB::unbind()
{
    if (!m_fbo)
    {
        DE_ERROR("No FBO")
        return;
    }

    GLint curFBO = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &curFBO);
    DE_TRACE("Current FBO = ",curFBO)

    GLenum attachments[] = {
        // GL_COLOR_ATTACHMENT0,
        GL_DEPTH_ATTACHMENT,
        GL_STENCIL_ATTACHMENT,
        // GL_DEPTH_STENCIL_ATTACHMENT
    };

    glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, attachments);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); GL_VALIDATE
}
void RT_RGB::clear( const glm::vec4& color )
{
    glClearColor(color.r,color.g,color.b,color.a);
    glClearDepth(1.0);
    glClearStencil(0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    GL_VALIDATE
}

bool RT_RGB::toImage(Image & img)
{
    return true;
}

// ===========================================================================
// ===========================================================================
// ===========================================================================

RT_HDR::RT_HDR()
    : m_driver(nullptr), m_fbo(0)
{

}

RT_HDR::~RT_HDR()
{

}

void RT_HDR::init( VideoDriver* driver, int w, int h )
{
    m_driver = driver;

    if (!m_fbo)
    {
        glGenFramebuffers(1, &m_fbo); GL_VALIDATE
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); GL_VALIDATE

    SamplerOptions so(1,
        SamplerOptions::Minify::Linear,
        SamplerOptions::Magnify::Linear,
        SamplerOptions::Wrap::ClampToEdge,
        SamplerOptions::Wrap::ClampToEdge,
        SamplerOptions::Wrap::ClampToEdge);

    auto color = driver->createTexture2D("rtHdr_color32f", w, h, nullptr, PixelFormat::RGBA32F, so );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color->target(), color->id(), 0); GL_VALIDATE

    auto depth = driver->createTexture2D("rtHdr_depth32f", w, h, nullptr, PixelFormat::D32F, so );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth->target(), depth->id(), 0); GL_VALIDATE

    auto stencil = driver->createTexture2D("rtHdr_stencil8", w, h, nullptr, PixelFormat::S8, so );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, stencil->target(), stencil->id(), 0); GL_VALIDATE

    // Check if the framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        DE_ERROR("Error: Framebuffer is not complete!")
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); GL_VALIDATE

    m_color.tex = color;
    m_color.fmt = PixelFormat::RGBA32F;
    m_color.attach = GL_COLOR_ATTACHMENT0;

    m_depth.tex = depth;
    m_depth.fmt = PixelFormat::D32F;
    m_depth.attach = GL_DEPTH_ATTACHMENT;

    m_stencil.tex = stencil;
    m_stencil.fmt = PixelFormat::S8;
    m_stencil.attach = GL_STENCIL_ATTACHMENT;
}

void RT_HDR::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); GL_VALIDATE
}
void RT_HDR::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  GL_VALIDATE
}
void RT_HDR::clear( const glm::vec4& color )
{
    glClearColor(color.r,color.g,color.b,color.a);
    glClearDepth(1.0);
    glClearStencil(0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    GL_VALIDATE
}
bool RT_HDR::toImage(Image & img)
{
    return true;
}

// ===========================================================================
VideoDriver::VideoDriver()
// ===========================================================================
    : m_device(nullptr)
    , m_screenWidth(800)
    , m_screenHeight(600)
    , m_modelMatrix( 1.0 )
{
    m_camera = &m_camera0;
}

VideoDriver::~VideoDriver()
{
    close();
}

void VideoDriver::setIrrlichtDevice(IrrlichtDevice* device)
{
    m_device = device;
}

IrrlichtDevice* VideoDriver::getIrrlichtDevice()
{
    return m_device;
}

bool VideoDriver::open(int w, int h)
{
    DE_OK("w(",w,"), h(",h,")")
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

    auto extensionList = glGetStdString(GL_EXTENSIONS);
    // DE_INFO("GL_EXTENSIONS = ", extensionList)
GL_VALIDATE
    auto extensions = StringUtil::split(extensionList,' ');
    DE_INFO("GL_EXTENSIONS = ", extensions.size())
    // for (size_t i = 0; i < extensions.size(); ++i)
    // {
    //     DE_INFO("GL_EXTENSION[",i,"] ", extensions[i])
    // }
    DE_INFO("GL_VERSION = ", glGetStdString(GL_VERSION))
GL_VALIDATE
    DE_INFO("GL_VENDOR = ", glGetStdString(GL_VENDOR))
GL_VALIDATE
    DE_INFO("GL_RENDERER = ", glGetStdString(GL_RENDERER))
GL_VALIDATE
    DE_INFO("GL_SHADING_LANGUAGE_VERSION = ", glGetStdString(GL_SHADING_LANGUAGE_VERSION))
GL_VALIDATE
    //dumpVideoModes();

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
    m_culling = Culling::query();
GL_VALIDATE
    m_depth = Depth::query();
GL_VALIDATE
    m_stencil = Stencil::query();
GL_VALIDATE
    m_blend = Blend::query();
GL_VALIDATE
    setCulling( Culling() );
GL_VALIDATE
    setDepth( Depth() );
GL_VALIDATE
    setStencil( Stencil() );
GL_VALIDATE
    setBlend( Blend::disabled() );
GL_VALIDATE
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
        DE_ERROR("")
        return;
    }

    m_screenWidth = w;
    m_screenHeight = h;

    if (getCamera())
    {
        getCamera()->setScreenSize(w,h);
        getCamera()->update();
    }
}

void VideoDriver::beginRender(const glm::vec4& clearColor)
{
    // DE_BENNI("beginRender()")
    // wglMakeCurrent(ps.hdc, self->_d->glrc);

    m_timeNow = dbTimeInSeconds() - m_timeEpoch;

    // DE_BENNI("m_time = ",m_timeNow)

    if (m_rt)
    {
        DE_WARN("RenderTarget still bound")
        m_rt = nullptr;
    }

    const int w = getScreenWidth();
    const int h = getScreenHeight();
    // glDisable(GL_SCISSOR_TEST);
    // glScissor(0,0,w,h);
    glViewport(0,0,w,h);
GL_VALIDATE
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
GL_VALIDATE
    glClearDepthf(1.0f);
GL_VALIDATE
    glClearStencil(0);
GL_VALIDATE
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
GL_VALIDATE

    // auto camera = getCamera();
    // if (camera)
    // {
    //     camera->setScreenSize(w,h);
    //     camera->update();
    // }
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
    glViewport(0,0,w,h);

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

State
VideoDriver::getState() const
{
    State state;
    state.culling = m_culling.curr;
    state.depth = m_depth.curr;
    state.stencil = m_stencil.curr;
    state.blend = m_blend.curr;
    // state.viewport = Viewport::query();
    // state.scissor = Scissor::query();
    // state.culling = Culling::query();
    // state.depth = Depth::query();
    // state.stencil = Stencil::query();
    // state.blend = Blend::query();
    // state.pointSize = PointSize::query();
    // state.lineWidth = LineWidth::query();
    // state.rasterizerDiscard = RasterizerDiscard::query();
    // state.polygonOffset = PolygonOffset::query();
    //state.clear = Clear::query();
    return state;
}

void
VideoDriver::setState( State const & state )
{
    //State state;
    //state.viewport = Viewport::apply( alt.viewport, neu.viewport );
    //state.scissor = Scissor::apply( alt.scissor, neu.scissor );
    m_culling.curr = Culling::apply( m_culling.last, state.culling );
    m_depth.curr = Depth::apply( m_depth.last, state.depth );
    m_stencil.curr = Stencil::apply( m_stencil.last, state.stencil );
    m_blend.curr = Blend::apply( m_blend.last, state.blend );
    //state.pointSize = PointSize::apply( alt.pointSize, neu.pointSize );
    //state.lineWidth = LineWidth::apply( alt.lineWidth, neu.lineWidth );
    //state.rasterizerDiscard = RasterizerDiscard::apply( alt.rasterizerDiscard, neu.rasterizerDiscard );
    //state.polygonOffset = PolygonOffset::apply( alt.polygonOffset, neu.polygonOffset );
    //state.clear = Clear::apply( alt.clear, neu.clear );
    //state.depthRange = DepthRange::apply( alt.depthRange, neu.depthRange );
}

Culling const &
VideoDriver::getCulling() const { return m_culling.curr; }

void
VideoDriver::setCulling( Culling const & state )
{
    m_culling.curr = Culling::apply( m_culling.last, state );
}

Depth const &
VideoDriver::getDepth() const { return m_depth.curr; }
void
VideoDriver::setDepth( Depth const & state ) {
    m_depth.curr = Depth::apply( m_depth.last, state );
}

Stencil const &
VideoDriver::getStencil() const { return m_stencil.curr; }
void
VideoDriver::setStencil( Stencil const & state )
{
    m_stencil.curr = Stencil::apply( m_stencil.last, state );
}

Blend const &
VideoDriver::getBlend() const { return m_blend.curr; }
void
VideoDriver::setBlend( Blend const & state ) { m_blend.curr = Blend::apply( m_blend.last, state ); }

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

bool VideoDriver::useTexture( Texture* tex, int stage )
{
    if (tex && tex->id())
    {
        glActiveTexture(GL_TEXTURE0 + stage); GL_VALIDATE
        glBindTexture(tex->target(), tex->id()); GL_VALIDATE
        m_texMgr.applySamplerOptions(tex->so());
        return true;
    }
    else
    {
        glActiveTexture(GL_TEXTURE0 + stage); GL_VALIDATE
        glBindTexture(GL_TEXTURE_2D, 0); GL_VALIDATE
        return false;
    }
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
      const std::string& fsSourceText )
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

    uint32_t shaderId = GT_createShader(name.c_str(), vsText, fsText);
    if (!shaderId)
    {
        DE_ERROR("Cannot create shader ", name)
        return nullptr;
    }

    auto shader = new Shader();
    shader->id = shaderId;
    shader->name = name;
    m_shaders[ name ] = shader;

    // if ( debug )
    // {
    // DE_DEBUG("Created shader ", name, " with id ", shaderId, ":")
    // DE_DEBUG("VS[", name, "]:\n", vs )
    // DE_DEBUG("FS[", name, "]:\n", fs )
    // }

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

void VideoDriver::setClearColor( glm::vec4 const & c ) { de_glClearColor( c.r, c.g, c.b, c.a ); }
void VideoDriver::setClearDepth( float d ) { de_glClearDepth( d ); }
void VideoDriver::setClearStencil( uint8_t s ) { de_glClearStencil( s ); }


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

void VideoDriver::draw2DPerfOverlay()
{
    const int w = getScreenWidth();
    const int h = getScreenHeight();
    const int mx = getIrrlichtDevice() ? getIrrlichtDevice()->getMouseX() : -1;
    const int my = getIrrlichtDevice() ? getIrrlichtDevice()->getMouseY() : -1;
    const int p = 10;

    uint32_t bgColor = dbRGBA(0,0,0,200);
    int x = w - 1 - p;
    int y = p;
    Align align = Align::TopRight;
    Font5x8 font5(6,6,0,0,1,1);
    Font5x8 font4(4,4,0,0,1,1);
    Font5x8 font3(3,3,0,0,1,1);
    //Font font( "garton", 36 );

    int ln5 = font5.getTextSize("W").height + p;
    auto s5 = dbStr("FPS ",int(getFPS()));
    draw2DText( x,y, s5, dbRGBA(255,255,100), align, font5, bgColor, 1 ); y += ln5;
    // auto s6 = dbStr("T ",int(getCpuThreadCount()));
    // auto s6 = dbStr("MHz ",int(getMHz()));
    // draw2DText( x,y, s5, dbRGBA(255,255,100), align, font5, bgColor, 1 ); y += ln5;

    int ln4 = font4.getTextSize("W").height + p;
    auto s1 = dbStr("Time ",StringUtil::seconds(getTime()),")");
    auto s2 = dbStr("Screen(",w,",",h,")");
    auto s3 = dbStr("Mouse(",mx,",",my,")");
    draw2DText( x,y, s1, dbRGBA(255,200,100), align, font4, bgColor, 1 ); y += ln4;
    draw2DText( x,y, s2, dbRGBA(255,155,100), align, font4, bgColor, 1 ); y += ln4;
    draw2DText( x,y, s3, dbRGBA(255,100,100), align, font4, bgColor, 1 ); y += ln4;

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
        draw2DText( x,y, s1, dbRGBA(  0,200,  0), align, font3, bgColor, 1 ); y += ln3;
        draw2DText( x,y, s2, dbRGBA(100,200,100), align, font3, bgColor, 1 ); y += ln3;
        //draw2DText( x,y, s3, dbRGBA(125,215,125), align, font3, bgColor, 1 ); y += ln3;
        //draw2DText( x,y, s4, dbRGBA(145,225,145), align, font3, bgColor, 1 ); y += ln3;
        //draw2DText( x,y, s5, dbRGBA(  0,200,  0), align, font3, bgColor, 1 ); y += ln3;
        draw2DText( x,y, s6, dbRGBA(125,215,125), align, font3, bgColor, 1 ); y += ln3;
        draw2DText( x,y, s7, dbRGBA(145,225,145), align, font3, bgColor, 1 ); y += ln3;
        draw2DText( x,y, s8, dbRGBA(100,200,100), align, font3, bgColor, 1 ); y += ln3;

    }

    //int ln = font4.getTextSize("W").height + 10;
    //draw2DText( x,y, dbStr("State = Idle"), 0xFFFFFFFF, align, font4, bgColor, 1 );
}

} // end namespace gpu.
} // end namespace de.
