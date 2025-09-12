#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <de_image/de_Image.h>
//#include <glm/glm.hpp>

namespace de {

template< typename A, typename B, typename C >
inline int
de_clampi( A a, B b, C c ) { return std::clamp( int(a), int(b), int(c) ); }

template< typename A, typename B, typename C >
inline float
de_fmaf( A a, B b, C c ) { return (float(a) * float(b)) + float(c); }

Image
genHeightMap( Image const & src );

Image
genNormalMap( Image const & heightMap );

void
convertToGrayscale( Image & img );

void
computeMinMaxColor( Image const & img, uint32_t & minColor, uint32_t & maxColor );

// If your normalMap is green and red then its 2d and lacks 3d z blue.
// Adds z=255 as (B)lue component to (RG) normalMap xy,
// normals are not normalized in length.
// Correction could simply be done in shader. This is for visual debug the world.
void
saturateImage( Image & img );

void
saturateRedChannel( Image & img );

void
saturateAlphaChannel( Image & img );

//
void
scaleAlphaChannel( Image & img, float scale );

//
void
removeAlphaChannel( Image & img );

// If your normalMap is green and red then its 2d and lacks 3d z blue.
// Adds z=255 as (B)lue component to (RG) normalMap xy,
// normals are not normalized in length.
// Correction could simply be done in shader. This is for visual debug the world.
void
correct2DNormals( Image & img, float amplitude = 1.0f );

// Expects one image with RGB encoded normals and second image with R(GB) heights.
// This is what the modified parallax shader expects
Image
fuseImages_NormalMap_plus_BumpMap(
      Image const & normalMap, Image const & heightMap );

} // end namespace de.
