// Copyright (C) 2002-2012, Benjamin Hampe
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ImageHTML_Writer.hpp"

#ifdef DE_IMAGE_WRITER_HTML_ENABLED

//#include <de/File.hpp>
#include <sstream>

namespace de {
namespace image {

bool
ImageWriterHtmlTable::save( Image const & img, std::string const & uri, uint32_t quality )
{
   //PerformanceTimer timer;
   //timer.start();

   if ( img.empty() )
   {
      DE_ERROR( "Source image is empty(", img.toString(), ")" )
      return false;
   }

   std::stringstream s; s <<
   R"(<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
   <html>
   <head>
     <title>ImageExport to HTML table, each cell a pixel.</title>
     <meta http-equiv="Content-Type" content="text/html; charset=windows-1252">
     <meta http-equiv="Content-Language" content="en-us">
     <meta name="author" content="Benjamin Hampe">
     <meta name="description" content="Exported image to HTML table">
     <style><!-- .normal { margin:none; padding:none; border:none; font-family: Arial; } --></style>
     <style><!-- table { margin:none; padding:none; border:none; } --></style>
     <style><!-- tr { min-width:1px; min-height:1px; } --></style>
     <style><!-- td { min-width:1px; min-height:1px; } --></style>
   </head>
   <body class="normal">
   <hr>
   <h1>TABLE_IMAGE: Each cell a pixel, for real.</h1>
   <hr><p class="normal">uri: ")";
   s << uri;
   s << R"(<hr>
   <table border="0" cellpadding="0" cellspacing="0" width="100%" height="65%">)";

   for ( int32_t y = 0; y < img.getHeight(); ++y )
   {
      s << "<tr>";
      if ( img.getWidth() > 3 ) s << '\n';
      for ( int32_t x = 0; x < img.getWidth(); ++x )
      {
         uint32_t const color = img.getPixel( x, y );

         s << "<td style=\"color:none; background-color: rgb("
           << int32_t( RGBA_R( color ) ) << ","
           << int32_t( RGBA_G( color ) ) << ","
           << int32_t( RGBA_B( color ) ) << ");\"></td>";
         if ( img.getWidth() > 3 ) s << '\n';
      }
      s << "</tr>\n";
   }

   s <<
      R"(</table>
      <hr>
      <p class="normal">Copyright @ 2020 Benjamin Hampe (benjaminhampe@gmx.de)</p>
      <hr>
      </body>
      </html>
      )";

   bool ok = FileSystem::saveText( uri, s.str() );

   //timer.stop();

   //DE_DEBUG( "needed(", timer.ms(), "), file(", uri, "), quality(", quality, "), img(", img.toString(), ")." )

   return ok;
}

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_HTML_ENABLED
