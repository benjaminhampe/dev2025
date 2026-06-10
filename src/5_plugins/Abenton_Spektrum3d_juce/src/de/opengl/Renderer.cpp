#include "Renderer.h"
#include <de_opengl.h>
#include <vector>
#include <utility>

#include <de/video/fonts.h>
#include <de/video/skybox_Miramar.h>
#include <de/video/rainbow_1k_webp.h>
#include <de/video/juce_icon_webp.h>
#include <de/video/benni_webp.h>

//#include <juce_opengl/juce_opengl.h>

#define ASSERT_GL_CONTEXT()

// #define ASSERT_GL_CONTEXT() \
//     jassert (juce::OpenGLHelpers::isContextActive())

// ===========================================================================
Renderer::Renderer()
// ===========================================================================
    : m_driver{ nullptr }
    , m_texBenniIcon{ nullptr }
    , m_texJuceIcon{ nullptr }
    , m_bFirstMouse{ false }
    , m_bCameraFreeLook{ false }
    , m_bMouseLeftPressed{ false }
    , m_bMouseRightPressed{ false }
    , m_bMouseMiddlePressed{ false }
    , m_bReserved1{ false }
    , m_bRenderingEnabled{ true }
    , m_bShowPerfOverlay{ true }
    , m_bRenderFftMatrix3D{ true }
    , m_mouseX{ 0 }
    , m_mouseY{ 0 }
    , m_lastMouseX{ 0 }
    , m_lastMouseY{ 0 }
    , m_mouseMoveX{ 0 }
    , m_mouseMoveY{ 0 }
    , m_fpsTimerId{ 0 }
    , m_matrix_fft_cols{ 0 }
    , m_matrix_fft_rows{ 0 }
    , m_matrix_fft_xmode{ 1 } // 0 = linear, 1 = log10
{
    dbAddFontFamily("Awesome",de::video::fontawesome463_ttf,de::video::fontawesome463_ttfSize);
    dbAddFontFamily("NotoSans",de::video::NotoSansRegular_ttf,de::video::NotoSansRegular_ttfSize);
    dbAddFontFamily("Postamt",de::video::Postamt_ttf,de::video::Postamt_ttfSize);
}

Renderer::~Renderer()
{
    uninitGL();
}

void Renderer::dsp_push(const de::TAlignedVector<float>& sum)
{
    // DE_BENNI("Renderer.dsp_push(",sum.size(),")")
    m_collector.dsp_push(sum);
}


void Renderer::initializeGL()
{
    ASSERT_GL_CONTEXT();

#if 0
    m_test.initializeGL();
#else

    if (!m_driver)
    {
        DE_BENNI("Create driver:")
        m_driver = de::gpu::createVideoDriver(1024,768,0);
    }

    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }

    m_mesh16Shader2D.setDriver( m_driver );
    m_mesh16Shader3D.setDriver( m_driver );



    de::Image img;
    dbLoadImage(img,rainbow_1k_webp,710,"rainbow_1k.webp");
    m_mesh16Material.tex0 = m_driver->createTexture2D("rainbow.webp",img);

    dbLoadImage(img,de::video::benni_webp,de::video::benni_webpSize,"benni_webp");
    m_texBenniIcon = m_driver->createTexture2D("benni_webp",img);

    dbLoadImage(img,de::video::juce_icon_webp,de::video::juce_icon_webpSize,"juce_icon_webp");
    m_texJuceIcon = m_driver->createTexture2D("juce_icon_webp",img);

    dbRandomize();

    de::Image nx;
    de::Image pz;
    de::Image px;
    de::Image nz;
    de::Image py;
    de::Image ny;

    dbLoadImage(nx, de::video::skybox::Miramar::nx_webp,
                    de::video::skybox::Miramar::nx_webpSize,
                    "Abenton/skybox_Miramar_webp/nx.webp");
    dbLoadImage(pz, de::video::skybox::Miramar::pz_webp,
                    de::video::skybox::Miramar::pz_webpSize,
                    "Abenton/skybox_Miramar_webp/pz.webp");
    dbLoadImage(px, de::video::skybox::Miramar::px_webp,
                    de::video::skybox::Miramar::px_webpSize,
                    "Abenton/skybox_Miramar_webp/px.webp");
    dbLoadImage(nz, de::video::skybox::Miramar::nz_webp,
                    de::video::skybox::Miramar::nz_webpSize,
                    "Abenton/skybox_Miramar_webp/nz.webp");
    dbLoadImage(py, de::video::skybox::Miramar::py_webp,
                    de::video::skybox::Miramar::py_webpSize,
                    "Abenton/skybox_Miramar_webp/py.webp");
    dbLoadImage(ny, de::video::skybox::Miramar::ny_webp,
                    de::video::skybox::Miramar::ny_webpSize,
                    "Abenton/skybox_Miramar_webp/ny.webp");

    m_driver->getSkyboxRenderer()->load(&nx,&px,&ny,&py,&nz,&pz);
#endif
}

void Renderer::uninitGL()
{
    ASSERT_GL_CONTEXT();
#if 0
    m_test.uninitGL();
#else
    if (m_driver)
    {
        DE_BENNI("Delete driver.")
        delete m_driver;
        m_driver = nullptr;
    }
#endif
}

void Renderer::resizeGL(int w, int h)
{
    ASSERT_GL_CONTEXT();
#if 0
    m_test.resizeGL(w,h);
#else
    if (m_driver)
    {
        //DE_BENNI("resizeGL(",w,",",h,")")
        m_driver->resize(w,h);
    }
#endif
}

void Renderer::paintGL()
{
    ASSERT_GL_CONTEXT();

    // juce::OpenGLHelpers().clear();
#if 0
    m_test.paintGL();
#else
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }

    // DE_BENNI("paintGL")

    m_driver->beginRender(glm::vec4(0.01,0.01,0.01,1.0));

    const int w = m_driver->getRenderWidth();
    const int h = m_driver->getRenderHeight();

    auto camera = m_driver->getCamera();
    if (camera)
    {
        camera->setScreenSize(w,h);
        camera->update();
    }

    m_driver->getSkyboxRenderer()->render();

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

    // draw2DLineStripL();
    // draw2DLineStripR();

    // draw2DLineStripFft();

    if (m_bShowPerfOverlay)
    {
        m_driver->draw2DPerfOverlay();
        draw2DFftOverlay();

        int tw = 128; // m_texJuceIcon->w();
        int th = 128; // m_texJuceIcon->h();

        m_driver->getScreenRenderer()->
            draw2DCircle(de::Recti(10,h-11-th,tw,th), 0xFFFFFFFF, m_texBenniIcon, 64);

        m_driver->getScreenRenderer()->
            draw2DRect(de::Recti(w-11-tw,h-11-th,tw,th), 0xFFFFFFFF, m_texJuceIcon);

        de::Font font("Postamt",32);

        std::wstring s1 = L"Spektrum 3D (c) 2026 by Abenton";
        std::wstring s2 = L"Made with JUCE 7.0.5";

        m_driver->draw2DText(
            20 + tw,
            h - 11 - th/2,
            s1,
            0xFFFFFFFF,
            de::Align::LeftMiddle,
            font);

        m_driver->draw2DText(
            w - 21 - tw,
            h - 11 - th/2,
            s2,
            0xFFFFFFFF,
            de::Align::RightMiddle,
            font);
    }


    m_driver->endRender();
#endif
}


void Renderer::draw2DFftOverlay()
{
    if (!m_driver) return;
    const int w = m_driver->getScreenWidth();
    const int h = m_driver->getScreenHeight();
    const int p = 10;

    uint32_t bgColor = dbRGBA(0,0,0,200);
    de::Align a = de::Align::TopRight;
    de::Font5x8 font5(6,6,0,0,1,1);
    de::Font5x8 font4(4,4,0,0,1,1);
    de::Font5x8 font3(3,3,0,0,1,1);

    const auto& p1 = m_collector;
    //auto p2 = App::instance()->getEndPoint();

    auto s0 = dbStr("SampleRate = ",p1.m_sampleRate, " Hz");
    auto s1 = dbStr("BlockSize = ",p1.m_blockSize);
    auto s2 = dbStr("FFT-Size = ",p1.m_fftSize);
    auto s3 = dbStr("FFT-Window = ",de::audio::WindowFunction::getString(p1.windowFunc()));
    auto s4 = dbStr("Matrix3D.Cols = ",p1.m_cols);
    auto s5 = dbStr("Matrix3D.Rows = ",p1.m_rows);

    int ln = font4.getTextSize("W").height + p;
    int x = w - 1 - p;
    int y = h - 1 - 10*ln;
    m_driver->draw2DText( x,y, s0, dbRGBA(255,255,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s1, dbRGBA(255,200,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s2, dbRGBA(255,155,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s3, dbRGBA(255,100,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s4, dbRGBA(255, 55,100), a, font4, bgColor, 1 ); y += ln;
    m_driver->draw2DText( x,y, s5, dbRGBA(155,100,100), a, font4, bgColor, 1 );
    // y += ln;
}

void buildTerrainStrip(uint32_t M, uint32_t N, Mesh16 & mesh)
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

void Renderer::draw3DAccumFftMatrix()
{
    if (!m_bRenderFftMatrix3D)
    {
        return;
    }

    const auto& d = m_collector.getAccumMat();

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
                float f = de::bin2freq(col,sampleRate,fftSize);
                float x = siz3d.x * de::freq2log(f,fMin,fMax,sampleRate);
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
                float t = de::clampf((dB - dBmin) * dBrangeInv,0.f,1.f);
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
                float t = de::clampf((dB - dBmin) * dBrangeInv,0.f,1.f);
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

    auto material = m_mesh16Material;
    material.alpha = 1.0f; // float(1.0/200.0);
    m_mesh16Shader3D.setMaterial(material, T);
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

/*
void Renderer::draw2DLineStripL()
{
    int w = m_driver->getScreenWidth()/4;
    int h = m_driver->getScreenHeight()/8;
    int y1 = h/2;

    m_driver->getScreenRenderer()->draw2DLine(0,y1,w,y1,
        0xFFFFFFFF, 0xFFFFFFFF);

    const auto& vL = m_collector.getL();

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

void Renderer::draw2DLineStripR()
{
    int w = m_driver->getScreenWidth()/4;
    int h = m_driver->getScreenHeight()/8;
    int y1 = h + h/2;
    m_driver->getScreenRenderer()->draw2DLine(0,y1,w,y1,
        0xFFFFFFFF, 0xFFFFFFFF);

    const auto& vR = m_collector.getR();

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
        m.Vertices.push_back( Mesh16_Vertex( x, y, z, t ) );
    }

    m.upload( true );


    m_mesh16Shader2D.setMaterial(m_mesh16Material,
        de::Rectf(0,y1,w,h));
    m_lineStripR.draw();

}
*/

void Renderer::draw2DLineStripFft()
{
    int w = m_driver->getScreenWidth();
    int h = m_driver->getScreenHeight();

    const auto& v = m_collector.getAccumVecOut(); // fftOut

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
    Mesh16_Material material2 = m_mesh16Material;
    material2.alpha = 0.75f;
    m_mesh16Shader2D.setMaterial(material2, de::Rectf(0,0,1,1));
    m_triStripFft.draw();

    m_mesh16Shader2D.setMaterial(m_mesh16Material, de::Rectf(0,0,1,1));
    m_lineStripFft.draw();
}


















// BORING:

/*
void Renderer::draw3DLineStripL()
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

void Renderer::draw3DLineStripR()
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

*/

    // Test ImageWriters 2026:
    // de::Image img;
    // dbLoadImage(img,rainbow_1k_webp,710,"rainbow_1k.webp");
    // dbSaveImage(img,"rainbow_1k.pam");
    // dbSaveImage(img,"rainbow_1k.ppm");
    // dbSaveImage(img,"rainbow_1k.png");
    // dbSaveImage(img,"rainbow_1k.webp");
    // dbSaveImage(img,"rainbow_1k.dds");
    // dbSaveImage(img,"rainbow_1k.tga");
    // dbSaveImage(img,"rainbow_1k.exr");
    // dbSaveImage(img,"rainbow_1k.xpm");
    // dbSaveImage(img,"rainbow_1k.gif");
    // dbSaveImage(img,"rainbow_1k.jpg");
    // dbSaveImage(img,"rainbow_1k.jpeg");
    // dbSaveImage(img,"rainbow_1k.ico");
    // dbSaveImage(img,"rainbow_1k.bmp");
    // dbSaveImage(img,"rainbow_1k.raw");
    // dbSaveImage(img,"rainbow_1k.rgb");
    // dbSaveImage(img,"rainbow_1k.rgba");
    // dbSaveImage(img,"rainbow_1k.sgi");
    // dbSaveImage(img,"rainbow_1k.tif");
    // dbSaveImage(img,"rainbow_1k.wal");

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
