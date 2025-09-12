// Copyright (C) 2002-2014 Benjamin Hampe
#include <de/ColorGradient.h>
#include <de/Math.h>

namespace de {

/*
void
LinearColorGradient::test()
{
   LinearColorGradient testObj;
   testObj.addStop( .0f, 0xFF8F0000 );
   testObj.addStop( .1f, 0xFF000000 );
   testObj.addStop( .25f,0xFFFFFFFF );
   testObj.addStop( .4f, 0xFF00FF00 );
   testObj.addStop( .5f, 0xFF00FFFF );
   testObj.addStop( .7f, 0xFF0000FF );
   testObj.addStop( .8f, 0xFFFF00FF );
   testObj.addStop( .9f, 0xFFFF0000 );
   testObj.addStop( 1.f, 0xFFF8F8FF );

   Image img = testObj.createImage( 256,1024, true );
   dbSaveImage( img, "LinearColorGradient_test1.png" );

   testObj.clearStops();
   testObj.addStop( .0f, 0xFFAAAAAA );
   testObj.addStop( .1f, 0xFF000000 );
   testObj.addStop( .4f, 0xFF00FF00 );
   testObj.addStop( .6f, 0xFF00FFFF );
   testObj.addStop( .8f, 0xFF0000FF );
   testObj.addStop( .9f, 0xFFFF00FF );
   testObj.addStop( 1.f, 0xFFFFFFFF );

   img = testObj.createImage( 1024,256, false );
   dbSaveImage( img, "LinearColorGradient_test2.png" );
}
*/

// static
glm::vec4
LinearColorGradient::lerpColor128( uint32_t A, uint32_t B, float t )
{
   // interpolate between prev and next neighbor color
   //
   // computed color = A + t*AB = A + t*(B-A)
   //
   float const startR = float(dbRGBA_R( A )) / 255.0f;
   float const startG = float(dbRGBA_G( A )) / 255.0f;
   float const startB = float(dbRGBA_B( A )) / 255.0f;
   float const startA = float(dbRGBA_A( A )) / 255.0f;
   float const deltaR = (float(dbRGBA_R( B )) / 255.0f) - startR;
   float const deltaG = (float(dbRGBA_G( B )) / 255.0f) - startG;
   float const deltaB = (float(dbRGBA_B( B )) / 255.0f) - startB;
   float const deltaA = (float(dbRGBA_A( B )) / 255.0f) - startA;
   float const r = std::clamp( startR + t * deltaR, 0.0f, 1.0f );
   float const g = std::clamp( startG + t * deltaG, 0.0f, 1.0f );
   float const b = std::clamp( startB + t * deltaB, 0.0f, 1.0f );
   float const a = std::clamp( startA + t * deltaA, 0.0f, 1.0f );
   return glm::vec4( r, g, b, a );
}

// create LookUpTable with 32 bit colors
std::vector< uint32_t >
LinearColorGradient::createLookUpTable32( int n ) const
{
   std::vector< uint32_t > colors;
   colors.reserve( n );

   float const step = 1.0f / float( n );

   for ( int32_t i = 0; i < n; ++i )
   {
      colors.emplace_back( getColor32( step*i ) );
   }

   return colors;
}

// create LookUpTable with 128 bit colors
std::vector< glm::vec4 >
LinearColorGradient::createLookUpTable128( int n ) const
{
   std::vector< glm::vec4 > colors;
   colors.reserve( n );

   float const step = 1.0f / float( n );

   for ( int32_t i = 0; i < n; ++i )
   {
      colors.emplace_back( getColor128( step*i ) );
   }

   return colors;
}


LinearColorGradient::LinearColorGradient()
   : m_sum( 0.0f )
   , m_inv_max( 0.0f )
   , m_Stops()
{
   // addStop( 0.0f, color );
}

LinearColorGradient::LinearColorGradient( uint32_t color )
{
   addStop( 0.0f, color );
}

LinearColorGradient::LinearColorGradient( uint32_t startColor, uint32_t endColor )
{
   addStop( 0.0f, startColor );
   addStop( 1.0f, endColor );
}

void
LinearColorGradient::clearStops()
{
   m_sum = 0.0f;
   m_inv_max = 0.0f;
   m_Stops.clear();
}


LinearColorGradient&
LinearColorGradient::addStop( float t, uint32_t color )
{
   m_sum = std::max( m_sum, t );
   m_inv_max = 0.0f;
   if ( t > 0.0f )
   {
      m_inv_max = 1.0f / float( m_sum );
   }
   m_Stops.emplace_back( t, color );
   // updateTable();

/*
   MyColorStruct entry;
   entry.Color = stopColor;
   entry.Position = core::clamp<float>( t, 0.0f, 1.0f );

   const uint32_t c = getColorCount();

   if (c==0)
   {
      Colors.push_back( entry );
   }
   else // if (c>0)
   {
      bool found_greater_t = false;
      uint32_t greater_t_index = 0;

      for (uint32_t i=0; i<c; i++)
      {
         if ( core::equals(t, Colors[i].Position) )
         {
            return *this;	// dont insert if any t does equal one of array-elements
         }
         else if ( Colors[i].Position > t )
         {
            found_greater_t = true;
            greater_t_index = i;
            break;
         }
      }

      if (found_greater_t)
         Colors.insert( entry, greater_t_index);
      else
         Colors.push_back( entry );
   }
*/
   return *this;
}

bool
LinearColorGradient::hasTransparentColor() const
{
   for ( ColorStop const & colorStop : m_Stops )
   {
      if ( dbRGBA_A( colorStop.color ) < 255 ) return true;
   }
   return false;
}

glm::vec4
LinearColorGradient::getColor128( float t ) const
{
   // DE_DEBUG("t = ", t )

   if ( m_Stops.size() < 1 )
   {
      return glm::vec4();
   }
   if ( m_Stops.size() < 2 )
   {
       return toRGBAf( m_Stops.back().color );
   }
   else if ( m_Stops.size() < 3 )
   {
      return lerpColor128( m_Stops[ 0 ].color, m_Stops[ 1 ].color, t*m_inv_max );
   }
   else
   {
      // find maximum and minimum neighbors
      int32_t found = -1; // found_greater_t

      for ( size_t i = 0; i < m_Stops.size(); ++i )
      {
         if ( std::abs( t - m_Stops[ i ].stop ) <= 1.0e-6f ) // dbEquals
         {
            return toRGBAf( m_Stops[ i ].color );
         }
         else if ( m_Stops[ i ].stop > t )
         {
            found = int32_t( i );
            break;
         }
      }

      if ( found < 0 )
      {
         return toRGBAf( m_Stops[ m_Stops.size()-1 ].color );
      }

      // interpolate between prev and next neighbor color
      uint32_t const A = m_Stops[ found - 1 ].color;
      uint32_t const B = m_Stops[ found ].color;
      float const min_t = m_Stops[ found - 1 ].stop;
      float const max_t = m_Stops[ found ].stop;
      float const k = std::abs( (t - min_t) / ( max_t - min_t ) );
      return lerpColor128( A, B, k );
   }
}

uint32_t
LinearColorGradient::getColor32( float t ) const
{
   return toRGBA( getColor128( t ) );
}

//LinearColorGradient&
//LinearColorGradient::setColor( int index, uint32_t color, float t )
//{
//   if ( size_t( index ) >= m_Stops.size() ) return *this;
//   m_Stops[ index ].color = color;
//   m_Stops[ index ].stop = dbClampf( t, 0.0f, 1.0f );
//   return *this;
//}


// ============================================================================

std::string
LinearColorGradient::toString() const
{
   std::stringstream s;

   /*
    const uint32_t color_count = getColorCount();

    // print number of colors used
    QString s("CLinearColorGradientTable[");
    s += color_count;

    // print name
    if (Name.size()>0)
    {
        s += Name;
        s += ", ";
    }

    // print
    s += "] = {\n";

    // print colors
    for (uint32_t i=0; i<color_count; i++)
    {
        s += "\t{ ";
        s += dbStr( Colors[i].Color ).c_str();
        s += ", ";
        s += Colors[i].Position;
        s += " }";

        if (i<color_count)
        {
            s += ",";
        }

        s += "\n";
    }

    // print
    s += "};\n";
    */
    return s.str();
}


#if 0

Image
LinearColorGradient::createImage( int w, int h, bool bVertical ) const
{
   if ( w < 1 || h < 1 )
   {
      return {};
   }

   Image img( w, h );

   // vertical filling
   if ( bVertical )
   {
      float const fy = 1.0f / float( h );

      for ( int32_t y = 0; y < h; ++y )
      {
         uint32_t color = getColor32( fy*y );

         for ( int32_t x = 0; x < w; ++x )
         {
            img.setPixel( x, y, color );
         }
      }
   }
   // horizontal filling
   else
   {
      float const fx = 1.0f / float( w );

      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = getColor32( fx*x );

         for ( int32_t y = 0; y < h; ++y )
         {
            img.setPixel( x, y, color );
         }
      }
   }

   return img;
}
#endif

} // end namespace de.


#if 0
#include <core/ColorGradient.h>

#include <core/String.h>

// ============================================================================
CLinearColorGradientTable*
CLinearColorGradientTable::createDefault( int transparency )
// ============================================================================
{
    CLinearColorGradientTable* gradient = new CLinearColorGradientTable(5,1024);

    if (gradient)
    {
        gradient->setColorCount( 5 );
        gradient->addColor( QColor(0,0,255,transparency), 0.00f);
        gradient->addColor( QColor(255,0,0,transparency), 0.25f);
        gradient->addColor( QColor(255,255,0,transparency), 0.50f);
        gradient->addColor( QColor(0,255,0,transparency), 0.75f);
        gradient->addColor( QColor(255,255,255,transparency), 1.00f);
        gradient->setTableSize( 1024 );
        gradient->updateTable();
    }

    return gradient;
}


// ============================================================================
CLinearColorGradientTable :: CLinearColorGradientTable( uint32_t color_count, uint32_t table_size )
    : IColorGradient("", color_count)
    , InvLookUpTableSize(0.0f)
// ============================================================================
{
    setTableSize( table_size, true );
}

// ============================================================================
CLinearColorGradientTable::~CLinearColorGradientTable()
// ============================================================================
{
    clear();
}

// ============================================================================
bool CLinearColorGradientTable :: read( irr::io::IXMLReader* pXml )
// ============================================================================
{
    if (!pXml)
        return false;

    return true;
}

// ============================================================================
bool CLinearColorGradientTable :: write ( irr::io::IXMLWriter* pXml ) const
// ============================================================================
{
    if (!pXml)
        return false;

    return true;
}












































namespace irr
{
namespace video
{

core::stringc toString( const SColor& color )
{
   core::stringc s("SColor(");
   s += color.getAlpha();
   s += ",";
   s += color.getRed();
   s += ",";
   s += color.getGreen();
   s += ",";
   s += color.getBlue();
   s += ")";
   return s;
}

core::stringc toString( const uint32_t& color )
{
   core::stringc s("uint32_t(");
   s += color.getAlpha();
   s += ",";
   s += color.getRed();
   s += ",";
   s += color.getGreen();
   s += ",";
   s += color.getBlue();
   s += ")";
   return s;
}

CLinearColorGradient::CLinearColorGradient() // const SColor& startColor, const SColor& endColor)
: Name("")
{
   #if _DEBUG
      os::Printer::log( "CLinearColorGradient::ctr()", ELL_INFORMATION );
   #endif // _DEBUG
//	addColor( startColor, 0.0f );
//	addColor( endColor, 1.0f );
}

CLinearColorGradient::~CLinearColorGradient()
{
   #if _DEBUG
      os::Printer::log( "CLinearColorGradient::dtr()", ELL_INFORMATION );
   #endif // _DEBUG
   clear();
}

CLinearColorGradient& CLinearColorGradient::clear()
{
   Colors.clear();
}


CLinearColorGradient& CLinearColorGradient::addColor( const SColor& stopColor, float t )
{
   return addColor( video::uint32_t(stopColor), t);
}

CLinearColorGradient& CLinearColorGradient::addColor( const uint32_t& stopColor, float t )
{
   #if _DEBUG
      os::Printer::log( "CLinearColorGradient::addColor()", ELL_INFORMATION );
   #endif // _DEBUG

   MyColorStruct entry;
   entry.Color = stopColor;
   entry.Position = core::clamp<float>( t, 0.0f, 1.0f );

   const uint32_t c = getColorCount();

   if (c==0)
   {
      Colors.push_back( entry );
   }
   else // if (c>0)
   {
      bool found_greater_t = false;
      uint32_t greater_t_index = 0;

      for (uint32_t i=0; i<c; i++)
      {
         if ( core::equals(t, Colors[i].Position) )
         {
            return *this;	// dont insert if any t does equal one of array-elements
         }
         else if ( Colors[i].Position > t )
         {
            found_greater_t = true;
            greater_t_index = i;
            break;
         }
      }

      if (found_greater_t)
         Colors.insert( entry, greater_t_index);
      else
         Colors.push_back( entry );
   }
   return *this;
}

CLinearColorGradient& CLinearColorGradient::setColor( uint32_t index, const SColor& stopColor, float t )
{
   const uint32_t size0 = getColorCount();
   if (index >= size0)
      return *this;

   Colors[index].Color = video::uint32_t(stopColor);
   Colors[index].Position = core::clamp<float>( t, 0.0f, 1.0f );
   return *this;
}

CLinearColorGradient& CLinearColorGradient::setColor( uint32_t index, const uint32_t& stopColor, float t )
{
   const uint32_t size0 = getColorCount();
   if (index >= size0)
      return *this;

   Colors[index].Color = stopColor;
   Colors[index].Position = core::clamp<float>( t, 0.0f, 1.0f );
   return *this;
}


SColor CLinearColorGradient::getColor( float t ) const
{
   return getColorf(t).toSColor();
}

uint32_t CLinearColorGradient::getColorf( float t ) const
{

//	#if _DEBUG
//		os::Printer::log( "CLinearColorGradient::getColor()", ELL_INFORMATION );
//	#endif // _DEBUG

   const uint32_t colorCount = getColorCount();

   // find maximum and minimum neighbors

   bool found_greater_t = false;
   uint32_t greater_t_index = 0;

   for (uint32_t i=0; i<colorCount; i++)
   {
      if ( core::equals(t, Colors[i].Position) )
      {
         return Colors[i].Color;
      }
      else if (Colors[i].Position > t)
      {
         found_greater_t = true;
         greater_t_index = i;
         break;
      }
   }

   if (!found_greater_t)
   {
      return Colors[colorCount-1].Color;
   }

   // interpolate between prev and next neighbor color
   const uint32_t& A = Colors[greater_t_index-1].Color;
   const uint32_t& B = Colors[greater_t_index].Color;
   const float min_t = Colors[greater_t_index-1].Position;
   const float max_t = Colors[greater_t_index].Position;
   const float dx = core::abs_<float>( (t - min_t) / (max_t - min_t) );
   const float fa = A.getAlpha() + dx*( B.getAlpha() - A.getAlpha() );
   const float fr = A.getRed() + dx*( B.getRed() - A.getRed() );
   const float fg = A.getGreen() + dx*( B.getGreen() - A.getGreen() );
   const float fb = A.getBlue() + dx*( B.getBlue() - A.getBlue() );

   return video::uint32_t( fr,fg,fb,fa );
}

uint32_t CLinearColorGradient::getColorCount() const
{
   return Colors.size();
}

core::stringc CLinearColorGradient::getName( ) const
{
   return Name;
}

CLinearColorGradient& CLinearColorGradient::setName( const core::stringc& name )
{
   Name = name;
   return *this;
}

bool CLinearColorGradient::isTransparent( ) const
{
   bool bTransparent = false;
   uint32_t i=0;
   uint32_t c=getColorCount();

   while (i<c)
   {
      if (Colors[i].Color.getAlpha() < 1.0f) // not 255
      {
         bTransparent = true;
         break;
      }
      i++; // dont ever forget again foo
   }

   return bTransparent;
}

E_MATERIAL_TYPE CLinearColorGradient::getMaterialType( ) const
{
   if (isTransparent())
      return EMT_TRANSPARENT_ALPHA_CHANNEL;
   else
      return EMT_SOLID;
}

ECOLOR_FORMAT CLinearColorGradient::getColorFormat( ) const
{
   if (isTransparent())
      return ECF_A8R8G8B8;
   else
      return ECF_R8G8B8;
}

// old
//IImage*	CLinearColorGradient::createImage( IVideoDriver* driver, uint32_t w, uint32_t h, bool bVertical ) const

// new
IImage*	CLinearColorGradient::createImage( uint32_t w, uint32_t h, bool bVertical ) const
{
   #if _DEBUG
   os::Printer::log( "CLinearColorGradient::createImage()", ELL_INFORMATION );
   #endif // _DEBUG

   if (w==0 || h==0)
   {
      os::Printer::log( "Can't create Image of size zero.", ELL_ERROR );
      return 0;
   }

   // old
   //IImage* tmp = driver->createImage( this->getColorFormat(), core::dimension2du(w,h) );

   // new
   IImage* tmp = (IImage*)new CImage( this->getColorFormat(), core::dimension2du(w,h) );
   if (!tmp)
   {
      os::Printer::log( "Could not create CImage", ELL_ERROR );
      return 0;
   }

   const core::dimension2du& ImageSize = tmp->getDimension();

//	#if _DEBUG
//		// os::Printer::log( core::sprintf("Created new CImage(%d,%d,%d)", ImageSize.Width, ImageSize.Height, tmp->getBitsPerPixel()).c_str(), ELL_INFORMATION );
//		// os::Printer::log( "start filling", ELL_INFORMATION );
//	#endif // _DEBUG

   // vertical filling
   if ( bVertical )
   {
      const float fy = 1.0f / (float)h;

      for (uint32_t y=0; y<ImageSize.Height; y++)
      {
         video::SColor color = getColor( fy*y );

         for (uint32_t x=0; x<ImageSize.Width; x++)
         {
            tmp->setPixel( x,y,color );
         }
      }

   }
   // horizontal filling
   else
   {
      const float fx = 1.0f / (float)w ;

      for (uint32_t x=0; x<ImageSize.Width; x++)
      {
         video::SColor color = getColor( fx*x );

         for (uint32_t y=0; y<ImageSize.Height; y++)
         {
            tmp->setPixel( x,y,color );
         }
      }
   }

//	#if _DEBUG
//		os::Printer::log( "finish image filling with CLinearColorGradient", ELL_INFORMATION );
//	#endif // _DEBUG

   return tmp;
}

core::stringc CLinearColorGradient::toString( ) const
{
   core::stringc s("CLinearColorGradient[");

   // print number of colors used
   uint32_t c = getColorCount();
   s += (s32)c;

   // print name
   if (Name.size()>0)
   {
      s += Name;
      s += ", ";
   }

   // print
   s += "] = {\n";

   // print colors
   for (uint32_t i=0; i<c; i++)
   {
      s += "\t{ ";
      s += video::toString( Colors[i].Color.toSColor() );
      s += ", ";
      s += Colors[i].Position;
      s += " }";
      if (i<c)
      {
         s += ",";
      }
      s += "\n";
   }

   // print
   s += "};\n";
   return s;
}

//IImage* createImageFromLinearColorGradient( uint32_t w, uint32_t h, bool bVertical, CLinearColorGradient* gradient, bool deleteGradient)
//{
//	if (!gradient)
//		return 0;
//
//	IImage* tmp = gradient->createImage( w,h,bVertical );
//
//	if (deleteGradient)
//	{
//		delete gradient;
//	}
//
//	return tmp;
//}

} // end namespace video

} // end namespace irr


#endif
