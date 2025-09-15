#include <de/gpu/renderer/SkyboxRenderer.h>
#include <de/gpu/VideoDriver.h>
#include <de/image/Utils.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gpu {
// static
uint32_t
SkyboxRenderer::loadCubemap( std::array<const Image*,6> const & faces )
{
    uint32_t textureID = 0;
    de_glGenTextures(1, &textureID);
    de_glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    de_glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    de_glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    de_glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    de_glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    de_glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (size_t i = 0; i < faces.size(); i++)
    {
        const Image* const img = faces[i];
        //removeAlphaChannel( img );
        if (img)
        {
            if (img->pixelFormat() == PixelFormat::R8G8B8)
            {
                de_glTexImage2D( GLenum(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
                         0,
                         GL_RGB,
                         img->w(),
                         img->h(),
                         0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE, img->data() );
            }
            else
            {
                DE_ERROR("Unsupported pixelformat Face[",i,"], ", img->str())
            }
        }
        else
        {
            DE_ERROR("Cubemap[",i,"] failed to load, ",faces[i])
        }
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0); GL_VALIDATE;
    return textureID;
}

// static
uint32_t
SkyboxRenderer::loadCubemap( std::vector<std::string> const & faces )
{
    uint32_t textureID = 0;
    glGenTextures(1, &textureID); GL_VALIDATE;
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); GL_VALIDATE;

    ImageLoadOptions loadOptions;
    loadOptions.debugLog = true;

    Image img;
    for (size_t i = 0; i < faces.size(); i++)
    {
        if (dbLoadImage(img,faces[i],loadOptions))
        {
            removeAlphaChannel( img );
            glTexImage2D( GLenum(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
                0,
                GL_RGB,
                img.w(),
                img.h(),
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE, img.data() );
            GL_VALIDATE;
        }
        else
        {
            DE_ERROR("Cubemap[",i,"] failed to load, ",faces[i])
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    GL_VALIDATE;
    return textureID;
}


SkyboxRenderer::SkyboxRenderer()
    : m_driver( nullptr )
    , m_shader( nullptr )
    , m_vao(0)
    , m_vbo(0)
    , m_cubeMap(0)
{}

SkyboxRenderer::~SkyboxRenderer()
{
    if ( m_vao || m_vbo || m_cubeMap )
    {
        DE_ERROR("Forgot to call SkyboxRenderer.destroy()")
    }
}

void
SkyboxRenderer::destroy()
{
    if ( m_vao )
    {
        de_glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if ( m_vbo )
    {
        de_glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if ( m_cubeMap )
    {
        de_glDeleteTextures(1, &m_cubeMap);
        m_cubeMap = 0;
    }
}


void
SkyboxRenderer::init(VideoDriver* driver)
{
    m_driver = driver;
    initShader();
    initGeometry();
}

void
SkyboxRenderer::load(const Image* nx, const Image* px,
                     const Image* ny, const Image* py,
                     const Image* nz, const Image* pz)
{
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }

    if (m_cubeMap)
    {
        DE_WARN("Rewrite cubeMap texture")
        de_glDeleteTextures( 1, &m_cubeMap);
        m_cubeMap = 0;
    }

    auto images = std::array<const Image*,6>{px,nx,py,ny,pz,nz };
    if (!m_cubeMap)
    {
        m_cubeMap = loadCubemap( images );
    }
}

void
SkyboxRenderer::load( std::string const & mediaDir,
                      std::string const & px, std::string const & nx,
                      std::string const & py, std::string const & ny,
                      std::string const & pz, std::string const & nz )
{
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }

    if (m_cubeMap)
    {
        DE_WARN("Rewrite cubeMap texture")
        de_glDeleteTextures( 1, &m_cubeMap);
        m_cubeMap = 0;
    }

    auto fileNames = std::vector<std::string>
    {   mediaDir+px,
        mediaDir+nx,
        mediaDir+py,
        mediaDir+ny,
        mediaDir+pz,
        mediaDir+nz
    };

    m_cubeMap = loadCubemap( fileNames );
}

void
SkyboxRenderer::render()
{
    if ( !m_driver ) { DE_ERROR("No driver") return; }

    initShader();
    initGeometry();

    if ( !m_shader ) { DE_ERROR("No shader") return; }
    if ( !m_cubeMap ) { DE_ERROR("No cubeMap") return; }



    //glDepthMask(GL_FALSE);
    //glEnable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);

    // === RenderStates ===
    State state;
    state.depth = Depth::disabled();
    state.culling = Culling::disabled();
    state.blend = Blend::disabled();
    m_driver->setState( state );

    // === Shader ===
    m_driver->useShader( m_shader );

    // === ShaderUniform: MVP matrix ===
    const float d = 3800.0f;
    const glm::mat4 modelMat = glm::scale(glm::mat4(1.0f), glm::vec3(d,d,d));
    glm::mat4 viewProjMat( 1.0f );
    auto camera = m_driver->getCamera();
    if (camera)
    {
        int w = m_driver->getScreenWidth();
        int h = m_driver->getScreenHeight();
        camera->setScreenSize(w,h);
        camera->update();
        viewProjMat = camera->getViewProjectionMatrix();
    }
    const glm::mat4 mvp = viewProjMat * modelMat;
    glUniformMatrix4fv(m_locMVP, 1, GL_FALSE, glm::value_ptr(mvp)); GL_VALIDATE

    // === ShaderUniform: CubeMap sampler ===
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
    glBindTextureUnit(0, m_cubeMap); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); GL_VALIDATE
    glUniform1i(m_locTex, 0); GL_VALIDATE

    if (m_vao < 1)
    {
        DE_ERROR("m_vao < 1")
    }
    // === Render geometry ===
    glBindVertexArray(m_vao); GL_VALIDATE
    glDrawArrays(GL_TRIANGLES, 0, 36); GL_VALIDATE

    glBindVertexArray(0); GL_VALIDATE
    glBindTextureUnit(0, 0); GL_VALIDATE

    //glDepthMask(GL_TRUE);
}

void
SkyboxRenderer::initShader()
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (m_shader) { return; } // Nothing todo.

    const std::string shaderName = "SkyboxCubeMap";

    const std::string vs = R"(
        layout (location = 0) in vec3 a_pos;

        out vec3 v_texCoords;

        uniform mat4 u_mvp;

        void main()
        {
            v_texCoords = a_pos; // must be in range [-1,1].
            gl_Position = u_mvp * vec4(a_pos, 1.0);
        }
    )";

    const std::string fs = R"(

        out vec4 fragColor;

        in vec3 v_texCoords;

        uniform samplerCube u_tex;

        void main()
        {
            fragColor = texture(u_tex, v_texCoords);
        }
    )";

    m_shader = m_driver->createShader( shaderName, vs, fs );

    m_locMVP = glGetUniformLocation(m_shader->id, "u_mvp"); GL_VALIDATE
    m_locTex = glGetUniformLocation(m_shader->id, "u_tex"); GL_VALIDATE
}

void
SkyboxRenderer::initGeometry()
{
    bool m_bNeedUpload = false;

    if (!m_vbo)
    {
        glGenBuffers(1, &m_vbo); GL_VALIDATE
        //DE_TRACE("m_vbo_vertices = ",m_vbo_vertices)
        m_bNeedUpload = true;
    }

    if (!m_vao)
    {
        glGenVertexArrays(1, &m_vao); GL_VALIDATE
        //DE_TRACE("m_vao = ",m_vao)
        m_bNeedUpload = true;
    }

    if (!m_bNeedUpload)
    {
        return;
    }

    auto addQuad = [&]( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d )
    {
        m_vertices.emplace_back( a );
        m_vertices.emplace_back( b );
        m_vertices.emplace_back( c );
        m_vertices.emplace_back( a );
        m_vertices.emplace_back( c );
        m_vertices.emplace_back( d );
    };

    // nx = left face
    addQuad(glm::vec3(-1.0f, -1.0f, -1.0f),  // top-right
            glm::vec3(-1.0f,  1.0f, -1.0f),  // top-left
            glm::vec3(-1.0f,  1.0f,  1.0f),  // bottom-left
            glm::vec3(-1.0f, -1.0f,  1.0f)); // bottom-right

    // nz = back face
    addQuad(glm::vec3( 1.0f, -1.0f, -1.0f),  // bottom-left
            glm::vec3( 1.0f,  1.0f, -1.0f),  // top-right
            glm::vec3(-1.0f,  1.0f, -1.0f),  // bottom-right
            glm::vec3(-1.0f, -1.0f, -1.0f)); // top-left

    // pz = front face
    addQuad(glm::vec3(-1.0f, -1.0f,  1.0f),  // bottom-left
            glm::vec3(-1.0f,  1.0f,  1.0f),  // bottom-right
            glm::vec3( 1.0f,  1.0f,  1.0f),  // top-right
            glm::vec3( 1.0f, -1.0f,  1.0f)); // top-left

    // ny = bottom face
    addQuad(glm::vec3(-1.0f, -1.0f, -1.0f),  // top-right
            glm::vec3( 1.0f, -1.0f, -1.0f),  // top-left
            glm::vec3( 1.0f, -1.0f,  1.0f),  // bottom-left
            glm::vec3(-1.0f, -1.0f,  1.0f)); // bottom-right

    // py = top face
    addQuad(glm::vec3(-1.0f,  1.0f, -1.0f),  // top-left
            glm::vec3( 1.0f,  1.0f, -1.0f),  // bottom-right
            glm::vec3( 1.0f,  1.0f,  1.0f),  // top-right
            glm::vec3(-1.0f,  1.0f,  1.0f)); // bottom-left

    // px = right face
    addQuad(glm::vec3(1.0f, -1.0f,  1.0f),   // top-left
            glm::vec3(1.0f,  1.0f,  1.0f),   // bottom-right
            glm::vec3(1.0f,  1.0f, -1.0f),   // top-right
            glm::vec3(1.0f, -1.0f, -1.0f));  // bottom-left


    //glGenBuffers(1, &m_vbo);
    //glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao); GL_VALIDATE
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo); GL_VALIDATE
    glBufferData(GL_ARRAY_BUFFER, GLint(m_vertices.size() * sizeof(glm::vec3)), m_vertices.data(), GL_STATIC_DRAW); GL_VALIDATE
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast< void* >(0)); GL_VALIDATE
    glEnableVertexAttribArray(0); GL_VALIDATE

    glBindVertexArray(0); GL_VALIDATE
    glBindBuffer(GL_ARRAY_BUFFER, 0); GL_VALIDATE
}

} // end namespace gpu.
} // end namespace de.
