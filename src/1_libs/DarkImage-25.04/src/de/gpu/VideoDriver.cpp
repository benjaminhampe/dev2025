#include <de/gpu/VideoDriver.h>
#include <de/IrrlichtDevice.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#else
    #define APIENTRY
#endif

namespace de {
namespace gpu {

VideoDriver* createVideoDriver( int w, int h, uint64_t windowId)
{
    VideoDriver* pDriver = new VideoDriver();
    pDriver->open(w,h);
    return pDriver;
}


static void APIENTRY VD_DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    std::ostringstream o;
    o << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
      << " type = 0x" << std::hex << type <<
        ", severity = 0x" << severity <<
        ", message = " << message << std::endl;
    DE_ERROR(o.str())
}


std::string glGetStdString( u32 value )
{
    auto p = (char const*)glGetString( value );
    if ( p )
    {
        std::string t = p;
        return t;
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

    auto color = driver->createTexture2D("rtHdr_color32", w, h, nullptr, PixelFormat::R8G8B8A8, so );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color->target(), color->id(), 0);

    auto depthStencil = driver->createTexture2D("rtHdr_d24s8", w, h, nullptr, PixelFormat::D24S8, so );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencil->target(), depthStencil->id(), 0);

    // Check if the framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        DE_ERROR("Error: Framebuffer is not complete!")
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer
    GL_VALIDATE

    m_color.tex = color;
    m_color.fmt = PixelFormat::R8G8B8A8;
    m_color.attach = GL_COLOR_ATTACHMENT0;

    m_depthStencil.tex = depthStencil;
    m_depthStencil.fmt = PixelFormat::D24S8;
    m_depthStencil.attach = GL_DEPTH_STENCIL_ATTACHMENT;
}


void RT_RGB::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);  GL_VALIDATE
}
void RT_RGB::unbind()
{
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
    // ==============================================
    // === Default camera, aka ViewProjectionMatrix ===
    // ==============================================
    m_camera0.setMoveSpeed(5.0);
    m_camera0.setStrafeSpeed(5.0);
    m_camera0.setUpSpeed(5.0);
    m_camera0.setUp( 0,1,0 );
    m_camera0.setPos( 170, 150, -150 );
    m_camera0.setTarget( 50, 50, 50 );
    m_camera0.setViewportEnabled( false );
    m_camera = &m_camera0;
    //m_cameraMovable = false;
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

    auto extensions = StringUtil::split(extensionList,' ');

    DE_INFO("GL_EXTENSIONS = ", extensions.size())
    // for (size_t i = 0; i < extensions.size(); ++i)
    // {
    //     DE_INFO("GL_EXTENSION[",i,"] ", extensions[i])
    // }
    DE_INFO("GL_VERSION = ", glGetStdString(GL_VERSION))
    DE_INFO("GL_VENDOR = ", glGetStdString(GL_VENDOR))
    DE_INFO("GL_RENDERER = ", glGetStdString(GL_RENDERER))
    DE_INFO("GL_SHADING_LANGUAGE_VERSION = ", glGetStdString(GL_SHADING_LANGUAGE_VERSION))

    //dumpVideoModes();

    // DebugOutput
    glEnable(GL_DEBUG_OUTPUT); GL_VALIDATE;
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); GL_VALIDATE;
    glDebugMessageCallback(VD_DebugMessageCallback, 0); GL_VALIDATE;

    // ShaderManager
    glGetIntegerv( GL_MAJOR_VERSION, &m_shaderVersionMajor ); GL_VALIDATE;
    glGetIntegerv( GL_MINOR_VERSION, &m_shaderVersionMinor ); GL_VALIDATE;

    // TexManager
    m_texMgr.init();

    addRenderTarget_HDR("hdr",1024,768);

    // RenderStates
    m_initState = State::query();
    m_state = m_initState;
    m_state.blend = Blend::alphaBlend();
    m_state.depth = Depth();
    m_state.culling = Culling();
    m_state = State::apply( m_initState, m_state );

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

    setClearColor( glm::vec4{0.1f,0.1f,0.3f,1.0f} );
    setClearDepth( 1.0f );
    setClearStencil( 0 );

    m_skyboxRenderer.init( this );
    m_smaterialRenderer.init( this );
	m_pmaterialRenderer.init( this );	
    m_screenRenderer.init( this );
    m_fontRenderer5x8.init( this );
    m_fontRenderer.init( this );

    m_line3dRenderer.init( this );
    m_postFxRenderer.init( this );
	
	m_sceneManager.init( this );
	
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
}

void VideoDriver::beginRender( IRenderTarget* rt )
{
    m_timeNow = dbTimeInSeconds() - m_timeEpoch;
    //m_timeFrameStart = m_timeNow;

    const int w = getScreenWidth();
    const int h = getScreenHeight();
    //DE_ERROR("w(",w,"), h(",h,")")

    glViewport(0, 0, w, h); GL_VALIDATE
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f); GL_VALIDATE
    glClearDepthf(1.0f); GL_VALIDATE
    glClearStencil(0); GL_VALIDATE
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_VALIDATE

    auto camera = getCamera();
    if (camera)
    {
        camera->setScreenSize(w,h);
        camera->update();
    }
}

void VideoDriver::endRender()
{
    glFlush(); GL_VALIDATE
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
}

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

State const & VideoDriver::getState() const
{
    return m_state;
}
void VideoDriver::setState( State const & state )
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

/*
bool VideoDriver::useTexture( TexRef ref, int stage )
{
    if (ref.tex && ref.tex->id())
    {
        glActiveTexture(GL_TEXTURE0 + stage);
        glBindTexture(ref.tex->target(), ref.tex->id());
        m_texMgr.applySamplerOptions(ref.tex->so());
        return true;
    }
    else
    {
        // glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }
}
*/

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
void VideoDriver::addShader( Shader* shader )
{
    if ( !shader )
    {
        //DE_ERROR("Got nullptr")
        return;
    }

    const std::string& shaderName = shader->getName();

    if ( findShader( shaderName ) > -1 )
    {
    //DE_ERROR("Shader already cached(",shaderName,").")
    return;
    }

    m_shaders.push_back( shader );
    //DE_DEBUG("[+] New managed Shader(", shader->getProgramID(), "), name(",shaderName,")")
}

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

IRenderTarget* VideoDriver::getRenderTarget(std::string name)
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

IRenderTarget* VideoDriver::addRenderTarget(std::string name, int w, int h,
                                            PixelFormat color, PixelFormat depthStencil)
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

IRenderTarget* VideoDriver::addRenderTarget_HDR(std::string name, int w, int h,
                                                PixelFormat color, PixelFormat depth, PixelFormat stencil)
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
    const int mx = getIrrlichtDevice()->getMouseX();
    const int my = getIrrlichtDevice()->getMouseY();
    const int p = 10;

    uint32_t bgColor = dbRGBA(0,0,0,200);
    int x = w - 1 - p;
    int y = p;
    Align align = Align::TopRight;
    Font5x8 font5(8,8,1,1,1,1);
    Font5x8 font4(4,4,1,1,1,1);
    Font5x8 font3(3,3,1,1,1,1);
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
