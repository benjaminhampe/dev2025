#pragma once
#include <de/image/Image.h>
#include <de/image/Bresenham.h>

// #include <de_image/paint/de_Pen.h>
// #include <de_image/paint/de_Brush.h>

// #ifdef DE_USE_TRUETYPE_FONTS
// #include <de_image/font/de_Font.h>
// #endif

namespace de {

/*
// ===========================================================================
struct Span
// ===========================================================================
{
   int32_t y;
   int32_t x1;
   int32_t x2;
   uint32_t color;
};

struct ImageRGBA32
{
    int w, h;
    std::vector<uint8_t> pixels; // RGBA: 4 Kanäle pro Pixel
};

// Hilfsfunktion für Lanczos-Kernel
double lanczosKernel(double x, int a)
{
    if (x == 0.0) return 1.0;
    if (x < -a || x > a) return 0.0;
    return a * std::sin(M_PI * x) * std::sin(M_PI * x / a) / (M_PI * M_PI * x * x);
}

// Neuberechnung eines Pixels
uint8_t getPixel(const ImageRGBA32& img, int x, int y, int c)
{
    if (x < 0 || x >= img.w || y < 0 || y >= img.h) return 0;
    return img.pixels[4 * (y * img.w + x) + c];
}

ImageRGBA32 resizeLanczos(const ImageRGBA32& src, int newWidth, int newHeight, int a = 3)
{
    ImageRGBA32 dst;
    dst.w = newWidth;
    dst.h = newHeight;
    dst.pixels.resize(newWidth * newHeight * 4);

    double scaleX = static_cast<double>(src.w) / newWidth;
    double scaleY = static_cast<double>(src.h) / newHeight;

    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            for (int c = 0; c < 4; ++c) { // Für RGBA-Kanäle
                double srcX = (x + 0.5) * scaleX - 0.5;
                double srcY = (y + 0.5) * scaleY - 0.5;
                int srcXInt = static_cast<int>(srcX);
                int srcYInt = static_cast<int>(srcY);

                double pixelValue = 0.0;
                double weightSum = 0.0;

                for (int dy = -a + 1; dy < a; ++dy) {
                    for (int dx = -a + 1; dx < a; ++dx) {
                        int sampleX = srcXInt + dx;
                        int sampleY = srcYInt + dy;
                        double weight = lanczosKernel(srcX - sampleX, a) * lanczosKernel(srcY - sampleY, a);

                        pixelValue += getPixel(src, sampleX, sampleY, c) * weight;
                        weightSum += weight;
                    }
                }

                dst.pixels[4 * (y * newWidth + x) + c] = std::clamp(static_cast<int>(pixelValue / weightSum + 0.5), 0, 255);
            }
        }
    }

    return dst;
}

int main() {
    // Beispielcode
    ImageRGBA32 image = { 2, 2, {255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 255, 255} };
    int newWidth = 4;
    int newHeight = 4;

    ImageRGBA32 resizedImage = resizeLanczos(image, newWidth, newHeight);

    std::cout << "Größe nach Skalierung: " << resizedImage.w << "x" << resizedImage.h << std::endl;
    return 0;
}
*/

// ===========================================================================
struct ImagePainter
// ===========================================================================
{
   DE_CREATE_LOGGER("de.ImagePainter")

#ifdef DE_USE_TRUETYPE_FONTS

   static TextSize
   getTextSize( std::wstring const & msg, Font font = Font() );

   static void
   drawText( Image & img, int32_t x, int32_t y, std::wstring const & msg, uint32_t color,
             Font font = Font(), Align align = Align::Default, bool blend = false );

#endif

   static void
   brighten( Image & img, float brightness );

   static void
   computeSaturation( Image const & img, uint32_t & color_min, uint32_t & color_max );

   static void
   autoSaturate( Image & img );

   static void
   replaceColor( Image & img,
      uint32_t search_color = 0xFFFFFFFF,
      uint32_t replace_color = 0x00000000 );

   static void
   makeTransparent( Image & img, uint32_t search_color = 0xFFFFFFFF )
   {
      replaceColor( img, search_color );
   }

   static void
   fill( Image & img, uint32_t color );
   static uint32_t
   getPixel( Image & img, int32_t x, int32_t y, uint32_t colorKey = 0x00000000 );
   static void
   setPixel( Image & img, int32_t x, int32_t y, uint32_t color, bool blend = false );


   static void
   drawImage( Image & img, Image const & src, int32_t x, int32_t y, bool blend = false );

   static void
   drawLineH( Image & img, int32_t x1, int32_t x2, int32_t y, uint32_t color, bool blend = false );

   static void
   drawLine( Image & img, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color, bool blend = false );
   inline static void
   drawLine( Image & img, glm::ivec2 const & a, glm::ivec2 const & b, uint32_t color, bool blend = false )
   {
      drawLine( img, a.x, a.y, b.x, b.y, color, blend );
   }
   inline static void
   drawLine( Image & img, Recti const & r, uint32_t color, bool blend = false )
   {
      drawLine( img, r.x1(), r.y1(), r.x2(), r.y2(), color, blend );
   }


   // Recti stores higher precision and makes more beautiful results.
   static void
   drawLineGrid( Image & img, Recti const & pos, uint32_t color, bool blend = false );
   static void
   drawLineGridX( Image & img, Recti const & pos, uint32_t color, bool blend = false );
   static void
   drawLineGridY( Image & img, Recti const & pos, uint32_t color, bool blend = false );
   static void
   drawLineMark( Image & img, int32_t x, int32_t y, uint32_t color, bool blend = false );
   static void
   drawDot( Image & img, int32_t x, int32_t y, int32_t w, uint32_t color, bool blend = false );

// ==== Rect ===

   // Quad ABCD:  --> u
   //             |
   // B---C       v
   // |\  |
   // | \ |
   // |  \|    (ccw) ACB + ADC
   // A---D    (cw) ABC + ACD
   //
   static void
   drawRect( Image & img, Recti const & pos,
            uint32_t colorA, uint32_t colorB,
            uint32_t colorC, uint32_t colorD, bool blend = false );

   static void
   drawRect( Image & img, Recti const & pos, uint32_t color, bool blend = false );

   static void
   drawRect( Image & img, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color, bool blend = false )
   {
      drawRect( img, Recti::fromPoints( x1, y1, x2, y2 ), color, blend );
   }
   static void
   drawRect( Image & img, glm::ivec2 const & a, glm::ivec2 const & b, uint32_t color, bool blend = false )
   {
      drawRect( img, Recti::fromPoints( a.x, a.y, b.x, b.y), color, blend );
   }


// ==== RectBorder ===
   static void
   drawRectBorder( Image & img, Recti const & pos, uint32_t color, bool blend = false );

//   static void
//   drawRectBorder( Image & img, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color, bool blend = false )
//   {
//      drawRectBorder( img, Recti::fromPoints( x1, y1, x2, y2 ), color, blend );
//   }
//   inline static void
//   drawRectBorder( Image & img, glm::ivec2 const & a, glm::ivec2 const & b, uint32_t color, bool blend = false )
//   {
//      drawRectBorder( img, a.x, a.y, b.x, b.y, color, blend );
//   }

   static void
   drawTriangleBorder( Image & img, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                     int32_t x3, int32_t y3, uint32_t color, bool blend = false );
   static void
   drawTriangle( Image & img,
      glm::ivec2 const & A,
      glm::ivec2 const & B,
      glm::ivec2 const & C, uint32_t color, bool blend = false );

/*
   static void
   floodFill(  int32_t x,
               int32_t y,
               uint32_t newColor,
               uint32_t oldColor );
*/

// Circle/Ellipse + RoundRect

   static void
   drawCircle( Image & img, Recti const & pos, uint32_t color, bool blend = false );
   static void
   drawCircleBorder( Image & img, Recti const & pos, uint32_t color, bool blend = false );

   static void
   drawRoundRect( Image & img, Recti const & pos, int rx, int ry, uint32_t color, bool blend = false );
   static void
   drawRoundRectBorder( Image & img, Recti const & pos, int rx, int ry, Pen const & pen, bool blend = false );

// RoundRect helpers

   static void
   drawCircleA( Image & img, Recti const & pos, uint32_t color, bool blend = false );
   static void
   drawCircleB( Image & img, Recti const & pos, uint32_t color, bool blend = false );
   static void
   drawCircleC( Image & img, Recti const & pos, uint32_t color, bool blend = false );
   static void
   drawCircleD( Image & img, Recti const & pos, uint32_t color, bool blend = false );

   static void
   drawCircleBorderA( Image & img, Recti const & pos, Pen const & pen, bool blend = false );
   static void
   drawCircleBorderB( Image & img, Recti const & pos, Pen const & pen, bool blend = false );
   static void
   drawCircleBorderC( Image & img, Recti const & pos, Pen const & pen, bool blend = false );
   static void
   drawCircleBorderD( Image & img, Recti const & pos, Pen const & pen, bool blend = false );

};

// ===========================================================================
struct ImageCutter
// ===========================================================================
{
   DE_CREATE_LOGGER("de.ImageCutter")
public:

   static Image
   copyRegionRect( Image const & src, Recti region ); // , uint32_t clearColor = 0x00000000

   static Image
   copyRegionHexagon( Image const & src, Recti region, uint32_t clearColor = 0x00000000 );

   static Image
   copyRegionEllipse( Image const & src, Recti region, uint32_t clearColor = 0x00000000 );
};

// ===========================================================================
struct ImageRepeat
// ===========================================================================
{
   DE_CREATE_LOGGER("de.ImageRepeat")
public:

   static Image
   repeatImage( Image const & src, int w, int h, bool preserveAspect );

//! returns a (u,v) repeated image (tiles will be scaled bilinear if necessary)
//IImage*
//repeat( IImage* src, u32 w, u32 h, f32 uRepeat, f32 vRepeat, s32 scaleOP, bool bPreserveAspect, bool bSharpen) const
//{
//	return 0;
//}

};

// ===========================================================================
struct ImageRotate
// ===========================================================================
{
   DE_CREATE_LOGGER("de.ImageRotate")
public:

   static bool
   flipH( Image & img );

   static bool
   flipV( Image & img );

   static bool
   rotate180( Image & img );

   static Image
   rotateLeft( Image const & img );

   static Image
   rotateRight( Image const & img );

   static Image
   rotate( Image const & img, float angleDegreesCCW, uint32_t colorKey );

   /// Shifts an image by 'n' pixels vertical-lines left (can be negative)
   static void
   shiftLeft( Image & img, int32_t n_shifts = 1 );

   /// Shifts an image by 'n' pixels vertical-lines right (can be negative)
   static void
   shiftRight( Image & img, int32_t n_shifts = 1 );

   /// Shifts an image by 'n' pixels horizontal-lines up (can be negative)
   static void
   shiftUp( Image & img, int32_t n_shifts = 1 );

   /// Shifts an image by 'n' pixels horizontal-lines down (can be negative)
   static void
   shiftDown( Image & img, int32_t n_shifts = 1 );

   static Image
   skewH( Image const & src, float angle );

   static Image
   skewV( Image const & src, float angle );
};

// ===========================================================================
struct ImageScaler
// ===========================================================================
{
   // int scaleOp = 0 - nearest
   // int scaleOp = 1 - bilinear
   // int scaleOp = 2 - bicubique
   static Image
   resize( Image const & src, int w, int h, int scaleOp = 1, uint32_t colorKey = 0x00000000 );

   // The word 'resize()' is more precise regarding what he function does
   // - a fixed size as new size is given, not a multiple of the old size (scaling)
   static Image
   resizeNearest( Image const & src, int w, int h, uint32_t colorKey = 0x00000000 );
   //                          int color_planes, bool bPreserveAspect, bool bSharpen)
   static Image
   resizeBilinear( Image const & src, int w, int h, uint32_t colorKey = 0x00000000 );
   //                           E_COLOR_PLANE planes, bool bPreserveAspect, bool bSharpen) const
   static Image
   resizeBicubic( Image const & src, int w, int h, uint32_t colorKey = 0x00000000 );
   //

   static uint32_t
   getNearestPixel( Image const & src, float fx, float fy, uint32_t colorKey = 0x00000000 );

   //static uint32_t
   //getBilinearPixelFast( IImage* src, float fx, float fy, E_COLOR_PLANE planes, uint32_t* colorKey);

   static uint32_t
   getBilinearPixel( Image const & src, float fx, float fy, uint32_t colorKey = 0x00000000 );

   static float
   getBicubicFactor( float const arg );

   static uint32_t
   getBicubicPixel( Image const & src, float fx, float fy, uint32_t* colorKey = nullptr );

   static Image
   scaleImage( Image const & src, int w, int h, bool preserveAspect );

   static bool
   resizeBicubic( Image & dst, Image const & src, uint32_t colorKey = 0x00000000 );
};

// ===========================================================================
struct ImageResizef
// ===========================================================================
{
    typedef glm::vec4 V4;

    static V4
    getNearestPixelf( const Image & src, float fx, float fy, const V4& colorKey = V4() );
    static V4
    getBilinearPixelf( const Image & src, float fx, float fy, const V4& colorKey = V4() );

    // int scaleOp = 0 - nearest
    // int scaleOp = 1 - bilinear
    // int scaleOp = 2 - bicubic
    static Image
    resizef( const Image & src, int w, int h, int scaleOp = 1, const V4& colorKey = V4() );

    static Image
    resizeNearestf( const Image & src, int w, int h, const V4& colorKey = V4() );
    static Image
    resizeBilinearf( const Image & src, int w, int h, const V4& colorKey = V4() );
};

// ===========================================================================
enum E_IMAGE_FILTER
// ===========================================================================
{
   EIF_SHARPEN=0,          // factory->filter()
   EIF_SMOOTHEN,           // factory->filter()
   EIF_BLUR,               // factory->filter()
   EIF_MEAN_REMOVAL,       // factory->filter()
   EIF_EMBOSS,             // factory->filter()
   EIF_EMBOSS_ALL,         // factory->filter()
   EIF_EMBOSS_LOSSY,       // factory->filter()
   EIF_EMBOSS_H,           // factory->filter()
   EIF_EMBOSS_V,           // factory->filter()
   EIF_EDGE_DETECT,        // factory->filter()
   EIF_SEPIA,              // factory->filter(), factory->sepia()
   EIF_INVERT,             // factory->filter(), factory->invert()
   EIF_BLACKWHITE,         // factory->filter(), factory->blackwhite()
   EIF_GREYSCALES,         // factory->filter(), factory->average()
   EIF_AVERAGE,            // factory->filter(), factory->average()
   EIF_HDR,                // factory->filter(), factory->hdr()
   EIF_SATURATE,     // factory->filter(), factory->correctColors()
   EIF_COUNT
};

// ===========================================================================
const char* const E_IMAGE_FILTER_STRINGS[] =
// ===========================================================================
{
   "EIF_SHARPEN",            // factory->filter()
   "EIF_SMOOTHEN",           // factory->filter()
   "EIF_BLUR",               // factory->filter()
   "EIF_MEAN_REMOVAL",       // factory->filter()
   "EIF_EMBOSS",             // factory->filter()
   "EIF_EMBOSS_ALL",         // factory->filter()
   "EIF_EMBOSS_LOSSY",       // factory->filter()
   "EIF_EMBOSS_H",           // factory->filter()
   "EIF_EMBOSS_V",           // factory->filter()
   "EIF_EDGE_DETECT",        // factory->filter()
   "EIF_SEPIA",              // factory->filter(), factory->sepia()
   "EIF_INVERT",             // factory->filter(), factory->invert()
   "EIF_BLACKWHITE",         // factory->filter(), factory->blackwhite()
   "EIF_GREYSCALES",         // factory->filter(), factory->average()
   "EIF_AVERAGE",            // factory->filter(), factory->average()
   "EIF_HDR",                // factory->filter(), factory->hdr()
   "EIF_SATURATE",     // factory->filter(), factory->correctColors()
   "EIF_COUNT"
};

// ===========================================================================
template < typename T > struct ImageFilter // : public virtual IReferenceCounted
// ===========================================================================
{
   // (matrix / factor) + offset
   T pixels[3][3]; // pixels weight matrix3x3
   T factor;		  // factor
   T offset;		  // offset

   T get(int32_t index)
   {
      index = index%9;
      int32_t col = index%3;
      int32_t row = (index-col)/3;
      return (pixels[row][col]/factor) + offset;
   }

   T get(int32_t row, int32_t col)
   {
      if (row<0) row=0;
      if (row>2) row=2;
      if (col<0) col=0;
      if (col>2) col=2;
      return (T)(((float)pixels[row][col]/(float)factor) + (float)offset);
   }

   explicit
   ImageFilter( T e00=T(0), T e01=T(0), T e02=T(0),
                 T e10=T(0), T e11=T(1), T e12=T(0),
                 T e20=T(0), T e21=T(0), T e22=T(0),
                 T factor=T(1),
                 T offset=T(0)) // default constructor
   {
      this->pixels[0][0] = e00;
      this->pixels[0][1] = e01;
      this->pixels[0][2] = e02;
      this->pixels[1][0] = e10;
      this->pixels[1][1] = e11;
      this->pixels[1][2] = e12;
      this->pixels[2][0] = e20;
      this->pixels[2][1] = e21;
      this->pixels[2][2] = e22;
      if (factor == T(0)) factor = T(1);
      this->factor = factor;
      this->offset = offset;
   }
};

template < typename T > struct ImageFilterSharpen : public ImageFilter<T>{public:ImageFilterSharpen():ImageFilter<T>(0,-2,0,-2,11,-2,0,-2,0,3,0){}};
template < typename T > struct ImageFilterSmoothing : public ImageFilter<T>{public:ImageFilterSmoothing():ImageFilter<T>(1,1,1,1,1,1,1,1,1,9,0){}};
template < typename T > struct ImageFilterGaussianBlur : public ImageFilter<T>{public:ImageFilterGaussianBlur():ImageFilter<T>(1,2,1,2,4,2,1,2,1,16,0){}};
template < typename T > struct ImageFilterMeanRemoval : public ImageFilter<T>{public:ImageFilterMeanRemoval():ImageFilter<T>(-1,-1,-1,-1,9,-1,-1,-1,-1,1,0){}};
template < typename T > struct ImageFilterEmboss : public ImageFilter<T>{public:ImageFilterEmboss():ImageFilter<T>(-1,0,-1,0,4,0,-1,0,-1,1,0){}};
template < typename T > struct ImageFilterEmbossAll : public ImageFilter<T>{public:ImageFilterEmbossAll():ImageFilter<T>(-1,-1,-1,-1,8,-1,-1,-1,-1,1,0){}};
template < typename T > struct ImageFilterEmbossLossy : public ImageFilter<T>{public:ImageFilterEmbossLossy():ImageFilter<T>(1,-2,1,-2,4,-2,-2,1,-2,1,127){}};
template < typename T > struct ImageFilterEmbossH : public ImageFilter<T>{public:ImageFilterEmbossH():ImageFilter<T>(0,0,0,-1,2,-1,0,0,0,1,127){}};
template < typename T > struct ImageFilterEmbossV : public ImageFilter<T>{public:ImageFilterEmbossV():ImageFilter<T>(0,-1,0,0,0,0,0,1,0,1,127){}};
template < typename T > struct ImageFilterEdgeDetect : public ImageFilter<T>{public:ImageFilterEdgeDetect():ImageFilter<T>(1,1,1,0,0,0,-1,-1,-1,1,0){}};

typedef ImageFilterSharpen< float > ImageFilterSharpenf;
typedef ImageFilterSmoothing< float > ImageFilterSmoothingf;
typedef ImageFilterGaussianBlur< float > ImageFilterGaussianBlurf;
typedef ImageFilterMeanRemoval< float > ImageFilterMeanRemovalf;
typedef ImageFilterEmboss< float > ImageFilterEmbossf;
typedef ImageFilterEmbossAll< float > ImageFilterEmbossAllf;
typedef ImageFilterEmbossLossy< float > ImageFilterEmbossLossyf;
typedef ImageFilterEmbossH< float > ImageFilterEmbossHf;
typedef ImageFilterEmbossV< float > ImageFilterEmbossVf;
typedef ImageFilterEdgeDetect< float > ImageFilterEdgeDetectf;

} // end namespace de.
