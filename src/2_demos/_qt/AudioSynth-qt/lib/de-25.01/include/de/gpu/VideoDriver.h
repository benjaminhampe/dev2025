#pragma once
#include <de/gpu/GPU.h>
#include <de/gpu/Camera.h>
#include <de/os/Window.h>
#include <de/os/CommonDialogs.h>
#include <de/gpu/renderer/ScreenRenderer.h>
#include <de/gpu/renderer/FontRenderer5x8.h>
#include <de/gpu/renderer/FontRenderer.h>
#include <de/gpu/renderer/SkyboxRenderer.h>
#include <de/gpu/smesh/SMeshRenderer.h>
#include <de/gpu/smesh/SMeshLibrary.h>
#include <de/gpu/renderer/Line3D_Renderer.h>
#include <de/gpu/renderer/PostFxRenderer.h>

//#include <de/gpu/mtl/PMesh.h>
#include <de/gpu/mtl/PMaterialRenderer.h>
//#include <de/gpu/mtl/PMeshSceneNode.h>

//#include <de/gpu/mtl/MTL.h>
// #include <de/gui/Env.h>
// #include <de/gui/Panel.h>
// #include <de/gui/Button.h>
// #include <de/gui/Label.h>

#include <de/gpu/scene/SceneManager.h>

namespace de {

struct IrrlichtDevice;

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

/*
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Screen quad shader source code
const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    void main() {
        TexCoord = aTexCoord;
        gl_Position = vec4(aPos, 0.0, 1.0);
    }
)";

const char *fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 TexCoord;
    uniform sampler2D colorTexture;
    void main() {
        FragColor = texture(colorTexture, TexCoord);
    }
)";

unsigned int createShader(GLenum type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    return shader;
}

// Setup framebuffer with color, depth, and stencil attachments
unsigned int setupFramebuffer(unsigned int &colorTexture, unsigned int &depthStencilTexture) {
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Create color attachment
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    // Create depth-stencil attachment
    glGenTextures(1, &depthStencilTexture);
    glBindTexture(GL_TEXTURE_2D, depthStencilTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTexture, 0);

    // Check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return FBO;
}


    // Creating HDR Color Attachment (RGBA16F)
    glGenTextures(1, &hdrColorTexture);
    glBindTexture(GL_TEXTURE_2D, hdrColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrColorTexture, 0);

    // Creating First Depth Texture Attachment
    glGenTextures(1, &depthTexture1);
    glBindTexture(GL_TEXTURE_2D, depthTexture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture1, 0);

    // Creating Second Depth Texture for Effects
    glGenTextures(1, &depthTexture2);
    glBindTexture(GL_TEXTURE_2D, depthTexture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture2, 0);

    // Creating Separate Stencil Buffer as Renderbuffer
    glGenRenderbuffers(1, &stencilRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, stencilRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, 800, 600);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilRenderbuffer);

    // Ensuring framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D hdrTexture;

void main() {
    vec3 hdrColor = texture(hdrTexture, TexCoord).rgb;
    hdrColor = hdrColor / (hdrColor + vec3(1.0)); // Reinhard tone mapping
    FragColor = vec4(hdrColor, 1.0);
}

// Quad setup
unsigned int setupQuad() {
    float quadVertices[] = {
        // Positions    // TexCoords
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return quadVAO;
}

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Framebuffer Rendering", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Create shaders
    unsigned int vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Setup FBO and textures
    unsigned int colorTexture, depthStencilTexture;
    unsigned int framebuffer = setupFramebuffer(colorTexture, depthStencilTexture);

    // Setup screen quad
    unsigned int quadVAO = setupQuad();

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Here, render the scene (e.g., 3D objects, models) into the framebuffer

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render quad with framebuffer texture
        glUseProgram(shaderProgram);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
*/


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
    //Recti m_pos;
    int32_t m_w;
    int32_t m_h;
    RT_Attachment m_color; // DiffuseColor RGBA 32 or RGBA_128
    RT_Attachment m_depthStencil; // DepthStencil D24_S8
    std::string m_name;

    RT_RGB();
    ~RT_RGB() override;
    const std::string& name() const override
    { return m_name; }
    uint32_t fbo() const override
    { return m_fbo; }
    void setFbo( uint32_t fbo ) override
    { m_fbo = fbo; }

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
    const RT_Attachment& depthAttachment( int i = 0 ) const override { return m_depthStencil; }
    const RT_Attachment& stencilAttachment( int i = 0 ) const override { return m_depthStencil; }
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

        if ( dbRGBA_A(curves[ name ].color) < 1 )
        {
            if ( dbRGBA_A(color) > 0 )
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
struct Light
// ===========================================================================
{
    glm::vec3 pos = glm::vec3(0.f,500.f,0.f);
    glm::vec3 color = glm::vec3(1.f,1.f,.8f);
    int typ = 0;
    float maxdist = 1000.0f; // distance
    //glm::vec3 siz;   // world size of light source
    //glm::vec3 dir;
    //float power;      // distance
    //float radius = 500.0f;
    //glm::vec3 clipPos; // for Shader, computed by CPU

    //SMeshBuffer mesh;
};

typedef std::vector< Light > Lights;

// ===========================================================================
struct VideoDriver
// ===========================================================================
{
    VideoDriver();
    ~VideoDriver();

    void                setIrrlichtDevice(IrrlichtDevice* device);
    IrrlichtDevice*     getIrrlichtDevice();

    bool                open(int w, int h);
    void                close();
    void                beginRender( IRenderTarget* rt = nullptr );
    void                endRender();

    int                 getScreenWidth() const { return m_screenWidth; }
    int                 getScreenHeight() const { return m_screenHeight; }
    double              getFPS() const;
    double              getTime() const;
    uint64_t            getFrameCount() const;


    // ##################
    // ### PerfTracer ###
    // ##################
    void                beginPerf( std::string name, uint32_t color = 0);
    void                endPerf();
    void                drawPerf( Recti pos);

    SceneManager*     	getSceneManager() { return &m_sceneManager; }
	
    PMaterialRenderer*  getPMaterialRenderer() { return &m_pmaterialRenderer; }
    SMaterialRenderer*  getSMaterialRenderer() { return &m_smaterialRenderer; }
	
    SkyboxRenderer*     getSkyboxRenderer() { return &m_skyboxRenderer; }

    ScreenRenderer*     getScreenRenderer() { return &m_screenRenderer; }
    FontRenderer5x8*    getFontRenderer5x8() { return &m_fontRenderer5x8; }
    FontRenderer*       getFontRenderer() { return &m_fontRenderer; }
    Line3D_Renderer*    getLine3DRenderer() { return &m_line3dRenderer; }
    PostFxRenderer*     getPostFxRenderer(){ return &m_postFxRenderer; }
    //gui::Env*           getGUIEnv() { return &m_guienv; }

    const glm::dmat4&   getModelMatrix() const { return m_modelMatrix; }
    void                setModelMatrix( const glm::dmat4& modelMatrix ) { m_modelMatrix = modelMatrix; }
    void                resetModelMatrix() { m_modelMatrix = glm::dmat4( 1.0 ); }

    Camera*             getCamera() { return m_camera; }
    void                setCamera( Camera* camera ) { m_camera = camera; }
    void                resetCamera() { m_camera = &m_camera0; }

    State const &       getState() const;
    void                setState( State const & state );

    // double getFPS() const { return m_fpsComputer.getFPS();   }
    // uint64_t getFrameCount() const { return m_fpsComputer.getFrameCount(); }
    // double getTimeInSeconds() const { return m_fpsComputer.getTime(); }

    // #####################
    // ###   Transform   ###
    // #####################

    void resize( int w, int h )
    {
        m_screenWidth = w;
        m_screenHeight = h;
        //setViewport( 0, 0, w, h );

        if (getCamera())
        {
            getCamera()->setScreenSize(w,h);
            getCamera()->update();
        }
    }

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

    // Get current RenderTarget:
    void setRenderTarget(IRenderTarget* rt = nullptr);
    IRenderTarget* getRenderTarget();

    // Get named RenderTarget:
    IRenderTarget* getRenderTarget(std::string name);

    // Get cached RenderTarget:
    IRenderTarget* getRenderTarget(uint32_t index);
    uint32_t getRenderTargetCount() const;

    // Add RenderTarget with combined depth-stencil attachment:
    IRenderTarget* addRenderTarget(std::string name, int w, int h,
                                   PixelFormat color = PixelFormat::R8G8B8A8,
                                   PixelFormat depthStencil = PixelFormat::D24S8);
    // Add RenderTarget with separate depth-stencil attachment:
    IRenderTarget* addRenderTarget_HDR(std::string name, int w, int h,
                                   PixelFormat color = PixelFormat::RGBA32F,
                                   PixelFormat depth = PixelFormat::D32F,
                                   PixelFormat stencil = PixelFormat::S8);

    // #####################
    // ### ShaderManager ###
    // #####################
    std::string getShaderVersionHeader() const;
    int32_t     getShaderVersionMajor() const;
    int32_t     getShaderVersionMinor() const;
    void        clearShaders();
    Shader*     getShader() const;
    Shader*     getShader( const std::string& name ) const;
    bool        useShader( Shader* shader );
    //void      addShader( IShader* shader );
    Shader*     createShader( const std::string& name, const std::string& vsText, const std::string& fsText );
    //Shader*   createShader( const std::string& name, const std::string& vs, const std::string& gs, const std::string& fs, bool debug = false );
    //int32_t   findShader( const std::string& name ) const;

    // ######################
    // ###   TexManager   ###
    // ######################
    bool        useTexture( Texture* tex, int stage );
    //bool      useTexture( TexRef ref, int stage = 0 );
    uint32_t    getMaxTex2DSize() const { return m_texMgr.getMaxTex2DSize(); }
    uint32_t    getNumTexUnits() const { return m_texMgr.getNumTexUnits(); }
    void        dumpTextures() const
                { m_texMgr.dumpTextures(); }
    void        updateTextures()
                { m_texMgr.updateTextures(); }
    void        removeTextures()
                { m_texMgr.removeTextures(); }
    void        removeTexture( const std::string& name )
                { m_texMgr.removeTexture(name); }
    void        removeTexture( Texture* &tex )
                { m_texMgr.removeTexture(tex); }
    Texture*    getTexture( const std::string& name ) const
                { return m_texMgr.getTexture(name); }
    Texture*    loadTexture2D( const std::string& name, const SamplerOptions& so = SamplerOptions(), const ImageLoadOptions& ilo = ImageLoadOptions() )
                { return m_texMgr.loadTexture2D(name,so,ilo); }
    // Texture*    createTexture( const std::string& name )
    //             { return m_texMgr.createTexture2D(name); }
    Texture*    createTexture2D( const std::string& name, int w, int h, void const* const pixels, PixelFormat fmt, const SamplerOptions& so = SamplerOptions() )
                { return m_texMgr.createTexture2D(name,w,h,pixels,fmt,so); }
    Texture*    createTexture2D( const std::string& name, const Image& img, const SamplerOptions& so = SamplerOptions() )
                { return m_texMgr.createTexture2D(name,img,so); }
    bool        uploadTexture2D( Texture* &tex, int w, int h, void const* const pixels, PixelFormat fmt, const SamplerOptions& so = SamplerOptions() )
                { return m_texMgr.uploadTexture2D(tex,w,h,pixels,fmt,so); }
    bool        uploadTexture2D( Texture* &tex, const Image& img, const SamplerOptions& so = SamplerOptions() )
                { return m_texMgr.uploadTexture2D(tex,img,so); }

    // ============================================================
    // FontRenderer (ttf) and Font5x8Renderer in action:
    // ============================================================

    // Draw Unicode String: (default for FontTTF unicode freetype2 ttf files)
    void        draw2DText( int x, int y, const std::wstring& msg,
                            const uint32_t color = 0xFFFFFFFF, const Align align = Align::Default,
                            const Font5x8 & font = Font5x8(), uint32_t bgColor = dbRGBA(0,0,0,200), int padding = 1);

    void        draw2DText( int x, int y, const std::wstring& msg,
                            const uint32_t color = 0xFFFFFFFF, const Align align = Align::Default,
                            const Font & font = Font("Arial",16), uint32_t bgColor = dbRGBA(0,0,0,200), int padding = 1);

    // Draw Multibyte String: (default for Font5x8, since there are so few glyphs we use 7/8 bit char* text)
    void        draw2DText( int x, int y, const std::string& msg,
                            const uint32_t color = 0xFFFFFFFF, const Align align = Align::Default,
                            const Font5x8 & font = Font5x8(), uint32_t bgColor = dbRGBA(0,0,0,200), int padding = 1);

    void        draw2DText( int x, int y, const std::string& msg,
                            const uint32_t color = 0xFFFFFFFF, const Align align = Align::Default,
                            const Font & font = Font("Arial",16), uint32_t bgColor = dbRGBA(0,0,0,200), int padding = 1);

    void draw2DPerfOverlay();

    /* typ = 0 is a point light like the sun, dir is computed by pos only
    * typ = 1 is a directional cone light like a Taschenlampe,
    * typ = 2 is a directional cylinder with round caps like a neon light
    */
    void
    addLight(glm::vec3 pos = glm::vec3(0,0,0),
             glm::vec3 color = glm::vec3(1,1,1),
             float maxdist = 500.0f,
             int typ = 1,
             glm::vec3 dir = glm::vec3(0,-1,0),
             glm::vec3 siz = glm::vec3(10,10,10)
             )
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

    uint32_t getLightCount() const { return uint32_t(m_lights.size()); }
    Lights & getLights() { return m_lights; }
    const Lights & getLights() const { return m_lights; }

    Light & getLight( uint32_t light ) { return m_lights[ light ]; }
    const Light & getLight( uint32_t light ) const { return m_lights[ light ]; }

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
    IrrlichtDevice* m_device;

    // ####################
    // ### StateManager ###
    // ####################
    int m_screenWidth;
    int m_screenHeight;

    State m_initState;
    State m_state;
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

    SMaterialRenderer m_smaterialRenderer;
    PMaterialRenderer m_pmaterialRenderer;
	
    ScreenRenderer m_screenRenderer;

    FontRenderer5x8 m_fontRenderer5x8;

    FontRenderer m_fontRenderer;    

    Line3D_Renderer m_line3dRenderer;

    PostFxRenderer m_postFxRenderer;

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
