#pragma once
#include "GL_Mesh16.h"

// // ==========================================================
// struct Wav2D
// // ==========================================================
// {
//     GL_Mesh16 m_mesh16;
//     glm::vec3 m_size;
//     glm::vec3 m_pos;
//     glm::mat4 m_modelMat;
//     de::BBox3f m_bbox;
//     std::string m_name;
// };

inline void create2DWav_LineStrip(
    GL_Mesh16 & m,
    glm::vec2 const & size,
    const float* __restrict__ src,
    size_t n )
{
    m.PrimType = de::gpu::PrimitiveType::LineStrip;
    m.Vertices.clear();
    m.Vertices.reserve(n);
    m.Indices.clear();

    const float dx = size.x / float(n-1);
    const float dy = size.y * 0.5f;

    for (size_t i = 0; i < n; ++i)
    {
        const float s = src[i];
        const float x = dx * i;
        const float y = dy * s;
        const float z = 0.0f;
        const float t = std::clamp((0.5f * s) + 0.5f, 0.f, 1.f);
        m.Vertices.push_back( GL_Mesh16_Vertex( x, y, z, t ) );
    }

    m.upload( true );
}



#if 0

void create2DWav_LineStrip(	GL_Mesh16 & m,
                        glm::vec3 const & d,
                        float* __restrict__ src, size_t n )
{

    m.PrimType = de::gpu::PrimitiveType::Triangles;
    m.Vertices.clear();
    m.Vertices.reserve( /* top */ cols * rows +
                       /* front */ 2*cols );
    m.Indices.clear();
    m.Indices.reserve( /* top */ ((cols - 1) * (rows - 1) * 6) +
                      /* front */((cols - 1) * 6) );

    const float dx = d.x / float(cols-1);
    const float dy = d.y / 2.0f;
    const float dz = d.z / float(rows-1);

    // [Top] out vertices: pcm expected in range {-1,1}:
    for (size_t j = 0; j < rows; j++)
    {
        const float* row = table.m_view[j];
        for (size_t i = 0; i < cols; i++)
        {
            const float s = row[i]; // expected in range [-1,1]
            const float t = std::clamp((0.5f * s) + 0.5f, 0.f, 1.f); // expected in range [0,1]
            const float x = dx * i;
            const float y = dy * s;
            const float z = dz * j;
            m.Vertices.push_back( GL_Mesh16_Vertex( x, y, z, t ) );
        }
    }
    // [Top] out quads:
    for (size_t j = 0; j < rows-1; j++)
    {
        for (size_t i = 0; i < cols-1; i++)
        {
            const uint32_t A = (cols * j) + i;           // A - 0
            const uint32_t B = (cols * (j+1)) + i;       // B - 1
            const uint32_t C = (cols * (j+1)) + i+1; // C - 3
            const uint32_t D = (cols * j) + i+1;     // D - 2
            m.addIndexedQuad( A,B,C,D );
        }
    }

    // [Front] vertices:
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
    // [Front] indices:
    for (size_t i = 0; i < cols - 1; ++i)
    {
        const uint32_t A = nF + 2*i;          // A - 0
        const uint32_t B = nF + 2*i + 1;      // B - 1
        const uint32_t C = nF + 2*(i+1) + 1;  // C - 3
        const uint32_t D = nF + 2*(i+1);      // D - 2
        m.addIndexedQuad( A,B,C,D );
    }

    // [Left] vertices:
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
    // [Left] indices:
    for (size_t i = 0; i < rows - 1; i++)
    {
        const uint32_t A = nL + 2*i;          // A - 0
        const uint32_t B = nL + 2*i + 1;      // B - 1
        const uint32_t C = nL + 2*(i+1) + 1;  // C - 3
        const uint32_t D = nL + 2*(i+1);      // D - 2
        m.addIndexedQuad( A,B,C,D );
    }

    // [Right] vertices:
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
    // Right indices:
    for (size_t i = 0; i < rows - 1; i++)
    {
        const uint32_t A = nR + 2*i;          // A - 0
        const uint32_t B = nR + 2*i + 1;      // B - 1
        const uint32_t C = nR + 2*(i+1) + 1;  // C - 3
        const uint32_t D = nR + 2*(i+1);      // D - 2
        m.addIndexedQuad( A,B,C,D );
    }

    m.upload( true );
}


#endif