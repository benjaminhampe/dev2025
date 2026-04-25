#include "GL_Renderer.h"
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>
#include <App.h>
#include "rainbow_1k_webp.h"

// ===========================================================================
GL_Renderer::GL_Renderer()
// ===========================================================================
    : m_driver{ nullptr }
    , m_showFftMatrix3D{ true }
{
#if 0
    m_wav_colorGradient.addStop( 0, dbRGBA(0,0,0) );
    m_wav_colorGradient.addStop( 0.1f, dbRGBA(255,255,255) );
    m_wav_colorGradient.addStop( 0.3f, dbRGBA(90,90,100) );
    m_wav_colorGradient.addStop( 0.2f, dbRGBA(0,0,255) );
    m_wav_colorGradient.addStop( 0.6f, dbRGBA(0,200,0) );
    m_wav_colorGradient.addStop( 0.8f, dbRGBA(255,155,0) );
    m_wav_colorGradient.addStop( 0.9f, dbRGBA(255,255,0) );
    m_wav_colorGradient.addStop( 1.0f, dbRGBA(255,0,0) );
    m_wav_colorGradient.addStop( 1.1f, dbRGBA(255,255,255) );
    m_wav_colorGradient.addStop( 1.2f, dbRGBA(255,0,255) );
    m_wav_colorGradient.addStop( 1.3f, dbRGBA(0,0,255) );
    m_wav_colorGradient.addStop( 1.5f, dbRGBA(0,0,155) );
    m_wav_colorGradient.addStop( 2.5f, dbRGBA(205,105,5) );

    m_fft_colorGradient.addStop( 0, dbRGBA(0,0,0) );
    m_fft_colorGradient.addStop( 0.45, dbRGBA(125,125,125) );
    m_fft_colorGradient.addStop( 0.5, dbRGBA(0,0,255) );
    m_fft_colorGradient.addStop( 0.6, dbRGBA(0,200,0) );
    m_fft_colorGradient.addStop( 0.8, dbRGBA(255,255,0) );
    m_fft_colorGradient.addStop( 1.0, dbRGBA(255,0,0) );
    m_fft_colorGradient.addStop( 1.1, dbRGBA(255,0,255) );

    dbSaveLinearColorGradient(m_wav_colorGradient,"VHS_wav_colorGradient.lcg");
    dbSaveLinearColorGradient(m_wav_colorGradient,"VHS_fft_colorGradient.lcg");

    de::Image
    tmp = dbImageFromLinearColorGradient(m_wav_colorGradient,256,256,false);
    dbSaveImage(tmp,"VHS_wav_colorGradient_h.png");

    tmp = dbImageFromLinearColorGradient(m_wav_colorGradient,256,256,true);
    dbSaveImage(tmp,"VHS_wav_colorGradient_v.png");

    tmp = dbImageFromLinearColorGradient(m_fft_colorGradient,256,256,false);
    dbSaveImage(tmp,"VHS_fft_colorGradient_h.png");

    tmp = dbImageFromLinearColorGradient(m_fft_colorGradient,256,256,true);
    dbSaveImage(tmp,"VHS_fft_colorGradient_v.png");
#endif
}

GL_Renderer::~GL_Renderer()
{}

void GL_Renderer::initializeGL(de::gpu::VideoDriver* driver)
{
    if (!driver)
    {
        DE_ERROR("No driver")
        return;
    }

    m_driver = driver;
    m_mesh16Shader2D.setDriver( driver );
    m_mesh16Shader3D.setDriver( driver );

    de::Image img;
    dbLoadImage(img,rainbow_1k_webp,710,"rainbow_1k.webp");
    m_mesh16Material.tex0 = m_driver->createTexture2D("rainbow.webp",img);

    dbRandomize();

    auto d = glm::vec3(4000,250,2000);
    m_matrix_fft.init(d, V3(-0.5f * d.x,0,0));
}

void GL_Renderer::paintGL()
{
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }
    // m_lineShader.resetModelMat();
    // m_lineShader.draw( bks_lines );
    // m_lineShader.draw( bks_x_lines );
    // m_lineShader.draw( bks_y_lines );
    // m_lineShader.draw( bks_z_lines );

    draw3DAccumFftMatrix();
    draw3DLineStripL();
    draw3DLineStripR();

    draw2DLineStripL();
    draw2DLineStripR();

    draw2DLineStripFft();
}

void GL_Renderer::draw3DAccumFftMatrix()
{
    if (m_showFftMatrix3D)
    {
        auto s = App::instance()->getSampleCollector();

        m_matrix_fft.draw(
            m_mesh16Shader3D,
            m_mesh16Material,
            s->getAccumMat());
    }
}

void GL_Renderer::draw3DLineStripL()
{
    // auto d = glm::vec3(4000,250,2000);
    auto s = App::instance()->getSampleCollector();

    const auto& vL = s->getL();

    auto siz = glm::vec2{2000,500};
    auto pos = glm::vec3{-2000.f,0,1000.0f};

    create2DWav_LineStrip(
        m_lineStripL,
        siz,
        vL.data(),
        vL.size());

    auto T  = glm::translate(glm::mat4(1.0f), pos);
    auto Ry = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3{0,1,0});
    m_mesh16Shader3D.setMaterial(m_mesh16Material, T * Ry);
    m_lineStripL.draw();
}

void GL_Renderer::draw3DLineStripR()
{
    // auto d = glm::vec3(4000,250,2000);
    auto s = App::instance()->getSampleCollector();

    const auto& vR = s->getR();

    create2DWav_LineStrip(
        m_lineStripR,
        glm::vec2{2000,500},
        vR.data(),
        vR.size());

    auto T  = glm::translate(glm::mat4(1.0f), glm::vec3{2000.f,0,1000.0f});
    auto Ry = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3{0,1,0});
    m_mesh16Shader3D.setMaterial(m_mesh16Material, T * Ry);
    m_lineStripR.draw();

}

void GL_Renderer::draw2DLineStripL()
{
    int w = m_driver->getScreenWidth()/4;
    int h = m_driver->getScreenHeight()/8;
    int y1 = h/2;

    m_driver->getScreenRenderer()->draw2DLine(0,y1,w,y1,
        0xFFFFFFFF, 0xFFFFFFFF);

    auto s = App::instance()->getSampleCollector();

    const auto& vL = s->getL();

    auto n = vL.size();
    auto & m = m_lineStripL;
    m.PrimType = de::gpu::PrimitiveType::LineStrip;
    m.Vertices.clear();
    m.Vertices.reserve(n);
    m.Indices.clear();

    const float dx = 1.0f / float(n-1);
    const float dy = 0.5f;

    for (size_t i = 0; i < n; ++i)
    {
        const float s = vL[i];
        const float x = dx * i;
        const float y = dy * s;
        const float z = 0.0f;
        const float t = std::clamp((0.5f * s) + 0.5f, 0.f, 1.f);
        m.Vertices.push_back( GL_Mesh16_Vertex( x, y, z, t ) );
    }

    m.upload( true );

    // DRAW
    m_mesh16Shader2D.setMaterial(m_mesh16Material,
        de::Rectf(0,h/2,w,h));
    m_lineStripL.draw();
}

void GL_Renderer::draw2DLineStripR()
{
    int w = m_driver->getScreenWidth()/4;
    int h = m_driver->getScreenHeight()/8;
    int y1 = h + h/2;
    m_driver->getScreenRenderer()->draw2DLine(0,y1,w,y1,
        0xFFFFFFFF, 0xFFFFFFFF);

    auto s = App::instance()->getSampleCollector();

    const auto& vR = s->getR();

    auto n = vR.size();
    auto & m = m_lineStripR;
    m.PrimType = de::gpu::PrimitiveType::LineStrip;
    m.Vertices.clear();
    m.Vertices.reserve(n);
    m.Indices.clear();

    const float dx = 1.0f / float(n-1);
    const float dy = 0.5f;

    for (size_t i = 0; i < n; ++i)
    {
        const float s = vR[i];
        const float x = dx * i;
        const float y = dy * s;
        const float z = 0.0f;
        const float t = std::clamp((0.5f * s) + 0.5f, 0.f, 1.f);
        m.Vertices.push_back( GL_Mesh16_Vertex( x, y, z, t ) );
    }

    m.upload( true );


    m_mesh16Shader2D.setMaterial(m_mesh16Material,
        de::Rectf(0,y1,w,h));
    m_lineStripR.draw();

}


void GL_Renderer::draw2DLineStripFft()
{
    int w = m_driver->getScreenWidth();
    int h = m_driver->getScreenHeight();

    auto s = App::instance()->getSampleCollector();
    const auto& v = s->getAccumVecOut(); // fftOut

    auto n = v.size();
    if ( n < 2 ) { DE_ERROR("No n") return; }

    //#############################
    // Create Vertices:
    //#############################

    auto dBmin = -120.0f;
    auto dBmax = 60.0f;

    const float dx = float(w) / float ( n - 1 ); // / (sampleRate_over_fftSize * colCount);
    const float dy = float(h) * 0.125f;

    auto & m = m_lineStripFft;

    m.PrimType = de::gpu::PrimitiveType::LineStrip;
    m.Vertices.clear();
    m.Vertices.reserve(n);
    m.Indices.clear();

    float dBrange = dBmax - dBmin;
    if ( dBrange < 1.0f ) dBrange = 1.0f;
    float dBrangeInv = 1.0f / dBrange;

    for (size_t i = 0; i < n; ++i)
    {
        float dB = v[ i ];
        float t = (dB - dBmin) * dBrangeInv;
        float x = dx * i; // m.XMap[ i ]
        float y = float(h) - dy * t;
        float z = 0.0f;
        m.Vertices.emplace_back( x,y,z,t );
    }

    // m_mesh16.uploadVertices();
    m.upload(true, false);

    m_mesh16Shader2D.setMaterial(m_mesh16Material, de::Rectf(0,0,1,1));
    m.draw();
}