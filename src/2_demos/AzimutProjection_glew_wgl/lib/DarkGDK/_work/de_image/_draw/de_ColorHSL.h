#pragma once
#include <de_image/de_ColorRGBA.h>
#include <glm/gtx/color_space.hpp>

namespace de {

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
