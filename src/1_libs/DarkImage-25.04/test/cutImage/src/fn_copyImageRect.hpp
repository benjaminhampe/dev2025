#pragma once
#include <DarkImage.h>

namespace de {

// Cut smaller image from larger image, larger image not touched
inline Image
copyImageRect( Image const & img, Recti region = Recti(0,0,0,0) )
{
   int img_w = img.w();
   int img_h = img.h();

   int copy_x = region.x;
   int copy_y = region.y;
   int copy_w = region.w > 0 ? region.w : img_w;
   int copy_h = region.h > 0 ? region.h : img_h;
	if ( copy_w + copy_x > img_w ) copy_w = img_w - copy_x;
	if ( copy_h + copy_y > img_h ) copy_h = img_h - copy_y;
  
   de::Image subImage( copy_w,copy_h, img.pixelFormat() );

   for ( int j = 0; j < copy_h; j++ )
   {
      for ( int i = 0; i < copy_w; i++ )
      {
         subImage.setPixel( i,j, img.getPixel( copy_x + i, copy_y + j ) );
      }
   }

   return subImage;
}

} // end namespace de.
