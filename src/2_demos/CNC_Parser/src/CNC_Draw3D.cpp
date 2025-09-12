#include "CNC_Draw3D.hpp"

namespace de {
namespace cnc {


// static
Image
CNC_Draw3D::createPreviewImage( int w, int h, std::vector< CNC_ColorVertex > const & vertices )
{
   PerformanceTimer perf;
   perf.start();

   // Compute BoundingBox
   auto bbox = CNC_ColorVertex::computeBoundingBox( vertices );
   auto bboxSize = bbox.Max - bbox.Min;

   // Create preview image
   perf.start();
   Image img( w, h );
   img.fill( 0xFFFFFFFF );
   glm::mat4 projMat = glm::perspectiveFovLH(
            glm::radians(45.0f),float(img.w()), float(img.h()), .1f, 38000.0f);

   glm::vec3 pos = glm::vec3(bbox.Min.x + bboxSize.x*.3f,
                             bbox.Min.y + bboxSize.y*.5f,
                             bbox.Min.z - bboxSize.z*1.1f);
   glm::vec3 target = glm::vec3(bbox.Min.x + bboxSize.x*0.6f,
                                bbox.Min.y + bboxSize.y*0.3f,
                                bbox.Min.z + bboxSize.z*.3f);
   glm::vec3 up = glm::vec3(0,1,0);

   glm::mat4 viewMat = glm::lookAt(pos, target, up);
   glm::mat4 mvp = (projMat * viewMat);
   CNC_Draw3D::drawLineStrip( img, vertices, mvp, true );
   CNC_Draw3D::drawLineBox(img, bbox.Min, bbox.Max, 0xFFCCCCCC, mvp, false );
   CNC_Draw3D::drawLine(img, glm::vec3(0,0,0), glm::vec3(100,0,0), 0xFF0000FF, mvp, false );
   CNC_Draw3D::drawLine(img, glm::vec3(0,0,0), glm::vec3(0,100,0), 0xFF00FF00, mvp, false );
   CNC_Draw3D::drawLine(img, glm::vec3(0,0,0), glm::vec3(0,0,100), 0xFFFF0000, mvp, false );

   std::ostringstream s;
   s << "CameraPos(" << pos << ")";
   ImagePainter::drawText5x8( img, 20, 20, s.str(), 0xFFDD9999 );
   s.str(""); s << "CameraTarget(" << target << ")";
   ImagePainter::drawText5x8( img, 20, 40, s.str(), 0xFFDDA088 );
   s.str(""); s << "CameraDir(" << glm::normalize(target - pos) << ")";
   ImagePainter::drawText5x8( img, 20, 60, s.str(), 0xFFDD8088 );
   s.str(""); s << "ColorLines(" << vertices.size() << ")";
   ImagePainter::drawText5x8( img, 20, 80, s.str(), 0xFFDD8088 );

   perf.stop();
   DE_DEBUG("needed(",perf.toSeconds(), "s)")

   return img;
}

// static
Image
CNC_Draw3D::createWallpaperImage( int w, int h, int n )
{
   PerformanceTimer perf;
   perf.start();

   // Create wallpaper image
   dbRandomize();

   Image img(w,h);
   img.fill( 0xFFFFFFFF );

   int cx;
   int cy;
   /*
   for ( int i = 0; i < n; ++i )
   {
      cx = rand()%w;
      cy = rand()%h;
      int r = 1 + (rand() % (std::min(w,h)/10));
      u32 color = randomColor();
      ImagePainter::drawCircle(img,cx,cy,r,color, false );
   }
   */
   for ( int i = 0; i < n; ++i )
   {
      cx = rand()%w;
      cy = rand()%h;
      int r = 1 + (rand() % (std::min(w,h)/8));
      u32 color = randomColor( 0x08 );
      ImagePainter::drawCircle(img,cx,cy,r,color, true );
   }

   cx = w/2;
   cy = h/2;
   float const k = float(2.0*M_PI) / float(10*n);

   for ( int i = 0; i < n; ++i )
   {
      float phi = float(i) * k;
      float s = sinf( phi );
      float c = cosf( phi );
      int x1 = cx + int(-s * float(cx));
      int y1 = cy + int(-c * float(cy));
      int x2 = cx + int( s * float(cx));
      int y2 = cy + int( c * float(cy));
      u32 color = 0x02FFFFFF;
      ImagePainter::drawLine( img, x1,y1,x2,y2, color, true );
   }

   for ( int i = 0; i < n; ++i )
   {
      cx = rand()%w;
      cy = rand()%h;
      int r = 1 + (rand() % (std::min(w,h)/2));
      u32 color = randomColor( 0x40 );
      ImagePainter::drawCircleBorder(img,cx,cy,r,color, true );
   }

   perf.stop();
   DE_DEBUG("Create wallpaper image needed ",perf.toSeconds(), " seconds.")

   return img;
}

// static
Image
CNC_Draw3D::testClipping( int w, int h )
{
   Image img(w,h);

   int const n = 1000;
   float const k = float(2.0*M_PI) / float(n);


   for ( int i = 0; i < 1000; ++i )
   {
      float phi = float(i) * k;
      float s = sinf( phi );
      float c = cosf( phi );

      //ImagePainter::drawLine( )
   }

   return img;
}


// static
void
CNC_Draw3D::drawLineBox( Image & img, glm::vec3 minCorner, glm::vec3 maxCorner, u32 color,
                         glm::mat4 const & mvp, bool clip )
{
   f32 x1 = minCorner.x;
   f32 y1 = minCorner.y;
   f32 z1 = minCorner.z;
   f32 x2 = maxCorner.x;
   f32 y2 = maxCorner.y;
   f32 z2 = maxCorner.z;
   glm::vec3 A(x1,y1,z1);
   glm::vec3 B(x1,y2,z1);
   glm::vec3 C(x2,y2,z1);
   glm::vec3 D(x2,y1,z1);
   glm::vec3 E(x1,y1,z2);
   glm::vec3 F(x1,y2,z2);
   glm::vec3 G(x2,y2,z2);
   glm::vec3 H(x2,y1,z2);
   drawLine( img, A,B, color, mvp, clip );
   drawLine( img, B,C, color, mvp, clip );
   drawLine( img, C,D, color, mvp, clip );
   drawLine( img, D,A, color, mvp, clip );

   drawLine( img, A,E, color, mvp, clip );
   drawLine( img, B,F, color, mvp, clip );
   drawLine( img, C,G, color, mvp, clip );
   drawLine( img, D,H, color, mvp, clip );

   drawLine( img, E,F, color, mvp, clip );
   drawLine( img, F,G, color, mvp, clip );
   drawLine( img, G,H, color, mvp, clip );
   drawLine( img, H,E, color, mvp, clip );

}

// static
void
CNC_Draw3D::drawLine( Image & img, glm::vec3 A, glm::vec3 B,
                      u32 color, glm::mat4 const & mvp, bool clip )
{
   glm::ivec4 line2d = transformLine( A, B, mvp, img.w(), img.h() );
   if ( clip ) line2d = clipLine( line2d, img.w(), img.h() );
   ImagePainter::drawLine( img, line2d.x, line2d.y, line2d.z, line2d.w, color, true );
}

// static 
void
CNC_Draw3D::drawLineStrip( Image & img, std::vector< CNC_ColorVertex > const & vertices,
                           glm::mat4 const & mvp, bool clip )
{
   if ( vertices.size() < 2 ) return;
   for ( size_t i = 1; i < vertices.size(); ++i )
   {
      auto const & A = vertices[i-1];
      auto const & B = vertices[i];
      drawLine( img, A.pos, B.pos, A.color, mvp, clip );
   }
}

// static
glm::ivec4
CNC_Draw3D::transformLine( glm::vec3 A, glm::vec3 B, glm::mat4 const & mvp, int w, int h )
{
   std::swap(A.y,A.z);
   std::swap(B.y,B.z);
   glm::vec4 clipSpaceA = mvp * glm::vec4(A,1.0f);
   glm::vec4 clipSpaceB = mvp * glm::vec4(B,1.0f);
   clipSpaceA /= clipSpaceA.w;
   clipSpaceB /= clipSpaceB.w;
   glm::vec2 const screenSize( w, h );
   glm::vec2 ndcA = screenSize * ((glm::vec2(clipSpaceA.x,clipSpaceA.y) * 0.5f) + 0.5f);
   glm::vec2 ndcB = screenSize * ((glm::vec2(clipSpaceB.x,clipSpaceB.y) * 0.5f) + 0.5f);

   return glm::ivec4( int(ndcA.x), int(ndcA.y), int(ndcB.x), int(ndcB.y) );
}

// static
glm::ivec4
CNC_Draw3D::clipLine( glm::ivec4 const & ab, int w, int h )
{
   s32 Ax = ab.x; // gimme timme
   s32 Ay = ab.y;
   s32 Bx = ab.z;
   s32 By = ab.w;

   if ( Ax > Bx ) std::swap(Ax,Bx); // sanitize
   if ( Ay > By ) std::swap(Ay,By);

   float fAx = Ax; // gimme timme
   float fAy = Ay;
   float fBx = Bx;
   float fBy = By;

   float dx = fBx - fAx; // delta-x
   float dy = fBy - fAy; // delta-y

   float m = float( dy ) / float( dx ); // Anstieg m = dy / dx
   float n = ((fAx*fBy) - (fAy*fBx)) / (fAx - fBx); // Benni math

   if ( std::isinf(m) || std::isnan(m) )
   {
      return glm::ivec4(0,0,0,0); // guess we dont draw this line.
   }

   if ( std::isinf(n) || std::isnan(n) )
   {
      return glm::ivec4(0,0,0,0); // guess we dont draw this line.
   }

   // Test benni math: Must be able to generate A,B from y=f(x)=mx+n
   float tAy = fAx * m + n;
   float tBy = fBx * m + n;

   auto almostEqual = [] ( float a, float b, float eps = 1.0 ) -> bool
   {
      return std::abs( b-a ) <= eps;
   };

   if ( !almostEqual(tAy,fAy) )
   {
      std::cout << "tAy("<<tAy<<") != fAy("<<fAy<<")" << std::endl;
   }

   if ( !almostEqual(tBy,fBy) )
   {
      std::cout << "tBy("<<tBy<<") != fBy("<<fBy<<")" << std::endl;
   }

   if ( fAx < 0 ) { fAx = 0; fAy = n; }
   if ( fAx > w ) { fAx = w; fAy = m*w + n; }
   if ( fBx < 0 ) { fBx = 0; fBy = n; }
   if ( fBx > w ) { fBx = w; fBy = m*w + n; }

   if ( fAy < 0 ) { fAx = -n/m; fAy = 0; }
   if ( fAy > h ) { fAx = (h-n)/m; fAy = h; }
   if ( fBy < 0 ) { fBx = -n/m; fBy = 0; }
   if ( fBy > h ) { fBx = (h-n)/m; fBy = h; }
   return glm::ivec4( fAx, fAy, fBx, fBy );
}

// static
glm::ivec4
CNC_Draw3D::clipLine( glm::ivec4 const & line, glm::ivec4 const & clipRect )
{
   int x1 = clipRect.x;
   int y1 = clipRect.y;
   int w = clipRect.z;
   int h = clipRect.w;
   int x2 = x1 + w;
   int y2 = y1 + h;

   s32 Ax = line.x;
   s32 Ay = line.y;
   s32 Bx = line.z;
   s32 By = line.w;

   if ( Ax > Bx ) std::swap(Ax,Bx); // sanitize
   if ( Ay > By ) std::swap(Ay,By);

   float fAx = Ax;
   float fAy = Ay;
   float fBx = Bx;
   float fBy = By;

   float dx = fBx - fAx; // delta-x
   float dy = fBy - fAy; // delta-y

   float m = float( dy ) / float( dx ); // Anstieg m = dy / dx
   float n = ((fAx*fBy) - (fAy*fBx)) / (fAx - fBx); // Benni math

   if ( std::isinf(m) || std::isnan(m) )
   {
      return glm::ivec4(0,0,0,0); // guess we dont draw this line.
   }

   if ( std::isinf(n) || std::isnan(n) )
   {
      return glm::ivec4(0,0,0,0); // guess we dont draw this line.
   }

   // Test benni math: Must be able to generate A,B from y=f(x)=mx+n
   float tAy = fAx * m + n;
   float tBy = fBx * m + n;

   auto almostEqual = [] ( float a, float b, float eps = 1.0 ) -> bool
   {
      return std::abs( b-a ) <= eps;
   };

   if ( !almostEqual(tAy,fAy) )
   {
      std::cout << "tAy("<<tAy<<") != fAy("<<fAy<<")" << std::endl;
   }

   if ( !almostEqual(tBy,fBy) )
   {
      std::cout << "tBy("<<tBy<<") != fBy("<<fBy<<")" << std::endl;
   }

   if ( fAx < 0 ) { fAx = 0; fAy = n; }
   if ( fAx > w ) { fAx = w; fAy = m*w + n; }
   if ( fBx < 0 ) { fBx = 0; fBy = n; }
   if ( fBx > w ) { fBx = w; fBy = m*w + n; }

   if ( fAy < 0 ) { fAx = -n/m; fAy = 0; }
   if ( fAy > h ) { fAx = (h-n)/m; fAy = h; }
   if ( fBy < 0 ) { fBx = -n/m; fBy = 0; }
   if ( fBy > h ) { fBx = (h-n)/m; fBy = h; }
   return glm::ivec4( fAx, fAy, fBx, fBy );
}

/*
// static
glm::ivec4
CNC_Draw3D::clipLine( glm::ivec4 const & ab, int w, int h )
{
   s32 Ax = ab.x; // gimme timme
   s32 Ay = ab.y;
   s32 Bx = ab.z;
   s32 By = ab.w;

   if ( Ax > Bx ) std::swap(Ax,Bx); // sanitize
   if ( Ay > By ) std::swap(Ay,By);

   float fAx = Ax; // gimme timme
   float fAy = Ay;
   float fBx = Bx;
   float fBy = By;

   float dx = fBx - fAx; // delta-x
   float dy = fBy - fAy; // delta-y

   float m = float( dy ) / float( dx ); // Anstieg m = dy / dx
   float n = ((fAx*fBy) - (fAy*fBx)) / (fAx - fBx); // Benni math

   if ( std::isinf(m) || std::isnan(m) )
   {
      return glm::ivec4(0,0,0,0); // guess we dont draw this line.
   }

   if ( std::isinf(n) || std::isnan(n) )
   {
      return glm::ivec4(0,0,0,0); // guess we dont draw this line.
   }

   // Test benni math: Must be able to generate A,B from y=f(x)=mx+n
   float tAy = fAx * m + n;
   float tBy = fBx * m + n;

   auto almostEqual = [] ( float a, float b, float eps = 1.0 ) -> bool
   {
      return std::abs( b-a ) <= eps;
   };

   if ( !almostEqual(tAy,fAy) )
   {
      std::cout << "tAy("<<tAy<<") != fAy("<<fAy<<")" << std::endl;
   }

   if ( !almostEqual(tBy,fBy) )
   {
      std::cout << "tBy("<<tBy<<") != fBy("<<fBy<<")" << std::endl;
   }

   if ( fAx < 0 ) { fAx = 0; fAy = n; }
   if ( fAx > w ) { fAx = w; fAy = m*w + n; }
   if ( fBx < 0 ) { fBx = 0; fBy = n; }
   if ( fBx > w ) { fBx = w; fBy = m*w + n; }

   if ( fAy < 0 ) { fAx = -n/m; fAy = 0; }
   if ( fAy > h ) { fAx = (h-n)/m; fAy = h; }
   if ( fBy < 0 ) { fBx = -n/m; fBy = 0; }
   if ( fBy > h ) { fBx = (h-n)/m; fBy = h; }
   return glm::ivec4( fAx, fAy, fBx, fBy );
}
*/

} // end namespace cnc
} // end namespace de
