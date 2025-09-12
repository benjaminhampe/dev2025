#pragma once
//#include <de/String.hpp>
#include <de_image/de_Image.h>
#include <de_image_formats/de_ImageLoadOptions.h>
//#include <de/ImagePainter.hpp>
//#include <de/Font5x8.hpp>
//#include <de/CommonDialogs.hpp>
//#include <de/Desktop.hpp>
//#include <de/CPU.hpp>
//#include <de/RAM.hpp>
//#include <de/RuntimeError.hpp>

bool
dbLoadImage(   de::Image & img,
               std::string uri,
               de::ImageLoadOptions const & options = de::ImageLoadOptions() );

bool
dbLoadImageFromMemory(  de::Image & img,
                        uint8_t const* data,
                        uint64_t bytes,
                        std::string uri,
                        de::ImageLoadOptions const & options = de::ImageLoadOptions() );

bool
dbLoadImageXPM(   de::Image & dst,
                  std::vector< std::string > const & xpm_data );

bool
dbSaveImage(   de::Image const & img,
               std::string uri,
               uint32_t quality = 0 );

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

/*
void
dbDrawText5x8(    de::Image & img, int x, int y,
               std::string const & msg, uint32_t color, de::Align align )
{
   auto font = de::getFontFace5x8();
   if ( !font ) return;
   font->drawText( img, x, y, msg, color, align );
}

*/
