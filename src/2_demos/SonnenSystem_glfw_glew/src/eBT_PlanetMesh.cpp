#include "eBT_PlanetMesh.h"
#include "OpenGLBridge.h"

// VertexSize = 20 Bytes (x,y,z,u,v)

// HIGH QUALITY (default): i have 8k textures and tesselation max. 64)
// Sphere 128 x 64 (with 64x tesselation = 8k x 4k)
// nVertices = 8385 [163.77kB] = (tessLon + 1) * (tessLat + 1) -> indices can 16bit.
// nIndices = 49152 [96kB] = tessLon * tessLat * 6

// EXTREME QUALITY:
// Sphere 256 x 128: (with 64x tesselation = 16k x 8k)
// nVertices = 33153 [647.52kB] = (tessLon + 1) * (tessLat + 1) -> indices can 16bit.
// nIndices = 196608 [384.00kB] = tessLon * tessLat * 6 ->

// 64 x 64: with 64x tesselation = 4k
// nVertices = 4225 = (tessLon + 1) * (tessLat + 1) -> indices can 16bit.
// nIndices = 24576 = tessLon * tessLat * 6

// 255 x 255: with 64x tesselation = ca. 16k
// nVertices = 65536 = (tessLon + 1) * (tessLat + 1) -> indices can 16bit.
// nIndices = 390150 = tessLon * tessLat * 6

void
eBT_PlanetMesh::upload()
{
    if (vao < 1)
    {
        glGenVertexArrays(1, &vao);
    }

    if (vbo < 1)
    {
        glGenBuffers(1, &vbo);
    }

    if (ibo < 1 && indices.size() > 0)
    {
        glGenBuffers(1, &ibo);
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(float),
                 vertices.data(),
                 GL_STATIC_DRAW);

    const GLsizei sizeofVertex = 5 * sizeof(float);
    size_t vOffset = 0;
    int k = 0;

    // a_pos
    // glVertexAttribPointer(k, 3, GL_FLOAT, GL_FALSE, sizeofVertex, (void*)vOffset);
    // glEnableVertexAttribArray(k);
    // vOffset += 3 * sizeof(float);
    // k++;

    // a_normal
    glVertexAttribPointer(k, 3, GL_FLOAT, GL_FALSE, sizeofVertex, (void*)vOffset);
    glEnableVertexAttribArray(k);
    vOffset += 3 * sizeof(float);
    k++;

    // a_texcoord
    glVertexAttribPointer(k, 2, GL_FLOAT, GL_FALSE, sizeofVertex, (void*)vOffset);
    glEnableVertexAttribArray(k);
    //vOffset += 2 * sizeof(float);
    //k++;

    if (indices.size() > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(uint16_t),
                     indices.data(),
                     GL_STATIC_DRAW);
    }

    // Unbind:
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (ibo > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void eBT_PlanetMesh::draw() const
{
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    glBindVertexArray(vao);

    glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_SHORT, 0);
}


// static
glm::vec3
eBT_PlanetMesh::computeSphereNormal( float lon, float lat ) // in degrees
{
    constexpr float DEG2RAD = M_PI / 180.0;
    if ( lat >= 89.999f ) { return glm::vec3( 0,1,0 ); } // NorthPole
    else if ( lat <= -89.999f ) { return glm::vec3( 0,-1,0 ); } // SouthPole
    float a = lon * DEG2RAD;
    float b = lat * DEG2RAD;
    float x = ::cosf( b ) * ::cosf( a );
    float y = ::sinf( b );
    float z = ::cosf( b ) * ::sinf( a );
    return glm::vec3( x,y,z );
}

// Sphere generation
void
eBT_PlanetMesh::create(int tessLon, int tessLat)
{
    if ( tessLon < 3 ) tessLon = 3;
    if ( tessLat < 3 ) tessLat = 3;

    // u-step
    float lonStep = 360.0f / float( tessLon );
    float lonStart = 0.0f; // Greenwich
    //float lonEnd = 360.0f;
    float lonUV = 1.0f / float( tessLon );

    // v-step
    float latStep = -180.0f / float( tessLat );
    float latStart = 90.0f;
    //float latEnd = -90.0f;
    float latUV = 1.0f / float( tessLat );

    // Predict and reserve memory, indexed quads.
    //o.moreVertices( 4 * tessLon * tessLat );
    //o.moreIndices( 6 * tessLon * tessLat );
    vertices.reserve( (tessLon + 1) * (tessLat + 1) );
    indices.reserve( tessLon * tessLat * 6 );

    for (size_t j = 0; j <= tessLat; ++j)
    {
        for (size_t i = 0; i <= tessLon; ++i)
        {
            float lon = lonStart + lonStep * i;
            float lat = latStart + latStep * j;
            auto n = computeSphereNormal( lon, lat );
            //auto p = n * radius + offset;
            float u = lonUV * i;
            float v = latUV * j;
            //vertices.push_back(p.x); // position
            //vertices.push_back(p.y);
            //vertices.push_back(p.z);
            vertices.push_back(n.x); // normal
            vertices.push_back(n.y);
            vertices.push_back(n.z);
            vertices.push_back(u);   // texcoord
            vertices.push_back(v);
        }
    }

    uint32_t stride = tessLon + 1;

    for (size_t j = 0; j < tessLat; ++j)
    {
        for (size_t i = 0; i < tessLon; ++i)
        {
            // int i0 = r * sectors + s;
            // int i1 = r * sectors + (s + 1);
            // int i2 = (r + 1) * sectors + (s + 1);
            // int i3 = (r + 1) * sectors + s;

            uint32_t i2 = i + 1;
            uint32_t j2 = j + 1;

            //if (i >= tessLon - 1) { i2 = 0; }
            //if (j >= tessLat - 1) { j2 = 0; }

            uint32_t a = j * stride + i;
            uint32_t b = j * stride + i2;
            uint32_t c = j2 * stride + i2;
            uint32_t d = j2 * stride + i;

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(d);
        }
    }
}
