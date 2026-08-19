#include "GL_Spectrum3D.h"
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>

// ===========================================================================
GL_Spectrum3D::GL_Spectrum3D()
    // ===========================================================================
    : m_driver(nullptr)
{
    // m_shiftMatrixL.resize( 1024, 64 );
    m_d = glm::vec3(1000,250,2000);

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

}

GL_Spectrum3D::~GL_Spectrum3D()
{}

void
//GL_Spectrum3D::initializeGL(de::gpu::TexManager* texManager)
GL_Spectrum3D::initializeGL(de::gpu::VideoDriver* driver)
{
    // initializeOpenGLFunctions();
    m_driver = driver;
    m_lineShader.setDriver( driver );
    m_lineShader.ensureShader();
    m_meshShader.setDriver( driver );

    de::Image img;
    dbLoadImage(img,"../../media/rainbow_1k.webp");
    dbSaveImage(img,"VHS_rainbow_1k_1.png");
    dbSaveImage(img,"VHS_rainbow_1k_1.webp");

    de::LinearColorGradient lcg;
    dbImageToLinearColorGradient(lcg,img,false);
    dbSaveLinearColorGradient(lcg,"VHS_rainbow_1k_1.xml");
    de::Image img2 = dbImageFromLinearColorGradient(lcg,img.w(),1,false);
    dbSaveImage(img2,"VHS_rainbow_1k_2.png");
    dbSaveImage(img2,"VHS_rainbow_1k_2.webp");

    m_meshMaterial.tex0 = m_driver->getTexture2D("../../media/rainbow_1k.webp");

    // Init meshbuffers:
    dbRandomize();

    m_d = glm::vec3(2000,250,2000);
    //glm::vec3 m_d; // V3(1000,100,4000);
    glm::vec3 m_e = m_d;
    m_e.z /= 2;
    glm::vec3 m_a = m_d; // V3(1000,100,8000);
    m_a.x *= 2.0f;
    // Lines:
    createBksLines(bks_lines, m_d.y );
    createBksLinesX(bks_x_lines, m_d.y );
    createBksLinesY(bks_y_lines, m_d.y );
    createBksLinesZ(bks_z_lines, m_d.y );
    // createVLines( L_raw_front_lines, d, dbAudioLoopback_GetFront_L());
    // createVLines( R_raw_front_lines, d, dbAudioLoopback_GetFront_R());
    // createVLines( L_raw_matrix_lines, d, dbAudioLoopback_GetMatrix_L());
    // createVLines( R_raw_matrix_lines, d, dbAudioLoopback_GetMatrix_R());

    // Raw native pcm samples:

    float s = 100.0f;
    float dx = s + m_d.x;
    float dz = s + m_d.z;
    float a = -dx*2;
    float b = -dx;
    float c = 0.0;
    initPlot3D(0, raw, m_d, dbAudioLoopback_GetRawMatrix(), V3(a,0,0));
    initPlot3D(2, raw_fft, m_d, dbAudioLoopback_GetFftRawMatrix(), V3(a,0,dz));
    initPlot3D(0, vol, m_d, dbAudioLoopback_GetVolMatrix(), V3(b,0,0));
    initPlot3D(2, vol_fft, m_d, dbAudioLoopback_GetFftVolMatrix(), V3(b,0,dz));
    initPlot3D(0, accum, m_a, dbAudioLoopback_GetAccumMatrix(), V3(c,0,0));
    initPlot3D(2, accum_fft, m_a, dbAudioLoopback_GetFftAccumMatrix(), V3(c,0,dz));
/*

    // Raw:
#ifdef BENNI_USE_RAW_MATRIX3D
    a = -4*dx;
    b = -3*dx;
    initPlot3D(0, L_raw, m_d, dbAudioLoopback_GetRawMatrix_L(), V3(a,0,0));
    initPlot3D(0, R_raw, m_d, dbAudioLoopback_GetRawMatrix_R(), V3(b,0,0));
    initPlot3D(2, L_raw_fft, m_d, dbAudioLoopback_GetFftRawMatrix_L(), V3(a,0,c));
    initPlot3D(2, R_raw_fft, m_d, dbAudioLoopback_GetFftRawMatrix_R(), V3(b,0,c));
#endif
    // Volume:
#ifdef BENNI_USE_VOL_MATRIX3D
    a = -2*dx;
    b = -1*dx;
    initPlot3D(0, L_vol, m_d, dbAudioLoopback_GetVolMatrix_L(), V3(a,0,0));
    initPlot3D(0, R_vol, m_d, dbAudioLoopback_GetVolMatrix_R(), V3(b,0,0));
    initPlot3D(2, L_vol_fft, m_d, dbAudioLoopback_GetFftVolMatrix_L(), V3(a,0,c));
    initPlot3D(2, R_vol_fft, m_d, dbAudioLoopback_GetFftVolMatrix_R(), V3(b,0,c));
#endif

    // Accum:
    a = 0.0;
    b = m_a.x + s;


    initPlot3D(0, R_accum, m_a, dbAudioLoopback_GetAccumMatrix_R(), V3(b,0,0));
    initPlot3D(0, R_accum, m_a, dbAudioLoopback_GetAccumMatrix_R(), V3(b,0,0));

    initPlot3D(2, R_accum_fft, m_a, dbAudioLoopback_GetFftAccumMatrix_R(), V3(b,0,c));

    // Blackman:
    a = 0*dx;
    b = 1*dx;
    initPlot3D(0, L_blackman, m_d, dbAudioLoopback_Data().m_blak_matrix_L, V3(a,0,0));
    initPlot3D(0, R_blackman, m_d, dbAudioLoopback_Data().m_blak_matrix_R, V3(b,0,0));
    initPlot3D(2, L_blackman_fft, m_d, dbAudioLoopback_Data().m_blak_fft_matrix_L, V3(a,0,dz));
    initPlot3D(2, R_blackman_fft, m_d, dbAudioLoopback_Data().m_blak_fft_matrix_R, V3(b,0,dz));

    // Hamming:
    a = 2*dx;s
    b = 3*dx;
    initPlot3D(0, L_hamm, m_d, dbAudioLoopback_Data().m_hamm_matrix_L, V3(a,0,0));
    initPlot3D(0, R_hamm, m_d, dbAudioLoopback_Data().m_hamm_matrix_R, V3(b,0,0));
    initPlot3D(2, L_hamm_fft, m_d, dbAudioLoopback_Data().m_hamm_fft_matrix_L, V3(a,0,dz));
    initPlot3D(2, R_hamm_fft, m_d, dbAudioLoopback_Data().m_hamm_fft_matrix_R, V3(b,0,dz));

    // Hann:
    a = 4*dx;
    b = 5*dx;
    initPlot3D(0, L_hann, m_d, dbAudioLoopback_Data().m_hann_matrix_L, V3(a,0,0));
    initPlot3D(0, R_hann, m_d, dbAudioLoopback_Data().m_hann_matrix_R, V3(b,0,0));
    initPlot3D(2, L_hann_fft, m_d, dbAudioLoopback_Data().m_hann_fft_matrix_L, V3(a,0,dz));
    initPlot3D(2, R_hann_fft, m_d, dbAudioLoopback_Data().m_hann_fft_matrix_R, V3(b,0,dz));
*/
}
de::BBox3f GL_Spectrum3D::recomputeBoundingBox(V3 const & d, DE_AlignedFloatShiftMatrix const & table)
{
    auto y = table.getMinMax();
    de::BBox3f bb;
    bb.m_min.x = 0.0f;
    bb.m_min.y = y.m_min;
    bb.m_min.z = 0.0f;
    bb.m_max.x = d.x;
    bb.m_max.y = y.m_max;
    bb.m_max.z = d.z;
    return bb;
}

void GL_Spectrum3D::initPlot3D(int typ, Plot3D & plot, V3 const & d,
                               DE_AlignedFloatShiftMatrix const & table,
                               V3 const & pos)
{
    plot.size = d;
    plot.pos = pos;
    plot.typ = typ;
    plot.bbox = recomputeBoundingBox( plot.size, table );
    plot.cols = table.columnCount();
    plot.rows = table.rowCount();

    plot.mesh16.Material.modelMat = glm::translate(glm::mat4(1.0f), pos);
    plot.mesh16.Material.tex0 = m_meshMaterial.tex0;

    if (plot.typ == 0)
    {
        //createWavMatrix( plot.mesh, plot.size, table);
        GL_Mesh16::createWavMatrix(plot.mesh16, plot.size, table, m_left);
    }
    else if (plot.typ == 1)
    {
        //createWavMatrix01( plot.mesh, plot.size, table);
        GL_Mesh16::createWavMatrix01(plot.mesh16, plot.size, table, m_left);
    }
    else if (plot.typ == 2)
    {
        //createFftMatrix( plot.mesh, plot.size, table);
        GL_Mesh16::createFftMatrix(plot.mesh16, plot.size, table, m_left);
    }
}

void GL_Spectrum3D::drawPlot3D(Plot3D & plot, DE_AlignedFloatShiftMatrix const & table)
{
    if (plot.typ == 0)
    {
        //createWavMatrix( plot.mesh, plot.size, table);
        GL_Mesh16::createWavMatrix(plot.mesh16, plot.size, table, m_left);
    }
    else if (plot.typ == 1)
    {
        //createWavMatrix01( plot.mesh, plot.size, table);
        GL_Mesh16::createWavMatrix01(plot.mesh16, plot.size, table, m_left);
    }
    else if (plot.typ == 2)
    {
        //createFftMatrix( plot.mesh, plot.size, table);
        GL_Mesh16::createFftMatrix(plot.mesh16, plot.size, table, m_left);
    }

    plot.bbox = recomputeBoundingBox( plot.size, table );
    plot.cols = table.columnCount();
    plot.rows = table.rowCount();

    // m_lineShader.setCamera( m_camera );
    // m_lineShader.setModelPos(plot.pos);
    // m_lineShader.draw(plot.mesh);

    m_meshShader.draw(plot.mesh16);
}

void
GL_Spectrum3D::paintGL()
{
    m_lineShader.resetModelMat();
    m_lineShader.draw( bks_lines );
    m_lineShader.draw( bks_x_lines );
    m_lineShader.draw( bks_y_lines );
    m_lineShader.draw( bks_z_lines );

    drawPlot3D( raw, dbAudioLoopback_GetRawMatrix());
    drawPlot3D( raw_fft, dbAudioLoopback_GetFftRawMatrix());
    drawPlot3D( vol, dbAudioLoopback_GetVolMatrix());
    drawPlot3D( vol_fft, dbAudioLoopback_GetFftVolMatrix());
    drawPlot3D( accum, dbAudioLoopback_GetAccumMatrix());
    drawPlot3D( accum_fft, dbAudioLoopback_GetFftAccumMatrix());

    /*
    // Raw:
    drawPlot3D( L_raw, dbAudioLoopback_GetRawMatrix_L());
    drawPlot3D( R_raw, dbAudioLoopback_GetRawMatrix_R());
    drawPlot3D( L_raw_fft, dbAudioLoopback_GetFftRawMatrix_L());
    drawPlot3D( R_raw_fft, dbAudioLoopback_GetFftRawMatrix_R());

    // Volume:
    drawPlot3D( L_vol, dbAudioLoopback_GetVolMatrix_L());
    drawPlot3D( R_vol, dbAudioLoopback_GetVolMatrix_R());
    drawPlot3D( L_vol_fft, dbAudioLoopback_GetFftVolMatrix_L());
    drawPlot3D( R_vol_fft, dbAudioLoopback_GetFftVolMatrix_R());

    // Accum:
    drawPlot3D( L_accum, dbAudioLoopback_GetAccumMatrix_L());
    drawPlot3D( R_accum, dbAudioLoopback_GetAccumMatrix_R());
    drawPlot3D( L_accum_fft, dbAudioLoopback_GetFftAccumMatrix_L());
    drawPlot3D( R_accum_fft, dbAudioLoopback_GetFftAccumMatrix_R());

    // Blackman:
    drawPlot3D( L_blackman, dbAudioLoopback_Data().m_blak_matrix_L);
    drawPlot3D( R_blackman, dbAudioLoopback_Data().m_blak_matrix_R);
    drawPlot3D( L_blackman_fft, dbAudioLoopback_Data().m_blak_fft_matrix_L);
    drawPlot3D( R_blackman_fft, dbAudioLoopback_Data().m_blak_fft_matrix_R);

    // Hamming:
    drawPlot3D( L_hamm, dbAudioLoopback_Data().m_hamm_matrix_L);
    drawPlot3D( R_hamm, dbAudioLoopback_Data().m_hamm_matrix_R);
    drawPlot3D( L_hamm_fft, dbAudioLoopback_Data().m_hamm_fft_matrix_L);
    drawPlot3D( R_hamm_fft, dbAudioLoopback_Data().m_hamm_fft_matrix_R);

    // Hann:
    drawPlot3D( L_hann, dbAudioLoopback_Data().m_hann_matrix_L);
    drawPlot3D( R_hann, dbAudioLoopback_Data().m_hann_matrix_R);
    drawPlot3D( L_hann_fft, dbAudioLoopback_Data().m_hann_fft_matrix_L);
    drawPlot3D( R_hann_fft, dbAudioLoopback_Data().m_hann_fft_matrix_R);
*/
}

void GL_Spectrum3D::createVLines( BenMeshBuffer & m, glm::vec3 const & d,
                                        DE_AlignedFloatVector const & pcm )
{
    const size_t n = pcm.size();
    const float* p = pcm.data();

    m.PrimType = de::gpu::PrimitiveType::Lines;
    m.Vertices.clear();
    m.Vertices.reserve( n*2 );
    m.Indices.clear();
    m.Indices.reserve( n*2 );

    const float dx = d.x / float(n - 1);
    const float dy = d.y * 0.5f;
    const auto c0 = m_wav_colorGradient.getColor32(0.5f);

    for (size_t i = 0; i < n; i++)
    {
        const float s = p[i];
        const float t = std::clamp((s * 0.5f) + 0.5f, 0.f, 1.f);

        const float x = (dx * i);
        const float y = (dy * s) + dy;
        const float z = 0.0f;

        const auto c = m_wav_colorGradient.getColor32(t);

        m.Vertices.emplace_back( x, y, z, c );
        m.Vertices.emplace_back( x, dy, z, c0 );
        m.Indices.emplace_back( 2*i );
        m.Indices.emplace_back( 2*i+1 );
    }

    m.upload( true );
}


void GL_Spectrum3D::createVLines(BenMeshBuffer & m, glm::vec3 const & d,
                                DE_AlignedFloatShiftMatrix const & table )
{
    auto rowCount = table.rowCount();
    auto colCount = table.columnCount();

    if ( rowCount < 1 || colCount < 1 )
    {
        return;
    }

    m.PrimType = de::gpu::PrimitiveType::Triangles;
    m.Vertices.clear();
    m.Indices.clear();
    m.Vertices.reserve( rowCount * colCount * 2 );
    m.Indices.reserve( rowCount * colCount * 2 );

    float dx = d.x / float(colCount-1);
    float dy = d.y * 0.5f;
    float dz = d.z / float(rowCount-1);
    uint32_t c0 = m_wav_colorGradient.getColor32( 0.5f );

    // Matrix Top
    uint32_t v0 = 0;
    for ( size_t j = 0; j < rowCount; ++j )
    {
        auto row = table.m_view[ j ];  // The row vector
        for ( size_t i = 0; i < colCount; ++i )
        {
            float s = row[i];
            float t = std::clamp( (0.5f*s) + 0.5f, 0.f, 1.f);
            float x = dx * i;
            float y = dy * s + dy;
            float z = dz * j;

            uint32_t c1 = m_wav_colorGradient.getColor32( t );
            //uint32_t c1 = de::RainbowColor::computeColor32( std::fabs( 1.0f - 0.5f * r ) );

            m.Vertices.push_back( BenVertex(x,dy,z,c0) );
            m.Vertices.push_back( BenVertex(x, y,z,c1) );
            m.Indices.push_back( v0 );
            m.Indices.push_back( v0+1 );

            v0 += 2;
        }
    }

    m.upload( true );
}
void GL_Spectrum3D::createLineBox(BenMeshBuffer & m, de::BBox3f const & bbox, uint32_t color )
{
    m.PrimType = de::gpu::PrimitiveType::Lines;
    m.Vertices.clear();
    m.Vertices.reserve( 8 );
    m.Indices.clear();
    m.Indices.reserve( 24 );

    const float x1 = bbox.m_min.x;
    const float y1 = bbox.m_min.y;
    const float z1 = bbox.m_min.z;
    const float x2 = bbox.m_max.x;
    const float y2 = bbox.m_max.y;
    const float z2 = bbox.m_max.z;

    m.Vertices.push_back( BenVertex( x1, y1, z1, color ) );
    m.Vertices.push_back( BenVertex( x1, y2, z1, color ) );
    m.Vertices.push_back( BenVertex( x2, y2, z1, color ) );
    m.Vertices.push_back( BenVertex( x2, y2, z1, color ) );
    m.Vertices.push_back( BenVertex( x1, y1, z2, color ) );
    m.Vertices.push_back( BenVertex( x1, y2, z2, color ) );
    m.Vertices.push_back( BenVertex( x2, y2, z2, color ) );
    m.Vertices.push_back( BenVertex( x2, y2, z2, color ) );

    uint32_t v0 = 0;
    m.addIndexedLine(v0 + 0, v0 + 1); // AB
    m.addIndexedLine(v0 + 1, v0 + 2); // BC
    m.addIndexedLine(v0 + 2, v0 + 3); // CD
    m.addIndexedLine(v0 + 3, v0 + 0); // DA

    m.addIndexedLine(v0 + 4, v0 + 5); // EF
    m.addIndexedLine(v0 + 5, v0 + 6); // FG
    m.addIndexedLine(v0 + 6, v0 + 7); // GH
    m.addIndexedLine(v0 + 7, v0 + 4); // HE

    m.addIndexedLine(v0 + 0, v0 + 4); // AE
    m.addIndexedLine(v0 + 1, v0 + 5); // BF
    m.addIndexedLine(v0 + 2, v0 + 6); // CG
    m.addIndexedLine(v0 + 3, v0 + 7); // DH

    m.upload( true );
}

void GL_Spectrum3D::createLineBox(BenMeshBuffer & m, de::BBox3f const & bbox )
{
    m.PrimType = de::gpu::PrimitiveType::Lines;
    m.Vertices.clear();
    m.Vertices.reserve( 8 );
    m.Indices.clear();
    m.Indices.reserve( 24 );

    const float x1 = bbox.m_min.x;
    const float y1 = bbox.m_min.y;
    const float z1 = bbox.m_min.z;
    const float x2 = bbox.m_max.x;
    const float y2 = bbox.m_max.y;
    const float z2 = bbox.m_max.z;

    m.Vertices.push_back( BenVertex( x1, y1, z1, 255,100,100 ) );
    m.Vertices.push_back( BenVertex( x1, y2, z1, 255,255,255 ) );
    m.Vertices.push_back( BenVertex( x2, y2, z1, 100,255,100 ) );
    m.Vertices.push_back( BenVertex( x2, y2, z1, 255,255,255 ) );
    m.Vertices.push_back( BenVertex( x1, y1, z2, 255,200,200 ) );
    m.Vertices.push_back( BenVertex( x1, y2, z2, 255,255,255 ) );
    m.Vertices.push_back( BenVertex( x2, y2, z2, 200,255,200 ) );
    m.Vertices.push_back( BenVertex( x2, y2, z2, 255,255,255 ) );

    uint32_t v0 = 0;
    m.addIndexedLine(v0 + 0, v0 + 1); // AB
    m.addIndexedLine(v0 + 1, v0 + 2); // BC
    m.addIndexedLine(v0 + 2, v0 + 3); // CD
    m.addIndexedLine(v0 + 3, v0 + 0); // DA

    m.addIndexedLine(v0 + 4, v0 + 5); // EF
    m.addIndexedLine(v0 + 5, v0 + 6); // FG
    m.addIndexedLine(v0 + 6, v0 + 7); // GH
    m.addIndexedLine(v0 + 7, v0 + 4); // HE

    m.addIndexedLine(v0 + 0, v0 + 4); // AE
    m.addIndexedLine(v0 + 1, v0 + 5); // BF
    m.addIndexedLine(v0 + 2, v0 + 6); // CG
    m.addIndexedLine(v0 + 3, v0 + 7); // DH

    m.upload( true );
}

void GL_Spectrum3D::createBksLines(BenMeshBuffer & m, float size3d )
{
    float d = size3d;
    m.PrimType = de::gpu::PrimitiveType::Lines;
    m.Vertices.clear();
    m.Vertices.reserve( 6 );
    m.Vertices.push_back( BenVertex( 0, 0, 0, 255,  0,  0 ) );
    m.Vertices.push_back( BenVertex( d, 0, 0, 255,255,255 ) );
    m.Vertices.push_back( BenVertex( 0, 0, 0,   0,255,  0 ) );
    m.Vertices.push_back( BenVertex( 0, d, 0, 255,255,255 ) );
    m.Vertices.push_back( BenVertex( 0, 0, 0,   0,  0,255 ) );
    m.Vertices.push_back( BenVertex( 0, 0, d, 255,255,255 ) );
    m.upload( true );
}

void GL_Spectrum3D::createBksLinesX(BenMeshBuffer & m, float d )
{
    m.PrimType = de::gpu::PrimitiveType::Lines;

    int n=100;

    m.Vertices.clear();
    m.Vertices.reserve( 6 );

    float dx = 10.0f;
    float dz = 50.0f;
    for (int i = 0; i < 100; ++i)
    {
        m.Vertices.push_back( BenVertex( dx * i, 0, 0, 255,  0,  0 ) );
        m.Vertices.push_back( BenVertex( dx * i, 0, -dz, 255,255,255 ) );
    }

    dx = 100.0f;
    dz = 100.0f;
    for (int i = 0; i < 10; ++i)
    {
        m.Vertices.push_back( BenVertex( dx * i, 0, 0, 255,  0,  0 ) );
        m.Vertices.push_back( BenVertex( dx * i, 0, -dz, 255,255,255 ) );
    }

    dx = 1000.0f;
    dz = 200.0f;
    for (int i = 0; i < 2; ++i)
    {
        m.Vertices.push_back( BenVertex( dx * i, 0, 0, 255,  0,  0 ) );
        m.Vertices.push_back( BenVertex( dx * i, 0, -dz, 255,255,255 ) );
    }

    m.upload( true );
}

void GL_Spectrum3D::createBksLinesZ(BenMeshBuffer & m, float d )
{
    m.PrimType = de::gpu::PrimitiveType::Lines;

    int n=100;

    m.Vertices.clear();
    m.Vertices.reserve( 6 );

    float dz = 10.0f;
    float dx = 50.0f;
    for (int i = 0; i < 100; ++i)
    {
        m.Vertices.push_back( BenVertex(   0, 0, dz * i, 0,  0,  255 ) );
        m.Vertices.push_back( BenVertex( -dx, 0, dz * i, 255,255,255 ) );
    }

    dz = 100.0f;
    dx = 100.0f;
    for (int i = 0; i < 10; ++i)
    {
        m.Vertices.push_back( BenVertex(   0, 0, dz * i, 0,  0,  255 ) );
        m.Vertices.push_back( BenVertex( -dx, 0, dz * i, 255,255,255 ) );
    }

    dz = 1000.0f;
    dx = 200.0f;
    for (int i = 0; i < 2; ++i)
    {
        m.Vertices.push_back( BenVertex(   0, 0, dz * i, 0,  0,  255 ) );
        m.Vertices.push_back( BenVertex( -dx, 0, dz * i, 255,255,255 ) );
    }

    m.upload( true );
}

void GL_Spectrum3D::createBksLinesY(BenMeshBuffer & m, float d )
{
    m.PrimType = de::gpu::PrimitiveType::Lines;

    int n=100;

    m.Vertices.clear();
    m.Vertices.reserve( 6 );

    float dy = 10.0f;
    float dz = 50.0f;
    for (int i = 0; i < 100; ++i)
    {
        m.Vertices.push_back( BenVertex( 0, dy * i, 0, 0,  255,  0 ) );
        m.Vertices.push_back( BenVertex( 0, dy * i, -dz, 255,255,255 ) );
    }

    dy = 100.0f;
    dz = 100.0f;
    for (int i = 0; i < 10; ++i)
    {
        m.Vertices.push_back( BenVertex( 0, dy * i, 0, 0,  255,  0 ) );
        m.Vertices.push_back( BenVertex( 0, dy * i, -dz, 255,255,255 ) );
    }

    dy = 1000.0f;
    dz = 200.0f;
    for (int i = 0; i < 2; ++i)
    {
        m.Vertices.push_back( BenVertex( 0, dy * i, 0, 0,  255,  0 ) );
        m.Vertices.push_back( BenVertex( 0, dy * i, -dz, 255,255,255 ) );
    }

    m.upload( true );
}

void GL_Spectrum3D::createWavLines(BenMeshBuffer & m, glm::vec3 const & d,
                                    DE_AlignedFloatVector const & pcm )
{
    const size_t n = pcm.size();
    const float* p = pcm.data();

    if (n < 2)
    {
        return;
    }

    m.PrimType = de::gpu::PrimitiveType::Lines;
    m.Vertices.clear();
    m.Vertices.reserve( n );
    m.Indices.clear();
    m.Indices.reserve( n * 2 );

    const float dx = d.x / float(n-1);
    const float dy = d.y * 0.5f;

    for (size_t i = 0; i < n; ++i)
    {
        const float s = p[i];
        const float t = std::clamp((0.5f * s) + 0.5f, 0.0f, 1.0f);
        const float x = dx * i;
        const float y = dy + dy * s;
        const float z = 0.0f;
        const uint32_t color = m_wav_colorGradient.getColor32(t);
        m.Vertices.push_back( BenVertex( x, y, z, color ) );
    }

    for (size_t i = 1; i < n-1; ++i)
    {
        uint32_t A = i - 1;
        uint32_t B = i;
        m.Indices.push_back( A );
        m.Indices.push_back( B );
    }

    m.upload( true );
}


void GL_Spectrum3D::createWavMatrix(BenMeshBuffer & m, glm::vec3 const & d,
                                    DE_AlignedFloatShiftMatrix const & table )
{
    const size_t cols = table.columnCount();
    const size_t rows = table.rowCount();

    m.PrimType = de::gpu::PrimitiveType::Triangles;
    m.Vertices.clear();
    m.Vertices.reserve( /* top */ cols * rows +
                       /* front */ 2*cols );
    m.Indices.clear();
    m.Indices.reserve( /* top */ ((cols - 1) * (rows - 1) * 6) +
                      /* front */((cols - 1) * 6) );

    const float dx = d.x / float(cols-1);
    const float dy = d.y;
    const float dz = d.z / float(rows-1);
    const uint32_t c0 = m_wav_colorGradient.getColor32(0.0f);

    // [Top] out vertices: pcm expected in range {-1,1}:
    for (size_t j = 0; j < rows; j++)
    {
        const float* row = table.m_view[j];
        for (size_t i = 0; i < cols; i++)
        {
            const float s = row[i];
            const float t = std::abs(s); // expected in range [0,1]
            const float x = dx * i;
            const float y = dy * s;
            const float z = dz * j;
            const uint32_t c = m_wav_colorGradient.getColor32(t);
            m.Vertices.push_back( BenVertex( x, y, z, c ) );
        }
    }
    // [Top] out quads:
    for (size_t j = 0; j < rows-1; j++)
    {
        for (size_t i = 0; i < cols-1; i++)
        {
            const uint32_t A = (cols * j) + i;           // A - 0
            const uint32_t B = (cols * j) + i + 1;       // B - 1
            const uint32_t C = (cols * (j + 1)) + i + 1; // C - 3
            const uint32_t D = (cols * (j + 1)) + i;     // D - 2
            m.addIndexedQuad( A,B,C,D );
        }
    }

    // [Front] vertices:
    table.getFrontVector( m_front );
    const uint32_t vF = m.Vertices.size();

    for (size_t i = 0; i < m_front.size(); ++i)
    {
        const float s = m_front[i];
        const float t = std::abs(s);
        const float x = dx * i;
        const float y = dy * s;
        const float z = 0.0f;
        const uint32_t c1 = m_wav_colorGradient.getColor32(t);
        m.Vertices.push_back( BenVertex( x, 0, z, c0 ) );
        m.Vertices.push_back( BenVertex( x, y, z, c1 ) );
    }
    // Front indices:
    for (size_t i = 0; i < m_front.size() - 1; ++i)
    {
        const uint32_t A = vF + 2*i;          // A - 0
        const uint32_t B = vF + 2*i + 1;      // B - 1
        const uint32_t C = vF + 2*(i+1) + 1;  // C - 3
        const uint32_t D = vF + 2*(i+1);      // D - 2
        m.addIndexedQuad( A,B,C,D );
    }

    // [Left] vertices:
    table.getLeftVector( m_left );
    const uint32_t vL = m.Vertices.size();

    for (size_t i = 0; i < m_left.size(); i++)
    {
        const float s = m_left[i];
        const float t = std::abs(s);
        const float x = 0.0f;
        const float y = dy * s;
        const float z = d.z - dz * i;
        const uint32_t c1 = m_wav_colorGradient.getColor32(t);
        m.Vertices.push_back( BenVertex( x, 0, z, c0 ) );
        m.Vertices.push_back( BenVertex( x, y, z, c1 ) );
    }
    // [Left] indices:
    for (size_t i = 0; i < m_left.size() - 1; i++)
    {
        const uint32_t A = vL + 2*i;          // A - 0
        const uint32_t B = vL + 2*i + 1;      // B - 1
        const uint32_t C = vL + 2*(i+1) + 1;  // C - 3
        const uint32_t D = vL + 2*(i+1);      // D - 2
        m.addIndexedQuad( A,B,C,D );
    }

    // [Right] vertices:
    table.getLeftVector( m_right );
    const uint32_t vR = m.Vertices.size();

    for (size_t i = 0; i < m_right.size(); i++)
    {
        const float s = m_right[i];
        const float t = std::abs(s);
        const float x = d.x;
        const float y = dy * s;
        const float z = dz * i;
        const uint32_t c1 = m_wav_colorGradient.getColor32(t);
        m.Vertices.push_back( BenVertex( x, 0, z, c0 ) );
        m.Vertices.push_back( BenVertex( x, y, z, c1 ) );
    }
    // Right indices:
    for (size_t i = 0; i < m_right.size() - 1; i++)
    {
        const uint32_t A = vR + 2*i;          // A - 0
        const uint32_t B = vR + 2*i + 1;      // B - 1
        const uint32_t C = vR + 2*(i+1) + 1;  // C - 3
        const uint32_t D = vR + 2*(i+1);      // D - 2
        m.addIndexedQuad( A,B,C,D );
    }

    m.upload( true );
}

void GL_Spectrum3D::createWavMatrix01(BenMeshBuffer & m,
                                    glm::vec3 const & d,
                                    DE_AlignedFloatShiftMatrix const & table )
{
    const size_t cols = table.columnCount();
    const size_t rows = table.rowCount();

    m.PrimType = de::gpu::PrimitiveType::Triangles;
    m.Vertices.clear();
    m.Vertices.reserve( /* top */   cols * rows +
                        /* front */ 2*cols +
                       ///* back */ 2*cols +
                        /* left */  2*rows +
                        /* right */ 2*rows
                       );
    m.Indices.clear();
    m.Indices.reserve(  /* top */   (cols - 1) * (rows - 1) * 6 +
                        /* front */ (cols - 1) * 6 +
                       ///* back */ (cols - 1) * 6 +
                        /* left */  (rows - 1) * 6 +
                        /* right */ (rows - 1) * 6
                      );

    const float dx = d.x / float(cols-1);
    const float dy = d.y;
    const float dz = d.z / float(rows-1);
    const uint32_t c0 = m_wav_colorGradient.getColor32(0.0f);

    // Top:
    for (size_t j = 0; j < rows; j++)
    {
        const float* row = table.m_view[j];
        for (size_t i = 0; i < cols; i++)
        {
            const float s = row[i]; // expected in range [0,1]
            const float t = std::abs(s); // expected in range [0,1]
            const float x = dx * i;
            const float y = dy * s;
            const float z = dz * j;
            const uint32_t c = m_wav_colorGradient.getColor32(t);
            m.Vertices.push_back( BenVertex( x, y, z, c ) );
        }
    }
    // Top:
    for (size_t j = 0; j < rows-1; j++)
    {
        for (size_t i = 0; i < cols-1; i++)
        {
            const uint32_t A = (cols * j) + i;           // A - 0
            const uint32_t B = (cols * j) + i + 1;       // B - 1
            const uint32_t C = (cols * (j + 1)) + i + 1; // C - 3
            const uint32_t D = (cols * (j + 1)) + i;     // D - 2
            m.addIndexedQuad(A,B,C,D);
        }
    }

    // [Front]:
    const uint32_t vF = m.Vertices.size();

    table.getFrontVector(m_front);
    for (size_t i = 0; i < m_front.size(); ++i)
    {
        const float s = m_front[i];
        const float t = std::abs(s); // expected in range [0,1]
        const float x = dx * i;
        const float y = dy * s;
        const float z = 0.0f;
        const uint32_t c = m_wav_colorGradient.getColor32(t);
        m.Vertices.push_back( BenVertex( x, 0, z, c0 ) );
        m.Vertices.push_back( BenVertex( x, y, z, c ) );
    }
    for (size_t i = 0; i < m_front.size() - 1; ++i)
    {
        const uint32_t A =  vF + 2*i;          // A - 0
        const uint32_t B =  vF + 2*i + 1;      // B - 1
        const uint32_t C =  vF + 2*(i+1) + 1;  // C - 3
        const uint32_t D =  vF + 2*(i+1);      // D - 2
        m.addIndexedQuad(A,B,C,D);
    }


    // [Left] vertices:
    const uint32_t vL = m.Vertices.size();

    table.getLeftVector( m_left );
    for (size_t i = 0; i < m_left.size(); i++)
    {
        const float s = m_left[i];
        const float t = std::abs(s); // expected in range [0,1]
        const float x = 0.0f;
        const float y = dy * s;
        const float z = d.z - dz * i;
        const uint32_t c1 = m_wav_colorGradient.getColor32(t);
        m.Vertices.push_back( BenVertex( x, 0, z, c0 ) );
        m.Vertices.push_back( BenVertex( x, y, z, c1 ) );
    }
    // [Left] indices:
    for (size_t i = 0; i < m_left.size() - 1; i++)
    {
        const uint32_t A = vL + 2*i;          // A - 0
        const uint32_t B = vL + 2*i + 1;      // B - 1
        const uint32_t C = vL + 2*(i+1) + 1;  // C - 3
        const uint32_t D = vL + 2*(i+1);      // D - 2
        m.addIndexedQuad(A,B,C,D);
    }

    // [Right] vertices:
    const uint32_t vR = m.Vertices.size();

    table.getRightVector( m_right );
    for (size_t i = 0; i < m_right.size(); i++)
    {
        const float s = m_right[i];
        const float t = std::abs(s); // expected in range [0,1]
        const float x = d.x;
        const float y = dy * s;
        const float z = dz * i;
        const uint32_t c1 = m_wav_colorGradient.getColor32(t);
        m.Vertices.push_back( BenVertex( x, 0, z, c0 ) );
        m.Vertices.push_back( BenVertex( x, y, z, c1 ) );
    }
    // [Right] indices:
    for (size_t i = 0; i < m_right.size() - 1; i++)
    {
        const uint32_t A = vR + 2*i;          // A - 0
        const uint32_t B = vR + 2*i + 1;      // B - 1
        const uint32_t C = vR + 2*(i+1) + 1;  // C - 3
        const uint32_t D = vR + 2*(i+1);      // D - 2
        m.addIndexedQuad(A,B,C,D);
    }

    m.upload( true );
}

// size3d.x = 2000.0f;
// size3d.y = 500.0f;
// size3d.z = 300.0f;
void GL_Spectrum3D::createFftMatrix(BenMeshBuffer & m, glm::vec3 const & d,
                                    DE_AlignedFloatShiftMatrix const & table )
{
    auto cols = table.columnCount();
    auto rows = table.rowCount();
    if ( rows < 1 || cols < 1 )
    {
        //DE_ERROR("No rows")
        return;
    }

    float dx = d.x / float ( cols - 1 ); // / (sampleRate_over_fftSize * colCount);
    float dy = d.y;
    float dz = d.z / float ( rows - 1 );
    uint32_t c0 = m_fft_colorGradient.getColor32( 0.0f );

    uint32_t mode = 1;
    uint32_t scaleXmode = 0;
    float sampleRate_over_fftSize = 48000.0f / 8192.0f;

    int ch = 0;

    m.PrimType = de::gpu::PrimitiveType::Triangles;

    m.Vertices.clear();
    m.Vertices.reserve(  /* top */   rows * cols
                       + /* front */ 2*cols
                       + /* back */  2*cols
                       + /* left */  2*rows
                       + /* right */ 2*rows );
    m.Indices.clear();
    m.Indices.reserve(  /* top */   6*(rows-1) * (cols-1)
                      + /* front */ 6*(cols-1)
                      + /* back */  6*(cols-1)
                      + /* left */  6*(rows-1)
                      + /* right */ 6*(rows-1) );

#if 0
    // if ( mode == 1 ) // Decibel mode, logarithmic dB scale
    float dBmin = -90;
    float dBmax = 70;
    float dBrange = dBmax - dBmin;
    if ( dBrange < 1.0f ) dBrange = 1.0f;
    float dBrangeInv = 1.0f / dBrange;

    if ( scaleXmode == 0 ) // X-axis is scaled logarithmicly.
    {
        dx = d.x / float( cols - 1 );
        //dy = m_sizeY * 0.5f;
        //dz = m_sizeZ / float ( rowCount - 1 );
    }
#endif


    // auto bb = table.getMinMax();
    // //DE_WARN("table.getMinMax(", bb.m_min,",",bb.m_max,")")

    // float dB = 0;
    // float dBmin = bb.m_min;
    // float dBmax = bb.m_max;

    // Matrix Top
    for ( size_t row = 0; row < rows; row++ )
    {
        for ( size_t col = 0; col < cols; col++ )
        {
            //int ix = col; // std::clamp( int(std::logf(col+1)), int(0), int(cols) - 1 );
            float x = dx * col;
            float y = dy * table.getPixel( col, row );  // The row data
            float z = dz * row;
            uint32_t color = m_fft_colorGradient.getColor32( y );
            m.Vertices.push_back( BenVertex(x,y,z,color) );
        }
#if 0
        float z = dz * j; // z is const for a same row.
        float kx = dx; // * (1.0f + fx * float( j )); // kx is const for same row.
        if ( ch == 0 ) // left channel goes from 0 to -size
        {
            kx = -kx;
        }


        if ( scaleXmode > 0 )                           // X-axis is scaled logarithmicly.
        {
            for ( size_t i = 0; i < cols; ++i )
            {
                float a = *pSrc++;


                float x = 0.0f;
                float freq = sampleRate_over_fftSize * i;
                if ( freq > 1e-16f ) // log(0) is -inf
                {
                    x = kx * (log10f( freq ) - 1.5f); // -1 = shift by 10^-1
                }
                float dB = std::clamp( a, dBmin, dBmax );
                float r = (dB-dBmin) * dBrangeInv;
                float y = dy * r;
                uint32_t color = m_fft_colorGradient.getColor32( r );
                //uint32_t color = de::RainbowColor::computeColor32( std::fabs( 1.0f - 0.5f * r ) );
                m.Vertices.push_back( BenVertex(x,y,z,color) );
            }
        }
        else // if ( scaleXmode == 0 ) // X-axis is scaled linear frequency.
        {
            float x = 0.0f;
            for ( size_t i = 0; i < cols; ++i )
            {
                //float dB = std::clamp( *pSrc++, dBmin, dBmax );
                float dB = *pSrc++;
                float r = (dB-dBmin) * dBrangeInv;
                float y = dy * r;
                uint32_t color = m_fft_colorGradient.getColor32( 1.0f - 0.5f * r ); // de::RainbowColor::computeColor32( t );
                //uint32_t color = de::RainbowColor::computeColor32( std::fabs( 1.0f - 0.5f * r ) );
                m.Vertices.push_back( BenVertex(x,y,z,color) );
                x += kx;
            }
        }
#endif
    }

    for ( size_t j = 0; j < rows-1; j++ )
    {
        for ( size_t i = 0; i < cols-1; i++ )
        {
#if 0
            const uint32_t A = (j)*cols + (i);
            const uint32_t B = (j+1)*cols + (i);
            const uint32_t C = (j+1)*cols + (i+1);
            const uint32_t D = (j)*cols + (i+1);
            m.addIndexedTriangleQuad(A,B,C,D);
#else
            const uint32_t A = (cols * j) + i;           // A - 0
            const uint32_t B = (cols * j) + i + 1;       // B - 1
            const uint32_t C = (cols * (j + 1)) + i + 1; // C - 3
            const uint32_t D = (cols * (j + 1)) + i;     // D - 2
            m.addIndexedQuad( A,B,C,D );
#endif
        }
    }

    // Front:
    const size_t nF = m.Vertices.size();
    for (size_t i = 0; i < cols; i++)
    {
        auto vF = m.Vertices[i];
        auto vO = vF;
        vO.pos.y = 0.0f;
        vO.color = c0;
        m.Vertices.push_back( vO );
        m.Vertices.push_back( vF );
    }
    for (size_t i = 0; i < cols - 1; i++)
    {
        const uint32_t A = nF + 2*i;           // A - 0
        const uint32_t B = nF + 2*i + 1;       // B - 1
        const uint32_t C = nF + 2*(i+1) + 1;   // C - 3
        const uint32_t D = nF + 2*(i+1);       // D - 2
        m.addIndexedQuad(A,B,C,D);
    }

    // [Left]:
    const size_t nL = m.Vertices.size();
    for (size_t i = 0; i < rows; i++)
    {
        auto vL = m.Vertices[cols * i];
        auto vO = vL;
        vO.pos.y = 0.0f;
        vO.color = c0;
        m.Vertices.push_back( vO );
        m.Vertices.push_back( vL );
    }
    for (size_t i = 0; i < rows - 1; i++)
    {
        const uint32_t A = nL + 2*i;           // A - 0
        const uint32_t B = nL + 2*i + 1;       // B - 1
        const uint32_t C = nL + 2*(i+1) + 1;   // C - 3
        const uint32_t D = nL + 2*(i+1);       // D - 2
        m.addIndexedQuad(A,B,C,D);
    }

    // [Right]:
    const size_t nR = m.Vertices.size();
    for (size_t i = 0; i < rows; i++)
    {
        auto vR = m.Vertices[cols * i + cols - 1];
        auto vO = vR;
        vO.pos.y = 0.0f;
        vO.color = c0;
        m.Vertices.push_back( vO );
        m.Vertices.push_back( vR );
    }
    for (size_t i = 0; i < rows - 1; i++)
    {
        const uint32_t A = nR + 2*i;           // A - 0
        const uint32_t B = nR + 2*i + 1;       // B - 1
        const uint32_t C = nR + 2*(i+1) + 1;   // C - 3
        const uint32_t D = nR + 2*(i+1);       // D - 2
        m.addIndexedQuad(A,B,C,D);
    }

    //de::gpu::SMeshBufferTool::computeNormals( o );

    m.upload( true );
}

#if 0
void GL_Spectrum3D_Meshes::createWavMatrix(
    SMeshBuffer & m, glm::vec3 const & size3d,
    DE_AlignedFloatShiftMatrix const & pcm )
{

    float m_sizeX = size3d.x; // 2000;
    float m_sizeY = size3d.y; // 1000;
    float m_sizeZ = size3d.z; // 4000;
    // float ox = 2000.0f;
    // float oy = 1000.0f;
    // float oz = 1200.0f;
    uint32_t mode = 1;
    uint32_t scaleXmode = 0;
    float sampleRate_over_fftSize = 48000.0f / 8192.0f;

    int ch = 0;

    // de::LinearColorGradient & gradient = m_fftColorGradient;

    auto rowCount = dB.rowCount();
    if ( rowCount < 1 )
    {
        //DE_ERROR("No rows")
        return;
    }

    auto colCount = dB.columnCount();
    auto vCount = rowCount * colCount + 2*colCount;
    auto iCount = 4*(rowCount-1) * (colCount-1) + 4*(colCount-1);

    //float fx = 15.0f / float ( rowCount );

    m.PrimType = PrimitiveType::Triangles;
    m.Vertices.clear();
    m.Vertices.reserve( vCount );

    if ( mode == 0 ) // PCM mode
    {
        float dx = m_sizeX / float( colCount );
        float dy = m_sizeY * 0.5f;
        float dz = m_sizeZ / float ( rowCount );

        // Matrix Top
        for ( size_t j = 0; j < rowCount; ++j )
        {
            float kx = dx; // * (1.0f + fx * float( j ));
            if ( ch == 0 ) // translate left channel in x-dir
            {
                kx = -kx;
            }

            std::vector< float >* row = shiftMatrixData[ j ];
            if ( row )
            {
                float const* pSrc = row->data();
                for ( size_t i = 0; i < colCount; ++i )
                {
                    float s = *pSrc;
                    float x = kx * float( i );
                    float y = dy * s;
                    float z = dz * j;
                    uint32_t color = gradient.getColor32( std::abs( s ) );
                    //uint32_t color = de::RainbowColor::computeColor32( std::fabs( 1.0f - 0.5f * s ) );
                    o.Vertices.push_back( BenVertex(x,y,z,color) );
                    pSrc++;
                }
            }
        }

    }
    else if ( mode == 1 ) // Decibel mode, logarithmic dB scale
    {
    float dBmin = -90;
    float dBmax = 70;
    float dBrange = dBmax - dBmin;
    if ( dBrange < 1.0f ) dBrange = 1.0f;
    float dBrangeInv = 1.0f / dBrange;
    float dx = m_sizeX / 2.0f; // / (sampleRate_over_fftSize * colCount);
    float dy = m_sizeY;
    float dz = m_sizeZ / float ( rowCount - 1 );
    if ( scaleXmode == 0 ) // X-axis is scaled logarithmicly.
    {
        dx = m_sizeX / float( colCount - 1 );
        //dy = m_sizeY * 0.5f;
        //dz = m_sizeZ / float ( rowCount - 1 );

    }
    // Matrix Top
    for ( size_t j = 0; j < rowCount; ++j )
    {
        float z = dz * j; // z is const for a same row.
        float kx = dx; // * (1.0f + fx * float( j )); // kx is const for same row.
        if ( ch == 0 ) // left channel goes from 0 to -size
        {
            kx = -kx;
        }

        std::vector< float >* row = shiftMatrixData[ j ];  // The row vector
        if ( row )
        {
            float const* pSrc = row->data();                // The row data

            if ( scaleXmode > 0 )                           // X-axis is scaled logarithmicly.
            {
                for ( size_t i = 0; i < colCount; ++i )
                {
                    float x = 0.0f;
                    float freq = sampleRate_over_fftSize * i;
                    if ( freq > 1e-16f ) // log(0) is -inf
                    {
                        x = kx * (log10f( freq ) - 1.5f); // -1 = shift by 10^-1
                    }
                    float dB = std::clamp( *pSrc++, dBmin, dBmax );
                    float r = (dB-dBmin) * dBrangeInv;
                    float y = dy * r;
                    uint32_t color = gradient.getColor32( r );
                    //uint32_t color = de::RainbowColor::computeColor32( std::fabs( 1.0f - 0.5f * r ) );
                    o.Vertices.push_back( BenVertex(ox+x,oy+y,oz+z,color) );
                }
            }
            else // if ( scaleXmode == 0 ) // X-axis is scaled linear frequency.
            {
                float x = 0.0f;
                for ( size_t i = 0; i < colCount; ++i )
                {
                    //float dB = std::clamp( *pSrc++, dBmin, dBmax );
                    float dB = *pSrc++;
                    float r = (dB-dBmin) * dBrangeInv;
                    float y = dy * r;
                    uint32_t color = gradient.getColor32( 1.0f - 0.5f * r ); // de::RainbowColor::computeColor32( t );
                    //uint32_t color = de::RainbowColor::computeColor32( std::fabs( 1.0f - 0.5f * r ) );
                    o.Vertices.push_back( BenVertex(ox+x,oy+y,oz+z,color) );
                    x += kx;
                }
            }
        }
    }

    }

    o.Indices.clear();
    o.Indices.reserve( iCount );
    for ( size_t j = 1; j < rowCount; ++j )
    {
        for ( size_t i = 1; i < colCount; ++i )
        {
            size_t iA = (j-1)*colCount + (i-1);
            size_t iB = (j  )*colCount + (i-1);
            size_t iC = (j  )*colCount + (i  );
            size_t iD = (j-1)*colCount + (i  );

            o.Indices.push_back( iA );
            o.Indices.push_back( iB );
            o.Indices.push_back( iC );

            o.Indices.push_back( iA );
            o.Indices.push_back( iC );
            o.Indices.push_back( iD );
        }
    }

    // Front
    uint32_t c0 = gradient.getColor32( 0.0f ); // de::RainbowColor::computeColor32( t );
    auto pSrc = shiftMatrixData[ 0 ]->data();
    uint32_t v = o.Vertices.size();
    for ( size_t i = 0; i < colCount; ++i )
    {
        float x = o.Vertices[i].pos.x;
        o.Vertices.push_back( BenVertex(ox+x, oy-90, oz, c0) );
        pSrc++;
    }

    for ( size_t i = 0; i < colCount-1; ++i )
    {
        size_t iA = v + i;
        size_t iB = i;
        size_t iC = i+1;
        size_t iD = v + i+1;
        //o.addIndexedQuad( iA,iB,iC,iD );

        o.Indices.push_back( iA );
        o.Indices.push_back( iB );
        o.Indices.push_back( iC );

        o.Indices.push_back( iA );
        o.Indices.push_back( iC );
        o.Indices.push_back( iD );
    }

    //de::gpu::SMeshBufferTool::computeNormals( o );

    GT_upload( m, true );
}


void
GL_Spectrum3D::createSpectrum()
{
    if (!m_sampleSource)
    {
        return;
    }

    const size_t nData = m_sampleSource->getShiftBuffer()->size();
    const float* pData = m_sampleSource->getShiftBuffer()->data();

    m_fft.setInput(pData,nData);

    m_fft.fft();

    if (m_decibels.size() < m_fft.size() / 2)
    {
        m_decibels.resize( m_fft.size() / 2 );
    }

    m_fft.getOutputInDecibel( m_decibels.data(), m_decibels.size() );


    m_spectrum.PrimType = PrimitiveType::TriangleStrip;
    m_spectrum.Vertices.clear();
    m_spectrum.Indices.clear();

    m_spectrum.Vertices.reserve( 2*m_decibels.size() );
    //m_lines2.Indices.reserve( nData );
    dbRandomize();

    const float dx = 2000.0f / float(m_decibels.size());
    const float dy = 500.0f;
    //const float dz = 300.0f;

    for (size_t i = 0; i < m_decibels.size(); ++i)
    {
        const float sample = m_decibels[i];
        const float x = 100.f + (dx * i);
        const float y = 500.0f + 5.0f*sample;
        const float z = 1000.f;
        const uint8_t r = dbRND() % 256;
        const uint8_t g = dbRND() % 256;
        const uint8_t b = dbRND() % 256;

        m_spectrum.Vertices.push_back( BenVertex( x, 0, z, 255, 0, 0, 0 ) );
        m_spectrum.Vertices.push_back( BenVertex( x, y, z, 255, 255, 0 ) );


        //m_lines2.Indices.push_back( i );
    }
}

#endif

