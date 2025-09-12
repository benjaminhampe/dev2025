#include <DarkImage.hpp>
#include <de/ImageManager.hpp>
#include <de/ImageXPM_Reader.hpp>

de::Image
dbResizeImage( de::Image const & img, int w, int h, int scaleOp, uint32_t keyColor )
{
   return de::ImageScaler::resize( img, w, h, scaleOp, keyColor );
}


// ===========================================================================
// DarkImageAPI
// ===========================================================================
bool
dbConvertFloatHeightmapToRGBA( de::Image const & src, de::Image & dst )
{
   de::PerformanceTimer perf;
   perf.start();
   int w = src.getWidth();
   int h = src.getHeight();
   dst.setFormat( de::ColorFormat::RGBA8888 );
   dst.resize( w, h );

   float const fmin = 1e7f;

   auto color_converter = [&] ( float height )
   {
      uint32_t color = uint32_t( height + fmin );
      uint8_t r = de::RGBA_R( color );
      uint8_t g = de::RGBA_G( color );
      uint8_t b = de::RGBA_B( color );
      return de::RGBA( r,g,b );
   };

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t const color = src.getPixel( x,y );
         float const height = *reinterpret_cast< float const* >( &color );
         dst.setPixel( x,y, color_converter( height ) );
      }
   }

   perf.stop();
   DG_DEBUG("[Convert] needed ",perf.ms()," ms.")
   return true;
}

bool
dbLoadImage( de::Image & img, std::string uri, de::ImageLoadOptions const & options )
{
   return de::ImageManager::getInstance()->loadImage( img, uri, options );
}


bool
dbLoadImageFromMemory( de::Image & img, uint8_t const* data, uint64_t bytes, std::string uri, de::ImageLoadOptions const & options )
{
   return de::ImageManager::getInstance()->loadImageFromMemory( img, data, bytes, uri, options );
}

bool
dbLoadImageXPM( de::Image & dst, std::vector< std::string > const & xpm_data )
{
//   char const ** start = xpm_data;
//   size_t count = 0;
//   while (*start)
//   {
//      count++;
//      start++;
//   }

//   std::vector< std::string > lines;
//   lines.reserve( count );

//   start = xpm_data;
//   while (*start)
//   {
//      std::string s = *start;
//      lines.emplace_back( s );
//      start++;
//   }

   bool ok = de::image::ImageReaderXPM::loadImpl( dst, xpm_data );
   if ( ok )
   {
      dst.setUri( "const_xpm_data_to_memory.xpm" );
   }
   return ok;
}

bool
dbSaveImage( de::Image const & img, std::string uri, uint32_t quality )
{
   return de::ImageManager::getInstance()->saveImage( img, uri, quality );
}

// ===========================================================================
// DarkFontAPI
// ===========================================================================
/*
// [TextImage].
de::Image
dbCreateGlyphImage(
   uint32_t unicode,
   uint32_t txtColor,
   uint32_t fillColor,
   de::Font const & font )
{
   auto face = de::FontManager::getInstance()->getFont( font );
   if ( face )
   {
      return face->createGlyphImage( unicode, txtColor, fillColor );
   }
   else
   {
      DEM_ERROR("No font impl")
      return {};
   }
}
de::PreparedGlyphText
dbPrepareText( std::wstring const & msg, de::Font const & font )
{
   de::PreparedGlyphText prepared;

   auto face = de::FontManager::getInstance()->getFont( font );
   if ( face )
   {
      prepared.needUploadAtlas = face->cacheString( msg );
      prepared.textSize = face->getTextSize( msg );
      prepared.isValid = true;
   }
   else
   {
      DEM_ERROR("No font impl")
   }

   return prepared;
}


// [TextDraw].
de::TextSize
dbGetTextSize(
   std::wstring const & msg,
   de::Font const & font )
{
   auto face = de::FontManager::getInstance()->getFont( font );
   if ( face )
   {
      return face->getTextSize( msg );
   }
   else
   {
      DEM_ERROR("No font impl, font = ", font.toString() )
      return {}; // text_w, text_h, baseline_y_off.
   }
}

// [TextDraw].
void
dbDrawText(
   de::Image & img,
   int x, int y,
   std::wstring const & msg,
   uint32_t color,
   de::Align align,
   de::Font const & font )
{
   auto face = de::FontManager::getInstance()->getFont( font );
   if ( face )
   {
      face->drawText( img, x,y, msg, color, align );
   }
   else
   {
      DEM_ERROR("No font impl")
   }
}

// [TextImage].
de::Image
dbCreateTextImage(
   std::wstring const & msg,
   uint32_t txtColor,
   uint32_t fillColor,
   de::Font const & font )
{
   auto face = de::FontManager::getInstance()->getFont( font );
   if ( face )
   {
      return face->createTextImage( msg, txtColor, fillColor );
   }
   else
   {
      DEM_ERROR("No font impl")
      return {};
   }
}

de::Image const*
dbGetFontAtlasImage( de::Font const & font )
{
   auto face = de::FontManager::getInstance()->getFont( font );
   if ( face )
   {
      return face->getAtlasImage();
   }
   else
   {
      DEM_ERROR("No font impl")
      return nullptr;
   }
}

void
dbSaveFonts()
{
   de::FontManager::getInstance()->saveFonts();
}
*/
