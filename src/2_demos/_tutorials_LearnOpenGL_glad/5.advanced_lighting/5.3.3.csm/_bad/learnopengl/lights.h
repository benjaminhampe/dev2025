#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace learnopengl {

struct Light
{
   glm::vec3 m_pos;
   glm::vec3 m_color; // alpha not really meaningful for a light source
   float m_intensity; // canbe in range 1...200 for hdr bloom uses color over 1, upto 200!
   float m_radius; // 0 means disabled

   Light()
   {
      m_color = glm::vec3(1,1,1);
      m_intensity = 1.0f;
      m_radius = 0.0f;
   }

   void setPos( glm::vec3 const & pos ) { m_pos = pos; }
   void setColor( glm::vec3 const & color ) { m_color = color; }
   void setIntensity( float intensity ) { m_intensity = intensity; }
   void setRadius( float radius ) { m_radius = radius; }
};

struct Lights
{
   std::vector< Light > m_lights;

    // // lighting info
    // // -------------
    // std::vector< Light > lights;
    // lights.push_back(Light( Light::V3(0.0f,  0.0f, 49.5f),
                            // Light::V3(1.0f, 1.0f, 1.0f), 200, 200) ); // back light
    // lights.push_back(Light( Light::V3(-1.4f, -1.9f, 9.0f),
                            // Light::V3(0.5f, 0.0f, 0.0f), 10, 100) );
    // lights.push_back(Light( Light::V3(0.0f, -1.8f, 4.0f),
                            // Light::V3(0.0f, 0.0f, 0.3f), 1, 50) );
    // lights.push_back(Light( Light::V3(0.8f, -1.7f, 6.0f),
                            // Light::V3(0.0f, 0.3f, 0.0f), 1, 50) );

   Lights()
   {
      m_lights.resize( 16 );
      setLight( 0, glm::vec3( 0,  100, 0), glm::vec3(1, 1, 1), 200, 10);
      setLight( 1, glm::vec3(-4.0f, 0.5f, -3.0f), glm::vec3(0.5f, 0.0f, 0.0f), 10, 10);
      setLight( 2, glm::vec3( 3.0f, 0.5f,  1.0f), glm::vec3(0.0f, 0.0f, 0.3f), 10, 10);
      setLight( 3, glm::vec3(-.8f,  2.4f, -1.0f), glm::vec3(0.0f, 0.3f, 0.0f), 10, 10);
   }

   void setLight( size_t i, glm::vec3 pos, glm::vec3 color, float intensity = 1.0f, float radius = 100.0f )
   {
      if ( i + 1 >= m_lights.size() ) m_lights.resize( i+1 );
      Light & light = m_lights[ i ];
      light.setPos( pos );
      light.setColor( color );

      float maxColor = std::max( std::max( color.r, color.g ), color.b );
      if ( maxColor > 1.0f )
      {
         color /= maxColor;
      }

      light.setIntensity( intensity * maxColor );
      light.setRadius( radius );
   }

   size_t capacity() const { return m_lights.capacity(); }
   size_t size() const { return m_lights.size(); }
   std::vector< Light >::iterator begin() { return m_lights.begin(); }
   std::vector< Light >::iterator end() { return m_lights.end(); }
   std::vector< Light >::const_iterator begin() const { return m_lights.begin(); }
   std::vector< Light >::const_iterator end() const { return m_lights.end(); }
   void resize( size_t nElements ) { m_lights.resize( nElements ); }
   void reserve( size_t nElements ) { m_lights.reserve( nElements ); }

   Light const & operator[] ( size_t i ) const
   {
      if ( i >= m_lights.size() )
      {
         //DE_RUNTIME_ERROR(std::to_string(i),"Invalid index")
      }

      return m_lights[ i ];
   }

   Light & operator[] ( size_t i )
   {
      if ( i >= m_lights.size() )
      {
         //DE_RUNTIME_ERROR(std::to_string(i),"Invalid index")
      }

      return m_lights[ i ];
   }
    // // lighting info
    // // -------------
    // // positions
    // std::vector<glm::vec3> lightPositions;
    // lightPositions.push_back(glm::vec3( 0.0f, 0.5f,  1.5f));
    // lightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
    // lightPositions.push_back(glm::vec3( 3.0f, 0.5f,  1.0f));
    // lightPositions.push_back(glm::vec3(-.8f,  2.4f, -1.0f));
    // // colors
    // std::vector<glm::vec3> lightColors;
    // lightColors.push_back(glm::vec3(5.0f,   5.0f,  5.0f));
    // lightColors.push_back(glm::vec3(10.0f,  0.0f,  0.0f));
    // lightColors.push_back(glm::vec3(0.0f,   0.0f,  15.0f));
    // lightColors.push_back(glm::vec3(0.0f,   5.0f,  0.0f));
};


} // end namespace learnopengl.
