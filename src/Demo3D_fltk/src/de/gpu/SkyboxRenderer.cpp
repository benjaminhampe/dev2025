#include "SkyboxRenderer.h"
// #include <de/gpu/VideoDriver.h>
// #include <de/image/Utils.h>
#include <de_opengl.h>
//#include <de/gpu/GL_debug_layer.h>

namespace {

// auto faces = std::array<const de::Image*,6>{&px,&nx,&py,&ny,&pz,&nz};
static void
debugSkybox( const std::array<const de::Image*,6>& faces )
{
    const de::Image& px = *faces[0];
    const de::Image& nx = *faces[1];
    const de::Image& py = *faces[2];
    const de::Image& ny = *faces[3];
    const de::Image& pz = *faces[4];
    const de::Image& nz = *faces[5];

    // de::Image px; dbLoadImage(px,faces[0]); // right
    // de::Image nx; dbLoadImage(nx,faces[1]); // left
    // de::Image py; dbLoadImage(py,faces[2]); // top
    // de::Image ny; dbLoadImage(ny,faces[3]); // bottom
    // de::Image nz; dbLoadImage(nz,faces[4]); // back
    // de::Image pz; dbLoadImage(pz,faces[5]); // front

    int w = px.w();
    int h = px.h();
    de::Image big( 4*w, 3*h );
    de::ImagePainter::drawImage( big, nx, 0*w,h );
    de::ImagePainter::drawImage( big, pz, 1*w,h ); // swapped by gl, because z goes other way
    de::ImagePainter::drawImage( big, px, 2*w,h );
    de::ImagePainter::drawImage( big, nz, 3*w,h ); // swapped by gl, because z goes other way
    de::ImagePainter::drawImage( big, py, w,0 );
    de::ImagePainter::drawImage( big, ny, w,2*h );

    de::Font5x8 font(4,4,1,1,1,1);
    int cx = w/2;
    int cy = h/2;
    uint32_t color = 0xFF0000FF; // green
    de::Align align = de::Align::Centered; // align center
    de::ImagePainter::drawText5x8( big, cx+0*w,cy+1*h, "NX", color, font, align );
    de::ImagePainter::drawText5x8( big, cx+1*w,cy+1*h, "PZ", color, font, align );
    de::ImagePainter::drawText5x8( big, cx+2*w,cy+1*h, "PX", color, font, align );
    de::ImagePainter::drawText5x8( big, cx+3*w,cy+1*h, "NZ", color, font, align );
    de::ImagePainter::drawText5x8( big, cx+1*w,cy+0*h, "PY", color, font, align );
    de::ImagePainter::drawText5x8( big, cx+1*w,cy+2*h, "NY", color, font, align );

    dbSaveImage( big, "Demo3D_debugSkybox.png" );
    //return big
}

//==========================================================
struct SkyboxRenderer
//==========================================================
{
    uint32_t shaderId = 0;
    int locMVP = -1;
    int locTex = -1;

    uint32_t vao = 0;
    uint32_t vbo = 0;
    uint32_t cubeMap = 0;
    std::vector< glm::vec3 > vertices;

    bool bOK = false;
};

} // end namespace.

static SkyboxRenderer g_sky;

void SkyboxRenderer_init()
{
    if (g_sky.bOK)
    {
        DE_WARN("Already OK")
        return;
    }

    // ----
    // ---- CubeMap Textures ----
    // ----
    de::Image nx;
    de::Image px;
    de::Image ny;
    de::Image py;
    de::Image nz;
    de::Image pz;
    std::string mediaDir = "media/Siedler3D/skybox/interstellar/interstellar_";
    if (!dbLoadImage(nx, mediaDir + "rt.webp"))
    {
        DE_ERROR("No nx") return;
    }
    if (!dbLoadImage(px, mediaDir + "lf.webp"))
    {
        DE_ERROR("No px") return;
    }
    if (!dbLoadImage(ny, mediaDir + "dn.webp"))
    {
        DE_ERROR("No ny") return;
    }
    if (!dbLoadImage(py, mediaDir + "up.webp"))
    {
        DE_ERROR("No py") return;
    }
    if (!dbLoadImage(nz, mediaDir + "bk.webp"))
    {
        DE_ERROR("No nz") return;
    }
    if (!dbLoadImage(pz, mediaDir + "ft.webp"))
    {
        DE_ERROR("No pz") return;
    }

    auto faces = std::array<const de::Image*,6>{&px,&nx,&py,&ny,&pz,&nz};

    debugSkybox( faces );

    uint32_t textureID = 0;
    glGenTextures(1, &textureID); GL_VALIDATE
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); GL_VALIDATE

    for (size_t i = 0; i < faces.size(); i++)
    {
        const de::Image* const img = faces[i];
        DE_DEBUG("Face[",i,"] ", img->str())
        //removeAlphaChannel( img );
        if (img)
        {
            if (img->pixelFormat() == de::PixelFormat::R8G8B8)
            {
                glTexImage2D( GLenum(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i),
                         0,
                         GL_RGB,
                         img->w(),
                         img->h(),
                         0,
                         GL_RGB,
                         GL_UNSIGNED_BYTE, img->data() );
                 GL_VALIDATE
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

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    GL_VALIDATE;

    g_sky.cubeMap = textureID;

    // ----
    // ---- Shader ----
    // ----

    const std::string shaderName = "SkyboxCubeMap";

    const std::string vs = R"(
        #version 330 core

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
        #version 330 core

        out vec4 fragColor;

        in vec3 v_texCoords;

        uniform samplerCube u_tex;

        void main()
        {
            fragColor = texture(u_tex, v_texCoords);
        }
    )";

    g_sky.shaderId = BGL::createShader( shaderName, vs, fs );

    g_sky.locMVP = glGetUniformLocation(g_sky.shaderId, "u_mvp");
    GL_VALIDATE
    g_sky.locTex = glGetUniformLocation(g_sky.shaderId, "u_tex");
    GL_VALIDATE

    // ----
    // ---- Geometry ----
    // ----

    glGenBuffers(1, &g_sky.vbo); GL_VALIDATE
    glGenVertexArrays(1, &g_sky.vao); GL_VALIDATE

    auto addQuad = [&]( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d )
    {
        g_sky.vertices.emplace_back( a );
        g_sky.vertices.emplace_back( b );
        g_sky.vertices.emplace_back( c );
        g_sky.vertices.emplace_back( a );
        g_sky.vertices.emplace_back( c );
        g_sky.vertices.emplace_back( d );
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
    addQuad(glm::vec3(-1.0f, -1.0f,  1.0f),  // top-right
            glm::vec3( 1.0f, -1.0f,  1.0f),  // top-left
            glm::vec3( 1.0f, -1.0f, -1.0f),  // bottom-left
            glm::vec3(-1.0f, -1.0f, -1.0f)); // bottom-right

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
    glBindVertexArray(g_sky.vao); GL_VALIDATE
    glBindBuffer(GL_ARRAY_BUFFER, g_sky.vbo); GL_VALIDATE
    glBufferData(GL_ARRAY_BUFFER, GLint(g_sky.vertices.size() * sizeof(glm::vec3)), g_sky.vertices.data(), GL_STATIC_DRAW); GL_VALIDATE
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast< void* >(0)); GL_VALIDATE
    glEnableVertexAttribArray(0); GL_VALIDATE

    glBindVertexArray(0); GL_VALIDATE
    glBindBuffer(GL_ARRAY_BUFFER, 0); GL_VALIDATE

    g_sky.bOK = true;
}

void SkyboxRenderer_draw( de::gpu::Camera* camera )
{
    if (!g_sky.bOK)
    {
        DE_ERROR("Not ok")
        return;
    }

    glCullFace( GL_BACK );      GL_VALIDATE
    glFrontFace( GL_CW );       GL_VALIDATE
    glEnable( GL_CULL_FACE );   GL_VALIDATE

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_ALWAYS );
    glDepthMask( GL_FALSE );

    glDisable( GL_BLEND );

    glUseProgram( g_sky.shaderId );

    // === ShaderUniform: MVP matrix ===
    const float d = 4800.0f;
    const glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), -90.0f, glm::vec3(0,1,0));
    const glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(d,d,d));
    const glm::mat4 M = Ry * S;
    glm::mat4 viewProjMat( 1.0f );
    if (camera)
    {
        viewProjMat = camera->getViewProjectionMatrix();
    }
    const glm::mat4 mvp = viewProjMat * M;
    glUniformMatrix4fv(g_sky.locMVP, 1, GL_FALSE, glm::value_ptr(mvp));
    GL_VALIDATE

    // === ShaderUniform: CubeMap sampler ===
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);
    glBindTextureUnit(0, g_sky.cubeMap); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); GL_VALIDATE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); GL_VALIDATE
    glUniform1i(g_sky.locTex, 0); GL_VALIDATE

    // === Render geometry ===
    glBindVertexArray(g_sky.vao);
    GL_VALIDATE
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GL_VALIDATE

    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glBindVertexArray(0);
    GL_VALIDATE
    // glBindTextureUnit(0, 0);
    GL_VALIDATE

    //glDepthMask(GL_TRUE);
}

void SkyboxRenderer_destroy()
{
    if ( g_sky.vao )
    {
        glDeleteVertexArrays(1, &g_sky.vao); GL_VALIDATE
        g_sky.vao = 0;
    }
    if ( g_sky.vbo )
    {
        glDeleteBuffers(1, &g_sky.vbo); GL_VALIDATE
        g_sky.vbo = 0;
    }
    if ( g_sky.cubeMap )
    {
        glDeleteTextures(1, &g_sky.cubeMap); GL_VALIDATE
        g_sky.cubeMap = 0;
    }
}
