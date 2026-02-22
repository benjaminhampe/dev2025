#pragma once
#include "Body.h"
#include "eBT_PlanetRenderer.h"



// Normals only tesselated and lighted planet/moon renderer, mesh must be a sphere.
struct BodyRenderer
{
    BodyRenderer();

    void init( de::gl::Bridge* driver );

    void draw( const Body& body );

    de::gl::Bridge* m_driver;

    eBT_PlanetRenderer m_planetRenderer;
};
