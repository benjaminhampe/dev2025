#pragma once
#include <de/Geom3D_Sphere.h>
#include <de/gpu/mtl/PMesh.h>

namespace de {
namespace gpu {

    void addSphere( PMeshBuffer & out, bool asIndexedTriangles,
               const glm::vec3 & siz = glm::vec3(100,100,100),
               const uint32_t tessLon = 12,
               const uint32_t tessLat = 12,
               const glm::vec3 & offset = glm::vec3(0,0,0) )
    {
        // As IndexedTriangles:
        if (asIndexedTriangles || out.m_indices.size() > 0)
        {
            auto getVertexCount = [&out](void)
            {
                return out.m_vertices.size();
            };

            auto addVertex = [&out]( glm::vec3 pos,
                                     glm::vec3 normal,
                                     glm::vec2 texcoords )
            {
                auto tangent = glm::normalize(glm::vec3(texcoords,0));
                out.m_vertices.emplace_back(pos,normal,tangent,0xFFFFFFFF,texcoords);
                out.m_bNeedUpload = true;
            };

            auto addIndexedTriangle = [&out]( uint32_t a,
                                              uint32_t b,
                                              uint32_t c )
            {
                out.m_indices.emplace_back(a);
                out.m_indices.emplace_back(b);
                out.m_indices.emplace_back(c);
            };

            Geom3D_Sphere::addIndexedTriangles(
                getVertexCount,
                addVertex,
                addIndexedTriangle,
                siz, tessLon, tessLat, offset);
        }
        // As VertexTriangles:
        else
        {
            auto addQuad = [&out]( glm::vec3 pA, glm::vec3 nA, glm::vec2 tcA,
                glm::vec3 pB, glm::vec3 nB, glm::vec2 tcB,
                glm::vec3 pC, glm::vec3 nC, glm::vec2 tcC,
                glm::vec3 pD, glm::vec3 nD, glm::vec2 tcD )
            {
                glm::vec3 T = glm::normalize(pD-pA);
                out.m_vertices.emplace_back(pA,nA,T,0xFFFFFFFF,tcA);
                out.m_vertices.emplace_back(pB,nB,T,0xFFFFFFFF,tcB);
                out.m_vertices.emplace_back(pC,nC,T,0xFFFFFFFF,tcC);

                out.m_vertices.emplace_back(pA,nA,T,0xFFFFFFFF,tcA);
                out.m_vertices.emplace_back(pC,nC,T,0xFFFFFFFF,tcC);
                out.m_vertices.emplace_back(pD,nD,T,0xFFFFFFFF,tcD);
                out.m_bNeedUpload = true;

            };

            Geom3D_Sphere::addTriangles( addQuad, siz,
                            tessLon, tessLat, offset );
        }

/*
        //typedef std::function< void( V3 pos, V3 normal, V2 texcoords ) > FN_ADD_VERTEX;
        typedef std::function< void( V3, V3, V2,
                                   V3, V3, V2,
                                   V3, V3, V2,
                                   V3, V3, V2 ) > FN_ADD_QUAD;

        // VertexOnly

*/
        // As IndexedTriangles:
/*
        typedef std::function< C4(void) >
            FN_GET_VERTEX_COUNT;
        typedef std::function< void( V3 pos, V3 normal, V2 texcoords ) >
            FN_ADD_VERTEX;
        typedef std::function< void( C4 a, C4 b, C4 c ) >
            FN_ADD_INDEX_TRIANGLE;

        // GL_TRIANGLES with IBO
        static void
        addIndexedTriangles( const FN_GET_VERTEX_COUNT & getVertexCount,
                             const FN_ADD_VERTEX & addVertex,
                             const FN_ADD_INDEX_TRIANGLE & addIndexedTriangle,
                             const V3 & siz = V3(100,100,100),
                             const C4 tessLon = 12,
                             const C4 tessLat = 12,
                             const V3 & offset = V3(0,0,0) )
*/
    }
	
} // namespace gpu.
} // namespace de.

