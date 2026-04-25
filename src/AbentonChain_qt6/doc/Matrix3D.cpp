#include <gui/viz/Matrix3D.h>
#include <App.h>
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>

Matrix3D::Matrix3D()
    : m_rows{0}
    , m_cols{0}
    , m_scaleModeX{ 1 } // 0 = linear, 1 = log10
    , m_pos{ 0,0,0 }
    , m_size{ 4000, 250, 2000 }
    , m_modelMat{ 1.0f }
    , m_bbox{ m_pos, m_pos + m_size }
    , m_mesh16()
    , m_name{ "Matrix3D" }
{

}

Matrix3D::~Matrix3D()
{

}

de::BBox3f Matrix3D::recomputeBoundingBox(V3 const & d, DE_AlignedFloatShiftMatrix const & table)
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

void Matrix3D::init(V3 size, V3 pos, std::string name)
{
    m_size = size;
    m_pos = pos;
    m_bbox = de::BBox3f(pos, pos +size);
}

void Matrix3D::draw(
    GL_Mesh16_Shader3D& shader,
    const GL_Mesh16_Material& material)
{
    // glm::mat4 M = glm::translate(glm::mat4(1.0f), pos)
    //             * glm::rotate(glm::mat4(1.0f), angle, axis)
    //             * glm::scale(glm::mat4(1.0f), scale);
    auto T = glm::translate(glm::mat4(1.0f), m_pos);
    //auto S = glm::scale(glm::mat4(1.0f), m_size);
    shader.setMaterial(material, T);
    m_mesh16.draw();
}

#if 0
void Matrix3D::updateIndices(DE_AlignedFloatShiftMatrix const & table)
{
    const uint32_t c = table.columnCount();
    const uint32_t r = table.rowCount();
    //plot.bbox = recomputeBoundingBox( plot.size, table );

    if ((m_cols != c) || (m_rows != r))
    {
        m_cols = c;
        m_rows = r;
        m_mesh16.initVAO(true);
        m_mesh16.Indices.clear();
        m_mesh16.Indices.reserve(  /* top */   (r-1) * (c-1) * 6
                      // + /* front */ 6*(cols-1)
                      // + /* back */  6*(cols-1)
                      // + /* left */  6*(rows-1)
                      // + /* right */ 6*(rows-1)
                      );

        for ( size_t j = 0; j < r-1; j++ )
        {
            for ( size_t i = 0; i < c-1; i++ )
            {
                const uint32_t A = (c * j) + i;       // A - 0
                const uint32_t B = (c * (j+1)) + i;   // B - 1
                const uint32_t C = (c * (j+1)) + i+1; // C - 3
                const uint32_t D = (c * j) + i+1;     // D - 2
                m_mesh16.addIndexedQuad( A,B,C,D );
            }
        }

        m_mesh16.uploadIndices();
    }
}

void Matrix3D::updateVertices(DE_AlignedFloatShiftMatrix const & table )
{
    auto cols = table.columnCount();
    auto rows = table.rowCount();
    if ( rows < 2 ) { DE_ERROR("No rows") return; }
    if ( cols < 2 ) { DE_ERROR("No cols") return; }

    //#############################
    // Create Indices:
    //#############################

    bool bNeedIndexUpload = false;
    if ((rows != m_rows) || (cols != m_cols))
    {
        m_cols = cols;
        m_rows = rows;
        m_mesh16.Indices.clear();
        m_mesh16.Indices.reserve( (cols) * (rows) * 6 // top
                      // + 6*(cols-1) // front
                      // + 6*(cols-1) // back
                      // + 6*(rows-1) // left
                      // + 6*(rows-1) // right
                      );

        for ( size_t j = 0; j < rows-1; j++ )
        {
            for ( size_t i = 0; i < cols-1; i++ )
            {
                const uint32_t A = (cols * j) + i;       // A - 0
                const uint32_t B = (cols * (j+1)) + i;   // B - 1
                const uint32_t C = (cols * (j+1)) + i+1; // C - 3
                const uint32_t D = (cols * j) + i+1;     // D - 2
                m_mesh16.addIndexedQuad( A,B,C,D );
            }
        }
        bNeedIndexUpload = true;

        // X-axis is scaled logarithmicly.
        if (cols != m_XMap.size())
        {
            m_XMap.resize(cols);

            // / (sampleRate_over_fftSize * colCount);
            const float sampleRate = 48000.0f;
            const float fftSize = cols;

            const float sampleRate_over_fftSize = sampleRate / fftSize;
            const float f = sampleRate_over_fftSize; //  / log10f( float(cols) );

            for ( size_t col = 0; col < cols; col++ )
            {
                //  - 1.5f -1 = shift by 10^-1
                m_XMap[ col ] = f * log10f( float(col+1) );
            }
        }
    }

    //#############################
    // Create Vertices:
    //#############################

    auto dBmin = -240.0f;
    auto dBmax = 120.0f;

    //const float dx = m_size.x / float ( cols - 1 );
    const float dx = m_size.x / m_XMap.back(); // / (sampleRate_over_fftSize * colCount);
    const float dy = m_size.y;
    const float dz = m_size.z / float ( rows - 1 );

    auto & m = m_mesh16;
    m.PrimType = de::gpu::PrimitiveType::Triangles;
    m.Vertices.clear();
    m.Vertices.reserve(
        rows * cols // top
        // + 2*cols // front
        // + 2*cols // back
        // + 2*rows // left
        // + 2*rows // right
    );

    float dBrange = dBmax - dBmin;
    if ( dBrange < 1.0f ) dBrange = 1.0f;
    float dBrangeInv = 1.0f / dBrange;

    // Matrix Top
    if ( m_scaleModeX == 1 ) // X-axis is scaled logarithmicly.
    {
        for ( size_t row = 0; row < rows; row++ )
        {
            for ( size_t col = 0; col < cols; col++ )
            {
                float dB = table.getPixel( col, row );  // The row data
                float t = (dB - dBmin) * dBrangeInv;
                float x = dx * m_XMap[ col ];
                float y = dy * t;
                float z = dz * row;
                m.Vertices.emplace_back( x,y,z,t );
            }
        }
    }
    // Matrix Top
    else // if ( m_scaleModeX == 0 ) // X-axis is linear.
    {
        for ( size_t row = 0; row < rows; row++ )
        {
            for ( size_t col = 0; col < cols; col++ )
            {
                float dB = table.getPixel( col, row );  // The row data
                float t = (dB - dBmin) * dBrangeInv;
                float x = dx * col;
                float y = dy * t;
                float z = dz * row;
                m.Vertices.emplace_back( x,y,z,t );
            }
        }
    }

    m.upload(true, bNeedIndexUpload);

#if 0
    // [Front]:
    const size_t nF = m.Vertices.size();
    for (size_t i = 0; i < cols; i++)
    {
        auto vF = m.Vertices[i];
        auto vO = vF;
        vO.set_y(0.0f);
        vO.set_t(0.0f);
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
        auto vL = m.Vertices[cols * (rows-1-i)];
        auto vO = vL;
        vO.set_y(0.0f);
        vO.set_t(0.0f);
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
        vO.set_y(0.0f);
        vO.set_t(0.0f);
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
#endif
}

#endif