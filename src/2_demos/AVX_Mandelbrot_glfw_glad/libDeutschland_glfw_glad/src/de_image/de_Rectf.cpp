#include <de_image/de_Rectf.h>
#include <de_image/de_Recti.h>

namespace de {

Rectf::Rectf()
   : m_x( 0 )
   , m_y( 0 )
   , m_w( 0 )
   , m_h( 0 )
{}

Rectf::Rectf( float x, float y, float w, float h )
   : m_x( x )
   , m_y( y )
   , m_w( w )
   , m_h( h )
{}

Rectf::Rectf( Rectf const & other )
   : m_x( other.m_x )
   , m_y( other.m_y )
   , m_w( other.m_w )
   , m_h( other.m_h )
{}

// static
float
Rectf::computeU1( int32_t x1, int32_t w, bool useOffset )
{
   if ( w < 2 ) return float(0);
   if ( useOffset )
      return ( float(0.5) + float( x1 )) / float( w );
   else
      return float( x1 ) / float( w );
}

// static
float
Rectf::computeV1( int32_t y1, int32_t h, bool useOffset )
{
   if ( h < 2 ) return float(0);
   if ( useOffset )
      return ( float(0.5) + float( y1 )) / float( h );
   else
      return float( y1 ) / float( h );
}

// static
float
Rectf::computeU2( int32_t x2, int32_t w, bool useOffset )
{
   if ( w < 2 ) return float(1);
   if ( useOffset )
      return ( float(0.5) + float( x2 )) / float( w );
   else
      return float( x2+1 ) / float( w );
}

// static
float
Rectf::computeV2( int32_t y2, int32_t h, bool useOffset )
{
   if ( h < 2 ) return float(1);
   if ( useOffset )
      return ( float(0.5) + float( y2 )) / float( h );
   else
      return float( y2+1 ) / float( h );
}

// static
Rectf
Rectf::fromRecti( Recti const & pos, int32_t w, int32_t h, bool useOffset )
{
   int32_t x1 = pos.x1();
   int32_t y1 = pos.y1();
   int32_t x2 = pos.x2();
   int32_t y2 = pos.y2();
   float u1 = computeU1( x1, w, useOffset );
   float v1 = computeV1( y1, h, useOffset );
   float u2 = computeU2( x2, w, useOffset );
   float v2 = computeV2( y2, h, useOffset );
//      float u1 = (float(x1)+0.5f) / float( w );
//      float v1 = (float(y1)+0.5f) / float( h );
//      float u2 = (float(x2)+0.5f) / float( w );
//      float v2 = (float(y2)+0.5f) / float( h );
//   float u1 = (float(x1) + float(0.5)) / float( w );
//   float v1 = (float(y1) + float(0.5)) / float( h );
//   float u2 = (float(pw)) / float( w );
//   float v2 = (float(ph)) / float( h );
   return Rectf( u1, v1, u2-u1, v2-v1 );
//   float u1 = (float(x1)+0.5f) / float( w );
//   float v1 = (float(y1)+0.5f) / float( h );
//   float u2 = (float(x2)) / float( w );
//   float v2 = (float(y2)) / float( h );
//   float tw = (float(pos.getWidth())-0.5f) / float( w );
//   float th = (float(pos.getHeight())-0.5f) / float( h );
//   return Rectf( u1, v1, tw, th );
}


std::string
Rectf::toString() const
{
   std::ostringstream s;
   s << m_x << "," << m_y << "," << m_w << "," << m_h;
   return s.str();
}

void
Rectf::zero()
{
   m_x = m_y = m_w = m_h = float(0);
}

// static
Rectf
Rectf::identity() { return Rectf( float(0), float(0), float(1), float(1) ); }

float Rectf::x() const { return m_x; }
float Rectf::y() const { return m_y; }
float Rectf::w() const { return m_w; }
float Rectf::h() const { return m_h; }
float Rectf::x1() const { return x(); }
float Rectf::y1() const { return y(); }
float Rectf::x2() const { return x()+w(); }
float Rectf::y2() const { return y()+h(); }
float Rectf::centerX() const { return m_x + float( 0.5 ) * w(); }
float Rectf::centerY() const { return m_y + float( 0.5 ) * h(); }

glm::tvec2< float > Rectf::center() const { return { centerX(), centerY() }; }
glm::tvec2< float > Rectf::topLeft() const { return { x1(), y1() }; }
glm::tvec2< float > Rectf::bottomRight() const { return { x2(), y2() }; }

float Rectf::getX() const { return x(); }
float Rectf::getY() const { return y(); }
float Rectf::getWidth() const { return w(); }
float Rectf::getHeight() const { return h(); }
float Rectf::getX1() const { return x1(); }
float Rectf::getY1() const { return y1(); }
float Rectf::getX2() const { return x2(); }
float Rectf::getY2() const { return y2(); }
float Rectf::getCenterX() const { return centerX(); }
float Rectf::getCenterY() const { return centerY(); }
glm::tvec2< float > Rectf::getCenter() const { return center(); }
glm::tvec2< float > Rectf::getTopLeft() const { return topLeft(); }
glm::tvec2< float > Rectf::getBottomRight() const { return bottomRight(); }

Recti
Rectf::toRecti( int image_w, int image_h ) const
{
   int x = int(std::roundf( m_x * float(image_w) ));
   int y = int(std::roundf( m_y * float(image_h) ));
   int w = int(std::roundf( m_w * float(image_w) ));
   int h = int(std::roundf( m_h * float(image_h) ));
   return Recti( x, y, w, h );
}

void Rectf::addInternalPoint( glm::tvec2< float > const & point )
{
   addInternalPoint( point.x, point.y );
}

void Rectf::addInternalPoint( float x, float y )
{
   // handle _x
   if ( x < m_x )  // _x is lower, reposition rect, increase rect size
   {
      float delta = m_x - x; // groesser - kleiner
      m_x = x;
      m_w += delta;
   }
   else  // _x is inside rect ( nothing todo ), or right from it ( increase rect size )
   {
      float x2 = m_x + m_w - 1;
      if ( x > x2 ) // _x is right from rectangle ( must increase rect size, no pos change )
      {
         float delta = x - x2; // groesser - kleiner
         m_w += delta; // increase w by portion that was not covered from x2 to _x
      }
      // else  // _x is inside rect ( nothing todo )
   }

   // handle _y
   if ( y < m_y )
   {
      float delta = m_y - y; // groesser - kleiner
      m_y = y;
      m_h += delta;
   }
   else // _y is inside rect ( nothing todo ), or below from it ( increase rect size )
   {
      float y2 = m_y + m_h - 1;
      if ( y > y2 ) // _y is below rectangle ( must increase rect size, no pos change )
      {
         float delta = y - y2; // groesser - kleiner
         m_h += delta; // increase h by portion that was not covered from y2 to _y
      }
      // else  // _y is inside rect ( nothing todo )
   }
}

float Rectf::u1() const { return x1(); }
float Rectf::v1() const { return y1(); }
float Rectf::u2() const { return x2(); }
float Rectf::v2() const { return y2(); }
float Rectf::du() const { return w(); }
float Rectf::dv() const { return h(); }

float Rectf::getU1() const { return x1(); }
float Rectf::getV1() const { return y1(); }
float Rectf::getU2() const { return x2(); }
float Rectf::getV2() const { return y2(); }
float Rectf::getDU() const { return w(); }
float Rectf::getDV() const { return h(); }


} // end namespace de.
