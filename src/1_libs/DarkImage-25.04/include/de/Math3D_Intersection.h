#pragma once
#include <de/Math3D.h>

namespace de {

    // =======================================================================
    template < typename T > 
    struct IntersectResult
    // =======================================================================
    {
       glm::tvec3< T > hitPos;
       T hitDist = T(0);
       Triangle3< T > hitTriangle;
       int32_t hitTriIndex = -1;
    };

    // =======================================================================
    template < typename T >
    bool
    intersectRay( const std::vector< Triangle3< T > > &triangles,
                  const Ray3< T > &ray, IntersectResult< T > &result )
    // =======================================================================                  
    {
        // if ( !intersectRayWithBoundingBox( ray, result ) ) { return false; }

        T const g_distMax = std::numeric_limits< T >::max();
        T g_hitDist = g_distMax;
        glm::tvec3< T > g_hitPos;
        uint32_t g_triIndex = 0;
        Triangle3< T > g_triangle;

        bool g_doesIntersect = false;

        for (size_t i = 0; i < triangles.size(); ++i )
        {
            const Triangle3< T >& l_triangle = triangles[i];
            glm::tvec3< T > l_hitPos;
            if ( l_triangle.intersectRay( ray.getPos(), ray.getDir(), l_hitPos ) )
            {
                g_doesIntersect = true;
                auto l_hitDist = glm::length( ray.getPos() - l_hitPos );
                if ( g_hitDist == g_distMax || g_hitDist >= l_hitDist )
                {
                    g_triangle = l_triangle;
                    g_hitDist = l_hitDist;
                    g_hitPos = l_hitPos;
                    g_triIndex = i;
                }
            }
        }

        if ( g_doesIntersect )
        {
            result.hitTriangle = g_triangle;
            result.hitTriIndex = g_triIndex; // triangle of bbox 0..11
            result.hitDist = g_hitDist;
            result.hitPos = g_hitPos;
        }

        return g_doesIntersect;
   }
   
} // end namespace de

