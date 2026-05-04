#include "GL_Renderer.h"
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>
#include <App.h>
#include "rainbow_1k_webp.h"
#include <vector>
#include <utility>
#include <de/audio/fft/approx_math.h>

// ===========================================================================
struct Axis
// ===========================================================================
{
    uint32_t m_sampleRate;
    uint32_t m_fftSize;
    uint32_t m_scaleMode; // 0=linear, 1=logarithm
    float m_scaleFactor;
    float m_fMin;
    float m_fMax;

    Axis()
        : m_sampleRate{ 48000 }
        , m_fftSize{ 2048 }
        , m_scaleMode{ 1 } // 0=linear, 1=logarithm
        , m_scaleFactor{ 1.0f }
        , m_fMin{ 0 }
        , m_fMax{ 256 }
    {

    }

    double x( double freq )
    {
        if ( m_scaleMode == 1 )
            return de::audio::math::freq2log(freq, m_fMin, m_fMax, m_sampleRate );
        else
            return de::audio::math::freq2lin(freq, m_fMin, m_fMax, m_sampleRate );
    }
/*
    // / (sampleRate_over_fftSize * colCount);
    const float sampleRate = 48000.0f;
    const float fftSize = cols;

    const float sampleRate_over_fftSize = sampleRate / fftSize;
    const float f = sampleRate_over_fftSize; //  / log10f( float(cols) );

    for ( size_t col = 0; col < cols; col++ )
    {
    //  - 1.5f -1 = shift by 10^-1
        m_matrix_fft_xmap[ col ] = f * log10f( float(col+1) );
    }
*/
};

// ===========================================================================
GL_Renderer::GL_Renderer()
// ===========================================================================
    : m_driver{ nullptr }
    , m_showFftMatrix3D{ true }
    , m_matrix_fft_cols{ 0 }
    , m_matrix_fft_rows{ 0 }
    , m_matrix_fft_xmode{ 1 } // 0 = linear, 1 = log10
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

    // auto d = glm::vec3(4000,500,2000);
    // m_matrix_fft.init(d, V3(-0.5f * d.x,0,0));

    de::Image sky;
    dbLoadImage(sky,"../../media/Abenton/skybox.png");

    int a = sky.w() / 4;
    int b = sky.h() / 3;
    de::Image nx = sky.copy(de::Recti(0*a,b,a,b));
    de::Image pz = sky.copy(de::Recti(1*a,b,a,b));
    de::Image px = sky.copy(de::Recti(2*a,b,a,b));
    de::Image nz = sky.copy(de::Recti(3*a,b,a,b));
    de::Image py = sky.copy(de::Recti(1*a,0*b,a,b));
    de::Image ny = sky.copy(de::Recti(1*a,2*b,a,b));
    m_driver->getSkyboxRenderer()->load(&nx,&px,&ny,&py,&nz,&pz);


}

void GL_Renderer::paintGL()
{
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }

    auto s = App::instance()->getSampleCollector();
    const auto& d = s->getL();
    if (d.size() < 8) { return; }

    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);          // Default depth comparison
    // glDepthMask(GL_TRUE);          // Allow depth writes
    // glClearDepth(1.0);             // Depth buffer clear value
    m_driver->setDepth(de::gpu::Depth());

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);           // Cull back-facing triangles
    // glFrontFace(GL_CCW);           // Counter-clockwise = front
    m_driver->setCulling(de::gpu::Culling());

    // m_lineShader.resetModelMat();
    // m_lineShader.draw( bks_lines );
    // m_lineShader.draw( bks_x_lines );
    // m_lineShader.draw( bks_y_lines );
    // m_lineShader.draw( bks_z_lines );

    draw3DAccumFftMatrix();
    // draw3DLineStripL();
    // draw3DLineStripR();

    draw2DLineStripL();
    draw2DLineStripR();

    draw2DLineStripFft();
}

void buildTerrainStrip(uint32_t M, uint32_t N, GL_Mesh16 & mesh)
{
    mesh.Indices.clear();
    mesh.Indices.reserve(M * N * 2);

    for (uint32_t y = 0; y < N - 1; ++y)
    {
        uint32_t r0 = y * M;
        uint32_t r1 = (y+1) * M;

        mesh.Indices.emplace_back(r1);

        for (uint32_t x = 0; x < M; ++x) // left -> right
        {
            mesh.addIndex(r1 + x);
            mesh.addIndex(r0 + x);
        }

        mesh.Indices.emplace_back(mesh.Indices.back());
    }
}

void GL_Renderer::draw3DAccumFftMatrix()
{
    if (!m_showFftMatrix3D)
    {
        return;
    }

    auto s = App::instance()->getSampleCollector();
    const auto& d = s->getAccumMat();

    auto & m = m_matrix_fft;

    auto siz3d = glm::vec3(3500,500,2000);
    auto pos3d = glm::vec3{-0.5f * siz3d.x,0,0 };

    // In log10 Mode shift mesh left. (or camera right)
    if (m_matrix_fft_xmode==1)
    {
        pos3d.x -= 0.06f * siz3d.x;
    }

    auto cols = d.columnCount();
    auto rows = d.rowCount();
    if ( rows < 2 ) { DE_ERROR("No rows") return; }
    if ( cols < 2 ) { DE_ERROR("No cols") return; }

    //#############################
    // Create Indices:
    //#############################

    bool bNeedIndexUpload = false;
    if ((cols != m_matrix_fft_cols) || (rows != m_matrix_fft_rows))
    {
        m_matrix_fft_cols = cols;
        m_matrix_fft_rows = rows;

        // Matrix16 FFT indices:
        m_matrix_fft.Indices.clear();

// OLD: Easy but Expensive Triangles
/*
        m_matrix_fft.Indices.reserve( (cols) * (rows) * 6);

        for ( size_t j = 0; j < rows-1; j++ )
        {
            for ( size_t i = 0; i < cols-1; i++ )
            {
                const uint32_t A = (cols * j) + i;       // A - 0
                const uint32_t B = (cols * (j+1)) + i;   // B - 1
                const uint32_t C = (cols * (j+1)) + i+1; // C - 3
                const uint32_t D = (cols * j) + i+1;     // D - 2
                m_matrix_fft.addIndexedQuad( A,B,C,D );
            }
        }
*/
        // NEW: Faster TriangleStrips
        buildTerrainStrip(cols,rows,m_matrix_fft);
        bNeedIndexUpload = true;

        // X-axis is scaled logarithmicly.
        if (m_matrix_fft_xmap.size() != cols)
        {
            m_matrix_fft_xmap.resize(cols);
            const float sampleRate = 48000.0f;
            const float fftSize = 2048;
            const float factor = fftSize / cols;
            const float fNyquist = sampleRate * 0.5f;
            const float fMin = 100.0f;
            const float fMax = fNyquist / factor;
            for ( size_t col = 0; col < cols; col++ )
            {
                float f = de::audio::math::bin2freq(col,sampleRate,fftSize);
                float x = siz3d.x * de::audio::math::freq2log(f,fMin,fMax,sampleRate);
                m_matrix_fft_xmap[ col ] = x;
            }
        }
    }

    //#############################
    // Create Vertices:
    //#############################

    float dBmin = -120.0f;
    float dBmax = 120.0f;
    float dBrange = dBmax - dBmin;
    float dBrangeInv = 1.0f / dBrange;

    //const float dx = m_size.x / float ( cols - 1 );
    const float dx = siz3d.x / m_matrix_fft_xmap.back(); // / (sampleRate_over_fftSize * colCount);
    const float dy = siz3d.y;
    const float dz = siz3d.z / float ( rows - 1 );

    m_matrix_fft.PrimType = de::gpu::PrimitiveType::TriangleStrip;
    m_matrix_fft.Vertices.clear();
    m_matrix_fft.Vertices.reserve(rows * cols);

    // Matrix Top
    if ( m_matrix_fft_xmode == 1 ) // X-axis is scaled logarithmicly.
    {
        for ( size_t row = 0; row < rows; ++row )
        {
            const float* __restrict__ pRow = d.m_rows[row];
            for ( size_t col = 0; col < cols; ++col )
            {
                float dB = *pRow++; // d.getPixel( col, row );  // The row data
                float t = de::audio::math::clampf((dB - dBmin) * dBrangeInv,0.f,1.f);
                float x = dx * m_matrix_fft_xmap[ col ];
                float y = dy * t;
                float z = dz * row;
                m_matrix_fft.Vertices.emplace_back( x,y,z,t );
            }
        }
    }
    // Matrix Top
    else // if ( m_matrix_fft_xmode == 0 ) // X-axis is linear.
    {
        for ( size_t row = 0; row < rows; ++row )
        {
            const float* __restrict__ pRow = d.m_rows[row];
            for ( size_t col = 0; col < cols; ++col )
            {
                float dB = *pRow++; // d.getPixel( col, row );  // The row data
                float t = de::audio::math::clampf((dB - dBmin) * dBrangeInv,0.f,1.f);
                float x = dx * col;
                float y = dy * t;
                float z = dz * row;
                m_matrix_fft.Vertices.emplace_back( x,y,z,t );
            }
        }
    }

    m_matrix_fft.upload(true, bNeedIndexUpload);

    //#############################
    // Create front:
    //#############################
    m_matrix_fft_front.PrimType = de::gpu::PrimitiveType::TriangleStrip;
    m_matrix_fft_front.Vertices.clear();
    m_matrix_fft_front.Vertices.reserve(cols*2);
    m_matrix_fft_front.Indices.clear();

    auto & vertices = m_matrix_fft.Vertices; // Use Matrix3D vertices to build front mesh
    for ( size_t col = 0; col < cols; ++col )
    {
        auto a = vertices[col];
        auto b = a;
        b.set_y(0.0f); //
        //b.set_y(-b.y()); // 0.0f
        m_matrix_fft_front.Vertices.emplace_back( std::move(a) );
        m_matrix_fft_front.Vertices.emplace_back( std::move(b) );
    }

    m_matrix_fft_front.upload(true, false);

    //#############################
    // Draw front + top (matrix)
    //#############################

    auto T = glm::translate(glm::mat4(1.0f), pos3d);
    m_mesh16Shader3D.setMaterial(m_mesh16Material, T);
    m_matrix_fft_front.draw();
    m_matrix_fft.draw();

    //#############################
    // Draw x-axis
    //#############################
/*
    m_matrix_fft_axis_x.PrimType = de::gpu::PrimitiveType::Lines;
    m_matrix_fft_axis_x.Indices.clear();
    m_matrix_fft_axis_x.Vertices.clear();
    m_matrix_fft_axis_x.Vertices.reserve(cols * 2);

    for ( size_t col = 0; col < cols; ++col )
    {
        auto a = m_matrix_fft.Vertices[col];
        auto b = a;
        a.set_y(0.0f);
        b.set_y(-100.0f);
        b.set_z(-100.0f);
        m_matrix_fft_axis_x.Vertices.emplace_back( std::move(a) );
        m_matrix_fft_axis_x.Vertices.emplace_back( std::move(b) );
    }

    m_matrix_fft_axis_x.upload(true, false);
    m_mesh16Shader3D.setMaterial(m_mesh16Material, T);
    m_matrix_fft_axis_x.draw();
*/
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
        m.Vertices.emplace_back( x, y, z, t );
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

    const float dBmin = -120.0f;
    const float dBmax = 120.0f;
    const float dBrange = dBmax - dBmin;
    const float dBrangeInv = 1.0f / dBrange;
    const float dx = float(w) / float ( n - 1 ); // / (sampleRate_over_fftSize * colCount);
    const float dy = float(h) * 0.125f;

    //##############################################
    // LineStrip 2D FFT:
    //##############################################
    m_lineStripFft.PrimType = de::gpu::PrimitiveType::LineStrip;
    m_lineStripFft.Vertices.clear();
    m_lineStripFft.Vertices.reserve(n);
    m_lineStripFft.Indices.clear();
    for (size_t i = 0; i < n; ++i)
    {
        float dB = v[ i ];
        float t = (dB - dBmin) * dBrangeInv;
        float x = dx * i; // m.XMap[ i ]
        float y = float(h) - dy * t;
        float z = 0.0f;
        m_lineStripFft.Vertices.emplace_back( x,y,z,t );
    }
    m_lineStripFft.upload(true, false);

    //#############################
    // TriangleStrip 2D FFT:
    //#############################
    m_triStripFft.PrimType = de::gpu::PrimitiveType::TriangleStrip;
    m_triStripFft.Vertices.clear();
    m_triStripFft.Vertices.reserve(n * 2);
    m_triStripFft.Indices.clear();
    for (size_t i = 0; i < n; ++i)
    {
        auto a = m_lineStripFft.Vertices[i];
        auto b = a;
        b.set_y(h);
        m_triStripFft.Vertices.emplace_back( std::move(a) );
        m_triStripFft.Vertices.emplace_back( std::move(b) );
    }
    m_triStripFft.upload(true, false);

    //#############################
    // Draw:
    //#############################
    GL_Mesh16_Material material2 = m_mesh16Material;
    material2.alpha = 0.75f;
    m_mesh16Shader2D.setMaterial(material2, de::Rectf(0,0,1,1));
    m_triStripFft.draw();

    m_mesh16Shader2D.setMaterial(m_mesh16Material, de::Rectf(0,0,1,1));
    m_lineStripFft.draw();
}


















// BORING:


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