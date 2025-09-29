#pragma once
//#include <juce_graphics/contexts/juce_LowLevelGraphicsContext.h>
/*
#include <juce_graphics/colour/juce_ColourGradient.h>
//#include <juce_graphics/colour/juce_FillType.h>
//#include <juce_graphics/images/juce_Image.h>
#include <juce_graphics/fonts/juce_Font.h>
#include <juce_graphics/placement/juce_Justification.h>
#include <juce_graphics/placement/juce_RectanglePlacement.h>
#include <juce_graphics/geometry/juce_Point.h>
#include <juce_graphics/geometry/juce_RectangleList.h>
#include <juce_graphics/geometry/juce_AffineTransform.h>
//#include <juce_graphics/geometry/juce_Path.h>
//#include <juce_graphics/geometry/juce_PathStrokeType.h>
#include <juce_graphics/geometry/juce_Line.h>
*/

namespace juce
{
/*
   struct Image;
   struct FillType;
   struct LowLevelGraphicsContext;
   struct Path;
   struct PathStrokeType;
   //struct RectanglePlacement;
*/   

    //==============================================================================
    /** Types of rendering quality that can be specified when drawing images.

        @see Graphics::setImageResamplingQuality
    */
    enum GraphicsResamplingQuality
    {
        Graphics_lowResamplingQuality     = 0,    /**< Just uses a nearest-neighbour algorithm for resampling. */
        Graphics_mediumResamplingQuality  = 1,    /**< Uses bilinear interpolation for upsampling and area-averaging for downsampling. */
        Graphics_highResamplingQuality    = 2,    /**< Uses bicubic interpolation for upsampling and area-averaging for downsampling. */
    };

} // end namespace juce