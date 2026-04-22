#include "GL_Spectrum3D.h"
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>
#include <App.h>
#include "rainbow_1k_webp.h"

// ===========================================================================
GL_Spectrum3D::GL_Spectrum3D()
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

GL_Spectrum3D::~GL_Spectrum3D()
{}

void GL_Spectrum3D::initializeGL(de::gpu::VideoDriver* driver)
{
    m_driver = driver;
    m_mesh16Shader.setDriver( driver );

    de::Image img;
    dbLoadImage(img,rainbow_1k_webp,710,"rainbow_1k.webp");

    m_mesh16Material.tex0 = m_driver->createTexture2D("rainbow.webp",img);

    // Init meshbuffers:
    dbRandomize();

    auto d = glm::vec3(4000,250,2000);

    m_matrix_fft.init(d, V3(-0.5f * d.x,0,0));
}

void GL_Spectrum3D::paintGL()
{
    // m_lineShader.resetModelMat();
    // m_lineShader.draw( bks_lines );
    // m_lineShader.draw( bks_x_lines );
    // m_lineShader.draw( bks_y_lines );
    // m_lineShader.draw( bks_z_lines );

    if (m_showFftMatrix3D)
    {
        const auto& m = App::instance()->getAudioCentral().getFftMatrix();
        m_matrix_fft.draw(m_mesh16Shader, m_mesh16Material, m);
    }
}

