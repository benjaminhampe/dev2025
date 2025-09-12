#pragma once
#include <de/Logger.hpp>
#include <de_glm.hpp>

namespace de {

struct Recti;

//=============================================================================
struct Rectf
//=============================================================================
{
   Rectf();
   Rectf( float x, float y, float w, float h );
   Rectf( Rectf const & other );

   static float computeU1( int32_t x1, int32_t w, bool useOffset = true );
   static float computeV1( int32_t y1, int32_t h, bool useOffset = true );
   static float computeU2( int32_t x2, int32_t w, bool useOffset = true );
   static float computeV2( int32_t y2, int32_t h, bool useOffset = true );

   static Rectf
   fromRecti( Recti const & pos, int32_t w, int32_t h, bool useOffset = true );

   std::string toString() const;
   void zero();

   static Rectf identity();

   float x() const;
   float y() const;
   float w() const;
   float h() const;
   float x1() const;
   float y1() const;
   float x2() const;
   float y2() const;
   float centerX() const;
   float centerY() const;

   glm::tvec2< float > center() const;
   glm::tvec2< float > topLeft() const;
   glm::tvec2< float > bottomRight() const;

   float getX() const;
   float getY() const;
   float getWidth() const;
   float getHeight() const;
   float getX1() const;
   float getY1() const;
   float getX2() const;
   float getY2() const;
   float getCenterX() const;
   float getCenterY() const;
   glm::tvec2< float > getCenter() const;
   glm::tvec2< float > getTopLeft() const;
   glm::tvec2< float > getBottomRight() const;

   Recti toRecti( int image_w = 1, int image_h = 1 ) const;
   void addInternalPoint( glm::tvec2< float > const & point );
   void addInternalPoint( float x, float y );

   float u1() const;
   float v1() const;
   float u2() const;
   float v2() const;
   float du() const;
   float dv() const;

   float getU1() const;
   float getV1() const;
   float getU2() const;
   float getV2() const;
   float getDU() const;
   float getDV() const;

   float m_x;
   float m_y;
   float m_w;
   float m_h;

   DE_CREATE_LOGGER("de.Rectf")

};


} // end namespace de.

//template < typename float >
inline std::ostream &
operator<< ( std::ostream & o, de::Rectf const & rect )
{
   o << rect.toString();
   return o;
}
