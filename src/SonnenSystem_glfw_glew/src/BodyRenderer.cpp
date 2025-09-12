#include "BodyRenderer.h"

void BodyRenderer::init( de::gl::Bridge* driver )
{
    m_driver = driver;
    m_planetRenderer.init( driver );
}

void BodyRenderer::draw( const Body& body )
{
    m_planetRenderer.draw( body );
}

