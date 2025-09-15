// Copyright (C) 2002-2014 Benjamin Hampe

#pragma once
#include <de/Color.h>

namespace de {

// ============================================================================
/// @class LinearColorGradient
// ============================================================================
struct LinearColorGradient
{
    static glm::vec4
    toRGBAf( uint32_t const color )
    {
        float r = float(dbRGBA_R( color )) / 255.0f;
        float g = float(dbRGBA_G( color )) / 255.0f;
        float b = float(dbRGBA_B( color )) / 255.0f;
        float a = float(dbRGBA_A( color )) / 255.0f;
        return glm::vec4(r,g,b,a);
    }

    static uint32_t
    toRGBA( glm::vec4 const& color )
    {
        uint8_t r = static_cast<uint8_t>(std::clamp(color.r * 255.0f, 0.0f, 255.0f));
        uint8_t g = static_cast<uint8_t>(std::clamp(color.g * 255.0f, 0.0f, 255.0f));
        uint8_t b = static_cast<uint8_t>(std::clamp(color.b * 255.0f, 0.0f, 255.0f));
        uint8_t a = static_cast<uint8_t>(std::clamp(color.a * 255.0f, 0.0f, 255.0f));
        return dbRGBA(r,g,b,a);
    }

   static glm::vec4
   lerpColor128( uint32_t A, uint32_t B, float t );

   // Empty
   LinearColorGradient();

   LinearColorGradient( uint32_t color );

   // Two colors ( start + end )
   LinearColorGradient( uint32_t startColor, uint32_t endColor );

   // create LookUpTable with 32 bit colors
   std::vector< uint32_t >
   createLookUpTable32( int n ) const;

   // create LookUpTable with 128 bit colors
   std::vector< glm::vec4 >
   createLookUpTable128( int n ) const;

   void
   clearStops();


   // create Image with 32 bit colors
//   Image
//   createImage( int n, bool bVertical ) const;

   // create Image with 32 bit colors


   bool
   hasTransparentColor() const;

   glm::vec4
   getColor128( float t ) const;

   uint32_t
   getColor32( float t ) const;

   LinearColorGradient&
   addStop( float t, uint32_t color = 0xFFFFFFFF );

//   LinearColorGradient&
//   setColor( int index, uint32_t color, float t );

   std::string
   toString() const;


#if 0
   Image
   createImage( int w, int h, bool bVertical = false ) const;

   Image
   toImage( int32_t w, int32_t h, bool vertical ) const;

#endif


   // static void
   // test();
public:

   struct ColorStop
   {
      ColorStop( float stop_param, uint32_t stop_color = 0xFFFFFFFF )
         : stop( stop_param )
         , color( stop_color )
      {}

      float stop;
      uint32_t color;
   };

   float m_sum;
   float m_inv_max;
   std::vector< ColorStop > m_Stops;
};

} // end namespace de.
