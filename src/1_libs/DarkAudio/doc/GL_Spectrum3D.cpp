#include "GL_Spectrum3D.h"
#include <de_opengl.h>

// ===========================================================================
GL_Spectrum3D::GL_Spectrum3D()
// ===========================================================================
    : m_fft( 4*2048, false )
{
    m_shiftMatrixL.resize( 1024, 64 );

    m_cgWaveform.addStop( 0, dbRGBA(0,0,0) );
    m_cgWaveform.addStop( 0.1f, dbRGBA(255,255,255) );
    m_cgWaveform.addStop( 0.2f, dbRGBA(0,0,255) );
    m_cgWaveform.addStop( 0.3f, dbRGBA(0,20,0) );
    m_cgWaveform.addStop( 0.6f, dbRGBA(0,200,0) );
    m_cgWaveform.addStop( 0.8f, dbRGBA(255,255,0) );
    m_cgWaveform.addStop( 1.0f, dbRGBA(255,0,0) );
    //   m_cgWaveform.addStop( 1.1f, dbRGBA(255,0,255) );
    //   m_cgWaveform.addStop( 1.3f, dbRGBA(0,0,255) );
    //   m_cgWaveform.addStop( 1.5f, dbRGBA(0,0,155) );
    m_cgWaveform.addStop( 2.5f, dbRGBA(205,105,5) );

    m_cgSpektrum.addStop( 0, dbRGBA(0,0,0) );
    m_cgSpektrum.addStop( 0.45, dbRGBA(125,125,125) );
    m_cgSpektrum.addStop( 0.5, dbRGBA(0,0,255) );
    m_cgSpektrum.addStop( 0.6, dbRGBA(0,200,0) );
    m_cgSpektrum.addStop( 0.8, dbRGBA(255,255,0) );
    m_cgSpektrum.addStop( 1.0, dbRGBA(255,0,0) );
}

GL_Spectrum3D::~GL_Spectrum3D()
{

}

void
GL_Spectrum3D::setSampleSource( DspSampleCollector* sampleSource )
{
    m_sampleSource = sampleSource;
}

void
GL_Spectrum3D::initializeGL()
{
    // initializeOpenGLFunctions();

    // Init shader program:
    m_lineShader.init();

    // Init meshbuffers:
    dbRandomize();
    float d = 1000.0f;
    m_lines.PrimType = PrimitiveType::Lines;
    m_lines.Vertices.clear();
    m_lines.Vertices.reserve( 6 );
    m_lines.Vertices.push_back( S3DVertex( 0, 0, 0, 255,  0,  0 ) );
    m_lines.Vertices.push_back( S3DVertex( d, 0, 0, 255,255,255 ) );
    m_lines.Vertices.push_back( S3DVertex( 0, 0, 0,   0,255,  0 ) );
    m_lines.Vertices.push_back( S3DVertex( 0, d, 0, 255,255,255 ) );
    m_lines.Vertices.push_back( S3DVertex( 0, 0, 0,   0,  0,255 ) );
    m_lines.Vertices.push_back( S3DVertex( 0, 0, d, 255,255,255 ) );
    GT_upload( m_lines );
}

void
GL_Spectrum3D::resizeGL(int w, int h)
{
    //m_projMat = glm::perspective(glm::radians(45.0f), float(w) / float(h), 0.1f, 38000.0f);
}

void
GL_Spectrum3D::paintGL( de::gpu::Camera* pCamera )
{
    createLines2();
    GT_upload( m_lines2, true );

    createTriangles();
    GT_upload( m_triangles, true );

    createSpectrum();
    GT_upload( m_spectrum, true );

    createSpectrumMatrix();
    GT_upload( m_spectrumMatrix, true );


    m_lineShader.useShader( pCamera, true );

    GT_draw( m_spectrumMatrix );

    GT_draw( m_spectrum );

    GT_draw( m_triangles );

    GT_draw( m_lines2 );

    GT_draw( m_lines );
}

void
GL_Spectrum3D::createLines2()
{
    m_lines2.PrimType = PrimitiveType::LineStrip;
    m_lines2.Vertices.clear();
    m_lines2.Indices.clear();

    if (!m_sampleSource)
    {
        return;
    }

    const size_t nData = m_sampleSource->getShiftBuffer()->size();
    const float* pData = m_sampleSource->getShiftBuffer()->data();
    m_lines2.Vertices.reserve( nData );
    //m_lines2.Indices.reserve( nData );
    dbRandomize();

    const float dx = 2000.0f / float(nData);
    const float dy = 100.0f;
    const float dz = 0.0f;

    for (size_t i = 0; i < nData; ++i)
    {
        const float sample = pData[i];
        const float x = 100.f + (dx * i);
        const float y = dy * sample;
        const float z = 100.f + dz;
        const uint8_t r = dbRND() % 256;
        const uint8_t g = dbRND() % 256;
        const uint8_t b = dbRND() % 256;
        m_lines2.Vertices.push_back( S3DVertex( x, y, z, r, g, b ) );
        //m_lines2.Indices.push_back( i );
    }
}

void
GL_Spectrum3D::createTriangles()
{
    m_triangles.PrimType = PrimitiveType::TriangleStrip;
    m_triangles.Vertices.clear();
    m_triangles.Indices.clear();

    if (!m_sampleSource)
    {
        return;
    }

    const size_t nData = m_sampleSource->getShiftBuffer()->size();
    const float* pData = m_sampleSource->getShiftBuffer()->data();
    m_triangles.Vertices.reserve( 2*nData );
    //m_lines2.Indices.reserve( nData );
    dbRandomize();

    const float dx = 2000.0f / float(nData);
    const float dy = 200.0f;
    //const float dz = 300.0f;

    for (size_t i = 0; i < nData; ++i)
    {
        const float sample = pData[i];
        const float x = 100.f + (dx * i);
        const float y = 300.0f + dy * sample;
        const float z = 300.f;
        const auto r = static_cast<uint8_t>(dbRND() % 256);
        const auto g = static_cast<uint8_t>(dbRND() % 256);
        const auto b = static_cast<uint8_t>(dbRND() % 256);

        m_triangles.Vertices.push_back( S3DVertex( x, y, z, 255, 255, 0 ) );
        m_triangles.Vertices.push_back( S3DVertex( x, 0, z, 0, 0, 0, 0 ) );
        //m_lines2.Indices.push_back( i );
    }
}

/*
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

        m_spectrum.Vertices.push_back( S3DVertex( x, 0, z, 255, 0, 0, 0 ) );
        m_spectrum.Vertices.push_back( S3DVertex( x, y, z, 255, 255, 0 ) );


        //m_lines2.Indices.push_back( i );
    }
}
*/

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

    if (m_decibels.size() < m_fft.size() / 4)
    {
        m_decibels.resize( m_fft.size() / 4 );
    }

    const size_t n = m_decibels.size();

    m_fft.getOutputInDecibel( m_decibels.data(), n );

    if (m_shiftMatrixL.columnCount() != n)
    {
        m_shiftMatrixL.resize( n, 256 );
    }

    m_shiftMatrixL.push( m_decibels.data(), m_decibels.size() );

    // auto onFull = [&] ()
    // {
    //     m_fft.setInput( m_fftR.data(), m_fft.size() );
    //     m_fft.fft();
    //     m_fft.getOutputInDecibel( m_fftOutput.data(), m_fft.size() );
    //     m_shiftMatrixR.push( m_fftOutput.data(), m_fft.size() / 8 );
    // };

    // // Fuse stereo to mono = (L+R)/2
    // DSP_FUSE_STEREO_TO_MONO( m_channelBuffer.data(), dst, dstFrames, dstChannels );

    // // Push fused stereo to mono into the 3D ShiftMatrix...
    // m_fftR.push( "m_fftR", m_channelBuffer.data(), dstFrames, false, onFull );

    m_spectrum.PrimType = PrimitiveType::Triangles;
    m_spectrum.Vertices.clear();
    m_spectrum.Indices.clear();

    m_spectrum.Vertices.reserve( 2*n );
    m_spectrum.Indices.reserve( 6 * (n - 1) );
    dbRandomize();

    const float dx = 2000.0f / float(n);
    const float dy = 500.0f;
    //const float dz = 300.0f;

    const float z = 1000.f;

    for (size_t i = 0; i < n; ++i)
    {
        const float s = m_decibels[i];
        const float x = 100.f + 1000.0f*log10f(dx * i);
        const float y = 500.0f + 5.0f*s;
        m_spectrum.Vertices.push_back( S3DVertex( x, 0, z, 255, 0, 0, 0 ) );
        m_spectrum.Vertices.push_back( S3DVertex( x, y, z, 255, 255, 0 ) );
    }

    for (size_t i = 0; i < n - 1; ++i)
    {
        m_spectrum.Indices.push_back( 2*i );            // A - 0
        m_spectrum.Indices.push_back( 2*(i+1) + 1 );    // C - 3
        m_spectrum.Indices.push_back( 2*i + 1 );        // B - 1

        m_spectrum.Indices.push_back( 2*i );            // A - 0
        m_spectrum.Indices.push_back( 2*(i+1) );        // D - 2
        m_spectrum.Indices.push_back( 2*(i+1) + 1 );    // C - 3
    }
}

void
GL_Spectrum3D::createSpectrumMatrix()
{
    float m_sizeX = 2000;
    float m_sizeY = 1000;
    float m_sizeZ = 4000;
    float ox = 2000.0f;
    float oy = 1000.0f;
    float oz = 1200.0f;
    uint32_t mode = 1;
    uint32_t scaleXmode = 0;
    float sampleRate_over_fftSize = 48000.0f / 8192.0f;

    int ch = 0;
    SMeshBuffer & o = m_spectrumMatrix;
    std::vector< std::vector< float >* > const & shiftMatrixData = m_shiftMatrixL.m_data;

    de::LinearColorGradient & gradient = m_cgSpektrum;


    auto rowCount = shiftMatrixData.size();
    if ( rowCount < 1 )
    {
        //DE_ERROR("No rows")
        return;
    }

    auto colCount = shiftMatrixData[0]->size();
    auto vCount = rowCount * colCount + 2*colCount;
    auto iCount = 4*(rowCount-1) * (colCount-1) + 4*(colCount-1);

    //float fx = 15.0f / float ( rowCount );

    o.PrimType = PrimitiveType::Triangles;
    //o.setLighting( 0 );
    //o.setCulling( false );

    o.Vertices.clear();
    o.Vertices.reserve( vCount );

    //float fx = 19.0f / float ( rowCount - 1 );

#if 0
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
                    o.Vertices.push_back( S3DVertex(x,y,z,color) );
                    pSrc++;
                }
            }
        }

    }
    else if ( mode == 1 ) // Decibel mode, logarithmic dB scale
    {
#endif
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
                        o.Vertices.push_back( S3DVertex(ox+x,oy+y,oz+z,color) );
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
                        o.Vertices.push_back( S3DVertex(ox+x,oy+y,oz+z,color) );
                        x += kx;
                    }
                }
            }
        }
#if 0
    }
#endif
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

    /*
    // Front
    uint32_t c0 = gradient.getColor32( 0.0f ); // de::RainbowColor::computeColor32( t );
    auto pSrc = shiftMatrixData[ 0 ]->data();
    uint32_t v = o.Vertices.size();
    for ( size_t i = 0; i < colCount; ++i )
    {
        float x = o.Vertices[i].pos.x;
        o.Vertices.push_back( S3DVertex(ox+x, oy-90, oz, c0) );
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
*/
    //de::gpu::SMeshBufferTool::computeNormals( o );
}













