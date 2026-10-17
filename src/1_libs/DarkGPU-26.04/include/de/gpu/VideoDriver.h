#pragma once
#include <de/gpu/GPU.h>
#include <de/gpu/Camera.h>
#include <de/os/Window.h>
#include <de/os/CommonDialogs.h>
#include <de/gpu/renderer/ScreenRenderer.h>
#include <de/gpu/renderer/FontRenderer5x8.h>
#include <de/gpu/renderer/FontRenderer.h>
#include <de/gpu/renderer/SkyboxRenderer.h>
#include <de/gpu/renderer/Line3D_Renderer.h>
#include <de/gpu/renderer/PostFxRenderer.h>
#include <de/gpu/renderer/ScreenQuadRenderer.h>
#include <de/smesh/SMeshRenderer.h>
#include <de/smesh/SMeshLibrary.h>
#include <de/smesh/SMeshIO.h>

//#include <de/gpu/mtl/PMesh.h>
//#include <de/gpu/mtl/PMaterialRenderer.h>
//#include <de/gpu/mtl/PMeshSceneNode.h>

//#include <de/gpu/mtl/MTL.h>
// #include <de/gui/Env.h>
// #include <de/gui/Panel.h>
// #include <de/gui/Button.h>
// #include <de/gui/Label.h>

#include <de/gpu/scene/SceneManager.h>

namespace de {

// struct IrrlichtDevice;

namespace gpu {

// ===========================================================================
struct Shader
// ===========================================================================
{
    uint32_t id = 0;
    std::string name;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setUInt(const std::string& name, uint32_t value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec1f(const std::string& name, float value) const;
    void setVec2f(const std::string& name, glm::vec2 const & value ) const;
    void setVec3f(const std::string& name, glm::vec3 const & value ) const;
    void setVec4f(const std::string& name, glm::vec4 const & value ) const;
    void setMat2f(const std::string& name, glm::mat2 const & mat) const;
    void setMat3f(const std::string& name, glm::mat3 const & mat) const;
    void setMat4f(const std::string& name, glm::mat4 const & mat) const;
};

// ===========================================================================
struct RT_Attachment
// ===========================================================================
{
    enum eAttachmentType
    {
        eAT_Color,
        eAT_Depth,
        eAT_Stencil,
        eAT_DepthStencil,
        eAT_Unknown
    };

    uint32_t view; // OpenGL 4.3+ glTextureView source.
    uint32_t attach; // GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER
    PixelFormat fmt;
    Texture* tex;

    RT_Attachment()
        : attach(eAT_Color), fmt(PixelFormat::Unknown), tex(nullptr)
    {}

    RT_Attachment( PixelFormat _fmt, Texture* _tex )
        : attach(eAT_Color), fmt(_fmt), tex(_tex)
    {}

    RT_Attachment( Texture* _tex )
        : attach(eAT_Color), fmt(PixelFormat::Unknown), tex(_tex)
    {
        if (tex) { fmt = tex->pixelFormat(); }
    }
};

// ===========================================================================
struct IRenderTarget
// ===========================================================================
{
    virtual ~IRenderTarget() {}
    virtual const std::string& name() const = 0;
    virtual uint32_t fbo() const = 0;
    virtual void setFbo( uint32_t fbo ) = 0;
    // Viewport
    //virtual const Recti& pos() const = 0;
    // virtual int32_t x() const = 0;
    // virtual int32_t y() const = 0;
    virtual int32_t w() const = 0;
    virtual int32_t h() const = 0;
    virtual void init( VideoDriver* driver, int w, int h ) = 0;
    //virtual PixelFormat pixFormat() const = 0;
    //virtual Texture* tex() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;
    virtual void clear( const glm::vec4& color ) = 0;
    virtual bool toImage(Image & img) = 0;

    virtual uint32_t colorAttachmentCount() const = 0;
    virtual uint32_t depthAttachmentCount() const = 0;
    virtual uint32_t stencilAttachmentCount() const = 0;

    virtual const RT_Attachment& colorAttachment( int i = 0 ) const = 0;
    virtual const RT_Attachment& depthAttachment( int i = 0 ) const = 0;
    virtual const RT_Attachment& stencilAttachment( int i = 0 ) const = 0;
};

// ===========================================================================
struct RT_RGB : public IRenderTarget
// ===========================================================================
{
    VideoDriver* m_driver;
    uint32_t m_fbo;
    uint32_t m_depthView;
    uint32_t m_stencilView;

    //Recti m_pos;
    int32_t m_w;
    int32_t m_h;
    RT_Attachment m_color; // DiffuseColor RGBA 32 or RGBA_128
    //RT_Attachment m_depthStencil; // DepthStencil D24_S8
    RT_Attachment m_depth; // D24
    RT_Attachment m_stencil; // Stencil S8
    std::string m_name;

    RT_RGB();
    ~RT_RGB() override;
    const std::string& name() const override { return m_name; }
    uint32_t fbo() const override { return m_fbo; }
    void setFbo( uint32_t fbo ) override { m_fbo = fbo; }

    void init( VideoDriver* driver, int w, int h ) override;
    void bind() override;
    void unbind() override;
    void clear( const glm::vec4& color ) override;
    bool toImage(Image & img) override;

    //const Recti& pos() const override { return m_pos; }
    int32_t w() const override { return m_w; }
    int32_t h() const override { return m_h; }

    uint32_t colorAttachmentCount() const override { return 1; }
    uint32_t depthAttachmentCount() const override { return 1; }
    uint32_t stencilAttachmentCount() const override { return 1; }

    const RT_Attachment& colorAttachment( int i = 0 ) const override { return m_color; }
    const RT_Attachment& depthAttachment( int i = 0 ) const override { return m_depth; }
    const RT_Attachment& stencilAttachment( int i = 0 ) const override { return m_stencil; }
    // const RT_Attachment& depthAttachment( int i = 0 ) const override { return m_depthStencil; }
    // const RT_Attachment& stencilAttachment( int i = 0 ) const override { return m_depthStencil; }
};

// ===========================================================================
struct RT_HDR : public IRenderTarget
// ===========================================================================
{
    VideoDriver* m_driver;
    uint32_t m_fbo;
    //Recti m_pos;
    int32_t m_w;
    int32_t m_h;
    RT_Attachment m_color; // GL_COLOR_ATTACHMENT0 GL_TEXTURE_2D GL_RGBA16F GL_RGBA GL_FLOAT
                           // GL_COLOR_ATTACHMENT0 GL_TEXTURE_2D GL_RGBA32F GL_RGBA GL_FLOAT
    RT_Attachment m_depth; // GL_DEPTH_ATTACHMENT GL_TEXTURE_2D GL_DEPTH_COMPONENT32 GL_DEPTH_COMPONENT GL_FLOAT;
    RT_Attachment m_stencil; // GL_STENCIL_ATTACHMENT GL_TEXTURE_2D GL_STENCIL_INDEX8 GL_STENCIL_INDEX GL_UNSIGNED_BYTE;
    std::string m_name;

    RT_HDR();
    ~RT_HDR() override;
    const std::string& name() const override { return m_name; }
    uint32_t fbo() const override { return m_fbo; }
    void setFbo( uint32_t fbo ) override { m_fbo = fbo; }

    void init( VideoDriver* driver, int w, int h ) override;
    void bind() override;
    void unbind() override;
    void clear( const glm::vec4& color ) override;
    bool toImage(Image & img) override;

    //const Recti& pos() const override { return m_pos; }
    int32_t w() const override { return m_w; }
    int32_t h() const override { return m_h; }

    uint32_t colorAttachmentCount() const override { return 1; }
    uint32_t depthAttachmentCount() const override { return 1; }
    uint32_t stencilAttachmentCount() const override { return 1; }

    const RT_Attachment& colorAttachment( int i = 0 ) const override { return m_color; }
    const RT_Attachment& depthAttachment( int i = 0 ) const override { return m_depth; }
    const RT_Attachment& stencilAttachment( int i = 0 ) const override { return m_stencil; }
};

// ===========================================================================
struct PerfCurve
// ===========================================================================
{
    struct Datum
    {
        double tStart;
        double tEnd;
    };

    uint32_t color;
    std::vector< Datum > data;
};

// ===========================================================================
struct PerfTracer
// ===========================================================================
{
    typedef std::map< std::string, PerfCurve> TMap;
    typedef TMap::iterator TItem;

    TMap curves;
    TItem curve;

    void begin( std::string name, uint32_t color = 0)
    {
        auto tStart = dbTimeInSeconds();

        curves[ name ].data.emplace_back();
        curves[ name ].data.back().tStart = tStart;

        if ( dbRGB_A(curves[ name ].color) < 1 )
        {
            if ( dbRGB_A(color) > 0 )
            {
                curves[ name ].color = color;
            }
            else
            {
                curves[ name ].color = randomColorRGB();
            }
        }

        curve = curves.find( name );
    }

    void end()
    {
        auto tEnd = dbTimeInSeconds();

        if ( curve == curves.end() )
        {
            DE_ERROR("Missing begin() call.")
            return;
        }

        curve->second.data.back().tEnd = tEnd;
    }
};

// ===========================================================================
struct VideoDriver
// ===========================================================================
{
    VideoDriver();
    ~VideoDriver();

    // void                setIrrlichtDevice(IrrlichtDevice* device);
    // IrrlichtDevice*     getIrrlichtDevice();

    bool
    open(int w, int h);

    void
    close();

    void
    resize( int w, int h );

    bool
    beginRender( const glm::vec4& clearColor = glm::vec4(0) );

    void
    endRender();

    void
    beginRender( IRenderTarget* rt, const glm::vec4& clearColor = glm::vec4(0) );

    void
    endRender( IRenderTarget* rt );

    int         getScreenWidth() const;
    int         getScreenHeight() const;
    int         getRenderWidth() const;
    int         getRenderHeight() const;

    double      getFPS() const;
    double      getTime() const;
    uint64_t    getFrameCount() const;

    // ##################
    // ### PerfTracer ###
    // ##################
    void        beginPerf( std::string name, uint32_t color = 0);
    void        endPerf();
    void        drawPerf( Recti pos);

    // ################
    // ### Renderer ###
    // ################

    SceneManager*     	getSceneManager() { return &m_sceneManager; }

    //PMaterialRenderer*  getPMaterialRenderer() { return &m_pmaterialRenderer; }
    smesh::SMaterialRenderer*  getSMaterialRenderer() { return &m_smaterialRenderer; }

    SkyboxRenderer*     getSkyboxRenderer() { return &m_skyboxRenderer; }

    ScreenRenderer*     getScreenRenderer() { return &m_screenRenderer; }
    FontRenderer5x8*    getFontRenderer5x8() { return &m_fontRenderer5x8; }
    FontRenderer*       getFontRenderer() { return &m_fontRenderer; }
    Line3D_Renderer*    getLine3DRenderer() { return &m_line3dRenderer; }
    PostFxRenderer*     getPostFxRenderer(){ return &m_postFxRenderer; }
    ScreenQuadRenderer* getScreenQuadRenderer(){ return &m_screenQuadRenderer; }
    //gui::Env*           getGUIEnv() { return &m_guienv; }

    State               getState() const;
    void                setState( State const & state );

    // Culling const &  getCulling() const;
    // void             setCulling( Culling const & state );

    // Depth const &    getDepth() const;
    // void             setDepth( Depth const & state );

    // Stencil const &  getStencil() const;
    // void             setStencil( Stencil const & state );

    // Blend const &    getBlend() const;
    // void             setBlend( Blend const & state );

    // ######################
    // ###   Transforms   ###
    // ######################

    const glm::dmat4&
    getModelMatrix() const { return m_modelMatrix; }

    void
    setModelMatrix( const glm::dmat4& modelMatrix ) { m_modelMatrix = modelMatrix; }

    void
    resetModelMatrix() { m_modelMatrix = glm::dmat4( 1.0 ); }

    Camera*
    getCamera() const { return m_camera; }

    void
    setCamera( Camera* camera ) { m_camera = camera; }

    void
    resetCamera() { m_camera = &m_camera0; }

    // TODO: Enable it again!

    //void setViewport( int x, int y, int w, int h );
    //void setViewport( const Recti& pos ) { setViewport(pos.x, pos.y, pos.w, pos.h); }

    //void setScissor( int x, int y, int w, int h );
    //void setScissor( const Recti& pos ) { setScissor(pos.x, pos.y, pos.w, pos.h); }

    void setClearColor( glm::vec4 const & c );
    void setClearDepth( float d );
    void setClearStencil( uint8_t s );

    // ###########################
    // ### RenderTargetManager ###
    // ###########################

    void
    clearRenderTargets();

    // Get current RenderTarget:
    void
    setRenderTarget(IRenderTarget* rt = nullptr);

    IRenderTarget*
    getRenderTarget();

    // Get named RenderTarget:
    IRenderTarget*
    getRenderTarget(const std::string& name);

    // Get cached RenderTarget:
    IRenderTarget*
    getRenderTarget(uint32_t index);

    uint32_t
    getRenderTargetCount() const;

    // Add RenderTarget with combined depth-stencil attachment:
    IRenderTarget*
    createRenderTarget(
        const std::string& name,
        int w, int h,
        PixelFormat color = PixelFormat::R8G8B8A8,
        PixelFormat depthStencil = PixelFormat::D24S8);

    // Add RenderTarget with separate depth-stencil attachment:
    IRenderTarget*
    createRenderTarget_HDR(
        const std::string& name,
        int w, int h,
        PixelFormat color = PixelFormat::RGBA32F,
        PixelFormat depth = PixelFormat::D32F,
        PixelFormat stencil = PixelFormat::S8);

    // #####################
    // ### ShaderManager ###
    // #####################
    bool
    useShader( Shader* shader );

    void
    clearShaders();

    std::string
    getShaderVersionHeader() const;

    int32_t
    getShaderVersionMajor() const;

    int32_t
    getShaderVersionMinor() const;

    Shader*
    getShader() const;

    Shader*
    getShader( const std::string& name ) const;

    Shader*
    createShader(
        const std::string& name,
        const std::string& vsText,
        const std::string& fsText,
        bool debug = false );

    /*
    Shader*     createShader(   const std::string& name,
                                const std::string& vs,
                                const std::string& gs,
                                const std::string& fs,
                                bool debug = false );
    int32_t   findShader( const std::string& name ) const;
    */


    // ######################
    // ###   TexManager   ###
    // ######################
    Texture*
    getTexture2D(
        const std::string& name,
        const SamplerOptions& so = SamplerOptions(),
        const ImageLoadOptions& opt = ImageLoadOptions() )
    {
        auto tex = getTexture(name);
        if (!tex)
        {
            DE_WARN("No tex named ", name, " try to load as file...")
            tex = loadTexture2D(name,so,opt);
            if (!tex)
            {
                DE_ERROR("Cannot load ", name)
                return nullptr;
            }
        }
        return tex;
    }

    bool
    useTexture( Texture* tex, int stage );

    //bool
    //useTexture( TexRef ref, int stage = 0 );

    uint32_t
    getMaxTex2DSize() const { return m_texMgr.getMaxTex2DSize(); }

    uint32_t
    getNumTexUnits() const { return m_texMgr.getNumTexUnits(); }

    void
    dumpTextures() const { m_texMgr.dumpTextures(); }

    void
    updateTextures() { m_texMgr.updateTextures(); }

    void
    removeTextures() { m_texMgr.removeTextures(); }

    void
    removeTexture( const std::string& name ) { m_texMgr.removeTexture(name); }

    void
    removeTexture( Texture* &tex ) { m_texMgr.removeTexture(tex); }

    Texture*
    getTexture( const std::string& name ) const { return m_texMgr.getTexture(name); }

    Texture*
    loadTexture2D(
        const std::string& name,
        const SamplerOptions& so = SamplerOptions(),
        const ImageLoadOptions& lopt = ImageLoadOptions() )
    {
        return m_texMgr.loadTexture2D(name,so,lopt);
    }

    Texture*
    createTexture2D(
        const std::string& name,
        int w,
        int h,
        void const* const pixels,
        PixelFormat fmt,
        const SamplerOptions& so = SamplerOptions() )
    {
        return m_texMgr.createTexture2D(name,w,h,pixels,fmt,so);
    }

    Texture*
    createTexture2D(
        const std::string& name,
        const Image& img,
        const SamplerOptions& so = SamplerOptions() )
    {
        return m_texMgr.createTexture2D(name,img,so);
    }

    bool
    uploadTexture2D(
        Texture* &tex,
        int w, int h,
        void const* const pixels,
        PixelFormat fmt )
    {
        return m_texMgr.uploadTexture2D(tex,w,h,pixels,fmt);
    }

    bool
    uploadTexture2D( Texture* &tex, const Image& img )
    {
        return m_texMgr.uploadTexture2D(tex,img);
    }

    // ============================================================
    // FontRenderer (ttf) and Font5x8Renderer in action:
    // ============================================================

    // Draw Unicode String: (default for FontTTF unicode freetype2 ttf files)
    void
    draw2DText( int x, int y, const std::wstring& msg,
                const uint32_t color = 0xFFFFFFFF,
                const Align align = Align::Default,
                const Font5x8 & font = Font5x8(),
                uint32_t bgColor = dbRGB(0,0,0,200),
                int padding = 1);

    void
    draw2DText( int x, int y, const std::wstring& msg,
                const uint32_t color = 0xFFFFFFFF,
                const Align align = Align::Default,
                const Font & font = Font("Arial",16),
                uint32_t bgColor = dbRGB(0,0,0,200),
                int padding = 1);

    // Draw Multibyte String: (default for Font5x8, since there are so few glyphs we use 7/8 bit char* text)
    void
    draw2DText( int x, int y, const std::string& msg,
                const uint32_t color = 0xFFFFFFFF,
                const Align align = Align::Default,
                const Font5x8 & font = Font5x8(),
                uint32_t bgColor = dbRGB(0,0,0,200),
                int padding = 1);

    void
    draw2DText( int x, int y, const std::string& msg,
                const uint32_t color = 0xFFFFFFFF,
                const Align align = Align::Default,
                const Font & font = Font("Arial",16),
                uint32_t bgColor = dbRGB(0,0,0,200),
                int padding = 1);

    void draw2DPerfOverlay(int mx, int my);

    /* typ = 0 is a point light like the sun, dir is computed by pos only
    * typ = 1 is a directional cone light like a Taschenlampe,
    * typ = 2 is a directional cylinder with round caps like a neon light
    */
    void
    addLight(   glm::vec3 pos = glm::vec3(0,0,0),
                glm::vec3 color = glm::vec3(1,1,1),
                float maxdist = 500.0f,
                int typ = 1,
                glm::vec3 dir = glm::vec3(0,-1,0),
                glm::vec3 siz = glm::vec3(10,10,10) )
    {
        Light light;
        light.typ = typ;
        //light.siz = siz;
        light.pos = pos;
        light.maxdist = maxdist;
        //light.dir = dir;
        light.color = color;
        //light.mesh.setPrimitiveType( PrimitiveType::Triangles );
        //light.mesh.setLighting( 0 );
        //SMeshSphere::add( light.mesh, light.siz, light.pos, color, 12, 12 );
        m_lights.emplace_back( std::move( light ) );
    }

    uint32_t
    getLightCount() const { return uint32_t(m_lights.size()); }
    Lights &
    getLights() { return m_lights; }
    const Lights &
    getLights() const { return m_lights; }
    Light &
    getLight( uint32_t light ) { return m_lights[ light ]; }
    const Light &
    getLight( uint32_t light ) const { return m_lights[ light ]; }

    // // ##########################
    // // ###   TexUnitManager   ###
    // // ##########################
    // uint32_t         getUnitCount() const;
    // int32_t          findUnit( uint32_t texId ) const;
    // bool             bindTextureId( int stage, uint32_t texId );
    // bool             bindTexture( int stage, ITexture* tex );
    // // Benni's more intelligent calls for raw/higher level TexClass.
    // // AutoSelect a free tex unit and return its index for use in setSamplerUniform().
    // int              bindTextureId( uint32_t texId );
    // bool             unbindTextureId( uint32_t texId );
    // int              bindTexture( ITexture* tex );
    // bool             unbindTexture( ITexture* tex );

    // int32_t findTex( Tex* tex ) const;
    // int32_t findTex( const std::string& name ) const;
    // bool hasTex( const std::string& name ) const;
    // TexRef getTex( const std::string& name ) const;
    // TexRef load2D( const std::string& name, std::string uri, bool keepImage = true, TexOptions so = TexOptions() );
    // TexRef add2D( const std::string& name, Image const & img, bool keepImage = true, TexOptions so = TexOptions() );

/*
   void beginRender( bool clearColor = true, bool clearDepth = true, bool clearStencil = true )
   {
      m_fpsComputer.beginFrame();

       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      uint32_t clearMask = 0;
      if ( clearColor )
      {
         clearMask |= GL_COLOR_BUFFER_BIT;
      }
      if ( clearDepth && m_createParams.d > 0 )
      {
         clearMask |= GL_DEPTH_BUFFER_BIT;
      }
      if ( clearStencil && m_createParams.s > 0 )
      {
         clearMask |= GL_STENCIL_BUFFER_BIT;
      }

      if ( clearMask > 0 )
      {
         glClear( clearMask );
         //std::cout << "Clear " << StringUtil::hex(clearMask) << "\n";
      }

   }

   void endRender()
   {
       glFlush();
      m_fpsComputer.endFrame();
   }
*/


protected:
    // IrrlichtDevice* m_device;

    // ####################
    // ### StateManager ###
    // ####################
    int m_screenWidth;
    int m_screenHeight;

    //State m_initState;
    //SM3<State> m_state;

    //SM3<Viewport> m_viewport;
    //SM3<Scissor> m_scissor;
    State m_state;
    // SM3<Culling> m_culling;
    // SM3<Depth> m_depth;
    // SM3<Stencil> m_stencil;
    // SM3<Blend> m_blend;
    //SM3<RasterizerDiscard> m_rasterizerDiscard;
    //SM3<LineWidth> m_lineWidth;
    //SM3<PointSize> m_pointSize;
    //SM3<PolygonOffset> m_polygonOffset;

    glm::dmat4 m_modelMatrix;
    // CreateParams m_createParams;
    // FPSComputer m_fpsComputer;
    Lights m_lights;
    Camera m_camera0;
    Camera* m_camera;
    //bool m_cameraMovable;

    // #####################
    // ### ShaderManager ###
    // #####################
    Shader* m_shader;
    int32_t m_shaderVersionMajor; // Brings first number, read from glGetInteger(GL_VERSION_MAJOR)
    int32_t m_shaderVersionMinor; // Brings second number, read from glGetInteger(GL_VERSION_MINOR)
    bool m_useCoreProfile;        // Brings <core> word, read from GL_RENDERER ? ( not read yet )
    bool m_useGLES;               // Brings <es> word, read from -DBENNI_USE_ES or GL_RENDERER ?
    // bool m_bPrintShaderTexts;
    std::unordered_map< std::string, Shader* > m_shaders;

    PerfTracer m_perfTracer;
    // ######################
    // ###   TexManager   ###
    // ######################
    TexManager m_texMgr;

    // ###########################
    // ### RenderTargetManager ###
    // ###########################
    IRenderTarget* m_rt;
    std::vector<IRenderTarget*> m_rts;

    SkyboxRenderer m_skyboxRenderer;

    smesh::SMaterialRenderer m_smaterialRenderer;
    //PMaterialRenderer m_pmaterialRenderer;

    ScreenRenderer m_screenRenderer;

    FontRenderer5x8 m_fontRenderer5x8;

    FontRenderer m_fontRenderer;

    Line3D_Renderer m_line3dRenderer;

    PostFxRenderer m_postFxRenderer;

    ScreenQuadRenderer m_screenQuadRenderer;

    SceneManager m_sceneManager;

    // CameraManager m_cameraManager;

    // gui::Env m_guienv;

    // FPSComputer()
    double m_timeEpoch;
    double m_timeNow;             // relative to m_timeStart!
    double m_timeLastFPSCompute;  // relative to m_timeStart!

    uint64_t m_frameCount;
    uint64_t m_frameCountLast;
    double m_fps;

    // double m_timeFrameStart;
    // double m_timeFrameEnd;

    // double m_timeFrameDurationMin;
    // double m_timeFrameDurationMax;

    // uint64_t m_drawCalls;
    // uint64_t m_drawCallsLastFrame;
    // uint64_t m_drawCallsLastSecond;
    // uint64_t m_drawCallsPerFrame;
    // uint64_t m_drawCallsPerSecond;


    // typ = 0 -> MeshBuffers per Frame
    // typ = 1 -> Vertices per Frame
    // typ = 2 -> Indices per Frame
    // typ = 3 -> Primitives per Frame
    // typ = 4 -> Total rendered Frames
    // typ = 5 -> Total rendered MeshBuffers
    // typ = 6 -> Total rendered Vertices
    // typ = 7 -> Total rendered Indices
    // typ = 8 -> Total rendered Primitives

    //   uint64_t getDrawCallsPerFrame() const { return m_drawCallsPerFrame; }
    //   uint64_t getDrawCallsPerSecond() const { return m_drawCallsPerSecond; }

    //   uint64_t getRenderStat( int typ ) const;


};

// VideoDriver* createVideoDriverOpenGL( CreateParams const & params );

VideoDriver* createVideoDriver( int w, int h, uint64_t windowId = 0);

} // end namespace gpu.
} // end namespace de.
