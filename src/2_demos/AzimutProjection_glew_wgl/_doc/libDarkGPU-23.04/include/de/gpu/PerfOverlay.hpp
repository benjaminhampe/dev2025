#pragma once
#include <de/scene/SMeshSceneNode.hpp>

namespace de {
namespace gpu {

struct VideoDriver;

// ===========================================================================
struct PerfOverlay
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.PerfOverlay")
   VideoDriver* m_driver;
   //scene::SMeshSceneNode* m_model;

   PerfOverlay( VideoDriver* driver ) : m_driver( driver ) {}
   ~PerfOverlay() {}
   void setDriver( VideoDriver* driver ) { m_driver = driver; }
   //void setModel( SMeshSceneNode* model ) { m_model = model; }
   void draw2D();
};

} // end namespace gpu.
} // end namespace de.
