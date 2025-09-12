#include <de_image_formats/de_ImageCodecManager.h>

// All the image codec #defines
#include <de_image_formats/de_ImageCodecConfig.h>

// Timed load and save ops
#include <de_core/de_PerformanceTimer.h>

// joinVector(), etc...
#include <de_core/de_StringUtil.h>

#ifdef DE_IMAGE_READER_JPG_ENABLED     // *.jpg, *.jpeg
   #include <de_image_formats/JPG/de_ImageJPG_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_JPG_ENABLED
   #include <de_image_formats/JPG/de_ImageJPG_Writer.h>
#endif
#ifdef DE_IMAGE_READER_PNG_ENABLED     // *.png
   #include <de_image_formats/PNG/de_ImagePNG_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_PNG_ENABLED
   #include <de_image_formats/PNG/de_ImagePNG_Writer.h>
#endif
#ifdef DE_IMAGE_READER_XPM_ENABLED     // *.xpm
   #include <de_image_formats/XPM/de_ImageXPM_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_XPM_ENABLED
   #include <de_image_formats/XPM/de_ImageXPM_Writer.h>
#endif
#ifdef DE_IMAGE_READER_ICO_ENABLED     // *.ico, *.cur
   #include <de_image_formats/ICO/de_ImageICO_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_ICO_ENABLED
   #include <de_image_formats/ICO/de_ImageICO_Writer.h>
#endif
#ifdef DE_IMAGE_READER_BMP_ENABLED     // *.bmp
   #include <de_image_formats/BMP/de_ImageBMP_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_BMP_ENABLED
   #include <de_image_formats/BMP/de_ImageBMP_Writer.h>
#endif
#ifdef DE_IMAGE_READER_GIF_ENABLED     // *.gif
   #include <de_image_formats/GIF/de_ImageGIF_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_GIF_ENABLED
   #include <de_image_formats/GIF/de_ImageGIF_Writer.h>
#endif
#ifdef DE_IMAGE_READER_PCX_ENABLED     // *.pcx
   #include <de_image_formats/PCX/de_ImagePCX_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_PCX_ENABLED
   #include <de_image_formats/PCX/de_ImagePCX_Writer.h>
#endif
#ifdef DE_IMAGE_READER_PPM_ENABLED     // *.ppm
   #include <de_image_formats/PPM/de_ImagePPM_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_PPM_ENABLED
   #include <de_image_formats/PPM/de_ImagePPM_Writer.h>
#endif
#ifdef DE_IMAGE_READER_TGA_ENABLED     // *.tga
   #include <de_image_formats/TGA/de_ImageTGA_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_TGA_ENABLED
   #include <de_image_formats/TGA/de_ImageTGA_Writer.h>
#endif
#ifdef DE_IMAGE_READER_TIF_ENABLED     // *.tif, *.tiff, *.tif4, *.tiff4
   #include <de_image_formats/TIF/de_ImageTIF_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_TIF_ENABLED
   #include <de_image_formats/TIF/de_ImageTIF_Writer.h>
#endif
#ifdef DE_IMAGE_READER_DDS_ENABLED     // *.dds
   #include <de_image_formats/DDS/de_ImageDDS_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_DDS_ENABLED
   #include <de_image_formats/DDS/de_ImageDDS_Writer.h>
#endif
#ifdef DE_IMAGE_READER_HTML_ENABLED    // *.html
   #include <de_image_formats/HTML/de_ImageHTML_Reader.h>
#endif
#ifdef DE_IMAGE_WRITER_HTML_ENABLED
   #include <de_image_formats/HTML/de_ImageHTML_Writer.h>
#endif

namespace de {

// ===========================================================================
// ===   ImageCodecManager
// ===========================================================================

ImageCodecManager::ImageCodecManager()
{
#ifdef DE_IMAGE_READER_ICO_ENABLED
   m_Reader.push_back( new image::ImageReaderICO );
#endif
#ifdef DE_IMAGE_WRITER_ICO_ENABLED
   m_Writer.push_back( new image::ImageWriterICO );
#endif

#ifdef DE_IMAGE_READER_BMP_ENABLED
   m_Reader.push_back( new image::ImageReaderBMP );
#endif
#ifdef DE_IMAGE_WRITER_BMP_ENABLED
   m_Writer.push_back( new image::ImageWriterBMP );
#endif

#ifdef DE_IMAGE_READER_JPG_ENABLED
   m_Reader.push_back( new image::ImageReaderJPG );
#endif
#ifdef DE_IMAGE_WRITER_JPG_ENABLED
   m_Writer.push_back( new image::ImageWriterJPG );
#endif

#ifdef DE_IMAGE_READER_PNG_ENABLED
   m_Reader.push_back( new image::ImageReaderPNG );
#endif
#ifdef DE_IMAGE_WRITER_PNG_ENABLED
   m_Writer.push_back( new image::ImageWriterPNG );
#endif

#ifdef DE_IMAGE_READER_GIF_ENABLED
   m_Reader.push_back( new image::ImageReaderGIF );
#endif
#ifdef DE_IMAGE_WRITER_GIF_ENABLED
   m_Writer.push_back( new image::ImageWriterGIF );
#endif

#ifdef DE_IMAGE_READER_PSD_ENABLED
   m_Reader.push_back( new image::ImageReaderPSD );
#endif
#ifdef DE_IMAGE_WRITER_PSD_ENABLED
   m_Writer.push_back( new image::ImageWriterPSD );
#endif

#ifdef DE_IMAGE_READER_RGB_ENABLED
   m_Reader.push_back( new image::ImageReaderRGB );
#endif
#ifdef DE_IMAGE_WRITER_RGB_ENABLED
   m_Writer.push_back( new image::ImageWriterRGB );
#endif

#ifdef DE_IMAGE_READER_DDS_ENABLED
   m_Reader.push_back( new image::ImageReaderDDS );
#endif
#ifdef DE_IMAGE_WRITER_DDS_ENABLED
   m_Writer.push_back( new image::ImageWriterDDS );
#endif

#ifdef DE_IMAGE_READER_TGA_ENABLED
   m_Reader.push_back( new image::ImageReaderTGA );
#endif
#ifdef DE_IMAGE_WRITER_TGA_ENABLED
   m_Writer.push_back( new image::ImageWriterTGA );
#endif

#ifdef DE_IMAGE_READER_TIF_ENABLED
   m_Reader.push_back( new image::ImageReaderTIF );
#endif
#ifdef DE_IMAGE_WRITER_TIF_ENABLED
   m_Writer.push_back( new image::ImageWriterTIF );
#endif

#ifdef DE_IMAGE_READER_PPM_ENABLED
   m_Reader.push_back( new image::ImageReaderPPM );
#endif
#ifdef DE_IMAGE_WRITER_PPM_ENABLED
   m_Writer.push_back( new image::ImageWriterPPM );
#endif

#ifdef DE_IMAGE_READER_PCX_ENABLED
   m_Reader.push_back( new image::ImageReaderPCX );
#endif
#ifdef DE_IMAGE_WRITER_PCX_ENABLED
   m_Writer.push_back( new image::ImageWriterPCX );
#endif

#ifdef DE_IMAGE_READER_XPM_ENABLED
   m_Reader.push_back( new image::ImageReaderXPM );
#endif
#ifdef DE_IMAGE_WRITER_XPM_ENABLED
   m_Writer.push_back( new image::ImageWriterXPM );
#endif

#ifdef DE_IMAGE_READER_WAL_ENABLED
   m_Reader.push_back( new image::ImageReaderWAL );
#endif
#ifdef DE_IMAGE_WRITER_WAL_ENABLED
   m_Writer.push_back( new image::ImageWriterWAL );
#endif

#ifdef DE_IMAGE_WRITER_HTML_ENABLED
   m_Writer.push_back( new image::ImageWriterHtmlTable );
#endif

#ifdef _DEBUG
   DE_DEBUG("Add image codec readers and writers")
   DE_DEBUG("Supported image reader: ", getSupportedReadExtensions())
   DE_DEBUG("Supported image writer: ", getSupportedWriteExtensions())
   // AsciiArt::test();
#endif
}

ImageCodecManager::~ImageCodecManager()
{
#ifdef _DEBUG
   std::string load_ext = getSupportedReadExtensions();
   std::string save_ext = getSupportedWriteExtensions();
   DE_DEBUG("Released ",m_Reader.size()," ImageReader with FileFormats: ", load_ext )
   DE_DEBUG("Released ",m_Writer.size()," ImageWriter with FileFormats: ", save_ext )
#endif
   for ( size_t i = 0; i < m_Reader.size(); ++i )
   {
      if ( m_Reader[ i ] )
      {
         delete m_Reader[ i ];
      }
   }
   m_Reader.clear();

   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      if ( m_Writer[ i ] )
      {
         delete m_Writer[ i ];
      }
   }
   m_Writer.clear();

   // clearImages();
}

// static
std::shared_ptr< ImageCodecManager >
ImageCodecManager::getInstance()
{
   static std::shared_ptr< ImageCodecManager > s_ImageManager( new ImageCodecManager() );
   return s_ImageManager;
}


bool
ImageCodecManager::loadImageFromMemory( Image & img, uint8_t const* src, uint64_t bytes, std::string uri, ImageLoadOptions const & options ) const
{
   PerformanceTimer timer;
   timer.start();

   IImageReader* reader = nullptr;

   // FileMagic::EMagic magic = FileMagic::fromFileExtension( uri );
   // if ( magic == FileMagic::MagicBMP )
   // {
      // reader = m_Reader[ 0 ];
   // }
   // else if ( magic == FileMagic::MagicJPG )
   // {
      // reader = m_Reader[ 1 ];
   // }
   // else if ( magic == FileMagic::MagicPNG )
   // {
      // reader = m_Reader[ 2 ];
   // }
   // else if ( magic == FileMagic::MagicGIF )
   // {
      // reader = m_Reader[ 3 ];
   // }
   // else
   // {
      std::string suffix = FileSystem::fileSuffix( uri );
      if ( suffix.empty() )
      {
         DE_ERROR("Empty suffix.")
      }

      for ( size_t i = 0; i < m_Reader.size(); ++i )
      {
         IImageReader* cached = m_Reader[ i ];
         if ( cached && cached->isSupportedReadExtension( suffix ) )
         {
            reader = cached;
            break;
         }
      }

   // }

   if ( !reader )
   {
      DE_ERROR("No ImageReader found file(",uri,")" ) // , magic(", magic, ")"
      return false;
   }

   Binary binary;
   bool ok = binary.openMemory( src, bytes, uri );
   if ( !ok )
   {
      DE_ERROR("No binary (",uri,")" ) // , magic(", magic, ")"
   }
   else
   {
      ok = reader->load( img, binary );
      // if ( ok )
      // {
         // if ( options.m_SearchColor != options.m_ReplaceColor )
         // {
            // // DE_MAIN_DEBUG("ReplaceColor")
            // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
         // }
         // if ( options.m_AutoSaturate )
         // {
            // // DE_MAIN_DEBUG("Saturate")
            // ImagePainter::autoSaturate( img );
         // }

         // if ( options.m_Brighten != 1.0f )
         // {
            // ImagePainter::brighten( img, options.m_Brighten );
         // }
         // if ( options.m_Gamma != 1.0f )
         // {
            // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
         // }
         // if ( options.m_Contrast != 1.0f )
         // {
            // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
         // }
      // }
   }

   timer.stop();
   if ( !ok )
   {
      DE_ERROR("Cant load image ms(",timer.ms(),"), uri(",uri,")")
   }
   else
   {
      DE_DEBUG("ms(",timer.ms(),") to load img(",img.toString(),")")
   }

   return ok;
}


bool
ImageCodecManager::loadImage( Image & img, std::string uri, ImageLoadOptions const & options ) const
{
   PerformanceTimer timer;
   timer.start();

   uri = FileSystem::makeAbsolute( uri );

   IImageReader* reader = nullptr;

   //FileMagic::EMagic magic = FileMagic::fromUri( uri );
   // if ( magic == FileMagic::MagicBMP )
   // {
      // reader = m_Reader[ 0 ];
   // }
   // else if ( magic == FileMagic::MagicJPG )
   // {
      // reader = m_Reader[ 1 ];
   // }
   // else if ( magic == FileMagic::MagicPNG )
   // {
      // reader = m_Reader[ 2 ];
   // }
   // else if ( magic == FileMagic::MagicGIF )
   // {
      // reader = m_Reader[ 3 ];
   // }
   // else
   // {
      std::string suffix = FileSystem::fileSuffix( uri );
      if ( suffix.empty() )
      {
         DE_ERROR("Empty suffix.")
      }

      for ( size_t i = 0; i < m_Reader.size(); ++i )
      {
         IImageReader* test = m_Reader[ i ];
         if ( test && test->isSupportedReadExtension( suffix ) )
         {
            reader = test;
            break;
         }
      }

   // }

   if ( !reader )
   {
      DE_ERROR("No ImageReader found file(",uri,"), suffix(", suffix, ")" )
      //DE_ERROR("No ImageReader found file(",uri,"), magic(", magic, "), suffix(", suffix, ")" )
      return false;
   }

   bool ok = false;
   if ( reader->load( img, uri ) )
   {
      ok = true;
   }

   img.setUri( uri );

   timer.stop();

   if ( ok )
   {
      // if ( options.m_SearchColor != options.m_ReplaceColor )
      // {
         // // DE_MAIN_DEBUG("ReplaceColor")
         // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
      // }
      // if ( options.m_AutoSaturate )
      // {
         // // DE_MAIN_DEBUG("Saturate")
         // ImagePainter::autoSaturate( img );
      // }

      // if ( options.m_Brighten != 1.0f )
      // {
         // ImagePainter::brighten( img, options.m_Brighten );
      // }
      // if ( options.m_Gamma != 1.0f )
      // {
         // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
      // }
      // if ( options.m_Contrast != 1.0f )
      // {
         // // ImagePainter::replaceColor( img, options.m_SearchColor, options.m_ReplaceColor );
      // }

      DE_INFO("Load [",suffix,"] ms(", timer.ms(), "), uri(", uri,"), img(", img.toString(), ")")
   }
   else
   {
      DE_ERROR("Cant Load [",suffix,"] ms(", timer.ms(), "), uri(", uri,")")
   }

   return ok;
}

bool
ImageCodecManager::saveImage( Image const & img, std::string uri, uint32_t quality ) const
{
   PerformanceTimer timer;
   timer.start();

   if ( uri.empty() )
   {
      DE_ERROR("Empty uri.")
      return false;
   }

   uri = FileSystem::makeAbsolute( uri );

   std::string suffix = FileSystem::fileSuffix( uri );
   if ( suffix.empty() )
   {
      DE_ERROR("Empty suffix")
      return false;
   }

   bool ok = false;

   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      IImageWriter* writer = m_Writer[ i ];
      if ( writer && writer->isSupportedWriteExtension( suffix ) )
      {
         // DE_DEBUG("Found ImageWriter(",i,") for ",suffix," file(",uri,")")
         std::string dir = FileSystem::fileDir( uri );

         if ( !FileSystem::existDirectory( dir ) )
         {
            //DE_DEBUG("[",suffix,"] Create directory(", dir ,") to save uri(", uri,")")
            FileSystem::createDirectory( dir );
         }

         if ( writer->save( img, uri, quality ) )
         {
            ok = true;
            break;
         }
      }
   }

   timer.stop();

   if ( ok )
   {
      DE_INFO("Save [",suffix,"] ms(",timer.ms(),"), uri(", uri, "), img(", img.toString(), ")")
   }
   else
   {
      DE_ERROR("Cant Save [",suffix,"] ms(",timer.ms(),"), uri(", uri, "), img(", img.toString(), ")")
   }

   return ok;
}


bool
ImageCodecManager::convertFile( std::string loadName, std::string saveName ) const
{
   std::string loadSuffix = FileSystem::fileSuffix( loadName );
   std::string saveSuffix = FileSystem::fileSuffix( saveName );
   if ( loadSuffix == saveSuffix )
   {
      //DE_DEBUG("Both files have same suffix(", loadSuffix, "), copy only, no conversion." )
      return FileSystem::copyFile( loadName, saveName );
   }

   loadName = FileSystem::makeAbsolute( loadName );
   saveName = FileSystem::makeAbsolute( saveName );

   Image img;
   bool ok = loadImage( img, loadName );
   if ( ok )
   {
      //DE_DEBUG("Loaded imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
      ok = saveImage( img, saveName );
      if ( ok )
      {
         DE_DEBUG("Converted imagefile(", loadSuffix, ") to (", saveSuffix, ")." )
      }
      else
      {
         DE_ERROR("Cant convert imagefile (", loadSuffix, ") to (", saveSuffix, ")." )
      }
   }
   else
   {
      DE_ERROR("Cant load imagefile with ext(", loadSuffix, "), uri(", loadName, ")." )
   }

   return ok;
}


std::vector< std::string >
ImageCodecManager::getSupportedReadExtensionVector() const
{
   std::vector< std::string > exts;
   for ( size_t i = 0; i < m_Reader.size(); ++i )
   {
      IImageReader* reader = m_Reader[ i ];
      if ( !reader ) continue;
      for ( std::string const & ext : reader->getSupportedReadExtensions() )
      {
         for ( size_t k = 0; k < exts.size(); ++k )
         {
            if ( exts[ k ] == ext ) continue;
         }
         exts.emplace_back( ext );
      }
   }
   return exts;
}

std::vector< std::string >
ImageCodecManager::getSupportedWriteExtensionVector() const
{
   std::vector< std::string > exts;
   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      IImageWriter* writer = m_Writer[ i ];
      if ( !writer ) continue;
      for ( std::string const & ext : writer->getSupportedWriteExtensions() )
      {
         for ( size_t k = 0; k < exts.size(); ++k )
         {
            if ( exts[ k ] == ext ) continue;
         }
         exts.emplace_back( ext );
      }
   }
   return exts;
}

std::string
ImageCodecManager::getSupportedReadExtensions() const
{
   return StringUtil::joinVector( getSupportedReadExtensionVector(), " " );
}

std::string
ImageCodecManager::getSupportedWriteExtensions() const
{
   return StringUtil::joinVector( getSupportedWriteExtensionVector(), " " );
}

bool
ImageCodecManager::isSupportedReadExtension( std::string const & suffix ) const
{
   for ( size_t i = 0; i < m_Reader.size(); ++i )
   {
      IImageReader* reader = m_Reader[ i ];
      if ( reader && reader->isSupportedReadExtension( suffix ) )
      {
         return true;
      }
   }
   return false;
}

bool
ImageCodecManager::isSupportedWriteExtension( std::string const & suffix ) const
{
   for ( size_t i = 0; i < m_Writer.size(); ++i )
   {
      IImageWriter* writer = m_Writer[ i ];
      if ( writer && writer->isSupportedWriteExtension( suffix ) )
      {
         return true;
      }
   }
   return false;
}

} // end namespace de.
