#pragma once
#include <DarkGDK/DarkCore.h>
#include <vector>

namespace DarkGDK
{    
    // ===========================================================================
    // ===   ImageLoadOptions
    // ===========================================================================
    struct ImageLoadOptions
    {
       uint32_t m_SearchColor;
       uint32_t m_ReplaceColor;

       bool m_ConvertToGrey121;
       bool m_ConvertToGrey111;
       bool m_AutoSaturate;

       float m_Brighten;
       float m_Gamma;
       float m_Contrast;

       std::string m_DefaultExportExt;

       ImageLoadOptions()
          : m_SearchColor( 0 )
          , m_ReplaceColor( 0 )
          , m_ConvertToGrey121( false )
          , m_ConvertToGrey111( false )
          , m_AutoSaturate( false )
          , m_Brighten( 1.0f )
          , m_Gamma( 1.0f )
          , m_Contrast( 1.0f )
          , m_DefaultExportExt( "raw" )
       {}
    };


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

} // end namespace DarkGDK.
