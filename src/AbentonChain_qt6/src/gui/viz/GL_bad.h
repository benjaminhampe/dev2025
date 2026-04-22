#pragma once

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

