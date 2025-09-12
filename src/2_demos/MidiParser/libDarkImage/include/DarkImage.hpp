#pragma once
//#include <de/String.hpp>
#include <de/Logger.hpp>
#include <de/Timer.hpp>
#include <de/PerformanceTimer.hpp>
#include <de/FileSystem.hpp>
#include <de/StringUtil.hpp>

#include <de/FileMagic.hpp>
#include <de/Binary.hpp>
// #include <de/File.hpp>
// #include <de/FileCategory.hpp>
// #include <de/FilePreview.hpp>
// #include <de/FileStream.hpp>
//#include <de/Math.hpp>
//#include <de/Hex.hpp>
#include <de/Recti.hpp>
#include <de/Rectf.hpp>
#include <de/Align.hpp>
#include <de/ColorGradient.hpp>
//#include <de/Console.hpp>
#include <de/Image.hpp>
//#include <de/ImageRef.hpp>
#include <de/ImageReader.hpp>
#include <de/ImageWriter.hpp>
#include <de/ImagePainter.hpp>
#include <de/Font5x8.hpp>
//#include <de/CommonDialogs.hpp>
//#include <de/Desktop.hpp>
//#include <de/CPU.hpp>
//#include <de/RAM.hpp>
//#include <de/RuntimeError.hpp>

bool dbLoadImage( de::Image & img, std::string uri, de::ImageLoadOptions const & options = de::ImageLoadOptions() );
bool dbLoadImageFromMemory( de::Image & img, uint8_t const* data, uint64_t bytes, std::string uri, de::ImageLoadOptions const & options = de::ImageLoadOptions() );
bool dbLoadImageXPM( de::Image & dst, std::vector< std::string > const & xpm_data );
bool dbSaveImage( de::Image const & img, std::string uri, uint32_t quality = 0 );
bool dbConvertFloatHeightmapToRGBA( de::Image const & src, de::Image & dst );

// bool dbConvertImageFile( std::string loadName, std::string saveName );
de::Image dbResizeImage( de::Image const & img, int w, int h, int scaleOp = 1, uint32_t keyColor = 0x00000000 );

//void dbDrawText5x8( de::Image & img, int x, int y, std::string const & msg, uint32_t color = 0xFF000000, de::Align align = de::Align::Default );

/*
// ===========================================================================
// ImagePainter::drawTextFont5x8()
// ===========================================================================
void
dbDrawText5x8( de::Image & img, int x, int y,
               std::string const & msg, uint32_t color, de::Align align )
{
   auto font = de::getFontFace5x8();
   if ( !font ) return;
   font->drawText( img, x, y, msg, color, align );
}
*/
