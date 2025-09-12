#include "DarkImage_details.h"

namespace DarkGDK
{

bool
dbExistImage( u32 imageId )
{
    return false;
}

bool
dbLoadImage( u32 imageId, tString const & uri, 
             ImageLoadOptions const & options )
{
    return false;
}

bool
dbLoadImage( u32 imageId, tString const & uri,
    u8 const* fileData, u64 fileSize,
    ImageLoadOptions const & options )
{
    return false;
}

bool
dbImageFromXPM( u32 imageId,
    std::vector< std::string > const & xpm_data )
{
    return false;
}

bool
dbSaveImage( u32 imageId, tString const & uri, u32 quality )
{
    return false;
}

bool
dbDeleteImage( u32 imageId )
{
    return false;
}

u32 
dbImageWidth( u32 imageId )
{
    return 0;
}

u32 
dbImageHeight( u32 imageId )
{
    return 0;
}

u32 
dbImageFormat( u32 imageId )
{
    return 0;
}

void*
dbImageRowPtr( u32 imageId, s32 y )
{
    return 0;
}

u32 
dbImageRowByteSize( u32 imageId )
{
    return 0;
}

u32 
dbImagePixelBytes( u32 imageId )
{
    return 0;
}

void* 
dbImagePixels( u32 imageId )
{
    return 0;
}

void*
dbImagePixels( u32 imageId, s32 x, s32 y )
{
    return 0;
}

u32 
dbGetImagePixel( u32 imageId, s32 x, s32 y )
{
    return 0;
}

void 
dbSetImagePixel( u32 imageId, s32 x, s32 y, u32 color )
{
    return;
}
    
} // end namespace DarkGDK.

