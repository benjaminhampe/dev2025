#pragma once
#include <de_core/de_Logger.h>
#include <de_glm.hpp>

namespace de {

struct Rectf;

struct Recti
{
   DE_CREATE_LOGGER("de.Recti")
   
   Recti( int dummy = 0 );
   Recti( int32_t x, int32_t y, int32_t w, int32_t h );
   Recti( Recti const & r );
   bool operator==( Recti const & o ) const;
   bool operator!=( Recti const & o ) const;
   void reset();
   void setWidth( int32_t w );
   void setHeight( int32_t h );
   void set( int32_t x, int32_t y, int32_t w, int32_t h );
   static Recti fromPoints( int32_t x1, int32_t y1, int32_t x2, int32_t y2 );
   void addInternalPoint( glm::ivec2 const & p );
   void addInternalPoint( int32_t x, int32_t y );

   inline bool isPointInside( int mx, int my ) const
   {
      int x1 = getX1();
      int y1 = getY1();
      int x2 = getX2();
      int y2 = getY2();
      if ( x1 > x2 ) std::swap( x1, x2 );
      if ( y1 > y2 ) std::swap( y1, y2 );
      if ( mx < x1 ) return false;
      if ( my < y1 ) return false;
      if ( mx > x2 ) return false;
      if ( my > y2 ) return false;
      return true;
   }

   int32_t x() const;
   int32_t y() const;
   int32_t w() const;
   int32_t h() const;
   int32_t centerX() const;
   int32_t centerY() const;
   int32_t x1() const;
   int32_t y1() const;
   int32_t x2() const;
   int32_t y2() const;
   glm::ivec2 pos() const;
   glm::ivec2 size() const;
   glm::ivec2 center() const;
   glm::ivec2 topLeft() const;
   glm::ivec2 bottomRight() const;

   int32_t getX() const;
   int32_t getY() const;
   int32_t getWidth() const;
   int32_t getHeight() const;
   int32_t getCenterX() const;
   int32_t getCenterY() const;
   int32_t getX1() const;
   int32_t getY1() const;
   int32_t getX2() const;
   int32_t getY2() const;
   glm::ivec2 getPos() const;
   glm::ivec2 getSize() const;
   glm::ivec2 getCenter() const;
   glm::ivec2 getTopLeft() const;
   glm::ivec2 getBottomRight() const;

   std::string toString() const;

//   Recti operator+ ( int32_t v ) const { return Recti( *this ) += v; }
//   Recti operator- ( int32_t v ) const { return Recti( *this ) -= v; }
//   Recti operator* ( int32_t v ) const { return Recti( *this ) *= v; }
//   Recti operator/ ( int32_t v ) const { return Recti( *this ) /= v; }

   // Translate x,y. (w,h) stay untouched.
//   Recti& operator+= ( glm::ivec2 const & p ) { x += p.x; y += p.y; return *this; }
//   Recti& operator-= ( glm::ivec2 const & p ) { x -= p.x; y -= p.y; return *this; }
//   Recti& operator+= ( int32_t v ) { x += v; y1 += v; x2 += v; y2 += v; return *this; }
//   Recti& operator-= ( int32_t v ) { x -= v; y1 -= v; x2 -= v; y2 -= v; return *this; }
//   Recti& operator*= ( int32_t v ) { x *= v; y *= v; x2 *= v; y2 *= v; return *this; }
//   Recti& operator/= ( int32_t v ) { x /= v; y /= v; x2 /= v; y2 /= v; return *this; }

   bool contains( int mx, int my, int touchWidth = 0 ) const;
   glm::vec4 toVec4( int atlas_w, int atlas_h ) const;

   static void test();

   constexpr static int32_t const s_Min = std::numeric_limits< int32_t >::lowest();
   constexpr static int32_t const s_Max = std::numeric_limits< int32_t >::max();

   int32_t m_x;
   int32_t m_y;
   int32_t m_w;
   int32_t m_h;

};

} // end namespace de.

inline std::ostream &
operator<< ( std::ostream & o, de::Recti const & rect )
{
   o << rect.toString();
   return o;
}
