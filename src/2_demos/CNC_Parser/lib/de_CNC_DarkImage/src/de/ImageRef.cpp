#include <de/ImageRef.hpp>

namespace de {

ImageRef::ImageRef()
   : m_img( nullptr ), m_rect( 0,0,0,0 )
{
}

ImageRef::ImageRef( Image const * src )
   : m_img( const_cast< Image* >( src ) ), m_rect( 0,0,0,0 )
{
   if ( m_img )
   {
      m_rect = Recti( 0, 0, m_img->getWidth(), m_img->getHeight() );
   }
}

ImageRef::ImageRef( Image const * src, Recti const & region )
   : m_img( const_cast< Image* >( src ) ), m_rect( region )
{}

Image
ImageRef::copyImage() const
{
   if ( !m_img ) return {};

   int32_t w = m_rect.w();
   int32_t h = m_rect.h();
   int32_t x = m_rect.x();
   int32_t y = m_rect.y();

   Image img( w,h );

   for ( int32_t j = 0; j < h; ++j )
   {
      for ( int32_t i = 0; i < w; ++i )
      {
         img.setPixel( i,j, m_img->getPixel( x+i, y+j ) );
      }
   }
   return img;
}

uint32_t
ImageRef::pixel( int32_t i, int32_t j ) const
{
   if ( !m_img ) return 0x00000000;
   return m_img->getPixel( i + x(), j + y() );
}

Rectf
ImageRef::texCoords( bool useOffset ) const
{
   if ( !m_img ) return Rectf();
   int w = m_img->getWidth();
   int h = m_img->getHeight();
   return Rectf::fromRecti( m_rect, w, h, useOffset );
}

std::string
ImageRef::toString() const
{
   std::stringstream s; s <<
   "X" << x() << "_"
   "Y" << y() << "_"
   "W" << w() << "_"
   "H" << h() << "_"
   "TexCoords" << texCoords().toString() << "_"
   "S" << getImage();
   return s.str();
}



} // end namespace de.



/*
// =======================================================================
struct TexUVCoords
// =======================================================================
{
   float u1;
   float v1;
   float u2;
   float v2;

   TexUVCoords()
      : u1( 0.f ), v1( 0.f ), u2( 1.f ), v2( 1.f )
   {}
   TexUVCoords( float U1, float V1, float U2, float V2 )
      : u1( U1 ), v1( V1 ), u2( U2 ), v2( V2 )
   {}

   std::string
   toString() const
   {
      std::ostringstream s;
      s << u1 << "," << v1 << "," << u2 << "," << v2;
      return s.str();
   }

   static TexUVCoords
   fromSubImage( Recti const & pos, Image const * img, bool useOffset = true )
   {
      TexUVCoords retVal;
      if ( img )
      {
         int w = img->getWidth();
         int h = img->getHeight();
         retVal.u1 = computeU1( pos.getX1(), w, useOffset );
         retVal.v1 = computeV1( pos.getY1(), h, useOffset );
         retVal.u2 = computeU2( pos.getX2(), w, useOffset );
         retVal.v2 = computeV2( pos.getY2(), h, useOffset );
      }
      return retVal;
   }

//   float getU1( bool useOffset = true ) const
//   {
//      return img ? computeU1( getX1(), img->getWidth(), useOffset ) : 0.0f;
//   }
//   float getV1( bool useOffset = true ) const
//   {
//      return img ? computeV1( getY1(), img->getHeight(), useOffset ) : 0.0f;
//   }
//   float getU2( bool useOffset = true ) const
//   {
//      return img ? computeU2( getX2(), img->getWidth(), useOffset ) : 1.0f;
//   }
//   float getV2( bool useOffset = true ) const
//   {
//      return img ? computeV2( getY2(), img->getHeight(), useOffset ) : 1.0f;
//   }

   static float
   computeU1( int32_t x1, int32_t w, bool useOffset = true )
   {
      if ( w < 2 ) return 0.f;
      if ( useOffset )
         return ( 0.5f + float( x1 )) / float( w );
      else
         return float( x1 ) / float( w );
   }

   static float
   computeV1( int32_t y1, int32_t h, bool useOffset = true )
   {
      if ( h < 2 ) return 0.f;
      if ( useOffset )
         return ( 0.5f + float( y1 )) / float( h );
      else
         return float( y1 ) / float( h );
   }

   static float
   computeU2( int32_t x2, int32_t w, bool useOffset = true )
   {
      if ( w < 2 ) return 1.f;
      if ( useOffset )
         return ( 0.5f + float( x2 )) / float( w );
      else
         return float( x2+1 ) / float( w );
   }

   static float
   computeV2( int32_t y2, int32_t h, bool useOffset = true )
   {
      if ( h < 2 ) return 1.f;
      if ( useOffset )
         return ( 0.5f + float( y2 )) / float( h );
      else
         return float( y2+1 ) / float( h );
   }

};

   float getU1( bool useOffset = true ) const
   {
      return img ? computeU1( getX1(), img->getWidth(), useOffset ) : 0.0f;
   }
   float getV1( bool useOffset = true ) const
   {
      return img ? computeV1( getY1(), img->getHeight(), useOffset ) : 0.0f;
   }
   float getU2( bool useOffset = true ) const
   {
      return img ? computeU2( getX2(), img->getWidth(), useOffset ) : 1.0f;
   }
   float getV2( bool useOffset = true ) const
   {
      return img ? computeV2( getY2(), img->getHeight(), useOffset ) : 1.0f;
   }

   static float
   computeU1( int32_t x1, int32_t w, bool useOffset = true )
   {
      if ( w < 2 ) return 0.f;
      if ( useOffset )
         return ( 0.5f + float( x1 )) / float( w );
      else
         return float( x1 ) / float( w );
   }

   static float
   computeV1( int32_t y1, int32_t h, bool useOffset = true )
   {
      if ( h < 2 ) return 0.f;
      if ( useOffset )
         return ( 0.5f + float( y1 )) / float( h );
      else
         return float( y1 ) / float( h );
   }

   static float
   computeU2( int32_t x2, int32_t w, bool useOffset = true )
   {
      if ( w < 2 ) return 1.f;
      if ( useOffset )
         return ( 0.5f + float( x2 )) / float( w );
      else
         return float( x2+1 ) / float( w );
   }

   static float
   computeV2( int32_t y2, int32_t h, bool useOffset = true )
   {
      if ( h < 2 ) return 1.f;
      if ( useOffset )
         return ( 0.5f + float( y2 )) / float( h );
      else
         return float( y2+1 ) / float( h );
   }

TexPatchCoords
ImageRef::getTexCoords( bool useOffset ) const
{
   auto coords = TexPatchCoords( getU1( useOffset ), getV1( useOffset ),
                             getU2( useOffset ), getV2( useOffset ) );
   //DE_DEBUG(toString())
   return coords;
}

float
ImageRef::getU1( bool useOffset ) const
{
   if ( !getImage() ) return 0.f;
   int32_t w = getImage()->getWidth();
   if ( w < 2 ) return 0.f;
   if ( useOffset )
      return ( 0.5f + float( getX1() )) / float( w );
   else
      return float( getX1() ) / float( w );
}

float
ImageRef::getV1( bool useOffset ) const
{
   if ( !getImage() ) return 0.f;
   int32_t h = getImage()->getHeight();
   if ( h < 2 ) return 0.f;
   if ( useOffset )
      return ( 0.5f + float( getY1() )) / float( h );
   else
      return float( getY1() ) / float( h );
}

float
ImageRef::getU2( bool useOffset ) const
{
   if ( !getImage() ) return 1.f;
   int32_t w = getImage()->getWidth();
   if ( w < 2 ) return 1.f;
   if ( useOffset )
      return ( 0.5f + float( getX2() )) / float( w );
   else
      return float( getX2()+1 ) / float( w );
}

float
ImageRef::getV2( bool useOffset ) const
{
   if ( !getImage() ) return 1.f;
   int32_t h = getImage()->getHeight();
   if ( h < 2 ) return 1.f;
   if ( useOffset )
      return ( 0.5f + float( getY2() )) / float( h );
   else
      return float( getY2()+1 ) / float( h );
}

*/
