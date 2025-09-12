// Copyright (C) 2002-2014 Benjamin Hampe

#ifndef DE_IMAGE_LINEARCOLORGRADIENT_HPP
#define DE_IMAGE_LINEARCOLORGRADIENT_HPP

#include <de/Image.hpp>

namespace de {

// ============================================================================
/// @class LinearColorGradient
// ============================================================================
struct LinearColorGradient
{
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
   Image
   createImage( int w, int h, bool bVertical = false ) const;

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

   Image
   toImage( int32_t w, int32_t h, bool vertical ) const;


   // static void
   // test();
public:
   DE_CREATE_LOGGER("de.image.LinearColorGradient")

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

#endif // ICOLORGRADIENT_H
