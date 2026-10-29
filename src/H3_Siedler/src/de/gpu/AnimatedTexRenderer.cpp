#include <de/gpu/AnimatedTexRenderer.h>
#include <de_opengl.h>
// #include <de/gpu/GL_debug_layer.h>

namespace de {
namespace gpu {

void createBubbleAnimation(std::vector<Image> & images)
{
    images.clear();
    images.reserve(100);

    struct Bubble
    {
        float r;
        float x;
        float y;
        float speed;
        uint32_t color = 0xFFFFFFFF;
    };
    std::vector<Bubble> bubbles;
    bubbles.reserve(100);
    for (int i = 0; i < 100; i++)
    {
        bubbles.emplace_back();
        Bubble & b = bubbles.back();
        b.x = std::clamp(float(rand() % 256) / 255.0f, 0.01f, 0.99f);
        b.y = std::clamp(float(rand() % 256) / 255.0f, 0.01f, 0.99f);
        b.r = 0.1f * std::clamp(float((rand() % 128)+1) / 255.0f, 0.01f, 0.1f);
        b.speed = 0.01f; // 0.1f * std::clamp(float((rand() % 256)+1) / 255.0f, 0.01f, 0.1f);
        int g = 255; // (rand()%128) + 128;
        b.color = dbRGB(g,g,g,g);
    }

    int scr_w = 256;
    int scr_h = 128;
    for (int i = 0; i < 100; i++)
    {
        images.emplace_back();
        Image & img = images.back();
        img.setPixelFormat(PixelFormat::RGBA_32);
        img.resize(scr_w,scr_h);
        img.fill(0);

        // Draw vertical lines, the longer the more transparent
        for (int j = 0; j < scr_w; j++)
        {
            int h = (rand() % scr_h) + 1;
            //int y2 = rand() % scr_h;
            //if (y2 < y1) std::swap(y1,y2);
            int a = (127 * h) / scr_h;
            uint32_t color = dbRGB(255,255,255,a);
            ImagePainter::drawLine(img,j,scr_h-1,j,scr_h-1-h,color,true);
        }

        for (int j = 0; j < bubbles.size(); j++)
        {
            auto & b = bubbles[j];
            int x = b.x * scr_w;
            int y = b.y * scr_h;
            int r = b.r * scr_w;
            ImagePainter::drawCircle(img,Recti(x,y,2*r,2*r), b.color,true);

            b.y -= b.speed;
            int alpha = std::clamp<int>(255 * (1.0f - b.y),0,255); // (rand()%128) + 128;
            dbRGB_setA(b.color,alpha);

            if (b.y < 0.0f)
            {
                b.y += 1.0f;
            }
        }
    }
}


void loadExplosionImages(std::vector<Image> & images)
{
    // "C:\GitHub\dev2025\src\H3_Siedler\media\H3\gfx\explosion02_webp\explosion02-frame001.webp"
    std::string mediaDir = "media/H3/gfx/explosion02_webp/explosion02-frame";

    images.clear();
    images.reserve(100);

    for (int i = 1; i <= 100; i++)
    {
        std::string s = "";
        if (i < 100) s += "0";
        if (i < 10) s += "0";
        std::string uri = dbStr(mediaDir,s,i,".webp");

        images.emplace_back();
        Image & img = images.back();
        if (!dbLoadImage(img,uri))
        {
            DE_ERROR("Cannot load ",uri)
        }
    }
}

void AnimatedTexRenderer::Material::setImages(const std::vector<const Image*>& images)
{
    de::gpu::SO so;
    so.setAF( 1.0f );
    so.setMin( de::gpu::SO::Minify::LinearMipmapLinear );
    so.setMag( de::gpu::SO::Magnify::Linear );
    so.setWrapR( de::gpu::SO::Wrap::Repeat );
    so.setWrapS( de::gpu::SO::Wrap::Repeat );
    so.setWrapT( de::gpu::SO::Wrap::Repeat );

    if (!tex.create("AniExplosion02",images,so))
    {
        DE_ERROR("Tex2DArray failed.")
    }

    bGhost = false;
    bAlphaBlend = true;
    bCulling = false;
    bLooping = true;
}

std::string AnimatedTexRenderer::Material::str() const
{
    std::ostringstream o; o <<
    "bGhost(" << bGhost << "), "
    "duration(" << duration << "), "
    "tex(" << tex.str() << ")";
    return o.str();
}

void AnimatedTexRenderer::Material::startAnimation()
{
    timeStart = dbTimeInSeconds();
    timeNow = 0.0;
}

void AnimatedTexRenderer::Material::tickAnimation()
{
    timeNow = dbTimeInSeconds() - timeStart;

    if (bLooping)
    {
        timeNow = fmod(timeNow, duration);
    }
    else
    {
        timeNow = std::clamp(timeNow, 0.0, duration);
    }

    layer = double(tex.layers) * (timeNow / duration);

    // DE_DEBUG("layer = ",layer)
}


//=============================================================
AnimatedTexRenderer::AnimatedTexRenderer()
//=============================================================
{
    // m_shader = nullptr;
    // m_vao = 0;
    // m_vbo_instanceMat = 0;
    // m_vbo_instanceAngle60 = 0;
}

AnimatedTexRenderer::~AnimatedTexRenderer()
{

}

void AnimatedTexRenderer::init(VideoDriver* driver)
{
    m_driver = driver;

    if (!m_driver) { DE_ERROR("No driver") return; }

    // auto tex = m_driver->createTexture2D( "H3_WaterTileRendTex", *img, so);

    //=========================================
    // Create default material
    //=========================================

    std::vector<Image> images;
    // loadExplosionImages(images);
    createBubbleAnimation(images);

    std::vector<const Image*> imagePtrs;
    imagePtrs.reserve(images.size());

    for (const auto& img : images)
    {
        imagePtrs.emplace_back(&img);
    }

    m_explosion02.setImages(imagePtrs);

    m_explosion02.startAnimation();
}

void AnimatedTexRenderer::destroy()
{
    m_explosion02.destroy();
}

void AnimatedTexRenderer::draw3D( const smesh::SMeshBuffer& mesh )
{
    m_explosion02.tickAnimation();
    draw3D(mesh, m_explosion02);
}

void AnimatedTexRenderer::draw3D( const smesh::SMeshBuffer& mesh, const Material& material )
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    // if (!m_vao || m_instanceMat.empty())
    // {
    //     return; // Nothing to draw.
    // }

    setMaterial( material );

    mesh.draw();
}

bool AnimatedTexRenderer::setMaterial( const Material& material )
{
    if (!m_driver) { DE_ERROR("No driver") return false; }

    // [UseProgram]
    gpu::Shader* shader = getShader( material );
    if ( !shader )
    {
        DE_ERROR("No shader in use for material(",material.str(),")")
        return false;
    }

    // [Camera]
    glm::dmat4 u_viewProjMat = glm::dmat4( 1.0 );
    //glm::vec3 u_cameraPos = glm::vec3( 0.0f );

    auto camera = m_driver->getCamera();
    if (camera)
    {
        camera->update();
        u_viewProjMat = camera->getViewProjectionMatrix();
        //u_cameraPos = camera->getPos();
    }

    shader->setMat4f( "u_projViewMat", u_viewProjMat );
    shader->setMat4f( "u_modelMat", m_driver->getModelMatrix() );

    const auto& tex = material.tex;
    glBindTextureUnit(0,tex.id);
    applySamplerOptions(tex.so);
    shader->setInt("u_tex",0);
    shader->setFloat("u_texLayer",material.layer);
    //shader->setVec4f( "u_texTransform",  );
    //shader->setVec2f( "u_texRepeat", diffuseMap.repeat );

    State state;
    if (!material.bCulling)
    {
        state.culling = gpu::Culling::disabled();
    }
    if ( material.bAlphaBlend )
    {
        state.blend = gpu::Blend::alphaBlend();
    }
    m_driver->setState( state );
    return true;
}

// static
std::string AnimatedTexRenderer::createShaderName(const Material & material)
{
    std::ostringstream o;
    o << "s3dAni";
    if ( material.bGhost ) o << "+ct";
    // o << "+illum" << material.Lighting;
    // if ( material.FogEnable ) o << "+fog";
    return o.str();
}

std::string AnimatedTexRenderer::createVS(const Material & material)
{
    std::ostringstream vs;
    vs << R"(
        precision highp float;

        in vec3 a_pos;
        in vec3 a_normal;
        in lowp vec4 a_color;
        in vec2 a_tex;

        uniform mat4 u_projViewMat;
        uniform mat4 u_modelMat;

        out vec3 v_pos;
        out vec3 v_normal;
        out vec4 v_color;
        out vec2 v_tex;

    void main()
    {
        gl_PointSize = 5.0;
        vec3 pos = a_pos;
        gl_Position = u_projViewMat * u_modelMat * vec4( pos, 1.0 );
        v_pos = a_pos;
        v_normal = a_normal;
        v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
        v_tex = a_tex;
    }
    )";

    return vs.str();
}

std::string AnimatedTexRenderer::createFS(const Material & material)
{
    std::ostringstream fs;
    fs << R"(
        precision highp float;

        out vec4 fragColor;

        in vec3 v_pos;
        in vec3 v_normal;
        in vec4 v_color;
        in vec2 v_tex;

        uniform sampler2DArray u_tex;
        uniform float u_texLayer;
        //uniform vec4 u_texTransform;

    void main()
    {
        vec3 texCoord = vec3(v_tex.x, v_tex.y, u_texLayer);

        fragColor = v_color * texture(u_tex, texCoord);
    }
    )";

    return fs.str();
}

Shader* AnimatedTexRenderer::getShader(const Material & material)
{
    gpu::Shader* shader = nullptr;

    if ( !m_driver ) { DE_ERROR("No driver.") return shader; }

    //uint32_t id = createShaderId( material );
    const std::string name = createShaderName( material );

    auto it = m_shaders.find( name );
    if ( it != m_shaders.end() )
    {
        shader = (*it).second;
    }
    else
    {
        shader = m_driver->getShader( name );
        if ( !shader )
        {
            shader = m_driver->createShader( name, createVS( material ), createFS( material ) );
        }

        if ( !shader )
        {
            DE_ERROR("No shader built(",name,")")
            return nullptr;
        }

        m_shaders[ name ] = shader; // Store new and valid shader
        m_uniforms[ name ] = enumerateUniforms(shader->id);

        // m_loc_pvMat = glGetUniformLocation(m_shader->id, "u_projViewMat");
        // m_loc_modelMat = glGetUniformLocation(m_shader->id, "u_modelMat");
        // m_loc_tex2DArray = glGetUniformLocation(m_shader->id, "u_tex2DArray");
        // m_loc_timeInSec = glGetUniformLocation(m_shader->id, "u_timeInSec");
        // m_loc_gradientMap = glGetUniformLocation(m_shader->id, "u_gradientMap");
        // m_loc_dudvMap = glGetUniformLocation(m_shader->id, "u_dudvMap");

    }

    if ( !shader )
    {
        DE_ERROR("No shader ",name);
        return nullptr;
    }

    // [UseProgram]
    if ( !m_driver->useShader( shader ) )
    {
        DE_ERROR("No active shader(",name,")")
        return nullptr;
    }

    return shader;
}

} // end namespace gpu.
} // end namespace de.
