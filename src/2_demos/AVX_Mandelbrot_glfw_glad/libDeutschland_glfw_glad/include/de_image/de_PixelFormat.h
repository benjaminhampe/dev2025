/// @author Benjamin Hampe <benjaminhampe@gmx.de>
#pragma once
#include <cstdint>

namespace de {

/// @brief A color format for images and textures mostly.
// ===================================================================
enum class PixelFormat
// ===================================================================
{
   R8G8B8A8 = 0,  // 32-bit color format, default for all graphics
   R8G8B8,        // 24-bit color format, for jpeg and bmp
   R5G6B5,        // 16-bit color format
   R5G5B5A1,      // 16-bit color format with 1-bit transparency (opaque=1)
   R8,
   R16,  // 16-bit short heightmap DTM geoTIFF (*.tif) for terrain meshes.
   R24,
   R32,
   R32F, // heightmap DTM geoTIFF (*.tif) for 3D engine terrain meshes.
   A1R5G5B5,
   B8G8R8A8,
   B8G8R8,
   D24S8,      // Depth-Stencil format
   D32F,       // Depth-Stencil format
   D32FS8,     // Depth-Stencil format
   Unknown
};

} // end namespace de.
