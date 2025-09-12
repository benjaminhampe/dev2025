#pragma once
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

// ===========================================================================
struct BaumStumpf
// ===========================================================================
{
   glm::vec3 m_start;
   glm::vec3 m_end;
   float m_startSize;
   float m_endSize;
   uint32_t m_startColor;
   uint32_t m_endColor;
   float m_mass;
   glm::vec3 m_massCenter;
};

// ===========================================================================
struct BaumBlatt
// ===========================================================================
{
   BaumStumpf m_desc;
};

// ===========================================================================
struct BaumAst
// ===========================================================================
{
   BaumStumpf m_desc;
   std::vector< BaumAst > m_aeste;
   std::vector< BaumBlatt > m_leaves;
};

// ===========================================================================
struct BaumWurzel
// ===========================================================================
{
   BaumStumpf m_desc;
   std::vector< BaumAst > m_aeste;
};

// ===========================================================================
struct Baum
// ===========================================================================
{
   BaumWurzel m_wurzel;
};


// ===========================================================================
struct TreeRenderer
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.TreeRenderer")
   VideoDriver* m_driver;
   SMeshBuffer m_baum0;

   TreeRenderer();
   //~TreeRenderer();
   void render();
   void setDriver( VideoDriver* driver );
   void clear();
};

} // end namespace gpu.
} // end namespace de.

