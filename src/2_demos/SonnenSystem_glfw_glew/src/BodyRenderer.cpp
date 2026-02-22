#include "BodyRenderer.h"

BodyRenderer::BodyRenderer()
    : m_driver( nullptr )
{

}

void BodyRenderer::init( de::gl::Bridge* driver )
{
    m_driver = driver;
    if (!m_driver)
    {
        throw std::runtime_error("No driver in BodyRenderer.init()");
    }
    m_planetRenderer.init( driver );
}

void BodyRenderer::draw( const Body& body )
{
    m_planetRenderer.draw( body );
}

