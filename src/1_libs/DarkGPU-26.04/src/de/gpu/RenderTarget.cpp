#include <de/gpu/RenderTarget.h>
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gpu {


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

} // end namespace gpu.
} // end namespace de.

