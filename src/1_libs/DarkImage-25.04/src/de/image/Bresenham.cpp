#include <de/image/Bresenham.h>

namespace de {


// ==============================================================================

// ==============================================================================

// ===        Bresenham         =================================================

// ==============================================================================

// ==============================================================================



// static
void
Bresenham::traverseCircle( int32_t radius, SetPixel_t const & setPixel )
{
   // Bresenham
   int32_t i = 0;
   int32_t j = radius;
   int32_t F = 1 - radius;

   while ( i < j )
   {
      ++i;
      if ( F < 0 )
      {
         F += ( i<<1 ) - 1;
      }
      else
      {
         F += ( ( i-j ) << 1 );
         --j;
      }

      // Verbesserungen by Benjamin Hampe (c) 2012
      setPixel( i, j );
   }
}

// static
void
Bresenham::drawCircleBorder( int32_t x, int32_t y, int32_t radius, SetPixel_t const & setPixel )
{
   if ( radius < 1 ) return;
   // 'Bresenham' Algorithmus (Achtelkreis Symmetrie)
   // ohne Trigonometrische- und Wurzel-Funktionen
   // und Spiegelung auf Restliche 7/8
   int32_t i = 0;
   int32_t j = radius;
   int32_t F = 1 - radius;
   while ( i < j )
   {
      ++i;
      if ( F < 0 )
      {
         F += ( i << 1 ) - 1;
      }
      else
      {
         F += ( ( i - j ) << 1 );
         --j;
      }
      // Verbesserungen by Benjamin Hampe (c) 2012
      setPixel( x+i-1,y-j ); // 1st quadrant
      setPixel( x+j-1,y-i ); // 1st quadrant
      setPixel( x+i-1,y+j-1 ); // 2nd quadrant
      setPixel( x+j-1,y+i-1 ); // 2nd quadrant
      setPixel( x-i,y+j-1 ); // 3rd quadrant
      setPixel( x-j,y+i-1 ); // 3rd quadrant
      setPixel( x-i,y-j ); // 4th quadrant
      setPixel( x-j,y-i ); // 4th quadrant
   }
}

// static
void
Bresenham::traverseEllipse( int32_t a, int32_t b, SetPixel_t const & setPixel )
{
   if ( a < 1 && b < 1 ) return;
   int32_t const aa = a * a; // Ellipsen Radien Quadrate
   int32_t const bb = b * b; // Ellipsen Radien Quadrate

   // Bresenham Anfangswerte
   // Trick: nur 1/4 Ellipse berechne und restliche 3/4 durch Spiegelung (gerade Ellipse!!!)
   int32_t ddF_x = 0;
   int32_t ddF_y = ( aa * b ) << 1;
   int32_t x = 0;
   int32_t y = b;

   // Bresenham Alghorithmus
   auto F = int32_t( std::round( f32( bb - aa * b ) + f32( aa ) * 0.25f ) );
   while ( ddF_x < ddF_y )
   {
      if ( F >= 0 )
      {
         y     -= 1;       // south = -y dir
         ddF_y -= aa << 1;
         F     -= ddF_y;
      }
      x     += 1;          // east = +x dir
      ddF_x += bb << 1;
      F     += ddF_x + bb;

      setPixel( x, y );
   }

   // Bresenham Alghorithmus
   F = int32_t( std::round( f32( bb ) * ( f32( x ) + 0.5f ) * ( f32( x ) + 0.5f ) +
                      f32( aa*(y-1)*(y-1) - aa*bb ) ) );
   while ( y > 0 )
   {
      if ( F <= 0 )
      {
         x     += 1;        // east
         ddF_x += bb << 1;
         F     += ddF_x;
      }
      y     -= 1;           // south
      ddF_y -= aa << 1;
      F     += aa - ddF_y;

      setPixel( x, y );
   }
}

/*

// static
void
Bresenham::drawOutlineEllipse( int32_t x1, int32_t y1, int32_t x2, int32_t y2, SetPixel_t const & setPixel )
{
   if ( x1 > x2 ) std::swap( x1, x2 );
   if ( y1 > y2 ) std::swap( y1, y2 );
   int32_t const w = x2 - x1;
   int32_t const h = y2 - y1;
   int32_t const a = w/2;
   int32_t const b = h/2;

   // Abbruchbedingung
   if ( a < 1 && b < 1 ) return;

//   if ( a == b )
//   {
//      drawCircle( dst, center, rx, color, border, blend );
//   }

   //if (rw==1 && rh==1) dest->setPixel(x,y,color);
   int32_t const cx = x1 + a;
   int32_t const cy = y1 + b;

   int32_t const a2 = a * a; // Ellipsen Radien Quadrate
   int32_t const b2 = b * b; // Ellipsen Radien Quadrate
   int32_t x = cx;
   int32_t y = cy;

   // Bresenham Anfangswerte
   // Trick: nur 1/4 Ellipse berechne und restliche 3/4 durch Spiegelung (gerade Ellipse!!!)
   int32_t ddF_x=0;
   int32_t ddF_y=((a2*b)<<1);
   x=0;
   y=b;

   // Bresenham Alghorithmus
   int32_t F = dbRound32( f32( b2-a2*b ) + f32( a2 )*0.25f );
   while ( ddF_x < ddF_y )
   {
      if (F >= 0)
      {
         y     -= 1;       // south
         ddF_y -= (a2<<1);
         F     -= ddF_y;
      }
      x     += 1;          // east
      ddF_x += (b2<<1);
      F     += ddF_x + b2;

      if ( border == 0 )
      {
         drawLine( cx-x, cy+y, cx+x, cy+y, setPixel );
         drawLine( cx-x, cy-y, cx+x, cy-y, setPixel );
      }
      else
      {
         setPixel( cx+x, cy+y );
         setPixel( cx-x, cy+y );
         setPixel( cx+x, cy-y );
         setPixel( cx-x, cy-y );

         // Spiegelung mit meinen Anpassungen
         // drawPixel( dst, x+mx, y+my, color, blend);
         // drawPixel( dst, -x+mx+1, y+my, color, blend);
         // drawPixel( dst, x+mx, -y+my+1, color, blend);
         // drawPixel( dst, -x+mx+1, -y+my+1, color, blend);
      }
   }

   // Bresenham Alghorithmus
   F = dbRound32( f32( b2 ) * ( f32( x ) + 0.5f ) * ( f32( x ) + 0.5f )
                     + f32( a2*(y-1)*(y-1) - a2*b2 ) );
   while ( y > 0 )
   {
      if ( F <= 0 )
      {
         x     += 1;        // east
         ddF_x += b2 << 1;
         F     += ddF_x;
      }
      y     -= 1;           // south
      ddF_y -= a2 << 1;
      F     += a2 - ddF_y;

      if ( border == 0 )
      {
         drawLine( cx-x, cy+y, cx+x, cy+y, setPixel );
         drawLine( cx-x, cy-y, cx+x, cy-y, setPixel );
      }
      else
      {
         setPixel( cx+x, cy+y );
         setPixel( cx-x, cy+y );
         setPixel( cx+x, cy-y );
         setPixel( cx-x, cy-y );
         // Spiegelung mit meinen Anpassungen
//			drawPixel( dst, x+mx, y+my, color, blend);
//			drawPixel( dst, -x+mx+1, y+my, color, blend);
//			drawPixel( dst, x+mx, -y+my+1, color, blend);
//			drawPixel( dst, -x+mx+1, -y+my+1, color, blend);
      }
   }
}

// static
void
Bresenham::drawFilledCircle( int32_t x, int32_t y, int32_t radius, SetPixel_t const & setPixel )
{
   if ( radius < 1 ) return;
   //
   //  ##
   //  #P
   //
   if ( radius == 1 )
   {
      setPixel( x-1, y-1 );
      setPixel( x-1, y );
      setPixel( x, y-1 );
      setPixel( x, y );
      return;
   }
   //   ##
   //  ####
   //  ##P#
   //   ##
   else if ( radius == 2 )
   {
      setPixel( x-1, y-2 ); setPixel( x, y-2 );
      setPixel( x-2, y-1 ); setPixel( x-1, y-1 ); setPixel( x, y-1 ); setPixel( x+1, y-1 );
      setPixel( x-2, y );   setPixel( x-1, y );   setPixel( x, y );   setPixel( x+1, y );
      setPixel( x-1, y+1 ); setPixel( x, y+1 );
      return;
   }

   // 'Bresenham' Algorithmus (Viertelkreis Symmetrie)
   int32_t i = 0;
   int32_t j = radius;
   int32_t F = 1 - radius;
   int32_t last_i = i;
   int32_t last_j = j;

   // DE_DEBUG("[BEGIN] cx=",x,", cy=",y,", radius=",radius )
   // size_t k = 0;

   while ( i < j )
   {
      // DE_DEBUG("k=",k,", i=",i, ", j=",j,", F=",F)
      //++k;

      // Verbesserungen by Benjamin Hampe (c) 2012
      //drawLineH( x-i, x+i-1, y+j-1, setPixel );
      //drawLineH( x-j, x+j-1, y+i-1, setPixel );
      //drawLineH( x-j, x+j-1, y-i, setPixel );
      //drawLineH( x-i, x+i-1, y-j, setPixel );
      ++i;
      if ( F < 0 )
      {
         F += ( i << 1 ) - 1;
         //DE_DEBUG("East")
      }
      else
      {
         F += ( ( i - j ) << 1 );
         --j;
         //DE_DEBUG("South")
      }

      if ( last_j != j )
      {
         last_j = j;

         drawLineH( x-i, x+i-1, y+j, setPixel );
         drawLineH( x-i, x+i-1, y-j, setPixel );

         // DE_DEBUG("Draw 1")
      }

      if ( last_i != i )
      {
         last_i = i;

         drawLineH( x-j, x+j-1, y+i, setPixel );
         drawLineH( x-j, x+j-1, y-i, setPixel );

         // DE_DEBUG("Draw 2")
      }
   }
}

// static
void
Bresenham::floodFill( glm::ivec2 const & p,
                      std::function< void( int32_t, int32_t ) > const & getPixel,
                      std::function< void( int32_t, int32_t ) > const & setPixel,
                      DWORD newColor,
                      DWORD oldColor )
{
   //DWORD oldColor=MemblockReadDword(mem,memX,memY,x,y);
   if ( getPixel( p ) == newColor ) return;
   int lg, rg, px = p.x;
   while ( getPixel( x,y ) == oldColor )
   {
      setPixel( x,y, newColor );
      x--;
   }
   lg = x+1;
   x = px+1;
   while ( getPixel( x,y ) == oldColor )
   {
      setPixel( x,y, newColor );
      x++;
   }
   rg = x-1;
   for ( x = rg; x >= lg; --x )
   {
      if ( ( getPixel( x,y-1 ) == oldColor ) && ( y - 1 > 0 ) )
      {
         floodFill( x, y-1, newColor, oldColor );
      }
      if ( ( getPixel( x,y+1 ) == oldColor ) && ( y < memY-1 ) )
      {
         floodFill( x, y+1, newColor, oldColor );
      }
   }
}

*/

// static
void
Bresenham::drawLineH( int32_t x1, int32_t x2, int32_t y, SetPixel_t const & setPixel )
{
   //DE_DEBUG("x1=",x1,", x2=", x2, "y=", y)
   if ( x1 > x2 ) std::swap( x1, x2 );
   // setPixel( x1, y1 ); // always draw atleast something point
   // if ( x1 == x2 ) return; // Nothing todo
   for ( int32_t x = x1; x <= x2; ++x )
   {
      setPixel( x, y );
   }
}

// static
void
Bresenham::drawRect( Recti pos, SetPixel_t const & setPixel )
{
   int32_t x1 = pos.x1();
   int32_t y1 = pos.y1();
   int32_t x2 = pos.x2();
   int32_t y2 = pos.y2();
   if ( x1 > x2 ) std::swap( x1, x2 );
   if ( y1 > y2 ) std::swap( y1, y2 );
   for ( int32_t y = y1; y <= y2; ++y ) {
   for ( int32_t x = x1; x <= x2; ++x ) {
      setPixel( x, y );
   }
   }
}


// static
void
Bresenham::drawRectBorder( Recti pos, SetPixel_t const & setPixel )
{
   int32_t x1 = pos.x1();
   int32_t y1 = pos.y1();
   int32_t x2 = pos.x2();
   int32_t y2 = pos.y2();
   drawLine( x1, y1, x2, y1, setPixel );
   for ( int32_t y = y1+1; y <= y2-1; ++y )
   {
      setPixel( x1, y );
      setPixel( x2, y );
   }
   drawLine( x1, y2, x2, y2, setPixel );
}


// static
void
Bresenham::drawSuperCoverLine( glm::ivec2 const & A, glm::ivec2 const & B, SetPixel_t const & setPixel )
{

}


// static
void
Bresenham::drawLine( glm::ivec2 const & A, glm::ivec2 const & B, SetPixel_t const & setPixel )
{
   // This is the fastest (32-bit) implementation you will ever find!
   int32_t const dx = std::abs( B.x - A.x );
   int32_t const dy = std::abs( B.y - A.y );
   if ( ( dx == 0 ) && ( dy == 0 ) ) return; // Nothing todo

   int32_t sx = 1; // sign
   int32_t sy = 1; // sign
   if ( A.x > B.x ) sx = -1; // we simply move negative ( mirrored )
   if ( A.y > B.y ) sy = -1; // we simply move negative ( mirrored )

   int32_t e1 = dx - dy;   // error delta has correct sign
   int32_t e2 = 0;         // always doubled before test to increase stability
   int32_t x = A.x;
   int32_t y = A.y;

   // The loop always finishes deterministicly, no numeric instability
   // because we only use fast integer add/sub and fast shifts.
   while( 1 )
   {
      setPixel( x, y );
      // Benni optimized.
      // Condition always met, no infinite loops here.
      if ( ( x == B.x ) && ( y == B.y ) ) break;
      e2 = e1 << 1; // double error before making a decision ( inc precision ).
      if ( e2 > -dy ) { e1 -= dy; x += sx; } // move x
      if ( e2 <  dx ) { e1 += dx; y += sy; } // move y
   }
}

// static
void
Bresenham::drawTriangleBorder( glm::ivec2 const & A, glm::ivec2 const & B, glm::ivec2 const & C, SetPixel_t const & setPixel )
{
   DE_DEBUG("")
   drawLine( glm::ivec2( A.x, A.y ), glm::ivec2( B.x, B.y ), setPixel );
   drawLine( glm::ivec2( B.x, B.y ), glm::ivec2( C.x, C.y ), setPixel );
   drawLine( glm::ivec2( C.x, C.y ), glm::ivec2( A.x, A.y ), setPixel );
}

// static
void
Bresenham::drawTriangle( glm::ivec2 const & A,
                         glm::ivec2 const & B,
                         glm::ivec2 const & C,
                         SetPixel_t const & setPixel )
{
   // determinant2d) is same as crossProduct2d()
   // s = determinant(P - V1, V2) / determinant(V1, V2)
   // t = determinant(V1, P - V2) / determinant(V1, V2)

   auto crossP = [] ( glm::ivec2 A, glm::ivec2 B )
   {
      return( A.y * B.x - A.x * B.y );
   };

   glm::vec2 AB = B - A;
   glm::vec2 AC = C - A;

   // Compute triangle bounding box
   int32_t x_min = std::min( std::min( A.x, B.x ), C.x );
   int32_t x_max = std::max( std::max( A.x, B.x ), C.x );
   int32_t y_min = std::min( std::min( A.y, B.y ), C.y );
   int32_t y_max = std::max( std::max( A.y, B.y ), C.y );

   // Draw outside triangle border
   //drawTriangleBorder( A, B, C, setPixel );

   // Draw inside triangle ( does not include all border pixels )
   for ( int32_t y = y_min; y <= y_max; ++y )
   {
      for ( int32_t x = x_min; x <= x_max; ++x )
      {
         glm::vec2 q = glm::vec2( x - A.x, y - A.y );
         float const n = 1.0f / float( crossP( AB, AC ) );
         float const s = n * float( crossP( q, AC ) );
         float const t = n * float( crossP( AB, q ) );

         if ( (s >= 0.0 )
           && (t >= 0.0 )
           && ( s + t <= 1.0 ) ) // + 3.0f*std::numeric_limits< float >::epsilon() ) )
         {
            setPixel( x, y );
         }
      }
   }
}


/*
 // static
void
Bresenham::drawFlatTopTriangle(  int32_t A.x, int32_t B.x, int32_t A.y2,
                                 int32_t x3, int32_t y3, SetPixel_t const & setPixel )
{
   DE_DEBUG("A(",A.x,",",A.y2,"), B(",B.x,",",A.y2,"), C(",x3,",",y3,")")

   if ( A.x > B.x ) { std::swap( A.x, B.x ); }
   auto connect13 = [=] ( int32_t x, int32_t y ) { Bresenham::drawLineH( x, x3, y, setPixel ); };
   Bresenham::drawLine( A.x, A.y2, x3, y3, connect13 );

   auto connect23 = [=] ( int32_t x, int32_t y ) { Bresenham::drawLineH( x, x3, y, setPixel ); };
   Bresenham::drawLine( B.x, A.y2, x3, y3, connect23 );
}


 Read DWORD
inline DWORD MemblockReadDword(int mem, int pos)
{
   return dbMemblockDword(mem,pos);
}
// Read DWORD
inline DWORD MemblockReadDword(int mem, const int& memX, const int& memY, int x, int y)
{
   if (x<0)      x = 0;
   if (y<0)      y = 0;
   if (x>memX-1) x = memX-1;
   if (y>memY-1) y = memY-1;
   return MemblockReadDword(mem,12+4*(x+y*memX));
}
// Write DWORD
inline void MemblockWriteDword(int mem, int pos, DWORD color)
{
   dbWriteMemblockDword(mem,pos,color);
}
// Write DWORD
inline void MemblockWriteDword(int mem, const int& memX, const int& memY, int x, int y, DWORD color)
{
   if (x<0)      x = 0;
   if (y<0)      y = 0;
   if (x>memX-1) x = memX-1;
   if (y>memY-1) y = memY-1;
   MemblockWriteDword(mem, 12+4*(x+y*memX), color);
}
inline void MemblockWriteDwordEx(int mem, const int& memX, const int& memY, int x, int y, DWORD color)
{
   if (x<0)      return;
   if (y<0)      return;
   if (x>memX-1) return;
   if (y>memY-1) return;
   MemblockWriteDword(mem, 12+4*(x+y*memX), color);
}

int MemblockFloodFill(int mem, int x, int y, DWORD newColor, DWORD oldColor)
{
   if (IsMemblock(mem)==0) return 0;

      const int memX=MemblockX(mem);
      const int memY=MemblockY(mem);
   //DWORD oldColor=MemblockReadDword(mem,memX,memY,x,y);

   if (MemblockReadDword(mem,memX,memY,x,y)==newColor) return 1;

   int lg, rg, px=x;
   while(MemblockReadDword(mem,memX,memY,x,y)==oldColor)
   {
      MemblockWriteDword(mem,memX,memY,x,y,newColor);
      x--;
   }

   lg=x+1;
   x=px+1;

   while(MemblockReadDword(mem,memX,memY,x,y)==oldColor)
   {
      MemblockWriteDword(mem,memX,memY,x,y,newColor);
      x++;
   }
   rg=x-1;

   for(x=rg; x>=lg; x--)
   {
      if((MemblockReadDword(mem,memX,memY,x,y-1)==oldColor) && (y-1>0))
         MemblockFloodFill(mem,x,y-1,newColor,oldColor);
      if((MemblockReadDword(mem,memX,memY,x,y+1)==oldColor) && (y<memY-1))
         MemblockFloodFill(mem,x,y+1,newColor,oldColor);
   }

   return 1;
}


// static
void
Bresenham::drawFlatBottomTriangle(  int32_t A.x, int32_t A.y, int32_t B.x, int32_t x3, int32_t B.y3,
                                     SetPixel_t const & setPixel )
{
   DE_DEBUG("A(",A.x,",",A.y,"), B(",B.x,",",B.y3,"), C(",x3,",",B.y3,")")
//   auto connect12 = [=] ( int32_t x, int32_t y ) { Bresenham::drawLineH( A.x, x, y, setPixel12 ); };

   int32_t x_min = std::min( std::min( A.x, B.x ), x3 );
   int32_t y_min = std::min( A.y, B.y3 );
   int32_t x_max = std::max( std::max( A.x, B.x ), x3 );
   int32_t y_max = std::max( A.y, B.y3 );

   DE_DEBUG("x_min(",x_min,"), y_min(",y_min,"), x_max(",x_max,"), y_max(",y_max,")")

   std::vector< glm::ivec2 > line12;
   std::vector< glm::ivec2 > line13;
   auto connect12 = [&] ( int32_t x, int32_t y ) { line12.emplace_back( x, y ); };
   auto connect13 = [&] ( int32_t x, int32_t y ) { line13.emplace_back( x, y ); };
   Bresenham::drawLine( A.x, A.y, B.x, B.y3, connect12 );
   Bresenham::drawLine( A.x, A.y, x3, B.y3, connect13 );

//   int32_t v_min = std::numeric_limits<int32_t>::max();
//   int32_t v_max = std::numeric_limits<int32_t>::lowest();


   //if ( A.x > B.x ) { std::swap( A.x, B.x ); }
   //if ( B.x > x3 ) { std::swap( B.x, x3 ); }
   //if ( B.x > x3 ) { std::swap( B.x, x3 ); }

   Bresenham::drawLine( A.x,A.y, B.x,B.y3, setPixel );
   Bresenham::drawLine( B.x,B.y3, x3,B.y3, setPixel );
   Bresenham::drawLine( A.x,A.y, x3,B.y3, setPixel );

}


*/


} // end namespace de.

















#if BACKUP_IMAGETOOL_DRAW





PointList Tool::traverseLine( const core::position2di& A, const core::position2di& B )
{
   PointList dst( irr::core::abs_<int32_t>(B.X-A.X)+irr::core::abs_<int32_t>(B.Y-A.Y) );
   return dst;
}

PointList Tool::traverseCircle( const core::position2di& center, int32_t radius )
{
   PointList dst( radius+3 );
   dst.set_used(0);

   // Bresenham
   int32_t i,j,F;
   i = 0;
   j = radius;
   F = 1 - radius;

   while (i < j)
   {
      ++i;
      if (F < 0)
      {
         F += (i<<1) - 1;
      }
      else
      {
         F += ((i - j)<<1);
         --j;
      }
      // Verbesserungen by Benjamin Hampe (c) 2012
      dst.push_back( core::position2di( cx+i,cy+j ) );
      dst.push_back( core::position2di( cx+j,cy+i ) );
   }
   return dst;
}


PointList Tool::traverseEllipse( const core::position2di& center, int32_t rx, int32_t ry )
{
//	const int32_t w = dest->getDimension().Width;
//	const int32_t h = dest->getDimension().Height;

   if (rx==ry)
      return traverseCircle( center, rx );

   PointList dst;
   dst.reallocate(rx+ry);
   dst.set_used(0);

   // Abbruchbedingung
   if ( (rx==0) && (ry==0) )
      return dst;

   const int32_t rx2=rx*rx; // Ellipsen Radien Quadrate
   const int32_t ry2=ry*ry; // Ellipsen Radien Quadrate
   int32_t x = cx;
   int32_t y = cy;
//	const int32_t mx = cx;	 // Ellipsen Mittelpunkt
//	const int32_t my = cy; 	 // Ellipsen Mittelpunkt

   // Bresenham Anfangswerte
   // Trick: nur 1/4 Ellipse berechne und restliche 3/4 durch Spiegelung (gerade Ellipse!!!)
   int32_t ddF_x=0;
   int32_t ddF_y=((rx2*ry)<<1);
   x=0;
   y=ry;

   // Bresenham Alghorithmus
   int32_t F=irr::core::round32((f32)(ry2-rx2*ry)+(f32)rx2*0.25f);
   while( ddF_x < ddF_y )
   {
      if (F >= 0)
      {
         y     -= 1;        // south
         ddF_y -= (rx2<<1);
         F     -= ddF_y;
      }
      x     += 1;          // east
      ddF_x += (ry2<<1);
      F     += ddF_x + ry2;

      dst.push_back( core::position2di( center.X+x, cy+y) );
   }

   // Bresenham Alghorithmus
   F = irr::core::round32((f32)ry2*((f32)x+0.5f)*((f32)x+0.5f) + (f32)(rx2*(y-1)*(y-1) - rx2*ry2));
   while( y > 0 )
   {
      if(F <= 0)
      {
         x     += 1;        // east
         ddF_x += (ry2<<1);
         F     += ddF_x;
      }
      y     -= 1;           // south
      ddF_y -= (rx2<<1);
      F     += rx2 - ddF_y;

      dst.push_back( core::position2di( center.X+x, cy+y) );
   }
   return dst;
}



void Tool::drawImage(
   IImage* src,
   IImage* dst,
   const core::position2di& pos,
   const core::dimension2du& size,
   bool bTransparent,
   bool blend)
{
   if (!src)
      return;

   const u32 x_max = irr::core::min_<u32>( dst->getDimension().Width, src->getDimension().Width );
   const u32 y_max = irr::core::min_<u32>( dst->getDimension().Height, src->getDimension().Height );

   for (u32 y=0; y<y_max; y++)
   {
      for (u32 x=0; x<x_max; x++)
      {
         const SColor& color = src->getPixel(x,y);

         drawPixel( dst, (int32_t)x+pos.X, (int32_t)y+pos.Y, color, blend);
      }
   }
}

//! draw a line with 1 color(s) to image ( Bresenham, no AA )
bool Tool::drawLine(
   video::IImage* dst,
   int32_t x0,
   int32_t y0,
   int32_t x1,
   int32_t y1,
   const video::SColor& color,
   bool blend )
{
   if (!dst) return false;

   const core::dimension2du img_size = dst->getDimension();

   if ((img_size.Width==0) || (img_size.Height==0)) return false;

   const int32_t dx = core::abs_<int32_t>( x1 - x0 );
   const int32_t dy = core::abs_<int32_t>( y1 - y0 );

   if ((dx==0) && (dy==0)) return false;

   int32_t sx=1; // sign
   int32_t sy=1; // sign

   if (x0>x1) sx = -1;
   if (y0>y1) sy = -1;

   int32_t err = dx-dy;
   int32_t e2 = 0;
   int32_t x = x0;
   int32_t y = y0;

   while(1)
   {
      drawPixel( dst,x,y,color, blend);

      if ((x==x1) && (y==y1)) break;

      e2 = err << 1;

      if (e2 > -dy) { err -= dy;	x += sx; }
      if (e2 < dx) { y += sy; err += dx; }
   }

   return true;
}

//! draw a line with 2 color(s) to image ( Bresenham, no AA )
bool Tool::drawLine(
   video::IImage* dst,
   int32_t x0,
   int32_t y0,
   int32_t x1,
   int32_t y1,
   const video::SColor& color_a,
   const video::SColor& color_b,
   bool blend )
{
   if (!dst) return false;

   const core::dimension2du img_size = dst->getDimension();

   if ((img_size.Width == 0) || (img_size.Height == 0))
      return false;

   const int32_t dx = core::abs_<int32_t>( x1 - x0 );
   const int32_t dy = core::abs_<int32_t>( y1 - y0 );

   if ((dx==0) && (dy==0)) return false;

   int32_t sx = 1; // sign
   int32_t sy = 1; // sign

   if (x0 > x1) sx = -1;
   if (y0 > y1) sy = -1;

   int32_t err = dx-dy;
   int32_t e2 = 0;
   int32_t x = x0;
   int32_t y = y0;

   int32_t numpixels = 0;

   // count pixels
   while (1)
   {
      numpixels++;

      if ((x==x1) && (y==y1)) break;

      e2 = err << 1;
      if (e2 > -dy) { err -= dy;	x += sx; }
      if (e2 < dx) { err += dx; y += sy; }
   }

   // reset vars;
   err = dx-dy;
   e2 = 0;
   x = x0;
   y = y0;

   // values for linear color interpolation
   f32 const A1=(f32)color_a.getAlpha();
   f32 const R1=(f32)color_a.getRed();
   f32 const G1=(f32)color_a.getGreen();
   f32 const B1=(f32)color_a.getBlue();
   f32 const dA=(f32)color_b.getAlpha()-A1;
   f32 const dR=(f32)color_b.getRed()-R1;
   f32 const dG=(f32)color_b.getGreen()-G1;
   f32 const dB=(f32)color_b.getBlue()-B1;

   // actual drawing
   f32 f=0.f;
   int32_t k=0;
   u32 cR=0, cG=0, cB=0, cA=0;
   while (1)
   {
      f = (f32)k/(f32)numpixels;
      k++;

      cA=A1;
      cR=R1;
      cG=G1;
      cB=B1;

      // maybe faster under the assumption that colors have most likely same alpha value
      if (dA>0) cA = (u32)core::clamp( core::round32(A1+dA*f), 0, 255);
      if (dR>0) cR = (u32)core::clamp( core::round32(R1+dR*f), 0, 255);
      if (dG>0) cG = (u32)core::clamp( core::round32(G1+dG*f), 0, 255);
      if (dB>0) cB = (u32)core::clamp( core::round32(B1+dB*f), 0, 255);

      drawPixel( dst, x, y, video::SColor( cA, cR, cG, cB), blend );

      if (x == x1 && y == y1) break;

      e2 = err << 1;
      if (e2 > -dy)	{	err -= dy;	x += sx; }
      if (e2 < dx)	{	err += dx;	y += sy; }
   }

   return true;
}

//! draw a line with 2 color(s) to image ( Bresenham, no AA )
bool Tool::drawLine(
   video::IImage* dst,
   const core::rectf& pos,
   const video::SColor& color,
   bool blend )
{
   const int32_t x0 = core::floor32( pos.UpperLeftCorner.X );
   const int32_t y0 = core::floor32( pos.UpperLeftCorner.Y );
   const int32_t x1 = core::floor32( pos.LowerRightCorner.X );
   const int32_t y1 = core::floor32( pos.LowerRightCorner.Y );

   return drawLine( dst, x0, y0, x1, y1, color, blend);
}

//! draw a line with 2 color(s) to image ( Bresenham, no AA )

bool Tool::drawLine(
   video::IImage* dst,
   const core::rectf& pos,
   const video::SColor& color_a,
   const video::SColor& color_b,
   bool blend )
{
   if (!dst)
      return false;

   const core::dimension2du img_size = dst->getDimension();

   if ( ( img_size.Width == 0 ) || ( img_size.Height == 0 ) )
      return false;

   const int32_t x0 = core::floor32( pos.UpperLeftCorner.X );
   const int32_t y0 = core::floor32( pos.UpperLeftCorner.Y );
   const int32_t x1 = core::floor32( pos.LowerRightCorner.X );
   const int32_t y1 = core::floor32( pos.LowerRightCorner.Y );

   return drawLine( dst, x0, y0, x1, y1, color_a, color_b, blend );
}


//! draw a bresenham rect with 1 color
//! if ( border <= 0 ) solid/filled else (border>0) outlined/border
void Tool::drawRect(
   IImage* dst,
   const core::recti& pos,
   const SColor& color,
   int32_t border,
   bool blend )
{
   int32_t x1 = pos.UpperLeftCorner.X;
   int32_t y1 = pos.UpperLeftCorner.Y;
   int32_t x2 = pos.LowerRightCorner.X;
   int32_t y2 = pos.LowerRightCorner.Y;

   if (border==0)
   {
      for (int32_t y=y1; y<=y2; y++)
      {
         for (int32_t x=x1; x<=x2; x++)
         {
            if (color.getAlpha()<255)
            {
               SColor pixel = getPixelAlways( dst, x,y );
               int32_t r = (int32_t)irr::core::clamp<f32>((f32)color.getAlpha()*(f32)color.getRed()/255.0f, 0,255.f);
               int32_t g = (int32_t)irr::core::clamp<f32>((f32)color.getAlpha()*(f32)color.getGreen()/255.0f, 0,255.f);
               int32_t b = (int32_t)irr::core::clamp<f32>((f32)color.getAlpha()*(f32)color.getBlue()/255.0f, 0,255.f);
               pixel.set( pixel.getAlpha(),
                  (u32)irr::core::s32_clamp(pixel.getRed() + r,0,255),
                  (u32)irr::core::s32_clamp(pixel.getGreen() + g,0,255),
                  (u32)irr::core::s32_clamp(pixel.getBlue() + b,0,255) );
               drawPixel( dst, x, y, pixel, blend);

            }
            else
            {
               drawPixel( dst, x, y, color, blend);
            }
         }
      }
   }
   else if ( border >= 1)
   {
      for (int32_t b=0; b<border; b++)
      {
         drawLine( dst, x1+b,y1+b, x2-b,y1+b, color, blend);
         drawLine( dst, x1+b,y1+b, x1+b,y2-b, color, blend);
         drawLine( dst, x2-b,y1+b, x2-b,y2-b, color, blend);
         drawLine( dst, x1+b,y2-b, x2-b,y2-b, color, blend);
      }
   }
}

//! draw a bresenham rect with 4 linear interpolated colors
//! if ( border <= 0 ) solid/filled else (border>0) outlined/border
void Tool::drawRect(
   IImage* dst,
   const core::recti& pos,
   const SColor& bottomLeft,
   const SColor& topLeft,
   const SColor& topRight,
   const SColor& bottomRight,
   int32_t border,
   bool blend )
{
   if (border==0)
   {
      const int32_t& x1 = pos.UpperLeftCorner.X;
      const int32_t& x2 = pos.LowerRightCorner.X;
      const int32_t& y1 = pos.UpperLeftCorner.Y-1;
      const int32_t& y2 = pos.LowerRightCorner.Y-1;

      // common factor for all colors ( 1 / (dx * dy) )
      f32 const n_inv = irr::core::reciprocal( (f32)(x2 - x1) * (f32)(y2 - y1) );

      for (int32_t y=y1; y<=y2; y++)
      {
         for (int32_t x=x1; x<=x2; x++)
         {
            f32 const f11 = (f32)(x2 - x) * (f32)(y2 - y) * n_inv; // bottomRight
            f32 const f21 = (f32)(x - x1) * (f32)(y2 - y) * n_inv; // bottomLeft
            f32 const f12 = (f32)(x2 - x) * (f32)(y - y1) * n_inv; // topRight
            f32 const f22 = (f32)(x - x1) * (f32)(y - y1) * n_inv; // topLeft

            f32 const fA = 	(f32)topLeft.getAlpha() * f11 +
                        (f32)topRight.getAlpha() * f21 +
                        (f32)bottomRight.getAlpha() * f22 +
                        (f32)bottomLeft.getAlpha() * f12;

            f32 const fR = 	(f32)topLeft.getRed() * f11 +
                        (f32)topRight.getRed() * f21 +
                        (f32)bottomRight.getRed() * f22 +
                        (f32)bottomLeft.getRed() * f12;

            f32 const fG = 	(f32)topLeft.getGreen() * f11 +
                        (f32)topRight.getGreen() * f21 +
                        (f32)bottomRight.getGreen() * f22 +
                        (f32)bottomLeft.getGreen() * f12;

            f32 const fB = 	(f32)topLeft.getBlue() * f11 +
                        (f32)topRight.getBlue() * f21 +
                        (f32)bottomRight.getBlue() * f22 +
                        (f32)bottomLeft.getBlue() * f12;

            SColor color(
               (u32)irr::core::clamp( irr::core::round32(fA), 0, 255),
               (u32)irr::core::clamp( irr::core::round32(fR), 0, 255),
               (u32)irr::core::clamp( irr::core::round32(fG), 0, 255),
               (u32)irr::core::clamp( irr::core::round32(fB), 0, 255) );

            drawPixel( dst, x, y, color, blend);
         }
      }
   }
   else if ( border == 1)
   {
//		drawLine( pos.UpperLeftCorner, core::position2di(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y), color, blend);
//		drawLine( pos.UpperLeftCorner, core::position2di(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y), color, blend);
//		drawLine( core::position2di(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y), pos.LowerRightCorner, color, blend);
//		drawLine( core::position2di(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y), pos.LowerRightCorner, color, blend);
//
      const int32_t x = pos.LowerRightCorner.X-1;
      const int32_t y = pos.LowerRightCorner.Y-1;
      drawLine( dst, pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y, x, pos.UpperLeftCorner.Y, topLeft, topRight, blend);
      drawLine( dst, pos.UpperLeftCorner.X, pos.UpperLeftCorner.Y, pos.UpperLeftCorner.X, y, topLeft, bottomLeft, blend);
      drawLine( dst, x, pos.UpperLeftCorner.Y, x,y, topRight, bottomRight, blend);
      drawLine( dst, pos.UpperLeftCorner.X, y, x,y, bottomLeft, bottomRight, blend);
   }
   else if ( border > 1)
   {
      IImage* tmp = new CImage( dst->getColorFormat(), core::dimension2du( (u32)pos.getWidth(), (u32)pos.getHeight() ) );
      if (!tmp)
         return;

      const SColor black(  0,  0,  0,  0);
      const SColor white(255,255,255,255);

      tmp->fill( black );
      drawRect( tmp, core::recti( core::position2di(0,0), tmp->getDimension()), white, border, blend);

      // common factor for all colors ( 1 / (w * h) )
      const u32 w = tmp->getDimension().Width;
      const u32 h = tmp->getDimension().Height;
      f32 const n_inv = irr::core::reciprocal( (f32)w * (f32)h );

      for (u32 y = 0; y < h; y++)
      {
         u32 x = 0;
         while (x < w)
         {
            const SColor& colorNow = tmp->getPixel(x,y);

            if (colorNow == white)
            {
               f32 const f11 = (f32)(w-1-x) * (f32)(h-1-y) * n_inv; // topLeft
               f32 const f21 = (f32)(x) * (f32)(h-1-y) * n_inv; // topRight
               f32 const f12 = (f32)(w-1-x) * (f32)(y) * n_inv; // bottomLeft
               f32 const f22 = (f32)(x) * (f32)(y) * n_inv; // bottomRight

               f32 const fA = (f32)topLeft.getAlpha() * f11 +
                           (f32)topRight.getAlpha() * f21 +
                           (f32)bottomRight.getAlpha() * f12 +
                           (f32)bottomLeft.getAlpha() * f22;

               f32 const fR = 	(f32)topLeft.getRed() * f11 +
                           (f32)topRight.getRed() * f21 +
                           (f32)bottomRight.getRed() * f12 +
                           (f32)bottomLeft.getRed() * f22;

               f32 const fG = 	(f32)topLeft.getGreen() * f11 +
                           (f32)topRight.getGreen() * f21 +
                           (f32)bottomRight.getGreen() * f12 +
                           (f32)bottomLeft.getGreen() * f22;

               f32 const fB = 	(f32)topLeft.getBlue() * f11 +
                           (f32)topRight.getBlue() * f21 +
                           (f32)bottomRight.getBlue() * f12 +
                           (f32)bottomLeft.getBlue() * f22;

               SColor color(
                  (u32)irr::core::clamp( irr::core::round32(fA), 0, 255),
                  (u32)irr::core::clamp( irr::core::round32(fR), 0, 255),
                  (u32)irr::core::clamp( irr::core::round32(fG), 0, 255),
                  (u32)irr::core::clamp( irr::core::round32(fB), 0, 255) );

               drawPixel( dst, x, y, color, blend);
            }
//				else
//				{
//					x += ( w - (border<<1)) - 2; // skip pixels to increase speed
//				}
            x++;
         }
      }

      drawImage( tmp, dst, pos.UpperLeftCorner, tmp->getDimension(), true, false);
      tmp->drop();
   }
}

//! draw a Bresenham-round-rect, 1 colors, filled (border=0) or outlined with border, option to blend
void Tool::drawRoundRect( IImage* dst, const core::recti& pos, int32_t rx, int32_t ry, const SColor& color, int32_t border, bool blend)
{
   // abort
   if (!dst)
      return;

   // abort
   if (rx < 1 && ry < 1)
      return;

   // set equal, if one radius is zero --> circle
   if (rx==0) rx = ry;
   if (ry==0) ry = rx;

   int32_t w = pos.getWidth();
   int32_t h = pos.getHeight();

//	core::position2di m = pos.getCenter();
//	core::position2di dA( (w-1)>>1, ry);
//	core::position2di lowerRight(w-1-rx, ry);
//	core::position2di upperLeft(rx, h-1-ry);
//	core::position2di upperRight(w-1-rx, h-1-ry);

   core::position2di lowerLeft = core::position2di(rx, ry) + pos.UpperLeftCorner;
   core::position2di lowerRight = core::position2di(w-1-rx, ry) + pos.UpperLeftCorner;
   core::position2di upperLeft = core::position2di(rx, h-1-ry) + pos.UpperLeftCorner;
   core::position2di upperRight = core::position2di(w-1-rx, h-1-ry) + pos.UpperLeftCorner;

   // draw a filled round-rect
   if (border == 0)
   {
      // Ellipsen-Code
      const int32_t rx2=rx*rx; // Ellipsen Radien Quadrate
      const int32_t ry2=ry*ry; // Ellipsen Radien Quadrate
      int32_t x = 0;
      int32_t y = 0;

      // Bresenham Ellipse Anfangswerte
      int32_t ddF_x=0;
      int32_t ddF_y=((rx2*ry)<<1);
      x=0;
      y=ry;

      // Bresenham Alghorithmus
      int32_t F=irr::core::round32((f32)(ry2-rx2*ry)+(f32)rx2*0.25f);
      while( ddF_x < ddF_y )
      {
         if (F >= 0)
         {
            y     -= 1;        // south
            ddF_y -= (rx2<<1);
            F     -= ddF_y;
         }
         x     += 1;          // east
         ddF_x += (ry2<<1);
         F     += ddF_x + ry2;
         drawLine( dst, upperLeft.X-x, upperLeft.Y+y, upperRight.X+x, upperRight.Y+y, color, blend);
         drawLine( dst, lowerLeft.X-x, lowerLeft.Y-y, lowerRight.X+x, lowerRight.Y-y, color, blend);
      }

      // Bresenham Alghorithmus
      F = irr::core::round32((f32)ry2*((f32)x+0.5f)*((f32)x+0.5f) + (f32)(rx2*(y-1)*(y-1) - rx2*ry2));
      while( y > 0 )
      {
         if(F <= 0)
         {
            x     += 1;        // east
            ddF_x += (ry2<<1);
            F     += ddF_x;
         }
         y     -= 1;           // south
         ddF_y -= (rx2<<1);
         F     += rx2 - ddF_y;

         drawLine( dst, upperLeft.X-x, upperLeft.Y+y, upperRight.X+x, upperRight.Y+y, color, blend);
         drawLine( dst, lowerLeft.X-x, lowerLeft.Y-y, lowerRight.X+x, lowerRight.Y-y, color, blend);
      }

      int32_t y1 = pos.UpperLeftCorner.Y + ry;
      int32_t y2 = pos.UpperLeftCorner.Y + pos.getHeight() - ry;
      y = y1;
      while( y < y2 )
      {
         drawLine( dst, pos.UpperLeftCorner.X , y, pos.LowerRightCorner.X, y, color, blend);
         y++;
      }

   }
   // draw a bordered round-rect
   else
   {
      if (border == 1)
      {
         PointList points = traverseEllipse( core::position2di(0,0), rx,ry);

         for (u32 i=0; i<points.size(); i++)
         {
            dst->setPixel(upperRight.X + points[i].X, upperRight.Y + points[i].Y, color);
            dst->setPixel(lowerRight.X + points[i].X, lowerRight.Y - points[i].Y, color);
            dst->setPixel(upperLeft.X - points[i].X, upperLeft.Y + points[i].Y, color);
            dst->setPixel(lowerLeft.X - points[i].X, lowerLeft.Y - points[i].Y, color);
         }

         //! core::recti has flaws, i.e. it does not guarantee that UpperLeftCorner is more upper and left than LowerRightCorner !!!
         const int32_t x1 = irr::core::s32_min( pos.UpperLeftCorner.X, pos.LowerRightCorner.X);
         const int32_t y1 = irr::core::s32_min( pos.UpperLeftCorner.Y, pos.LowerRightCorner.Y);
         const int32_t x2 = irr::core::s32_max( pos.UpperLeftCorner.X, pos.LowerRightCorner.X);
         const int32_t y2 = irr::core::s32_max( pos.UpperLeftCorner.Y, pos.LowerRightCorner.Y);

         drawLine( dst, x1+rx, y1, x2-1-rx, y1, color, blend);
         drawLine( dst, x1+rx, y2-1, x2-1-rx, y2-1, color, blend);
         drawLine( dst, x1, y1+ry, x1, y2-1-ry, color, blend);
         drawLine( dst, x2-1, y1+ry, x2-1, y2-1-ry, color, blend);
      }
      else
      {
         IImage* tmp = new CImage( dst->getColorFormat(), core::dimension2du(w+1,h+1));
         if (!tmp)
            return;

         tmp->fill(0);
         drawRoundRect( tmp, core::recti(0,0,(int32_t)w, (int32_t)h), rx,ry, color, 0, false);
         drawRoundRect( tmp, core::recti(border, border, (int32_t)w-border, (int32_t)h-border), rx,ry, 0, 0, false);

         drawImage( tmp, dst, pos.UpperLeftCorner, tmp->getDimension(), true, false);
         tmp->drop();

//			floodFill(x1)
      }
   }
}

void Tool::drawRoundRect( IImage* dst, const core::recti& pos, int32_t rx, int32_t ry, const SColor& topLeft, const SColor& topRight, const SColor& bottomRight, const SColor& bottomLeft, int32_t border, bool blend)
{
   // abort
   if (!dst)
      return;

   //! guarantee that UpperLeftCorner is more upperleft than LowerRightCorner
   const int32_t x1 = irr::core::s32_min( pos.UpperLeftCorner.X, pos.LowerRightCorner.X);
   const int32_t y1 = irr::core::s32_min( pos.UpperLeftCorner.Y, pos.LowerRightCorner.Y);
   const int32_t x2 = irr::core::s32_max( pos.UpperLeftCorner.X, pos.LowerRightCorner.X);
   const int32_t y2 = irr::core::s32_max( pos.UpperLeftCorner.Y, pos.LowerRightCorner.Y);

   core::dimension2du size( (u32)(x2-x1)+1, (u32)(y2-y1)+1 );

   // abort
   if ((size.Width == 0) || (size.Height == 0))
      return;

   // create temporary canvas-image
   IImage* tmp = new CImage( dst->getColorFormat(), size);

   // abort
   if (!tmp)
      return;

   const SColor black(0,0,0,0);
   const SColor white(255,255,255,255);

   tmp->fill( black );

   drawRoundRect( tmp, core::recti( core::position2di(0,0), size), rx,ry, white, border, false);

   // interpolate colors
   // common factor for all colors ( 1 / (dx * dy) )
   f32 const n_inv = irr::core::reciprocal( (f32)(x2 - x1) * (f32)(y2 - y1) );

   for (u32 y=0; y<size.Height; y++)
   {
      for (u32 x=0; x<size.Width; x++)
      {
         if (getPixel( tmp,x,y ) == white)
         {
            f32 const f11 = (f32)(size.Width-1-x) * (f32)(size.Height-1 - y) * n_inv; // topLeft
            f32 const f21 = (f32)(x) * (f32)(size.Height-1 - y) * n_inv; // topRight
            f32 const f12 = (f32)(size.Width-1 - x) * (f32)(y) * n_inv; // bottomLeft
            f32 const f22 = (f32)(x) * (f32)(y) * n_inv; // bottomRight

            f32 const fA = 	(f32)topLeft.getAlpha() * f11 +
                        (f32)topRight.getAlpha() * f21 +
                        (f32)bottomRight.getAlpha() * f12 +
                        (f32)bottomLeft.getAlpha() * f22;

            f32 const fR = 	(f32)topLeft.getRed() * f11 +
                        (f32)topRight.getRed() * f21 +
                        (f32)bottomRight.getRed() * f12 +
                        (f32)bottomLeft.getRed() * f22;

            f32 const fG = 	(f32)topLeft.getGreen() * f11 +
                        (f32)topRight.getGreen() * f21 +
                        (f32)bottomRight.getGreen() * f12 +
                        (f32)bottomLeft.getGreen() * f22;

            f32 const fB = 	(f32)topLeft.getBlue() * f11 +
                        (f32)topRight.getBlue() * f21 +
                        (f32)bottomRight.getBlue() * f12 +
                        (f32)bottomLeft.getBlue() * f22;

            SColor color(
               (u32)irr::core::clamp( irr::core::round32(fA), 0, 255),
               (u32)irr::core::clamp( irr::core::round32(fR), 0, 255),
               (u32)irr::core::clamp( irr::core::round32(fG), 0, 255),
               (u32)irr::core::clamp( irr::core::round32(fB), 0, 255) );

            drawPixel( tmp, x, y, color, blend);
         }
      }
   }

   // drawing finished and copy dst to srcImage
   drawImage( dst, tmp, core::position2di(x1,y1), size, true, false);
   tmp->drop();
}


//! Bresenham circle with 1 color(s), if border <= 0 then filled/solid shape
void Tool::drawCircle( IImage* dst, const core::position2di& center,
   int32_t radius,	const SColor& color, int32_t border, bool blend)
{
   if ( !dst ) return;
   if ( radius < 0 ) return;

   else if ( radius == 0 )
   {
      drawPixel( dst, center.X, cy, color, blend);
   }
   else if ( radius == 1)
   {
      if ( border == 0 )
      {
         drawPixel( dst, center.X,cy,color,blend);
      }
      drawPixel( dst, center.X-1,cy,color,blend);
      drawPixel( dst, center.X,cy-1,color,blend);
      drawPixel( dst, center.X,cy+1,color,blend);
      drawPixel( dst, center.X+1,cy,color,blend);
   }
   else if ( radius == 2 )
   {
      if ( border == 0 )
      {
         drawPixel( dst, center.X-1,cy+1,color,blend);
         drawPixel( dst, center.X-1,cy,color,blend);
         drawPixel( dst, center.X,cy+1,color,blend);
         drawPixel( dst, center.X,cy,color,blend);
      }

      drawPixel( dst, center.X-2,cy+1,color,blend);
      drawPixel( dst, center.X-2,cy,  color,blend);
      drawPixel( dst, center.X-1,cy+2,color,blend);
      drawPixel( dst, center.X-1,cy-1,color,blend);
      drawPixel( dst, center.X,  cy+2,color,blend);
      drawPixel( dst, center.X,  cy-1,color,blend);
      drawPixel( dst, center.X+1,cy+1,color,blend);
      drawPixel( dst, center.X+1,cy,  color,blend);
   }
   else
   {
      // 'Bresenham' Algorithmus (Achtelkreis Symmetrie)
      // ohne Trigonometrische- und Wurzel-Funktionen
      // und Spiegelung auf Restliche 7/8

      if ( border == 0 ) // filled
      {
         int32_t i,j,F;
         i = 0;
         j = radius;
         F = 1 - radius;

         while (i < j)
         {
            ++i;
            if (F < 0)
            {
               F += (i<<1) - 1;
            }
            else
            {
               F += ((i - j)<<1);
               --j;
            }

            // Verbesserungen by Benjamin Hampe (c) 2012
            drawLine( dst, center.X-i, cy+j-1, center.X+i-1,cy+j-1, color, blend );
            drawLine( dst, center.X-j, cy+i-1, center.X+j-1,cy+i-1, color, blend );
            drawLine( dst, center.X-j, cy-i, center.X+j-1, cy-i, color, blend );
            drawLine( dst, center.X-i, cy-j, center.X+i-1, cy-j, color, blend );
         }
      }

      // 'Bresenham' Algorithmus (Achtelkreis Symmetrie)
      // ohne Trigonometrische- und Wurzel-Funktionen
      // und Spiegelung auf Restliche 7/8

      else if (border == 1)
      {
         int32_t i,j,F;
         i = 0;
         j = radius;
         F = 1 - radius;

         while (i < j)
         {
            ++i;
            if (F < 0)
            {
               F += (i<<1) - 1;
            }
            else
            {
               F += ((i - j)<<1);
               --j;
            }
            // Verbesserungen by Benjamin Hampe (c) 2012
            drawPixel( dst, center.X+i-1,cy-j,color, blend); // 1st quadrant
            drawPixel( dst, center.X+j-1,cy-i,color, blend); // 1st quadrant
            drawPixel( dst, center.X+i-1,cy+j-1,color, blend); // 2nd quadrant
            drawPixel( dst, center.X+j-1,cy+i-1,color, blend); // 2nd quadrant
            drawPixel( dst, center.X-i,cy+j-1,color, blend); // 3rd quadrant
            drawPixel( dst, center.X-j,cy+i-1,color, blend); // 3rd quadrant
            drawPixel( dst, center.X-i,cy-j,color, blend); // 4th quadrant
            drawPixel( dst, center.X-j,cy-i,color, blend); // 4th quadrant
         }
      }

      // by Benjamin Hampe
      // create circle from undistorted temporary image

      else if (border > 1)
      {
         if (radius - border > 1)
         {
            IImage* tmp = new CImage( dst->getColorFormat(), core::dimension2du( radius<<1, radius<<1) );
            if (!tmp)
               return;

//				tmp->enableColorKey();
            tmp->fill( 0 );
            drawCircle( tmp, core::position2di(radius,radius), radius, color, 0, blend);
            drawCircle( tmp, core::position2di(radius,radius), radius - border, 0, 0, blend);
            drawImage( tmp, dst, center - core::position2di(radius,radius), tmp->getDimension(), true, false);
            tmp->drop();
         }
      }
   }
}

//! Bresenham circle with 2 interpolated color(s) from center to outline
//! if (border <= 0) then filled/solid shape
//void ImageTool::drawCircle( IImage* dst, const core::position2di& center, int32_t radius, const SColor& c1, const SColor& c2, int32_t border, bool blend)
//{
//	// abort
//	if (!dst) return;
//
//	// abort
//	if (radius < 0)
//	{
//		return;
//	}
//	else if (radius == 0)
//	{
//		drawPixel( dst,center.X,cy,c2,blend);
//		return;
//	}
//	else if (radius == 1)
//	{
//		drawPixel( dst,center.X,cy,c2,blend);
//		drawPixel( dst,center.X,cy-1,c1,blend);
//		drawPixel( dst,center.X,cy+1,c1,blend);
//		drawPixel( dst,center.X+1,cy,c1,blend);
//		drawPixel( dst,center.X-1,cy,c1,blend);
//		return;
//	}
//	else
//	{
//		if (border==0) // filled
//		{
//			IImage* tmp = new CImage( dst->getColorFormat(), core::dimension2du( radius<<1, radius<<1) );
//			if (!tmp)
//				return;
//			tmp->fill( 0 );
//			drawCircle( tmp, core::position2di(radius,radius), radius, SColor(255,255,255,255), 0, blend);
//
//			f32 const fA = (f32)c1.getAlpha(), fdA = (f32)c2.getAlpha() - fA;
//			f32 const fR = (f32)c1.getRed(), fdR = (f32)c2.getRed() - fR;
//			f32 const fG = (f32)c1.getGreen(), fdG = (f32)c2.getGreen() - fG;
//			f32 const fB = (f32)c1.getBlue(), fdB = (f32)c2.getBlue() - fB;
//			const SColor white(255,255,255,255);
//			f32 const fw = 0.5f*(f32)tmp->getDimension().Width;
//			f32 const fh = 0.5f*(f32)tmp->getDimension().Height;
//			f32 const invRadius = irr::core::reciprocal( (f32)radius );
//
//			const core::dimension2du& Size = tmp->getDimension();
//			for (u32 y = 0; y < Size.Height; y++)
//			{
//				for (u32 x = 0; x < Size.Width; x++)
//				{
//					const SColor& colorNow = tmp->getPixel(x,y);
//					if (colorNow == white)
//					{
//						f32 const dx = (f32)x - fw;
//						f32 const dy = (f32)y - fh;
//						f32 f = squareroot( dx*dx + dy*dy ) * invRadius;
//						u32 uA = (u32)irr::core::s32_clamp( irr::core::round32( fA + f*fdA ), 0, 255);
//						u32 uR = (u32)irr::core::s32_clamp( irr::core::round32( fR + f*fdR ), 0, 255);
//						u32 uG = (u32)irr::core::s32_clamp( irr::core::round32( fG + f*fdG ), 0, 255);
//						u32 uB = (u32)irr::core::s32_clamp( irr::core::round32( fB + f*fdB ), 0, 255);
//						tmp->setPixel( x, y, SColor(uA,uR,uG,uB), blend );
//					}
//				}
//			}
//
//			drawImage( tmp, dst, center - core::position2di(radius,radius), tmp->getDimension(), true, false);
//			tmp->drop();
//			return;
//		}
//		else if (border == 1) // outline with color c1
//		{
//			drawCircle( dst, center, radius, c1, 1, blend);
//		}
//		else if (border > 1)
//		{
//			if (radius - border > 0)
//			{
//				IImage* tmp = new CImage( dst->getColorFormat(), core::dimension2du( radius<<1, radius<<1) );
//				if (!tmp)
//					return;
//
//				const SColor black(  0,  0,  0,  0);
//				const SColor white(255,255,255,255);
//
//				tmp->fill( black );
//				drawCircle( tmp, core::position2di(radius,radius), radius, white, 0, blend);
//				drawCircle( tmp, core::position2di(radius,radius), radius-border, black, 0, blend);
//
//				f32 const fA = (f32)c1.getAlpha(), fdA = (f32)c2.getAlpha() - fA;
//				f32 const fR = (f32)c1.getRed(), fdR = (f32)c2.getRed() - fR;
//				f32 const fG = (f32)c1.getGreen(), fdG = (f32)c2.getGreen() - fG;
//				f32 const fB = (f32)c1.getBlue(), fdB = (f32)c2.getBlue() - fB;
//
//				const core::dimension2du& Size = tmp->getDimension();
//
//				f32 const fw = 0.5f*(f32)Size.Width;
//				f32 const fh = 0.5f*(f32)Size.Height;
//				f32 const invRadius = irr::core::reciprocal( (f32)border );
//				const s32 innerCircle = radius-border;
//
//				for (u32 y = 0; y < Size.Height; y++)
//				{
//					for (u32 x = 0; x < Size.Width; x++)
//					{
//						const SColor& colorNow = tmp->getPixel(x,y);
//						if (colorNow == white)
//						{
//							f32 const dx = (f32)x - fw;
//							f32 const dy = (f32)y - fh;
//
//							f32 f = (squareroot( dx*dx + dy*dy ) - (f32)innerCircle ) * invRadius;
//							u32 uA = (u32)irr::core::s32_clamp( irr::core::round32( fA + f*fdA ), 0, 255);
//							u32 uR = (u32)irr::core::s32_clamp( irr::core::round32( fR + f*fdR ), 0, 255);
//							u32 uG = (u32)irr::core::s32_clamp( irr::core::round32( fG + f*fdG ), 0, 255);
//							u32 uB = (u32)irr::core::s32_clamp( irr::core::round32( fB + f*fdB ), 0, 255);
//							tmp->setPixel( x, y, SColor(uA,uR,uG,uB), blend );
//						}
//					}
//				}
//
//				drawImage( tmp, dst, center - core::position2di(radius,radius), tmp->getDimension(), true, false);
//				tmp->drop();
//			}
//		}
//	}
//}


//! draw a Bresenham-ellipse, 1 color, filled or outlined, option to blend
void Tool::drawEllipse( IImage* dst, const core::position2di& center, s32 rx, s32 ry, const SColor& color, s32 border, bool blend)

{
   if (!dst) return; // abort
//			const s32 w = dest->getDimension().Width;
//			const s32 h = dest->getDimension().Height;

   // Abbruchbedingung
   if (rx < 1 && ry < 1) return;
   if (rx==0) rx = ry;
   if (ry==0) ry = rx;

   if (rx==ry)
      drawCircle( dst, center, rx, color, border, blend);

   //if (rw==1 && rh==1) dest->setPixel(x,y,color);

   const s32 rx2=rx*rx; // Ellipsen Radien Quadrate
   const s32 ry2=ry*ry; // Ellipsen Radien Quadrate
   s32 x = center.X;
   s32 y = cy;
//	const s32 mx = center.X;	 // Ellipsen Mittelpunkt
//	const s32 my = cy; 	 // Ellipsen Mittelpunkt

   // Bresenham Anfangswerte
   // Trick: nur 1/4 Ellipse berechne und restliche 3/4 durch Spiegelung (gerade Ellipse!!!)
   s32 ddF_x=0;
   s32 ddF_y=((rx2*ry)<<1);
   x=0;
   y=ry;

   // Bresenham Alghorithmus
   s32 F=irr::core::round32((f32)(ry2-rx2*ry)+(f32)rx2*0.25f);
   while( ddF_x < ddF_y )
   {
      if (F >= 0)
      {
         y     -= 1;        // south
         ddF_y -= (rx2<<1);
         F     -= ddF_y;
      }
      x     += 1;          // east
      ddF_x += (ry2<<1);
      F     += ddF_x + ry2;

      if (border==0)
      {
         drawLine( dst, center.X-x, cy+y, center.X+x, cy+y, color, blend);
         drawLine( dst, center.X-x, cy-y, center.X+x, cy-y, color, blend);
      }
      else
      {
         drawPixel( dst, x+center.X, y+cy, color, blend);
         drawPixel( dst, -x+center.X, y+cy, color, blend);
         drawPixel( dst, x+center.X, -y+cy, color, blend);
         drawPixel( dst, -x+center.X, -y+cy, color, blend);

         // Spiegelung mit meinen Anpassungen
         // drawPixel( dst, x+mx, y+my, color, blend);
         // drawPixel( dst, -x+mx+1, y+my, color, blend);
         // drawPixel( dst, x+mx, -y+my+1, color, blend);
         // drawPixel( dst, -x+mx+1, -y+my+1, color, blend);
      }
   }

   // Bresenham Alghorithmus
   F = irr::core::round32((f32)ry2*((f32)x+0.5f)*((f32)x+0.5f) + (f32)(rx2*(y-1)*(y-1) - rx2*ry2));
   while( y > 0 )
   {
      if(F <= 0)
      {
         x     += 1;        // east
         ddF_x += (ry2<<1);
         F     += ddF_x;
      }
      y     -= 1;           // south
      ddF_y -= (rx2<<1);
      F     += rx2 - ddF_y;

      if (border==0)
      {
         drawLine( dst, center.X-x, cy+y, center.X+x, cy+y, color, blend);
         drawLine( dst, center.X-x, cy-y, center.X+x, cy-y, color, blend);
      }
      else
      {
         drawPixel( dst, x+center.X, y+cy, color, blend);
         drawPixel( dst, -x+center.X, y+cy, color, blend);
         drawPixel( dst, x+center.X, -y+cy, color, blend);
         drawPixel( dst, -x+center.X, -y+cy, color, blend);

         // Spiegelung mit meinen Anpassungen
//			drawPixel( dst, x+mx, y+my, color, blend);
//			drawPixel( dst, -x+mx+1, y+my, color, blend);
//			drawPixel( dst, x+mx, -y+my+1, color, blend);
//			drawPixel( dst, -x+mx+1, -y+my+1, color, blend);
      }
   }
}

//! Bresenham ellipse
void Tool::drawEllipse( IImage* dst, const core::position2di& center, s32 rx, s32 ry, const SColor& c1, const SColor& c2, s32 border, bool blend)
{
   if (!dst) return; // abort
//			const s32 w = dest->getDimension().Width;
//			const s32 h = dest->getDimension().Height;

   // Abbruchbedingung
   if (rx < 1 && ry < 1) return;
   if (rx==0) rx = ry;
   if (ry==0) ry = rx;

   if (rx==ry)
      drawCircle( dst, center, rx, c1, border, blend);

   //if (rw==1 && rh==1) dest->setPixel(x,y,color);

   const s32 rx2=rx*rx; // Ellipsen Radien Quadrate
   const s32 ry2=ry*ry; // Ellipsen Radien Quadrate
   s32 x = center.X;
   s32 y = cy;
//	const s32 mx = center.X;	 // Ellipsen Mittelpunkt
//	const s32 my = cy; 	 // Ellipsen Mittelpunkt

   // Bresenham Anfangswerte
   // Trick: nur 1/4 Ellipse berechne und restliche 3/4 durch Spiegelung (gerade Ellipse!!!)
   s32 ddF_x=0;
   s32 ddF_y=((rx2*ry)<<1);
   x=0;
   y=ry;

   // Bresenham Alghorithmus
   s32 F=irr::core::round32((f32)(ry2-rx2*ry)+(f32)rx2*0.25f);
   while( ddF_x < ddF_y )
   {
      if (F >= 0)
      {
         y     -= 1;        // south
         ddF_y -= (rx2<<1);
         F     -= ddF_y;
      }
      x     += 1;          // east
      ddF_x += (ry2<<1);
      F     += ddF_x + ry2;

      if (border==0)
      {
         drawLine( dst, center.X-x, cy+y, center.X+x, cy+y, c1, blend);
         drawLine( dst, center.X-x, cy-y, center.X+x, cy-y, c1, blend);
      }
      else
      {
         drawPixel( dst, x+center.X, y+cy, c1, blend);
         drawPixel( dst, -x+center.X, y+cy, c1, blend);
         drawPixel( dst, x+center.X, -y+cy, c1, blend);
         drawPixel( dst, -x+center.X, -y+cy, c1, blend);
      }
   }

   // Bresenham Alghorithmus
   F = irr::core::round32((f32)ry2*((f32)x+0.5f)*((f32)x+0.5f) + (f32)(rx2*(y-1)*(y-1) - rx2*ry2));
   while( y > 0 )
   {
      if(F <= 0)
      {
         x     += 1;        // east
         ddF_x += (ry2<<1);
         F     += ddF_x;
      }
      y     -= 1;           // south
      ddF_y -= (rx2<<1);
      F     += rx2 - ddF_y;

      if (border==0)
      {
         drawLine( dst, center.X-x, cy+y, center.X+x, cy+y, c1, blend);
         drawLine( dst, center.X-x, cy-y, center.X+x, cy-y, c1, blend);
      }
      else
      {
         drawPixel( dst, x+center.X, y+cy, c1, blend);
         drawPixel( dst, -x+center.X, y+cy, c1, blend);
         drawPixel( dst, x+center.X, -y+cy, c1, blend);
         drawPixel( dst, -x+center.X, -y+cy, c1, blend);
      }
   }
}

#endif

