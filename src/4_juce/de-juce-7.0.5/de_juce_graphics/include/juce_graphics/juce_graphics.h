/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.md file.


 BEGIN_SAFT_MODULE_DECLARATION

  ID:                 juce_graphics
  vendor:             juce
  version:            7.0.5
  name:               JUCE graphics classes
  description:        Classes for 2D vector graphics, image loading/saving, font handling, etc.
  website:            http://www.juce.com/juce
  license:            GPL/Commercial
  minimumCppStandard: 17

  dependencies:       juce_events
  OSXFrameworks:      Cocoa QuartzCore
  iOSFrameworks:      CoreGraphics CoreImage CoreText QuartzCore
  linuxPackages:      freetype2

 END_SAFT_MODULE_DECLARATION

*******************************************************************************/

#pragma once
#define JUCE_GRAPHICS_H_INCLUDED
#define JUCE_MODULE_AVAILABLE_juce_graphics 1

#include <juce_graphics/juce_graphics_config.h>
#include <juce_graphics/geometry/juce_AffineTransform.h>
#include <juce_graphics/geometry/juce_Point.h>
#include <juce_graphics/geometry/juce_Line.h>
#include <juce_graphics/geometry/juce_Rectangle.h>
#include <juce_graphics/geometry/juce_Parallelogram.h>
#include <juce_graphics/placement/juce_Justification.h>
#include <juce_graphics/geometry/juce_Path.h>
#include <juce_graphics/geometry/juce_RectangleList.h>
#include <juce_graphics/colour/juce_PixelFormats.h>
#include <juce_graphics/colour/juce_Colour.h>
#include <juce_graphics/colour/juce_ColourGradient.h>
#include <juce_graphics/colour/juce_Colours.h>
#include <juce_graphics/geometry/juce_BorderSize.h>
#include <juce_graphics/geometry/juce_EdgeTable.h>
#include <juce_graphics/geometry/juce_PathIterator.h>
#include <juce_graphics/geometry/juce_PathStrokeType.h>
#include <juce_graphics/placement/juce_RectanglePlacement.h>
#include <juce_graphics/images/juce_ImageCache.h>
#include <juce_graphics/images/juce_ImageConvolutionKernel.h>
#include <juce_graphics/images/juce_ImageFileFormat.h>
#include <juce_graphics/fonts/juce_Typeface.h>
#include <juce_graphics/fonts/juce_Font.h>
#include <juce_graphics/fonts/juce_AttributedString.h>
#include <juce_graphics/fonts/juce_GlyphArrangement.h>
#include <juce_graphics/fonts/juce_TextLayout.h>
#include <juce_graphics/fonts/juce_CustomTypeface.h>
#include <juce_graphics/contexts/juce_GraphicsContext.h>
#include <juce_graphics/contexts/juce_LowLevelGraphicsContext.h>
#include <juce_graphics/images/juce_Image.h>
#include <juce_graphics/images/juce_ScaledImage.h>
#include <juce_graphics/colour/juce_FillType.h>
#include <juce_graphics/native/juce_RenderingHelpers.h>
#include <juce_graphics/contexts/juce_LowLevelGraphicsSoftwareRenderer.h>
#include <juce_graphics/contexts/juce_LowLevelGraphicsPostScriptRenderer.h>
#include <juce_graphics/effects/juce_ImageEffectFilter.h>
#include <juce_graphics/effects/juce_DropShadowEffect.h>
#include <juce_graphics/effects/juce_GlowEffect.h>