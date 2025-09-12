#pragma once
#include <de_image/de_ColorRGBA.h>
#include <glm/gtx/color_space.hpp>

namespace de {

// Representing a hue, saturation, luminance color
// stored in 32bit floating point variables.
// Hue is in range [0,360],
// Luminance and Saturation are in percent [0,100]

struct ColorHSL
{
   float Hue;
   float Saturation;
   float Luminance;

   ColorHSL ( float h = 0.f, float s = 0.f, float l = 0.f )
      : Hue( h )
      , Saturation( s )
      , Luminance( l )
   {
      while (Hue < 0.0f) { Hue += 360.0f; }
      while (Hue > 360.0f) { Hue -= 360.0f; }
   }

   // algorithm from Foley/Van-Dam
   inline float toRGB1(float rm1, float rm2, float rh) const
   {
      if (rh<0) rh += 1;
      if (rh>1) rh -= 1;

      if (rh < 1.f/6.f)      rm1 = rm1 + (rm2 - rm1) * rh*6.f;
      else if (rh < 0.5f)    rm1 = rm2;
      else if (rh < 2.f/3.f) rm1 = rm1 + (rm2 - rm1) * ((2.f/3.f)-rh)*6.f;

      return rm1;
   }

   uint32_t toRGB( uint8_t alpha = 255 ) const
   {
      float const l = Luminance * 0.01f;
      if ( Saturation <= 0.000001f ) // is_zero = grey
      {
         uint8_t r = uint8_t( std::clamp( int(255.0f * l), 0, 255 ) );
         return RGBA( r, r, r, alpha );
      }

      float rm2;
      if ( Luminance <= 50.0f )
      {
         rm2 = l + l * (Saturation*0.01f);
      }
      else
      {
         rm2 = l + (1 - l) * (Saturation*0.01f);
      }

      float const rm1 = 2.0f * l - rm2;
      float const h = Hue / 360.0f;
      uint8_t r = uint8_t( std::clamp( int(255.f * toRGB1(rm1, rm2, h + 1.f/3.f)), 0, 255 ) );
      uint8_t g = uint8_t( std::clamp( int(255.f * toRGB1(rm1, rm2, h)), 0, 255 ) );
      uint8_t b = uint8_t( std::clamp( int(255.f * toRGB1(rm1, rm2, h - 1.f/3.f)), 0, 255 ) );

      return RGBA(r,g,b,alpha);
   }

   /*
   void fromRGB( float r, float g, float b )
   {
      const float maxVal = core::max_(color.getRed(), color.getGreen(), color.getBlue());
      const float minVal = (float)core::min_(color.getRed(), color.getGreen(), color.getBlue());
      Luminance = (maxVal+minVal)*50;
      if (core::equals(maxVal, minVal))
      {
         Hue=0.f;
         Saturation=0.f;
         return;
      }

      const float delta = maxVal-minVal;
      if ( Luminance <= 50 )
      {
         Saturation = (delta)/(maxVal+minVal);
      }
      else
      {
         Saturation = (delta)/(2-maxVal-minVal);
      }
      Saturation *= 100;

      if (core::equals(maxVal, color.getRed()))
         Hue = (color.getGreen()-color.getBlue())/delta;
      else if (core::equals(maxVal, color.getGreen()))
         Hue = 2+((color.getBlue()-color.getRed())/delta);
      else // blue is max
         Hue = 4+((color.getRed()-color.getGreen())/delta);

      Hue *= 60.0f;
      while ( Hue < 0.f )
         Hue += 360;
   }
   */

};

} // end namespace de

/*

wxImage::HSVValue wxImage::RGBtoHSV(const RGBValue& rgb)
{
    const double red = rgb.red / 255.0,
                 green = rgb.green / 255.0,
                 blue = rgb.blue / 255.0;

    // find the min and max intensity (and remember which one was it for the
    // latter)
    double minimumRGB = red;
    if ( green < minimumRGB )
        minimumRGB = green;
    if ( blue < minimumRGB )
        minimumRGB = blue;

    enum { RED, GREEN, BLUE } chMax = RED;
    double maximumRGB = red;
    if ( green > maximumRGB )
    {
        chMax = GREEN;
        maximumRGB = green;
    }
    if ( blue > maximumRGB )
    {
        chMax = BLUE;
        maximumRGB = blue;
    }

    const double value = maximumRGB;

    double hue = 0.0, saturation;
    const double deltaRGB = maximumRGB - minimumRGB;
    if ( wxIsNullDouble(deltaRGB) )
    {
        // Gray has no color
        hue = 0.0;
        saturation = 0.0;
    }
    else
    {
        switch ( chMax )
        {
            case RED:
                hue = (green - blue) / deltaRGB;
                break;

            case GREEN:
                hue = 2.0 + (blue - red) / deltaRGB;
                break;

            case BLUE:
                hue = 4.0 + (red - green) / deltaRGB;
                break;
        }

        hue /= 6.0;

        if ( hue < 0.0 )
            hue += 1.0;

        saturation = deltaRGB / maximumRGB;
    }

    return HSVValue(hue, saturation, value);
}

wxImage::RGBValue wxImage::HSVtoRGB(const HSVValue& hsv)
{
    double red, green, blue;

    if ( wxIsNullDouble(hsv.saturation) )
    {
        // Grey
        red = hsv.value;
        green = hsv.value;
        blue = hsv.value;
    }
    else // not grey
    {
        double hue = hsv.hue * 6.0;      // sector 0 to 5
        int i = (int)floor(hue);
        double f = hue - i;          // fractional part of h
        double p = hsv.value * (1.0 - hsv.saturation);

        switch (i)
        {
            case 0:
                red = hsv.value;
                green = hsv.value * (1.0 - hsv.saturation * (1.0 - f));
                blue = p;
                break;

            case 1:
                red = hsv.value * (1.0 - hsv.saturation * f);
                green = hsv.value;
                blue = p;
                break;

            case 2:
                red = p;
                green = hsv.value;
                blue = hsv.value * (1.0 - hsv.saturation * (1.0 - f));
                break;

            case 3:
                red = p;
                green = hsv.value * (1.0 - hsv.saturation * f);
                blue = hsv.value;
                break;

            case 4:
                red = hsv.value * (1.0 - hsv.saturation * (1.0 - f));
                green = p;
                blue = hsv.value;
                break;

            default:    // case 5:
                red = hsv.value;
                green = p;
                blue = hsv.value * (1.0 - hsv.saturation * f);
                break;
        }
    }

    return RGBValue((unsigned char)(red * 255.0),
                    (unsigned char)(green * 255.0),
                    (unsigned char)(blue * 255.0));
}

*/
