#pragma once
#include <DarkGDK/DarkImage_types.h>

namespace DarkGDK
{    
    bool
    dbExistImage( u32 imageId );
    
    bool
    dbLoadImage( u32 imageId, tString const & uri, 
        ImageLoadOptions const & options = ImageLoadOptions() );

    bool
    dbLoadImage( u32 imageId, tString const & uri,
        u8 const* fileData, u64 fileSize,
        ImageLoadOptions const & options = ImageLoadOptions() );

    bool
    dbImageFromXPM( u32 imageId,
        std::vector< std::string > const & xpm_data );

    bool
    dbSaveImage( u32 imageId, tString const & uri, u32 quality = 0 );

    bool
    dbDeleteImage( u32 imageId );

    u32 
    dbImageWidth( u32 imageId );
    
    u32 
    dbImageHeight( u32 imageId );
    
    u32 
    dbImageFormat( u32 imageId );

    void*
    dbImageRowPtr( u32 imageId, s32 y );
    
    u32 
    dbImageRowByteSize( u32 imageId );    

    u32 
    dbImagePixelBytes( u32 imageId );

    void* 
    dbImagePixels( u32 imageId );
    
    void*
    dbImagePixels( u32 imageId, s32 x, s32 y );
    
    u32 
    dbGetImagePixel( u32 imageId, s32 x, s32 y );
    
    void 
    dbSetImagePixel( u32 imageId, s32 x, s32 y, u32 color );

    /*
    bool
    dbConvertFloatHeightmapToRGBA( de::Image const & src, de::Image & dst );

    //de::Image
    //dbResizeImage(  de::Image const & img,
    //                int w,
    //                int h,
    //                int scaleOp = 1,
    //                uint32_t keyColor = 0x00000000 );

    //bool
    //dbConvertImageFile( std::string loadName, std::string saveName );

    //void
    //dbDrawText5x8(  de::Image & img,
    //                int x,
    //                int y,
    //                std::string const & msg,
    //                uint32_t color = 0xFF000000,
    //                de::Align align = de::Align::Default );


    void
    dbDrawText5x8(    de::Image & img, int x, int y,
                   std::string const & msg, uint32_t color, de::Align align )
    {
       auto font = de::getFontFace5x8();
       if ( !font ) return;
       font->drawText( img, x, y, msg, color, align );
    }

    */

} // end namespace DarkGDK.
