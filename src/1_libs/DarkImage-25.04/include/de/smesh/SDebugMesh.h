#pragma once
#include <de/smesh/SMesh.h>

namespace de {
namespace smesh {

struct SDebugMesh
{
    static void
    addNormals( const SMeshBuffer& in, SMeshBuffer& out, float len )
    {
        typedef glm::vec2 V2;
        typedef glm::vec3 V3;

        uint32_t colorA = 0xFF0000FF; // red
        uint32_t colorB = 0xFF0080FF; // orange

        out.primitiveType = gpu::PrimitiveType::Lines;

        if (out.indices.empty())
        {
            for (const auto& v : in.vertices)
            {
                out.vertices.emplace_back( v.pos, V3(0,0,0), colorA, V2(0,0) );
                out.vertices.emplace_back( v.pos + (v.normal * len), V3(0,0,0), colorB, V2(0,0) );
            }
        }
        else
        {
            uint32_t v0 = in.vertices.size();
            uint32_t k0 = 0;
            for (const auto& v : in.vertices)
            {
                out.vertices.emplace_back( v.pos, V3(0,0,0), colorA, V2(0,0) );
                out.vertices.emplace_back( v.pos + (v.normal * len), V3(0,0,0), colorB, V2(0,0) );
                out.indices.emplace_back( v0 + k0 );
                out.indices.emplace_back( v0 + k0 + 1 );
                k0 += 2;
            }
        }

        out.recalculateBoundingBox();
    }
};

} // end namespace gpu.
} // end namespace de.
