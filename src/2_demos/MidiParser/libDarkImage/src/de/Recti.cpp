#include <de/Recti.hpp>
#include <de/Rectf.hpp>

namespace de {

Recti::Recti( int dummy )
   : m_x(0)
   , m_y(0)
   , m_w(0)
   , m_h(0)
{}

Recti::Recti( int32_t x, int32_t y, int32_t w, int32_t h )
   : m_x(0)
   , m_y(0)
   , m_w(0)
   , m_h(0)
{
   set( x, y, w, h );
}

Recti::Recti( Recti const & r )
   : m_x(r.m_x)
   , m_y(r.m_y)
   , m_w(r.m_w)
   , m_h(r.m_h)
{}

bool
Recti::operator==( Recti const & o ) const
{
   return m_x == o.m_x && m_y == o.m_y && m_w == o.m_w && m_h == o.m_h;
}

bool
Recti::operator!=( Recti const & o ) const
{
   return !( o == *this );
}

void
Recti::reset()
{
   m_x = m_y = m_w = m_h = 0;
}

void
Recti::setWidth( int32_t w )
{
   if ( w >= 0 )
   {
      m_w = w;
   }
   else
   {
      m_w = 0;
   }
}

void
Recti::setHeight( int32_t h )
{
   if ( h >= 0 )
   {
      m_h = h;
   }
   else
   {
      m_h = 0;
   }
}

void
Recti::set( int32_t x, int32_t y, int32_t w, int32_t h )
{
   m_x = x;
   m_y = y;
   setWidth( w );
   setHeight( h );
}

//static
Recti
Recti::fromPoints( int32_t x1, int32_t y1, int32_t x2, int32_t y2 )
{
   if ( x1 > x2 ) std::swap( x1, x2 );
   if ( y1 > y2 ) std::swap( y1, y2 );
   int w = x2 - x1 + 1;
   int h = y2 - y1 + 1;
   w = std::clamp( w, 0, 1024*1024*1024 ); // clamp w in [0,enough]
   h = std::clamp( h, 0, 1024*1024*1024 ); // clamp h in [0,enough]
   return Recti( x1, y1, w, h );
}

void
Recti::addInternalPoint( glm::ivec2 const & p )
{
   addInternalPoint( p.x, p.y );
}

void
Recti::addInternalPoint( int32_t x, int32_t y )
{
   // handle x
   if ( x < m_x ) // x is lower, reposition rect, increase rect size
   {
      int32_t delta = m_x - x; // groesser - kleiner
      m_x = x;
      m_w += delta;
   }
   else // x is inside rect ( nothing todo ), or right from it ( increase rect size )
   {
      int32_t x2 = m_x + m_w - 1;
      if ( x > x2 ) // x is right from rectangle ( must increase rect size, no pos change )
      {
         int32_t delta = x - x2; // groesser - kleiner
         m_w += delta; // increase w by portion that was not covered from x2 to _x
      }
      // else  // x is inside rect ( nothing todo )
   }

   // handle y
   if ( y < m_y )
   {
      int32_t delta = m_y - y; // groesser - kleiner
      m_y = y;
      m_h += delta;
   }
   else // y is inside rect ( nothing todo ), or below from it ( increase rect size )
   {
      int32_t y2 = m_y + m_h - 1;
      if ( y > y2 ) // y is below rectangle ( must increase rect size, no pos change )
      {
         int32_t delta = y - y2; // groesser - kleiner
         m_h += delta; // increase h by portion that was not covered from y2 to _y
      }
      // else  // y is inside rect ( nothing todo )
   }
}

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

int32_t     Recti::x() const { return m_x; }
int32_t     Recti::y() const { return m_y; }
int32_t     Recti::w() const { return m_w; }
int32_t     Recti::h() const { return m_h; }
int32_t     Recti::centerX() const { return m_x + m_w/2; }
int32_t     Recti::centerY() const { return m_y + m_h/2; }
int32_t     Recti::x1() const { return m_x; }
int32_t     Recti::y1() const { return m_y; }
int32_t     Recti::x2() const { return m_w > 1 ? m_x + m_w - 1 : m_x; }
int32_t     Recti::y2() const { return m_h > 1 ? m_y + m_h - 1 : m_y; }
glm::ivec2  Recti::pos() const { return { m_x, m_y }; }
glm::ivec2  Recti::size() const { return { m_w, m_h }; }
glm::ivec2  Recti::center() const { return { centerX(), centerY() }; }
glm::ivec2  Recti::topLeft() const { return { m_x, m_y }; }
glm::ivec2  Recti::bottomRight() const { return { x2(), y2() }; }

int32_t     Recti::getX() const { return x(); }
int32_t     Recti::getY() const { return y(); }
int32_t     Recti::getWidth() const { return w(); }
int32_t     Recti::getHeight() const { return h(); }
int32_t     Recti::getCenterX() const { return centerX(); }
int32_t     Recti::getCenterY() const { return centerY(); }
int32_t     Recti::getX1() const { return x1(); }
int32_t     Recti::getY1() const { return y1(); }
int32_t     Recti::getX2() const { return x2(); }
int32_t     Recti::getY2() const { return y2(); }
glm::ivec2  Recti::getPos() const { return pos(); }
glm::ivec2  Recti::getSize() const { return size(); }
glm::ivec2  Recti::getCenter() const { return center(); }
glm::ivec2  Recti::getTopLeft() const { return topLeft(); }
glm::ivec2  Recti::getBottomRight() const { return bottomRight(); }

std::string Recti::toString() const
{
   std::ostringstream s;
   s << m_x << "," << m_y << "," << m_w << "," << m_h;
   return s.str();
}

bool        Recti::contains( int mx, int my, int touchWidth ) const
{
   if ( mx < x1() + touchWidth ) return false;
   if ( my < y1() + touchWidth ) return false;
   if ( mx > x2() - touchWidth ) return false;
   if ( my > y2() - touchWidth ) return false;
   return true;
}

glm::vec4   Recti::toVec4( int atlas_w, int atlas_h ) const
{
   float fx = float( x() ) / float( atlas_w-1 );
   float fy = float( y() ) / float( atlas_h-1 );
   float fw = float( w() ) / float( atlas_w );
   float fh = float( h() ) / float( atlas_h );
   return glm::vec4( fx, fy, fw, fh );
}

void
Recti::test()
{
   // DE_DEBUG("typename(int32_t) = ", typename(int32_t) )
   DE_DEBUG("s_Min = ", s_Min )
   DE_DEBUG("s_Max = ", s_Max )

   Recti a( 0,0,100,50);
   Recti b( 50,50,33,66);
   Recti c( 33,33,21,123);
   DE_DEBUG("abc_width = ", a.w() + b.w() + c.w() )
   DE_DEBUG("abc_height = ", a.h() + b.h() + c.h() )

//      if ( a.getWidth() + b.getWidth() + c.getWidth() != 239 )
//      {
//         throw std::runtime_error( "Rect< int32_t >.getWidth() failed" );
//      }

//      if ( a.getHeight() + b.getHeight() + c.getHeight() != 239 )
//      {
//         throw std::runtime_error( "Rect< int32_t >.getHeight() failed" );
//      }
}

} // end namespace de.
